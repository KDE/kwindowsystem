/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "windowshadow.h"
#include "logging.h"
#include "waylandintegration.h"

#include <KWayland/Client/shm_pool.h>
#include <KWayland/Client/surface.h>

#include <QDebug>
#include <QExposeEvent>

WindowShadowTile::WindowShadowTile() {}
WindowShadowTile::~WindowShadowTile() {}

bool WindowShadowTile::create()
{
    m_shmPool.reset(WaylandIntegration::self()->createShmPool());
    if (!m_shmPool) {
        return false;
    }
    buffer = m_shmPool->createBuffer(image);
    return true;
}

void WindowShadowTile::destroy()
{
    buffer = nullptr;
}

WindowShadowTile *WindowShadowTile::get(const KWindowShadowTile *tile)
{
    KWindowShadowTilePrivate *d = KWindowShadowTilePrivate::get(tile);
    return static_cast<WindowShadowTile *>(d);
}

static KWayland::Client::Buffer::Ptr bufferForTile(const KWindowShadowTile::Ptr &tile)
{
    if (!tile) {
        return KWayland::Client::Buffer::Ptr();
    }
    WindowShadowTile *d = WindowShadowTile::get(tile.data());
    return d->buffer;
}

bool WindowShadow::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)
    if (event->type() == QEvent::Expose) {
        QExposeEvent *exposeEvent = static_cast<QExposeEvent *>(event);
        if (!exposeEvent->region().isNull()) {
            if (!internalCreate()) {
                qCWarning(KWAYLAND_KWS) << "Failed to recreate shadow for" << window;
            }
        }
    } else if (event->type() == QEvent::Hide) {
        internalDestroy();
    }
    return false;
}

bool WindowShadow::internalCreate()
{
    if (shadow) {
        return true;
    }
    KWayland::Client::ShadowManager *shadowManager = WaylandIntegration::self()->waylandShadowManager();
    if (!shadowManager) {
        return false;
    }
    KWayland::Client::Surface *surface = KWayland::Client::Surface::fromWindow(window);
    if (!surface) {
        return false;
    }

    shadow = shadowManager->createShadow(surface, surface);
    shadow->attachLeft(bufferForTile(leftTile));
    shadow->attachTopLeft(bufferForTile(topLeftTile));
    shadow->attachTop(bufferForTile(topTile));
    shadow->attachTopRight(bufferForTile(topRightTile));
    shadow->attachRight(bufferForTile(rightTile));
    shadow->attachBottomRight(bufferForTile(bottomRightTile));
    shadow->attachBottom(bufferForTile(bottomTile));
    shadow->attachBottomLeft(bufferForTile(bottomLeftTile));
    shadow->setOffsets(padding);
    shadow->commit();

    // Commit wl_surface at the next available time.
    window->requestUpdate();

    return true;
}

bool WindowShadow::create()
{
    if (!internalCreate()) {
        return false;
    }
    window->installEventFilter(this);
    return true;
}

void WindowShadow::internalDestroy()
{
    delete shadow;
    shadow = nullptr;
    if (window) {
        window->requestUpdate();
    }
}

void WindowShadow::destroy()
{
    if (window) {
        window->removeEventFilter(this);
    }
    internalDestroy();
}
