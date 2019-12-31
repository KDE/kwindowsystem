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

#include "waylandintegration.h"
#include "logging.h"

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/compositor.h>
#include <KWayland/Client/plasmawindowmanagement.h>
#include <KWayland/Client/plasmashell.h>
#include <KWayland/Client/surface.h>
#include <KWayland/Client/blur.h>
#include <KWayland/Client/contrast.h>
#include <KWayland/Client/region.h>
#include <KWayland/Client/slide.h>
#include <KWayland/Client/shadow.h>
#include <KWayland/Client/shm_pool.h>

#include <KWindowSystem/KWindowSystem>

#include <QGuiApplication>

class WaylandIntegrationSingleton
{
public:
    WaylandIntegration self;
};

Q_GLOBAL_STATIC(WaylandIntegrationSingleton, privateWaylandIntegrationSelf)

WaylandIntegration::WaylandIntegration()
    : QObject()
{
    setupKWaylandIntegration();
}

WaylandIntegration::~WaylandIntegration()
{}

void WaylandIntegration::setupKWaylandIntegration()
{
    using namespace KWayland::Client;
    m_waylandConnection = ConnectionThread::fromApplication(this);
    if (!m_waylandConnection) {
        qCWarning(KWAYLAND_KWS) << "Failed getting Wayland connection from QPA";
        return;
    }
    m_registry = new Registry(qApp);
    m_registry->create(m_waylandConnection);
    m_waylandCompositor = Compositor::fromApplication(this);

    m_registry->setup();
    m_waylandConnection->roundtrip();
}

WaylandIntegration *WaylandIntegration::self()
{
    return &privateWaylandIntegrationSelf()->self;
}


KWayland::Client::ConnectionThread *WaylandIntegration::waylandConnection() const
{
    return m_waylandConnection;
}

KWayland::Client::BlurManager *WaylandIntegration::waylandBlurManager()
{
    if (!m_waylandBlurManager && m_registry) {
        const KWayland::Client::Registry::AnnouncedInterface wmInterface = m_registry->interface(KWayland::Client::Registry::Interface::Blur);

        if (wmInterface.name == 0) {
            return nullptr;
        }

        m_waylandBlurManager = m_registry->createBlurManager(wmInterface.name, wmInterface.version, qApp);

        connect(m_waylandBlurManager, &KWayland::Client::BlurManager::removed, this,
            [this] () {
                m_waylandBlurManager->deleteLater();
            }
        );
    }

    return m_waylandBlurManager;
}

KWayland::Client::ContrastManager *WaylandIntegration::waylandContrastManager()
{
    if (!m_waylandContrastManager && m_registry) {
        const KWayland::Client::Registry::AnnouncedInterface wmInterface = m_registry->interface(KWayland::Client::Registry::Interface::Contrast);

        if (wmInterface.name == 0) {
            return nullptr;
        }

        m_waylandContrastManager = m_registry->createContrastManager(wmInterface.name, wmInterface.version, qApp);

        connect(m_waylandContrastManager, &KWayland::Client::ContrastManager::removed, this,
            [this] () {
                m_waylandContrastManager->deleteLater();
            }
        );
    }
    return m_waylandContrastManager;
}

KWayland::Client::SlideManager *WaylandIntegration::waylandSlideManager()
{
    if (!m_waylandSlideManager && m_registry) {
        const KWayland::Client::Registry::AnnouncedInterface wmInterface = m_registry->interface(KWayland::Client::Registry::Interface::Slide);

        if (wmInterface.name == 0) {
            return nullptr;
        }

        m_waylandSlideManager = m_registry->createSlideManager(wmInterface.name, wmInterface.version, qApp);

        connect(m_waylandSlideManager, &KWayland::Client::SlideManager::removed, this,
            [this] () {
                m_waylandSlideManager->deleteLater();
            }
        );
    }

    return m_waylandSlideManager;
}

