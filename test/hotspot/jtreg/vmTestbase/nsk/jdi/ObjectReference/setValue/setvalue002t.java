/*
 * Copyright (c) 2002, 2025, Oracle and/or its affiliates. All rights reserved.
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

package nsk.jdi.ObjectReference.setValue;

import nsk.share.*;
import nsk.share.jpda.*;
import nsk.share.jdi.*;

/**
 * This is a debuggee class.
 */
public class setvalue002t {
    static Thread testThread = null;

    // tested static fields
    static byte    sByteFld = 127;
    static short   sShortFld = -32768;
    static int     sIntFld = 2147483647;
    static long    sLongFld = 9223372036854775807L;
    static float   sFloatFld = 5.1F;
    static double  sDoubleFld = 6.2D;
    static char    sCharFld = 'a';
    static boolean sBooleanFld = false;
    static String  sStrFld = "instance field";

    // tested instance fields
    byte    byteFld = 127;
    short   shortFld = -32768;
    int     intFld = 2147483647;
    long    longFld = 9223372036854775807L;
    float   floatFld = 5.1F;
    double  doubleFld = 6.2D;
    char    charFld = 'a';
    boolean booleanFld = false;
    String  strFld = "instance field";

    public static void main(String args[]) {
        System.exit(run(args) + Consts.JCK_STATUS_BASE);
    }

    public static int run(String args[]) {
        return new setvalue002t().runIt(args);
    }

    private int runIt(String args[]) {
        ArgumentHandler argHandler = new ArgumentHandler(args);
        IOPipe pipe = argHandler.createDebugeeIOPipe();
        setvalue002tDummyClass dummyCls = new setvalue002tDummyClass();

        testThread = Thread.currentThread();
        testThread.setName(setvalue002.DEBUGGEE_THRNAME);

        pipe.println(setvalue002.COMMAND_READY);
        String cmd = pipe.readln();
        if (!cmd.equals(setvalue002.COMMAND_QUIT)) {
            System.err.println("TEST BUG: unknown debugger command: "
                + cmd);
            System.exit(Consts.JCK_STATUS_BASE +
                Consts.TEST_FAILED);
        }
        return Consts.TEST_PASSED;
    }
}

// Dummy class used to provoke IllegalArgumentException in debugger
class setvalue002tDummyClass {
    // tested static fields
    static byte    sByteFld = 127;
    static short   sShortFld = -32768;
    static int     sIntFld = 2147483647;
    static long    sLongFld = 9223372036854775807L;
    static float   sFloatFld = 5.1F;
    static double  sDoubleFld = 6.2D;
    static char    sCharFld = 'a';
    static boolean sBooleanFld = false;
    static String  sStrFld = "instance field";

    // tested instance fields
    byte    byteFld = 127;
    short   shortFld = -32768;
    int     intFld = 2147483647;
    long    longFld = 9223372036854775807L;
    float   floatFld = 5.1F;
    double  doubleFld = 6.2D;
    char    charFld = 'a';
    boolean booleanFld = false;
    String  strFld = "instance field";
}
