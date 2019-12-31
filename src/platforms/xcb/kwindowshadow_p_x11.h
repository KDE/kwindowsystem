/*
    Copyright (C) 2019 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KWINDOWSHADOW_P_X11_H
#define KWINDOWSHADOW_P_X11_H

#include "kwindowshadow_p.h"

#include <xcb/xcb.h>

class KWindowShadowTilePrivateX11 final : public KWindowShadowTilePrivate
{
public:
    bool create() override;
    void destroy() override;

    static KWindowShadowTilePrivateX11 *get(const KWindowShadowTile *tile);

    xcb_pixmap_t pixmap = XCB_PIXMAP_NONE;
    xcb_gcontext_t gc = XCB_NONE;
};

class KWindowShadowPrivateX11 final : public KWindowShadowPrivate
{
public:
    bool create() override;
    void destroy() override;

    KWindowShadowTile::Ptr getOrCreateEmptyTile();

    KWindowShadowTile::Ptr emptyTile;
};

#endif // KWINDOWSHADOW_P_X11_H
