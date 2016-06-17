/*
 * Copyright 2015 Martin Gräßlin <mgraesslin@kde.org>
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
#include "windowsystem.h"
#include "waylandintegration.h"
#include "logging.h"

#include <KWindowSystem/KWindowSystem>

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/plasmawindowmanagement.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/plasmashell.h>
#include <KWayland/Client/surface.h>

#include <QPixmap>
#include <QPoint>
#include <QString>

using namespace KWayland::Client;

WindowSystem::WindowSystem()
    : QObject()
    , KWindowSystemPrivate()
{
}

KWayland::Client::PlasmaWindow *WindowSystem::window(WId wid) const
{
    if (!WaylandIntegration::self()->plasmaWindowManagement()) {
        return nullptr;
    }

    const auto &windows = WaylandIntegration::self()->plasmaWindowManagement()->windows();
    auto it = std::find_if(windows.begin(), windows.end(), [wid] (PlasmaWindow *w) { return w->internalId() == wid; } );
    if (it != windows.end()) {
        return *it;
    }
    return nullptr;
}

void WindowSystem::activateWindow(WId win, long int time)
{
    Q_UNUSED(time)
    if (PlasmaWindow *w = window(win)) {
        w->requestActivate();
    }
}

void WindowSystem::forceActiveWindow(WId win, long int time)
{
    Q_UNUSED(time)
    if (PlasmaWindow *w = window(win)) {
        w->requestActivate();
    }
}

WId WindowSystem::activeWindow()
{
    KWayland::Client::PlasmaWindowManagement *wm = WaylandIntegration::self()->plasmaWindowManagement();
    if (wm && wm->activeWindow()) {
        return wm->activeWindow()->internalId();
    }
    return 0;
}

bool WindowSystem::allowedActionsSupported()
{
    return false;
}

void WindowSystem::allowExternalProcessWindowActivation(int pid)
{
    Q_UNUSED(pid)
}

bool WindowSystem::compositingActive()
{
    // wayland is always composited
    return true;
}

void WindowSystem::connectNotify(const QMetaMethod &signal)
{
    Q_UNUSED(signal)
}

QPoint WindowSystem::constrainViewportRelativePosition(const QPoint &pos)
{
    Q_UNUSED(pos)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support viewport positions";
    return QPoint();
}

int WindowSystem::currentDesktop()
{
    qCDebug(KWAYLAND_KWS) << "This plugin does not support virtual desktops";
    return 0;
}

void WindowSystem::demandAttention(WId win, bool set)
{
    Q_UNUSED(win)
    Q_UNUSED(set)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support demanding attention";
}

QString WindowSystem::desktopName(int desktop)
{
    Q_UNUSED(desktop)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support virtual desktops";
    return QString();
}

QPoint WindowSystem::desktopToViewport(int desktop, bool absolute)
{
    Q_UNUSED(desktop)
    Q_UNUSED(absolute)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support viewport positions";
    return QPoint();
}

WId WindowSystem::groupLeader(WId window)
{
    Q_UNUSED(window)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support group leader";
    return 0;
}

bool WindowSystem::icccmCompliantMappingState()
{
    return false;
}

QPixmap WindowSystem::icon(WId win, int width, int height, bool scale, int flags)
{
    Q_UNUSED(scale)
    Q_UNUSED(flags)
    if (PlasmaWindow *w = window(win)) {
        return w->icon().pixmap(width, height);
    }
    return QPixmap();
}

void WindowSystem::lowerWindow(WId win)
{
    Q_UNUSED(win)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support lower window";
}

bool WindowSystem::mapViewport()
{
    qCDebug(KWAYLAND_KWS) << "This plugin does not support viewport positions";
    return false;
}

void WindowSystem::minimizeWindow(WId win)
{
    if (PlasmaWindow *w = window(win)) {
        if (!w->isMinimized()) {
            w->requestToggleMinimized();
        }
    }
}

void WindowSystem::unminimizeWindow(WId win)
{
    if (PlasmaWindow *w = window(win)) {
        if (w->isMinimized()) {
            w->requestToggleMinimized();
        }
    }
}

int WindowSystem::numberOfDesktops()
{
    qCDebug(KWAYLAND_KWS) << "This plugin does not support virtual desktops";
    return 1;
}

void WindowSystem::raiseWindow(WId win)
{
    Q_UNUSED(win)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support raising windows";
}

QString WindowSystem::readNameProperty(WId window, long unsigned int atom)
{
    Q_UNUSED(window)
    Q_UNUSED(atom)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support reading X11 properties";
    return QString();
}

void WindowSystem::setBlockingCompositing(WId window, bool active)
{
    Q_UNUSED(window)
    Q_UNUSED(active)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support blocking compositing";
}

void WindowSystem::setCurrentDesktop(int desktop)
{
    Q_UNUSED(desktop)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support virtual desktops";
}

void WindowSystem::setDesktopName(int desktop, const QString &name)
{
    Q_UNUSED(desktop)
    Q_UNUSED(name)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support virtual desktops";
}

void WindowSystem::setExtendedStrut(WId win, int left_width, int left_start, int left_end, int right_width, int right_start, int right_end, int top_width, int top_start, int top_end, int bottom_width, int bottom_start, int bottom_end)
{
    Q_UNUSED(win)
    Q_UNUSED(left_width)
    Q_UNUSED(left_start)
    Q_UNUSED(left_end)
    Q_UNUSED(right_width)
    Q_UNUSED(right_start)
    Q_UNUSED(right_end)
    Q_UNUSED(top_width)
    Q_UNUSED(top_start)
    Q_UNUSED(top_end)
    Q_UNUSED(bottom_width)
    Q_UNUSED(bottom_start)
    Q_UNUSED(bottom_end)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support window struts";
}

void WindowSystem::setStrut(WId win, int left, int right, int top, int bottom)
{
    Q_UNUSED(win)
    Q_UNUSED(left)
    Q_UNUSED(right)
    Q_UNUSED(top)
    Q_UNUSED(bottom)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support window struts";
}

void WindowSystem::setIcons(WId win, const QPixmap &icon, const QPixmap &miniIcon)
{
    Q_UNUSED(win)
    Q_UNUSED(icon)
    Q_UNUSED(miniIcon)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support setting window icons";
}

void WindowSystem::setOnActivities(WId win, const QStringList &activities)
{
    Q_UNUSED(win)
    Q_UNUSED(activities)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support activities";
}

void WindowSystem::setOnAllDesktops(WId win, bool b)
{
    Q_UNUSED(win)
    Q_UNUSED(b)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support setting window on all desktops";
}

void WindowSystem::setOnDesktop(WId win, int desktop)
{
    if (PlasmaWindow *w = window(win)) {
        w->requestVirtualDesktop(desktop - 1);
    }
}

void WindowSystem::setShowingDesktop(bool showing)
{
    if (!WaylandIntegration::self()->plasmaWindowManagement()) {
        return;
    }
    WaylandIntegration::self()->plasmaWindowManagement()->setShowingDesktop(showing);
}

void WindowSystem::clearState(WId win, NET::States state)
{
    Surface *s = Surface::fromQtWinId(win);
    if (!s) {
        return;
    }

    KWayland::Client::PlasmaShellSurface *plasmaShellSurface = 0;

    if (state & NET::SkipTaskbar) {
        if (!WaylandIntegration::self()->waylandPlasmaShell()) {
            return;
        }
        plasmaShellSurface = PlasmaShellSurface::get(s);
        if (!plasmaShellSurface) {
            plasmaShellSurface = WaylandIntegration::self()->waylandPlasmaShell()->createSurface(s, this);
        }
        if (!plasmaShellSurface) {
            return;
        }
    }

    if (state & NET::SkipTaskbar) {
        plasmaShellSurface->setSkipTaskbar(false);
    }

    if (state & NET::Max) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing Max window state";
    }
    if (state & NET::FullScreen) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing FullScreen window state";
    }
    if (state & NET::Modal) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing Modal window state";
    }
    if (state & NET::Sticky) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing Sticky window state";
    }
    if (state & NET::Shaded) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing Shaded window state";
    }
    if (state & NET::KeepAbove) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing KeepAbove window state";
    }
    if (state & NET::StaysOnTop) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing StaysOnTop window state";
    }
    if (state & NET::SkipPager) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing SkipPager window state";
    }
    if (state & NET::Hidden) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing Hidden window state";
    }
    if (state & NET::KeepBelow) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing KeepBelow window state";
    }
    if (state & NET::DemandsAttention) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing DemandsAttention window state";
    }
}

void WindowSystem::setState(WId win, NET::States state)
{
    Surface *s = Surface::fromQtWinId(win);
    if (!s) {
        return;
    }

    KWayland::Client::PlasmaShellSurface *plasmaShellSurface = 0;

    if (state & NET::SkipTaskbar) {
        if (!WaylandIntegration::self()->waylandPlasmaShell()) {
            return;
        }
        plasmaShellSurface = PlasmaShellSurface::get(s);
        if (!plasmaShellSurface) {
            plasmaShellSurface = WaylandIntegration::self()->waylandPlasmaShell()->createSurface(s, this);
        }
        if (!plasmaShellSurface) {
            return;
        }
    }

    if (state & NET::SkipTaskbar) {
        plasmaShellSurface->setSkipTaskbar(true);
    }

    if (state & NET::Max) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing Max window state";
    }
    if (state & NET::FullScreen) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing FullScreen window state";
    }
    if (state & NET::Modal) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing Modal window state";
    }
    if (state & NET::Sticky) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing Sticky window state";
    }
    if (state & NET::Shaded) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing Shaded window state";
    }
    if (state & NET::KeepAbove) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing KeepAbove window state";
    }
    if (state & NET::StaysOnTop) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing StaysOnTop window state";
    }
    if (state & NET::SkipPager) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing SkipPager window state";
    }
    if (state & NET::Hidden) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing Hidden window state";
    }
    if (state & NET::KeepBelow) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing KeepBelow window state";
    }
    if (state & NET::DemandsAttention) {
        qCDebug(KWAYLAND_KWS) << "This plugin does not support changing DemandsAttention window state";
    }
}

void WindowSystem::setType(WId win, NET::WindowType windowType)
{
    if (!WaylandIntegration::self()->waylandPlasmaShell()) {
        return;
    }
    KWayland::Client::PlasmaShellSurface::Role role;

    switch (windowType) {
    case NET::Normal:
        role = KWayland::Client::PlasmaShellSurface::Role::Normal;
        break;
    case NET::Desktop:
        role = KWayland::Client::PlasmaShellSurface::Role::Desktop;
        break;
    case NET::Dock:
        role = KWayland::Client::PlasmaShellSurface::Role::Panel;
        break;
    case NET::OnScreenDisplay:
        role = KWayland::Client::PlasmaShellSurface::Role::OnScreenDisplay;
        break;
    case NET::Notification:
        role = KWayland::Client::PlasmaShellSurface::Role::Notification;
    default:
        return;
    }
    Surface *s = Surface::fromQtWinId(win);
    if (!s) {
        return;
    }
    KWayland::Client::PlasmaShellSurface *shellSurface = WaylandIntegration::self()->waylandPlasmaShell()->createSurface(s, this);

    shellSurface->setRole(role);
}

void WindowSystem::setUserTime(WId win, long int time)
{
    Q_UNUSED(win)
    Q_UNUSED(time)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support setting user type";
}

bool WindowSystem::showingDesktop()
{
    if (!WaylandIntegration::self()->plasmaWindowManagement()) {
        return false;
    }
    return WaylandIntegration::self()->plasmaWindowManagement()->isShowingDesktop();
}

QList< WId > WindowSystem::stackingOrder()
{
    if (!WaylandIntegration::self()->plasmaWindowManagement()) {
        return QList<WId>();
    }

    const auto &windows = WaylandIntegration::self()->plasmaWindowManagement()->windows();
    QList<WId> ret;
    for (auto w : windows) {
        ret << w->internalId();
    }
    return ret;
}

WId WindowSystem::transientFor(WId window)
{
    Q_UNUSED(window)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support transient for windows";
    return 0;
}

int WindowSystem::viewportToDesktop(const QPoint &pos)
{
    Q_UNUSED(pos)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support viewports";
    return 0;
}

int WindowSystem::viewportWindowToDesktop(const QRect &r)
{
    Q_UNUSED(r)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support viewports";
    return 0;
}

QList< WId > WindowSystem::windows()
{
    return stackingOrder();
}

QRect WindowSystem::workArea(const QList< WId > &excludes, int desktop)
{
    Q_UNUSED(excludes)
    Q_UNUSED(desktop)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support work area";
    return QRect();
}

QRect WindowSystem::workArea(int desktop)
{
    Q_UNUSED(desktop)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support work area";
    return QRect();
}
