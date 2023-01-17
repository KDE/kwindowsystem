/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "waylandintegration.h"
#include "logging.h"

#include <KWayland/Client/compositor.h>
#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/contrast.h>
#include <KWayland/Client/plasmashell.h>
#include <KWayland/Client/plasmawindowmanagement.h>
#include <KWayland/Client/region.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/shadow.h>
#include <KWayland/Client/shm_pool.h>
#include <KWayland/Client/slide.h>
#include <KWayland/Client/surface.h>

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
{
    setupKWaylandIntegration();
}

WaylandIntegration::~WaylandIntegration()
{
}

void WaylandIntegration::setupKWaylandIntegration()
{
    using namespace KWayland::Client;
    m_waylandConnection = ConnectionThread::fromApplication(this);
    if (!m_waylandConnection) {
        qCWarning(KWAYLAND_KWS) << "Failed getting Wayland connection from QPA";
        return;
    }
    m_registry = new Registry(qApp);
    connect(m_registry, &KWayland::Client::Registry::interfaceAnnounced, this, [this](const QByteArray &interfaceName, quint32 name, quint32 version) {
        if (interfaceName != xdg_activation_v1_interface.name)
            return;

        m_activationInterface = {name, version};
    });
    m_registry->create(m_waylandConnection);
    m_waylandCompositor = Compositor::fromApplication(this);

    m_registry->setup();
    m_waylandConnection->roundtrip();
}

WaylandIntegration *WaylandIntegration::self()
{
    return &privateWaylandIntegrationSelf()->self;
}

KWayland::Client::Registry *WaylandIntegration::registry() const
{
    return m_registry;
}

KWayland::Client::ConnectionThread *WaylandIntegration::waylandConnection() const
{
    return m_waylandConnection;
}

KWayland::Client::ContrastManager *WaylandIntegration::waylandContrastManager()
{
    if (!m_waylandContrastManager && m_registry) {
        const KWayland::Client::Registry::AnnouncedInterface wmInterface = m_registry->interface(KWayland::Client::Registry::Interface::Contrast);

        if (wmInterface.name == 0) {
            return nullptr;
        }

        m_waylandContrastManager = m_registry->createContrastManager(wmInterface.name, wmInterface.version, qApp);

        connect(m_waylandContrastManager, &KWayland::Client::ContrastManager::removed, this, [this]() {
            m_waylandContrastManager->deleteLater();
        });
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

        connect(m_waylandSlideManager, &KWayland::Client::SlideManager::removed, this, [this]() {
            m_waylandSlideManager->deleteLater();
        });
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

        connect(m_waylandShadowManager, &KWayland::Client::ShadowManager::removed, this, [this]() {
            m_waylandShadowManager->deleteLater();
        });
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
        connect(m_wm, &PlasmaWindowManagement::showingDesktopChanged, KWindowSystem::self(), &KWindowSystem::showingDesktopChanged);
        qCDebug(KWAYLAND_KWS) << "Plasma Window Management interface bound";

        connect(m_wm, &KWayland::Client::PlasmaWindowManagement::removed, this, [this]() {
            m_wm->deleteLater();
        });
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

KWayland::Client::ShmPool *WaylandIntegration::createShmPool()
{
    if (m_registry) {
        const KWayland::Client::Registry::AnnouncedInterface wmInterface = m_registry->interface(KWayland::Client::Registry::Interface::Shm);

        if (wmInterface.name == 0) {
            return nullptr;
        }

        return m_registry->createShmPool(wmInterface.name, wmInterface.version);
    }

    return nullptr;
}

WaylandXdgActivationV1 *WaylandIntegration::activation()
{
    if (!m_activation && m_registry && m_activationInterface.name) {
        m_activation = new WaylandXdgActivationV1(*m_registry, m_activationInterface.name, m_activationInterface.version);
    }
    return m_activation;
}
