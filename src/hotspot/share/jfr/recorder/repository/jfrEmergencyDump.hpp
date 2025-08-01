/*
 * Copyright (c) 2018, 2025, Oracle and/or its affiliates. All rights reserved.
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

#ifndef SHARE_JFR_RECORDER_REPOSITORY_JFREMERGENCYDUMP_HPP
#define SHARE_JFR_RECORDER_REPOSITORY_JFREMERGENCYDUMP_HPP

#include "memory/allStatic.hpp"

class outputStream;

//
// Responsible for creating an hs_err<pid>.jfr file in exceptional shutdown situations (crash, OOM)
//
class JfrEmergencyDump : AllStatic {
 public:
  static const char* get_dump_path();
  static void set_dump_path(const char* dump_path);
  static const char* chunk_path(const char* repository_path);
  static void on_vm_error(const char* repository_path);
  static void on_vm_error_report(outputStream* st, const char* repository_path);
  static void on_vm_shutdown(bool emit_old_object_samples, bool emit_event_shutdown);
};

#endif // SHARE_JFR_RECORDER_REPOSITORY_JFREMERGENCYDUMP_HPP
