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
#include "waylandintegration.h"

#include <QDebug>

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/plasmawindowmanagement.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/plasmashell.h>
#include <KWayland/Client/compositor.h>
#include <KWayland/Client/surface.h>
#include <KWayland/Client/blur.h>
#include <KWayland/Client/contrast.h>
#include <KWayland/Client/region.h>
#include <KWayland/Client/slide.h>

WindowEffects::WindowEffects()
    : QObject(),
      KWindowEffectsPrivate()
{
}

WindowEffects::~WindowEffects()
{}

bool WindowEffects::isEffectAvailable(KWindowEffects::Effect effect)
{
    switch (effect) {
    case KWindowEffects::BackgroundContrast:
        return WaylandIntegration::self()->waylandContrastManager() != nullptr;
    case KWindowEffects::BlurBehind:
        return WaylandIntegration::self()->waylandBlurManager() != nullptr;
    default:
        return false;
    }
}

void WindowEffects::slideWindow(WId id, KWindowEffects::SlideFromLocation location, int offset)
{
    if (!WaylandIntegration::self()->waylandSlideManager()) {
        return;
    }
    KWayland::Client::Surface *surface = KWayland::Client::Surface::fromQtWinId(id);
    if (surface) {
        if (location != KWindowEffects::SlideFromLocation::NoEdge) {
            auto slide = WaylandIntegration::self()->waylandSlideManager()->createSlide(surface, surface);

            KWayland::Client::Slide::Location convertedLoc;
            switch (location) {
            case KWindowEffects::SlideFromLocation::TopEdge:
                convertedLoc = KWayland::Client::Slide::Location::Top;
                break;
            case KWindowEffects::SlideFromLocation::LeftEdge:
                convertedLoc = KWayland::Client::Slide::Location::Left;
                break;
            case KWindowEffects::SlideFromLocation::RightEdge:
                convertedLoc = KWayland::Client::Slide::Location::Right;
                break;
            case KWindowEffects::SlideFromLocation::BottomEdge:
            default:
                convertedLoc = KWayland::Client::Slide::Location::Bottom;
                break;
            }

            slide->setLocation(convertedLoc);
            slide->setOffset(offset);
            slide->commit();
        } else {
            WaylandIntegration::self()->waylandSlideManager()->removeSlide(surface);
        }
        surface->commit(KWayland::Client::Surface::CommitFlag::None);

        WaylandIntegration::self()->waylandConnection()->flush();
    }
}

void WindowEffects::slideWindow(QWidget *widget, KWindowEffects::SlideFromLocation location)
{
    slideWindow(widget->winId(), location, 0);
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
    if (!WaylandIntegration::self()->waylandBlurManager()) {
        return;
    }
    KWayland::Client::Surface *surface = KWayland::Client::Surface::fromQtWinId(window);
    if (surface) {
        if (enable) {
            auto blur = WaylandIntegration::self()->waylandBlurManager()->createBlur(surface, surface);
            blur->setRegion(WaylandIntegration::self()->waylandCompositor()->createRegion(region, nullptr));
            blur->commit();
        } else {
            WaylandIntegration::self()->waylandBlurManager()->removeBlur(surface);
        }
        surface->commit(KWayland::Client::Surface::CommitFlag::None);

        WaylandIntegration::self()->waylandConnection()->flush();
    }
}

void WindowEffects::enableBackgroundContrast(WId window, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    if (!WaylandIntegration::self()->waylandContrastManager()) {
        return;
    }
    KWayland::Client::Surface *surface = KWayland::Client::Surface::fromQtWinId(window);
    if (surface) {
        if (enable) {
            auto backgroundContrast = WaylandIntegration::self()->waylandContrastManager()->createContrast(surface, surface);
            backgroundContrast->setRegion(WaylandIntegration::self()->waylandCompositor()->createRegion(region, nullptr));
            backgroundContrast->setContrast(contrast);
            backgroundContrast->setIntensity(intensity);
            backgroundContrast->setSaturation(saturation);
            backgroundContrast->commit();
        } else {
            WaylandIntegration::self()->waylandContrastManager()->removeContrast(surface);
        }
        surface->commit(KWayland::Client::Surface::CommitFlag::None);

        WaylandIntegration::self()->waylandConnection()->flush();
    }
}

void WindowEffects::markAsDashboard(WId window)
{
    Q_UNUSED(window)
}


