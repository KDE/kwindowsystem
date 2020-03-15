/*
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KWINDOWSYSTEM_WAYLAND_PLUGIN_H
#define KWINDOWSYSTEM_WAYLAND_PLUGIN_H

#include "kwindowsystemplugininterface_p.h"

class WaylandPlugin : public KWindowSystemPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kwindowsystem.KWindowSystemPluginInterface" FILE "wayland.json")
    Q_INTERFACES(KWindowSystemPluginInterface)

public:
    explicit WaylandPlugin(QObject *parent = nullptr);
    ~WaylandPlugin() override;

    KWindowSystemPrivate *createWindowSystem() override;
};

#endif