KWayland::Client::ShadowManager *WaylandIntegration::waylandShadowManager()
{
    if (!m_waylandShadowManager && m_registry) {
        const KWayland::Client::Registry::AnnouncedInterface wmInterface = m_registry->interface(KWayland::Client::Registry::Interface::Shadow);

        if (wmInterface.name == 0) {
            qCWarning(KWAYLAND_KWS) << "This compositor does not support the Shadow interface";
            return nullptr;
        }

        m_waylandShadowManager = m_registry->createShadowManager(wmInterface.name, wmInterface.version, qApp);

        connect(m_waylandShadowManager, &KWayland::Client::ShadowManager::removed, this,
            [this] () {
                m_waylandShadowManager->deleteLater();
            }
        );
    }

    return m_waylandShadowManager;
}

KWayland::Client::Compositor *WaylandIntegration::waylandCompositor() const
{
    return m_waylandCompositor;
}

KWayland::Client::PlasmaWindowManagement *WaylandIntegration::plasmaWindowManagement()
{
    using namespace KWayland::Client;

    if (!m_wm && m_registry) {
        const Registry::AnnouncedInterface wmInterface = m_registry->interface(Registry::Interface::PlasmaWindowManagement);

        if (wmInterface.name == 0) {
            qCWarning(KWAYLAND_KWS) << "This compositor does not support the Plasma Window Management interface";
            return nullptr;
        }

        m_wm = m_registry->createPlasmaWindowManagement(wmInterface.name, wmInterface.version, qApp);
        connect(m_wm, &PlasmaWindowManagement::windowCreated, this,
            [this] (PlasmaWindow *w) {
                emit KWindowSystem::self()->windowAdded(w->internalId());
                emit KWindowSystem::self()->stackingOrderChanged();
                connect(w, &PlasmaWindow::unmapped, this,
                    [w] {
                        emit KWindowSystem::self()->windowRemoved(w->internalId());
                        emit KWindowSystem::self()->stackingOrderChanged();
                    }
                );
            }
        );
        connect(m_wm, &PlasmaWindowManagement::activeWindowChanged, this,
            [this] {
                if (PlasmaWindow *w = m_wm->activeWindow()) {
                    emit KWindowSystem::self()->activeWindowChanged(w->internalId());
                } else {
                    emit KWindowSystem::self()->activeWindowChanged(0);
                }
            }
        );
        connect(m_wm, &PlasmaWindowManagement::showingDesktopChanged, KWindowSystem::self(), &KWindowSystem::showingDesktopChanged);
        qCDebug(KWAYLAND_KWS) << "Plasma Window Management interface bound";

        connect(m_wm, &KWayland::Client::PlasmaWindowManagement::removed, this,
            [this] () {
                m_wm->deleteLater();
            }
        );
    }

    return m_wm;
}

KWayland::Client::PlasmaShell *WaylandIntegration::waylandPlasmaShell()
{
    if (!m_waylandPlasmaShell && m_registry) {
        const KWayland::Client::Registry::AnnouncedInterface wmInterface = m_registry->interface(KWayland::Client::Registry::Interface::PlasmaShell);

        if (wmInterface.name == 0) {
            return nullptr;
        }

        m_waylandPlasmaShell = m_registry->createPlasmaShell(wmInterface.name, wmInterface.version, qApp);
    }
    return m_waylandPlasmaShell;
}

KWayland::Client::ShmPool *WaylandIntegration::waylandShmPool()
{
    if (!m_waylandShmPool && m_registry) {
        const KWayland::Client::Registry::AnnouncedInterface wmInterface = m_registry->interface(KWayland::Client::Registry::Interface::Shm);

        if (wmInterface.name == 0) {
            return nullptr;
        }

        m_waylandShmPool = m_registry->createShmPool(wmInterface.name, wmInterface.version, qApp);

        connect(m_waylandShmPool, &KWayland::Client::ShmPool::removed, this,
            [this] () {
                m_waylandShmPool->deleteLater();
            }
        );
    }

    return m_waylandShmPool;
}
