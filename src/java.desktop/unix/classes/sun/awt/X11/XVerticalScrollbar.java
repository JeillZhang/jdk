/*
 * Copyright (c) 2002, 2007, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
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

package sun.awt.X11;

import java.awt.*;

/**
* A simple vertical scroll bar.
*/
final class XVerticalScrollbar extends XScrollbar {
    public XVerticalScrollbar(XScrollbarClient sb) {
        super(ALIGNMENT_VERTICAL, sb);
    }

    @Override
    public void setSize(int width, int height) {
        super.setSize(width, height);
        this.barWidth = width;
        this.barLength = height;
        calculateArrowWidth();
        rebuildArrows();
    }

    @Override
    protected void rebuildArrows() {
        firstArrow = createArrowShape(true, true);
        secondArrow = createArrowShape(true, false);
    }

    @Override
    boolean beforeThumb(int x, int y) {
        Rectangle pos = calculateThumbRect();
        return (y < pos.y);
    }

    @Override
    protected Rectangle getThumbArea() {
        return new Rectangle(2, getArrowAreaWidth(), width-4, height - 2*getArrowAreaWidth());
    }
}
