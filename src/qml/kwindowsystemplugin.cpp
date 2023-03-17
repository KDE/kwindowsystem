/*
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kwindowsystemplugin.h"

#include <QtQml>

#include <KWindowSystem>

#include "config-kwindowsystem.h"

#if KWINDOWSYSTEM_HAVE_X11
#include <KX11Extras>
#endif

void KWindowSystemPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QString::fromLatin1(uri) == QLatin1String("org.kde.kwindowsystem"));

#if KWINDOWSYSTEM_HAVE_X11
    qmlRegisterSingletonInstance<KX11Extras>("org.kde.kwindowsystem", 1, 0, "KX11Extras", KX11Extras::self());
#endif

    qmlRegisterSingletonInstance<KWindowSystem>("org.kde.kwindowsystem", 1, 0, "KWindowSystem", KWindowSystem::self());
}
