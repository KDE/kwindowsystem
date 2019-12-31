/*
 * Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
 * Copyright 2015 Marco Martin <mart@kde.org>
 * Copyright 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef WAYLANDINTEGRATION_H
#define WAYLANDINTEGRATION_H
#include <KWindowSystem/private/kwindoweffects_p.h>

#include <QPointer>

namespace KWayland
{
    namespace Client
    {
        class BlurManager;
        class ContrastManager;
        class Compositor;
        class ConnectionThread;
        class PlasmaWindowManagement;
        class PlasmaShell;
        class Registry;
        class ShadowManager;
        class ShmPool;
        class SlideManager;
    }
}

class WaylandIntegration : public QObject
{
public:
    explicit WaylandIntegration();
    ~WaylandIntegration();
    void setupKWaylandIntegration();

    static WaylandIntegration *self();

    KWayland::Client::ConnectionThread *waylandConnection() const;
    KWayland::Client::BlurManager *waylandBlurManager();
    KWayland::Client::ContrastManager *waylandContrastManager();
    KWayland::Client::SlideManager *waylandSlideManager();
    KWayland::Client::ShadowManager *waylandShadowManager();
    KWayland::Client::Compositor *waylandCompositor() const;
    KWayland::Client::PlasmaWindowManagement *plasmaWindowManagement();
    KWayland::Client::PlasmaShell *waylandPlasmaShell();
    KWayland::Client::ShmPool *waylandShmPool();

private:
    QPointer<KWayland::Client::ConnectionThread> m_waylandConnection;
    QPointer<KWayland::Client::Compositor> m_waylandCompositor;
    QPointer<KWayland::Client::Registry> m_registry;
    QPointer<KWayland::Client::BlurManager> m_waylandBlurManager;
    QPointer<KWayland::Client::ContrastManager> m_waylandContrastManager;
    QPointer<KWayland::Client::SlideManager> m_waylandSlideManager;
    QPointer<KWayland::Client::ShadowManager> m_waylandShadowManager;
    QPointer<KWayland::Client::PlasmaWindowManagement> m_wm;
    QPointer<KWayland::Client::PlasmaShell> m_waylandPlasmaShell;
    QPointer<KWayland::Client::ShmPool> m_waylandShmPool;
};

#endif
