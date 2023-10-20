/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "waylandintegration.h"
#include "logging.h"

#include <KWindowSystem>

#include "waylandxdgactivationv1_p.h"
#include <QGuiApplication>

class WaylandIntegrationSingleton
{
public:
    WaylandIntegration self;
};

Q_GLOBAL_STATIC(WaylandIntegrationSingleton, privateWaylandIntegrationSelf)

WaylandIntegration::WaylandIntegration()
    : QObject()
    , m_activation(new WaylandXdgActivationV1)
{
}

WaylandIntegration::~WaylandIntegration()
{
}

WaylandIntegration *WaylandIntegration::self()
{
    return &privateWaylandIntegrationSelf()->self;
}

WaylandXdgActivationV1 *WaylandIntegration::activation()
{
    return m_activation.get();
}
