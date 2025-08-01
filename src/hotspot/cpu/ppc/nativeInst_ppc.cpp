/*
 * Copyright (c) 1997, 2025, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2012, 2024 SAP SE. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#include "asm/macroAssembler.inline.hpp"
#include "code/compiledIC.hpp"
#include "memory/resourceArea.hpp"
#include "nativeInst_ppc.hpp"
#include "oops/compressedOops.inline.hpp"
#include "oops/oop.hpp"
#include "runtime/handles.hpp"
#include "runtime/orderAccess.hpp"
#include "runtime/safepoint.hpp"
#include "runtime/sharedRuntime.hpp"
#include "runtime/stubRoutines.hpp"
#include "utilities/ostream.hpp"
#ifdef COMPILER1
#include "c1/c1_Runtime1.hpp"
#endif

#ifdef ASSERT
void NativeInstruction::verify() {
  // Make sure code pattern is actually an instruction address.
  address addr = addr_at(0);
  if (addr == nullptr || ((intptr_t)addr & 3) != 0) {
    fatal("not an instruction address");
  }
}
#endif // ASSERT

// Extract call destination from a NativeCall. The call might use a trampoline stub.
address NativeCall::destination() const {
  address addr = (address)this;
  address destination = Assembler::bxx_destination(addr);

  // Do we use a trampoline stub for this call?
  // Trampoline stubs are located behind the main code.
  if (destination > addr) {
    // Filter out recursive method invocation (call to verified/unverified entry point).
    CodeBlob* cb = CodeCache::find_blob(addr);
    assert(cb && cb->is_nmethod(), "sanity");
    nmethod *nm = (nmethod *)cb;
    if (nm->stub_contains(destination) && is_NativeCallTrampolineStub_at(destination)) {
      // Yes we do, so get the destination from the trampoline stub.
      const address trampoline_stub_addr = destination;
      destination = NativeCallTrampolineStub_at(trampoline_stub_addr)->destination(nm);
    }
  }

  return destination;
}

// Similar to replace_mt_safe, but just changes the destination. The
// important thing is that free-running threads are able to execute this
// call instruction at all times. Thus, the displacement field must be
// instruction-word-aligned.
//
// Used in the runtime linkage of calls; see class CompiledIC.
//
// Add parameter assert_lock to switch off assertion
// during code generation, where no lock is needed.
void NativeCall::set_destination_mt_safe(address dest, bool assert_lock) {
  assert(!assert_lock ||
         (CodeCache_lock->is_locked() || SafepointSynchronize::is_at_safepoint()) ||
         CompiledICLocker::is_safe(addr_at(0)),
         "concurrent code patching");

  ResourceMark rm;
  int code_size = 1 * BytesPerInstWord;
  address addr_call = addr_at(0);
  assert(MacroAssembler::is_bl(*(int*)addr_call), "unexpected code at call-site");

  CodeBuffer cb(addr_call, code_size + 1);
  MacroAssembler* a = new MacroAssembler(&cb);

  // Patch the call.
  if (!ReoptimizeCallSequences || !a->is_within_range_of_b(dest, addr_call)) {
    address trampoline_stub_addr = get_trampoline();

    // We did not find a trampoline stub because the current codeblob
    // does not provide this information. The branch will be patched
    // later during a final fixup, when all necessary information is
    // available.
    if (trampoline_stub_addr == nullptr)
      return;

    // Patch the constant in the call's trampoline stub.
    NativeCallTrampolineStub_at(trampoline_stub_addr)->set_destination(dest);
    dest = trampoline_stub_addr;
  }

  OrderAccess::release();
  a->bl(dest);

  ICache::ppc64_flush_icache_bytes(addr_call, code_size);
}

address NativeCall::get_trampoline() {
  address call_addr = addr_at(0);

  CodeBlob *code = CodeCache::find_blob(call_addr);
  assert(code != nullptr, "Could not find the containing code blob");

  // There are no relocations available when the code gets relocated
  // because of CodeBuffer expansion.
  if (code->relocation_size() == 0)
    return nullptr;

  address bl_destination = Assembler::bxx_destination(call_addr);
  if (code->contains(bl_destination) &&
      is_NativeCallTrampolineStub_at(bl_destination))
    return bl_destination;

  // If the codeBlob is not a nmethod, this is because we get here from the
  // CodeBlob constructor, which is called within the nmethod constructor.
  return trampoline_stub_Relocation::get_trampoline_for(call_addr, (nmethod*)code);
}

#ifdef ASSERT
void NativeCall::verify() {
  address addr = addr_at(0);

  if (!NativeCall::is_call_at(addr)) {
    tty->print_cr("not a NativeCall at " PTR_FORMAT, p2i(addr));
    // TODO: PPC port: Disassembler::decode(addr - 20, addr + 20, tty);
    fatal("not a NativeCall at " PTR_FORMAT, p2i(addr));
  }
}
#endif // ASSERT

#ifdef ASSERT
void NativeFarCall::verify() {
  address addr = addr_at(0);

  NativeInstruction::verify();
  if (!NativeFarCall::is_far_call_at(addr)) {
    tty->print_cr("not a NativeFarCall at " PTR_FORMAT, p2i(addr));
    // TODO: PPC port: Disassembler::decode(addr, 20, 20, tty);
    fatal("not a NativeFarCall at " PTR_FORMAT, p2i(addr));
  }
}
#endif // ASSERT

address NativeMovConstReg::next_instruction_address() const {
#ifdef ASSERT
  CodeBlob* nm = CodeCache::find_blob(instruction_address());
  assert(nm != nullptr, "Could not find code blob");
  assert(!MacroAssembler::is_set_narrow_oop(addr_at(0), nm->content_begin()), "Should not patch narrow oop here");
#endif

  if (MacroAssembler::is_load_const_from_method_toc_at(addr_at(0))) {
    return addr_at(load_const_from_method_toc_instruction_size);
  } else {
    return addr_at(load_const_instruction_size);
  }
}

intptr_t NativeMovConstReg::data() const {
  address   addr = addr_at(0);

  if (MacroAssembler::is_load_const_at(addr)) {
    return MacroAssembler::get_const(addr);
  }

  CodeBlob* cb = CodeCache::find_blob(addr);
  assert(cb != nullptr, "Could not find code blob");
  if (MacroAssembler::is_set_narrow_oop(addr, cb->content_begin())) {
    narrowOop no = MacroAssembler::get_narrow_oop(addr, cb->content_begin());
    // We can reach here during GC with 'no' pointing to new object location
    // while 'heap()->is_in' still reports false (e.g. with SerialGC).
    // Therefore we use raw decoding.
    if (CompressedOops::is_null(no)) return 0;
    return cast_from_oop<intptr_t>(CompressedOops::decode_raw(no));
  } else if (MacroAssembler::is_load_const_from_method_toc_at(addr)) {
    address ctable = cb->content_begin();
    int offset = MacroAssembler::get_offset_of_load_const_from_method_toc_at(addr);
    return *(intptr_t *)(ctable + offset);
  } else {
    assert(MacroAssembler::is_calculate_address_from_global_toc_at(addr, addr - BytesPerInstWord),
           "must be calculate_address_from_global_toc");
    return (intptr_t) MacroAssembler::get_address_of_calculate_address_from_global_toc_at(addr, addr - BytesPerInstWord);
  }
}

address NativeMovConstReg::set_data_plain(intptr_t data, CodeBlob *cb) {
  address addr         = instruction_address();
  address next_address = nullptr;
  if (!cb) cb = CodeCache::find_blob(addr);

  if (cb != nullptr && MacroAssembler::is_load_const_from_method_toc_at(addr)) {
    // A load from the method's TOC (ctable).
    assert(cb->is_nmethod(), "must be nmethod");
    const address ctable = cb->content_begin();
    const int toc_offset = MacroAssembler::get_offset_of_load_const_from_method_toc_at(addr);
    *(intptr_t *)(ctable + toc_offset) = data;
    next_address = addr + BytesPerInstWord;
  } else if (cb != nullptr &&
             MacroAssembler::is_calculate_address_from_global_toc_at(addr, cb->content_begin())) {
    // A calculation relative to the global TOC.
    if (MacroAssembler::get_address_of_calculate_address_from_global_toc_at(addr, cb->content_begin()) !=
        (address)data) {
      const address inst2_addr = addr;
      const address inst1_addr =
        MacroAssembler::patch_calculate_address_from_global_toc_at(inst2_addr, cb->content_begin(),
                                                                   (address)data);
      assert(inst1_addr != nullptr && inst1_addr < inst2_addr, "first instruction must be found");
      const int range = inst2_addr - inst1_addr + BytesPerInstWord;
      ICache::ppc64_flush_icache_bytes(inst1_addr, range);
    }
    next_address = addr + 1 * BytesPerInstWord;
  } else if (MacroAssembler::is_load_const_at(addr)) {
    // A normal 5 instruction load_const code sequence.
    if (MacroAssembler::get_const(addr) != (long)data) {
      // This is not mt safe, ok in methods like CodeBuffer::copy_code().
      MacroAssembler::patch_const(addr, (long)data);
      ICache::ppc64_flush_icache_bytes(addr, load_const_instruction_size);
    }
    next_address = addr + 5 * BytesPerInstWord;
  } else if (MacroAssembler::is_bl(* (int*) addr)) {
    // A single branch-and-link instruction.
    ResourceMark rm;
    const int code_size = 1 * BytesPerInstWord;
    CodeBuffer cb(addr, code_size + 1);
    MacroAssembler* a = new MacroAssembler(&cb);
    a->bl((address) data);
    ICache::ppc64_flush_icache_bytes(addr, code_size);
    next_address = addr + code_size;
  } else {
    ShouldNotReachHere();
  }

  return next_address;
}

void NativeMovConstReg::set_data(intptr_t data) {
  // Store the value into the instruction stream.
  CodeBlob *cb = CodeCache::find_blob(instruction_address());
  address next_address = set_data_plain(data, cb);

  // Also store the value into an oop_Relocation cell, if any.
  if (cb && cb->is_nmethod()) {
    RelocIterator iter((nmethod *) cb, instruction_address(), next_address);
    oop* oop_addr = nullptr;
    Metadata** metadata_addr = nullptr;
    while (iter.next()) {
      if (iter.type() == relocInfo::oop_type) {
        oop_Relocation *r = iter.oop_reloc();
        if (oop_addr == nullptr) {
          oop_addr = r->oop_addr();
          *oop_addr = cast_to_oop(data);
        } else {
          assert(oop_addr == r->oop_addr(), "must be only one set-oop here");
        }
      }
      if (iter.type() == relocInfo::metadata_type) {
        metadata_Relocation *r = iter.metadata_reloc();
        if (metadata_addr == nullptr) {
          metadata_addr = r->metadata_addr();
          *metadata_addr = (Metadata*)data;
        } else {
          assert(metadata_addr == r->metadata_addr(), "must be only one set-metadata here");
        }
      }
    }
  }
}

void NativeMovConstReg::set_narrow_oop(narrowOop data, CodeBlob *code /* = nullptr */) {
  address   inst2_addr = addr_at(0);
  CodeBlob* cb = (code) ? code : CodeCache::find_blob(instruction_address());
  assert(cb != nullptr, "Could not find code blob");
  if (MacroAssembler::get_narrow_oop(inst2_addr, cb->content_begin()) == data) {
    return;
  }
  const address inst1_addr =
    MacroAssembler::patch_set_narrow_oop(inst2_addr, cb->content_begin(), data);
  assert(inst1_addr != nullptr && inst1_addr < inst2_addr, "first instruction must be found");
  const int range = inst2_addr - inst1_addr + BytesPerInstWord;
  ICache::ppc64_flush_icache_bytes(inst1_addr, range);
}

