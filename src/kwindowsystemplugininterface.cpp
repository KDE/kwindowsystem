/*
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kwindowsystemplugininterface_p.h"

KWindowSystemPluginInterface::KWindowSystemPluginInterface(QObject *parent)
    : QObject(parent)
{
}

KWindowSystemPluginInterface::~KWindowSystemPluginInterface()
{
}

KWindowEffectsPrivate *KWindowSystemPluginInterface::createEffects()
{
    return nullptr;
}

KWindowSystemPrivate *KWindowSystemPluginInterface::createWindowSystem()
{
    return nullptr;
}

KWindowInfoPrivate *KWindowSystemPluginInterface::createWindowInfo(WId window, NET::Properties properties, NET::Properties2 properties2)
{
    Q_UNUSED(window)
    Q_UNUSED(properties)
    Q_UNUSED(properties2)
    return nullptr;
}

KWindowShadowPrivate *KWindowSystemPluginInterface::createWindowShadow()
{
    return nullptr;
}

KWindowShadowTilePrivate *KWindowSystemPluginInterface::createWindowShadowTile()
{
    return nullptr;
}
