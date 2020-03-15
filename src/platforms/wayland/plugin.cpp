/*
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "plugin.h"
#include "kwindowsystem_p_wayland.h"

WaylandPlugin::WaylandPlugin(QObject *parent)
    : KWindowSystemPluginInterface(parent)
{
}

WaylandPlugin::~WaylandPlugin()
{
}

KWindowSystemPrivate *WaylandPlugin::createWindowSystem()
{
    return new KWindowSystemPrivateWayland();
}