// Do not use an assertion here. Let clients decide whether they only
// want this when assertions are enabled.
#ifdef ASSERT
void NativeMovConstReg::verify() {
  address   addr = addr_at(0);
  if (! MacroAssembler::is_load_const_at(addr) &&
      ! MacroAssembler::is_load_const_from_method_toc_at(addr)) {
    CodeBlob* cb = CodeCache::find_blob(addr);
    if (! (cb != nullptr && MacroAssembler::is_calculate_address_from_global_toc_at(addr, cb->content_begin())) &&
        ! (cb != nullptr && MacroAssembler::is_set_narrow_oop(addr, cb->content_begin())) &&
        ! MacroAssembler::is_bl(*((int*) addr))) {
      tty->print_cr("not a NativeMovConstReg at " PTR_FORMAT, p2i(addr));
      // TODO: PPC port: Disassembler::decode(addr, 20, 20, tty);
      fatal("not a NativeMovConstReg at " PTR_FORMAT, p2i(addr));
    }
  }
}
#endif // ASSERT

#ifdef ASSERT
void NativeJump::verify() {
  address addr = addr_at(0);

  NativeInstruction::verify();
  if (!NativeJump::is_jump_at(addr)) {
    tty->print_cr("not a NativeJump at " PTR_FORMAT, p2i(addr));
    // TODO: PPC port: Disassembler::decode(addr, 20, 20, tty);
    fatal("not a NativeJump at " PTR_FORMAT, p2i(addr));
  }
}
#endif // ASSERT


