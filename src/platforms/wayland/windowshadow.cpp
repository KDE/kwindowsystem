/*
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

#include "windowshadow.h"
#include "waylandintegration.h"

#include <KWayland/Client/shm_pool.h>
#include <KWayland/Client/surface.h>

bool WindowShadowTile::create()
{
    KWayland::Client::ShmPool *shmPool = WaylandIntegration::self()->waylandShmPool();
    if (!shmPool) {
        return false;
    }
    buffer = shmPool->createBuffer(image);
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

bool WindowShadow::create()
{
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

void WindowShadow::destroy()
{
    if (!shadow) {
        return;
    }
    KWayland::Client::ShadowManager *shadowManager = WaylandIntegration::self()->waylandShadowManager();
    if (!shadowManager) {
        return;
    }
    KWayland::Client::Surface *surface = KWayland::Client::Surface::fromWindow(window);
    if (!surface) {
        return;
    }
    shadowManager->removeShadow(surface);
    shadow = nullptr;
    window->requestUpdate();
}
