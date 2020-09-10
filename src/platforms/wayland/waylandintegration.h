/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
