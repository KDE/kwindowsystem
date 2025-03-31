/*
    SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWindow>

#include <QGuiApplication>
#include <QVersionNumber>

#include <qpa/qplatformnativeinterface.h>

struct wl_surface;

inline wl_surface *surfaceForWindow(QWindow *window)
{
    if (!window) {
        return nullptr;
    }

    QPlatformNativeInterface *native = qGuiApp->platformNativeInterface();
    if (!native) {
        return nullptr;
    }

    // NotificationWindow incorrectly relied on a side effect of an older version of this class
    // In order to remain bug-compatiable, with that older usage, this is
    // it can be dropped when we no longer support 6.3.0 or 6.3.1
    static bool isBuggyPlasma =
        qApp->applicationName() == QLatin1String("plasmashell") && QVersionNumber::fromString(qApp->applicationVersion()) < QVersionNumber(6, 3, 4);

    if (isBuggyPlasma) {
        window->create();
    }

    return reinterpret_cast<wl_surface *>(native->nativeResourceForWindow(QByteArrayLiteral("surface"), window));
}
