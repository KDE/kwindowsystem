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
#include "logging.h"

#include <KWindowSystem/KWindowSystem>

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/plasmawindowmanagement.h>
#include <KWayland/Client/registry.h>

#include <QPixmap>
#include <QPoint>
#include <QString>

using namespace KWayland::Client;

WindowSystem::WindowSystem()
    : QObject()
    , KWindowSystemPrivate()
{
}

void WindowSystem::setupKWaylandIntegration()
{
    ConnectionThread *connection = ConnectionThread::fromApplication(this);
    if (!connection) {
        qCWarning(KWAYLAND_KWS) << "Failed getting Wayland connection from QPA";
        return;
    }
    Registry *registry = new Registry(this);
    registry->create(connection);
    connect(registry, &Registry::interfacesAnnounced, this,
        [this] {
            if (!m_wm) {
                qCWarning(KWAYLAND_KWS) << "This compositor does not support the Plasma Window Management interface";
            }
        }
    );
    connect(registry, &Registry::plasmaWindowManagementAnnounced, this,
        [this, registry] (quint32 name, quint32 version) {
            m_wm = registry->createPlasmaWindowManagement(name, version, this);
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
            emit KWindowSystem::self()->compositingChanged(true);
            emit KWindowSystem::self()->showingDesktopChanged(m_wm->isShowingDesktop());
            emit KWindowSystem::self()->stackingOrderChanged();
            if (PlasmaWindow *w = m_wm->activeWindow()) {
                emit KWindowSystem::self()->activeWindowChanged(w->internalId());
            }
            qCDebug(KWAYLAND_KWS) << "Plasma Window Management interface bound";
        }
    );

    registry->setup();
}

KWayland::Client::PlasmaWindow *WindowSystem::window(WId wid) const
{
    if (!m_wm) {
        return nullptr;
    }
    const auto &windows = m_wm->windows();
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
    if (m_wm && m_wm->activeWindow()) {
        return m_wm->activeWindow()->internalId();
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
    Q_UNUSED(win)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support minimize window";
}

void WindowSystem::unminimizeWindow(WId win)
{
    Q_UNUSED(win)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support minimizing windows";
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
    if (m_wm) {
        m_wm->setShowingDesktop(showing);
    }
}

void WindowSystem::clearState(WId win, NET::States state)
{
    Q_UNUSED(win)
    Q_UNUSED(state)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support changing window states";
}

void WindowSystem::setState(WId win, NET::States state)
{
    Q_UNUSED(win)
    Q_UNUSED(state)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support changing window states";
}

void WindowSystem::setType(WId win, NET::WindowType windowType)
{
    Q_UNUSED(win)
    Q_UNUSED(windowType)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support changing window types";
}

void WindowSystem::setUserTime(WId win, long int time)
{
    Q_UNUSED(win)
    Q_UNUSED(time)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support setting user type";
}

bool WindowSystem::showingDesktop()
{
    if (m_wm) {
        return m_wm->isShowingDesktop();
    }
    return false;
}

QList< WId > WindowSystem::stackingOrder()
{
    if (m_wm) {
        const auto &windows = m_wm->windows();
        QList<WId> ret;
        for (auto w : windows) {
            ret << w->internalId();
        }
        return ret;
    }
    return QList<WId>();
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
