/*
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kwindowsystemplugin.h"

#include <QQmlEngine>

#include <KWindowSystem>

#include "config-kwindowsystem.h"

#if KWINDOWSYSTEM_HAVE_X11
#include <KX11Extras>
#endif

void KWindowSystemPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QString::fromLatin1(uri) == QLatin1String("org.kde.kwindowsystem"));

#if KWINDOWSYSTEM_HAVE_X11
    qmlRegisterSingletonType<KX11Extras>("org.kde.kwindowsystem", 1, 0, "KX11Extras", [](QQmlEngine *, QJSEngine *) -> QObject * {
        QQmlEngine::setObjectOwnership(KX11Extras::self(), QQmlEngine::CppOwnership);
        return KX11Extras::self();
    });
#endif
    qmlRegisterSingletonType<KWindowSystem>("org.kde.kwindowsystem", 1, 0, "KWindowSystem", [](QQmlEngine *, QJSEngine *) -> QObject * {
        QQmlEngine::setObjectOwnership(KWindowSystem::self(), QQmlEngine::CppOwnership);
        return KWindowSystem::self();
    });
}
