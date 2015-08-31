/*
 * Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
 * Copyright 2015 Marco Martin <mart@kde.org>
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

#include "windoweffects.h"

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/compositor.h>
#include <KWayland/Client/surface.h>
#include <KWayland/Client/blur.h>
#include <KWayland/Client/region.h>

WindowEffects::WindowEffects()
    : QObject(),
      KWindowEffectsPrivate(),
      m_waylandConnection(nullptr),
      m_waylandBlurManager(nullptr),
      m_waylandCompositor(nullptr)
{
    setupKWaylandIntegration();
}

WindowEffects::~WindowEffects()
{}

void WindowEffects::setupKWaylandIntegration()
{
    using namespace KWayland::Client;
    m_waylandConnection = ConnectionThread::fromApplication(this);
    if (!m_waylandConnection) {
        return;
    }
    Registry *registry = new Registry(this);
    registry->create(m_waylandConnection);
    connect(registry, &Registry::compositorAnnounced, this,
        [this, registry] (quint32 name, quint32 version) {
            m_waylandCompositor = registry->createCompositor(name, version, this);
        }
    );
    connect(registry, &Registry::blurAnnounced, this,
        [this, registry] (quint32 name, quint32 version) {
            m_waylandBlurManager = registry->createBlurManager(name, version, this);

            connect(m_waylandBlurManager, &BlurManager::removed, this,
                [this] () {
                    m_waylandBlurManager->deleteLater();
                    m_waylandBlurManager = nullptr;
                }
            );
        }
    );

    registry->setup();
    m_waylandConnection->roundtrip();
}

bool WindowEffects::isEffectAvailable(KWindowEffects::Effect effect)
{
    switch (effect) {
    //TODO: implement BackgroundContrast, using blur instead for now
    case KWindowEffects::BackgroundContrast:
        return m_waylandBlurManager != nullptr;
    case KWindowEffects::BlurBehind:
        return m_waylandBlurManager != nullptr;
    default:
        return false;
    }
}

void WindowEffects::slideWindow(WId id, KWindowEffects::SlideFromLocation location, int offset)
{
    Q_UNUSED(id)
    Q_UNUSED(location)
    Q_UNUSED(offset)
}

void WindowEffects::slideWindow(QWidget *widget, KWindowEffects::SlideFromLocation location)
{
    Q_UNUSED(widget)
    Q_UNUSED(location)
}

QList<QSize> WindowEffects::windowSizes(const QList<WId> &ids)
{
    Q_UNUSED(ids)
    QList<QSize> sizes;
    return sizes;
}

void WindowEffects::presentWindows(WId controller, const QList<WId> &ids)
{
    Q_UNUSED(controller)
    Q_UNUSED(ids)
}

void WindowEffects::presentWindows(WId controller, int desktop)
{
    Q_UNUSED(controller)
    Q_UNUSED(desktop)
}

void WindowEffects::highlightWindows(WId controller, const QList<WId> &ids)
{
    Q_UNUSED(controller)
    Q_UNUSED(ids)
}

void WindowEffects::enableBlurBehind(WId window, bool enable, const QRegion &region)
{
    if (!m_waylandBlurManager) {
        return;
    }
    KWayland::Client::Surface *surface = KWayland::Client::Surface::fromQtWinId(window);
    if (surface) {
        if (enable) {
            auto blur = m_waylandBlurManager->createBlur(surface, surface);
            blur->setRegion(m_waylandCompositor->createRegion(region, nullptr));
            blur->commit();
        } else {
            m_waylandBlurManager->removeBlur(surface);
        }
        surface->commit(KWayland::Client::Surface::CommitFlag::None);

        m_waylandConnection->flush();
    }
}

void WindowEffects::enableBackgroundContrast(WId window, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    Q_UNUSED(window)
    Q_UNUSED(enable)
    Q_UNUSED(contrast)
    Q_UNUSED(intensity)
    Q_UNUSED(saturation)
    Q_UNUSED(region)
}

void WindowEffects::markAsDashboard(WId window)
{
    Q_UNUSED(window)
}


