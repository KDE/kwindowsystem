/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "waylandxdgtopleveltagv1_p.h"

#include <QGuiApplication>

WaylandXdgToplevelTagManagerV1::WaylandXdgToplevelTagManagerV1()
    : QWaylandClientExtensionTemplate<WaylandXdgToplevelTagManagerV1>(1)
{
    initialize();
}

WaylandXdgToplevelTagManagerV1::~WaylandXdgToplevelTagManagerV1()
{
    if (qGuiApp && isActive()) {
        destroy();
    }
}

WaylandXdgToplevelTagManagerV1 *WaylandXdgToplevelTagManagerV1::self()
{
    static WaylandXdgToplevelTagManagerV1 *instance = new WaylandXdgToplevelTagManagerV1;
    return instance;
}

#include "moc_waylandxdgtopleveltagv1_p.cpp"
