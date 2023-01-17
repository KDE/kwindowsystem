/*
    SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWindow>

#include <QGuiApplication>
#include <private/qwaylandwindow_p.h>
#include <qpa/qplatformnativeinterface.h>

wl_surface *surfaceForWindow(QWindow *window)
{
    if (!window) {
        return nullptr;
    }

    QPlatformNativeInterface *native = qGuiApp->platformNativeInterface();
    if (!native) {
        return nullptr;
    }
    window->create();
    return reinterpret_cast<wl_surface *>(native->nativeResourceForWindow(QByteArrayLiteral("surface"), window));
}