void NativeGeneralJump::insert_unconditional(address code_pos, address entry) {
  CodeBuffer cb(code_pos, BytesPerInstWord + 1);
  MacroAssembler a(&cb);
  a.b(entry);
  ICache::ppc64_flush_icache_bytes(code_pos, NativeGeneralJump::instruction_size);
}

// MT-safe patching of a jmp instruction.
void NativeGeneralJump::replace_mt_safe(address instr_addr, address code_buffer) {
  // Bytes beyond offset NativeGeneralJump::instruction_size are copied by caller.

  // Finally patch out the jump.
  volatile juint *jump_addr = (volatile juint*)instr_addr;
  // Release not needed because caller uses invalidate_range after copying the remaining bytes.
  //Atomic::release_store(jump_addr, *((juint*)code_buffer));
  *jump_addr = *((juint*)code_buffer); // atomically store code over branch instruction
  ICache::ppc64_flush_icache_bytes(instr_addr, NativeGeneralJump::instruction_size);
}


//-------------------------------------------------------------------

// Call trampoline stubs.
//
// Layout and instructions of a call trampoline stub:
//    0:  load the TOC (part 1)
//    4:  load the TOC (part 2)
//    8:  load the call target from the constant pool (part 1)
//  [12:  load the call target from the constant pool (part 2, optional)]
//   ..:  branch via CTR
//

