/*
 * Copyright (c) 2011, 2025, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2013 SAP SE. All rights reserved.
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

#ifndef OS_AIX_DECODER_AIX_HPP
#define OS_AIX_DECODER_AIX_HPP

#include "utilities/decoder.hpp"
#include "porting_aix.hpp"

// Provide simple AIXDecoder which enables decoding of C frames in VM.
class AIXDecoder: public AbstractDecoder {
 public:
  AIXDecoder() : AbstractDecoder(no_error) {}
  virtual ~AIXDecoder() {}

  virtual bool demangle(const char* symbol, char* buf, int buflen) { return false; } // use AixSymbols::get_function_name to demangle

  virtual bool decode(address addr, char* buf, int buflen, int* offset, const char* modulepath, bool demangle) {
    return AixSymbols::get_function_name(addr, buf, buflen, offset, 0, demangle);
  }
  virtual bool decode(address addr, char *buf, int buflen, int* offset, const void *base) {
    ShouldNotReachHere();
    return false;
  }

};

#endif // OS_AIX_DECODER_AIX_HPP
