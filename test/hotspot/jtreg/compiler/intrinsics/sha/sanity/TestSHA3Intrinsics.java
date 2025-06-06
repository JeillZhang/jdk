/*
 * Copyright (c) 2021, 2025, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2020, Huawei Technologies Co., Ltd. All rights reserved.
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
 */

/**
 * @test
 * @bug 8252204
 * @summary Verify that SHA3-224, SHA3-256, SHA3-384, SHA3-512 intrinsic is actually used.
 *
 * @library /test/lib /
 * @modules java.base/jdk.internal.misc
 *          java.management
 *
 * @build jdk.test.whitebox.WhiteBox
 * @run driver jdk.test.lib.helpers.ClassFileInstaller jdk.test.whitebox.WhiteBox
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_224.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions -XX:+UseSIMDForSHA3Intrinsic
 *                   -Dalgorithm=SHA3-224
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=negative_224.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:-UseSHA3Intrinsics
 *                   -Dalgorithm=SHA3-224
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_256.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions -XX:+UseSIMDForSHA3Intrinsic
 *                   -Dalgorithm=SHA3-256
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=negative_256.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:-UseSHA3Intrinsics
 *                   -Dalgorithm=SHA3-256
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_384.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions -XX:+UseSIMDForSHA3Intrinsic
 *                   -Dalgorithm=SHA3-384
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=negative_384.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:-UseSHA3Intrinsics
 *                   -Dalgorithm=SHA3-384
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_512.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions -XX:+UseSIMDForSHA3Intrinsic
 *                   -Dalgorithm=SHA3-512
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=negative_512.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:-UseSHA3Intrinsics
 *                   -Dalgorithm=SHA3-512
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -DverificationStrategy=VERIFY_INTRINSIC_USAGE
 *                    compiler.testlibrary.intrinsics.Verifier positive_224.log positive_256.log positive_384.log positive_512.log
 *                    negative_224.log negative_256.log negative_384.log negative_512.log
 */

/**
 * @test
 * @bug 8337666
 * @requires os.arch == "aarch64"
 * @summary Verify that SHA3-224, SHA3-256, SHA3-384, SHA3-512 intrinsic is actually used.
 *          -XX:-UseSIMDForSHA3Intrinsic -XX:-PreserveFramePointer
 *
 * @library /test/lib /
 * @modules java.base/jdk.internal.misc
 *          java.management
 *
 * @build jdk.test.whitebox.WhiteBox
 * @run driver jdk.test.lib.helpers.ClassFileInstaller jdk.test.whitebox.WhiteBox
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_224.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions
 *                   -XX:-UseSIMDForSHA3Intrinsic -XX:-PreserveFramePointer
 *                   -Dalgorithm=SHA3-224
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_256.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions
 *                   -XX:-UseSIMDForSHA3Intrinsic -XX:-PreserveFramePointer
 *                   -Dalgorithm=SHA3-256
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_384.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions
 *                   -XX:-UseSIMDForSHA3Intrinsic -XX:-PreserveFramePointer
 *                   -Dalgorithm=SHA3-384
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_512.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions
 *                   -XX:-UseSIMDForSHA3Intrinsic -XX:-PreserveFramePointer
 *                   -Dalgorithm=SHA3-512
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -DverificationStrategy=VERIFY_INTRINSIC_USAGE
 *                    compiler.testlibrary.intrinsics.Verifier positive_224.log positive_256.log positive_384.log positive_512.log
 */

/**
 * @test
 * @bug 8337666
 * @requires os.arch == "aarch64"
 * @summary Verify that SHA3-224, SHA3-256, SHA3-384, SHA3-512 intrinsic is actually used.
 *          -XX:-UseSIMDForSHA3Intrinsic -XX:+PreserveFramePointer
 *
 * @library /test/lib /
 * @modules java.base/jdk.internal.misc
 *          java.management
 *
 * @build jdk.test.whitebox.WhiteBox
 * @run driver jdk.test.lib.helpers.ClassFileInstaller jdk.test.whitebox.WhiteBox
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_224.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions
 *                   -XX:-UseSIMDForSHA3Intrinsic -XX:+PreserveFramePointer
 *                   -Dalgorithm=SHA3-224
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_256.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions
 *                   -XX:-UseSIMDForSHA3Intrinsic -XX:+PreserveFramePointer
 *                   -Dalgorithm=SHA3-256
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_384.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions
 *                   -XX:-UseSIMDForSHA3Intrinsic -XX:+PreserveFramePointer
 *                   -Dalgorithm=SHA3-384
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions
 *                   -XX:+WhiteBoxAPI -Xbatch -XX:CompileThreshold=500
 *                   -XX:Tier4InvocationThreshold=500
 *                   -XX:+LogCompilation -XX:LogFile=positive_512.log
 *                   -XX:CompileOnly=sun.security.provider.DigestBase::*
 *                   -XX:CompileOnly=sun.security.provider.SHA3::*
 *                   -XX:+UseSHA3Intrinsics
 *                   -XX:+IgnoreUnrecognizedVMOptions
 *                   -XX:-UseSIMDForSHA3Intrinsic -XX:+PreserveFramePointer
 *                   -Dalgorithm=SHA3-512
 *                   compiler.intrinsics.sha.sanity.TestSHA3Intrinsics
 * @run main/othervm -DverificationStrategy=VERIFY_INTRINSIC_USAGE
 *                    compiler.testlibrary.intrinsics.Verifier positive_224.log positive_256.log positive_384.log positive_512.log
 */

package compiler.intrinsics.sha.sanity;

import compiler.testlibrary.sha.predicate.IntrinsicPredicates;

public class TestSHA3Intrinsics {
    public static void main(String args[]) throws Exception {
        new DigestSanityTestBase(IntrinsicPredicates.isSHA3IntrinsicAvailable(),
                DigestSanityTestBase.SHA3_INTRINSIC_ID).test();
    }
}