address NativeCallTrampolineStub::encoded_destination_addr() const {
  address instruction_addr = addr_at(0 * BytesPerInstWord);
  if (!MacroAssembler::is_ld_largeoffset(instruction_addr)) {
    instruction_addr = addr_at(2 * BytesPerInstWord);
    assert(MacroAssembler::is_ld_largeoffset(instruction_addr),
           "must be a ld with large offset (from the constant pool)");
  }
  return instruction_addr;
}

address NativeCallTrampolineStub::destination(nmethod *nm) const {
  CodeBlob* cb = nm ? nm : CodeCache::find_blob(addr_at(0));
  assert(cb != nullptr, "Could not find code blob");
  address ctable = cb->content_begin();

  return *(address*)(ctable + destination_toc_offset());
}

int NativeCallTrampolineStub::destination_toc_offset() const {
  return MacroAssembler::get_ld_largeoffset_offset(encoded_destination_addr());
}

void NativeCallTrampolineStub::set_destination(address new_destination) {
  CodeBlob* cb = CodeCache::find_blob(addr_at(0));
  assert(cb != nullptr, "Could not find code blob");
  address ctable = cb->content_begin();

  *(address*)(ctable + destination_toc_offset()) = new_destination;
}

void NativePostCallNop::make_deopt() {
  NativeDeoptInstruction::insert(addr_at(0));
}

bool NativePostCallNop::patch(int32_t oopmap_slot, int32_t cb_offset) {
  int32_t i2, i1;
  assert(is_aligned(cb_offset, 4), "cb offset alignment does not match instruction alignment");
  assert(!decode(i1, i2), "already patched");

  cb_offset = cb_offset >> 2;
  if (((oopmap_slot & ppc_oopmap_slot_mask) != oopmap_slot) || ((cb_offset & ppc_cb_offset_mask) != cb_offset)) {
    return false;  // cannot encode
  }
  const uint32_t data = oopmap_slot << ppc_cb_offset_bits | cb_offset;
  const uint32_t lo_data = data & ppc_data_lo_mask;
  const uint32_t hi_data = data >> ppc_data_lo_bits;
  const uint32_t nineth_bit = 1 << (31 - 9);
  uint32_t instr = Assembler::CMPLI_OPCODE | hi_data << ppc_data_hi_shift | nineth_bit | lo_data;
  *(uint32_t*)addr_at(0) = instr;

  int32_t oopmap_slot_dec, cb_offset_dec;
  assert(is_post_call_nop(), "pcn not recognized");
  assert(decode(oopmap_slot_dec, cb_offset_dec), "encoding failed");
  assert(oopmap_slot == oopmap_slot_dec, "oopmap slot encoding is wrong");
  assert((cb_offset << 2) == cb_offset_dec, "cb offset encoding is wrong");

  return true;  // encoding succeeded
}

void NativeDeoptInstruction::verify() {
}

bool NativeDeoptInstruction::is_deopt_at(address code_pos) {
  if (!Assembler::is_illtrap(code_pos)) return false;
  CodeBlob* cb = CodeCache::find_blob(code_pos);
  if (cb == nullptr || !cb->is_nmethod()) return false;
  return true;
}

// Inserts an instruction which is specified to cause a SIGILL at a given pc
void NativeDeoptInstruction::insert(address code_pos) {
  ResourceMark rm;
  int code_size = 1 * BytesPerInstWord;
  CodeBuffer cb(code_pos, code_size + 1);
  MacroAssembler* a = new MacroAssembler(&cb);
  a->illtrap();
  ICache::ppc64_flush_icache_bytes(code_pos, code_size);
}
