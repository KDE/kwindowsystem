/*
 *   Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "kwindowsystem.h"
#include "kwindowsystem_p.h"

#include <config-kwindowsystem.h>

#if KWINDOWSYSTEM_HAVE_X11
#include "kwindowsystem_p_x11.h"
#endif

#include <QGuiApplication>
#include <QMetaMethod>
#include <QPixmap>
#include <QWidget>
#include <QWindow>

class KWindowSystemStaticContainer
{
public:
    KWindowSystemStaticContainer() {
#if KWINDOWSYSTEM_HAVE_X11
        if (d.isNull() && (QGuiApplication::platformName() == QStringLiteral("xcb"))) {
            d.reset(new KWindowSystemPrivateX11());
        }
#endif
        if (d.isNull()) {
            d.reset(new KWindowSystemPrivateDummy());
        }

        kwm.moveToThread(QCoreApplication::instance()->thread());
    }
    KWindowSystem kwm;
    QScopedPointer<KWindowSystemPrivate> d;
};

Q_GLOBAL_STATIC(KWindowSystemStaticContainer, g_kwmInstanceContainer)

KWindowSystemPrivate::~KWindowSystemPrivate()
{
}

QList<WId> KWindowSystemPrivateDummy::windows()
{
    return QList<WId>();
}

QList<WId> KWindowSystemPrivateDummy::stackingOrder()
{
    return QList<WId>();
}

WId KWindowSystemPrivateDummy::activeWindow()
{
    return 0;
}

void KWindowSystemPrivateDummy::activateWindow(WId win, long time)
{
    Q_UNUSED(win)
    Q_UNUSED(time)
}

void KWindowSystemPrivateDummy::forceActiveWindow(WId win, long time)
{
    Q_UNUSED(win)
    Q_UNUSED(time)
}

void KWindowSystemPrivateDummy::demandAttention(WId win, bool set)
{
    Q_UNUSED(win)
    Q_UNUSED(set)
}

bool KWindowSystemPrivateDummy::compositingActive()
{
    return false;
}

int KWindowSystemPrivateDummy::currentDesktop()
{
    return 0;
}

int KWindowSystemPrivateDummy::numberOfDesktops()
{
    return 0;
}

void KWindowSystemPrivateDummy::setCurrentDesktop(int desktop)
{
    Q_UNUSED(desktop)
}

void KWindowSystemPrivateDummy::setOnAllDesktops(WId win, bool b)
{
    Q_UNUSED(win)
    Q_UNUSED(b)
}

void KWindowSystemPrivateDummy::setOnDesktop(WId win, int desktop)
{
    Q_UNUSED(win)
    Q_UNUSED(desktop)
}

void KWindowSystemPrivateDummy::setOnActivities(WId win, const QStringList &activities)
{
    Q_UNUSED(win)
    Q_UNUSED(activities)
}

#ifndef KWINDOWSYSTEM_NO_DEPRECATED
WId KWindowSystemPrivateDummy::transientFor(WId window)
{
    Q_UNUSED(window)
    return 0;
}

WId KWindowSystemPrivateDummy::groupLeader(WId window)
{
    Q_UNUSED(window)
    return 0;
}
#endif

QPixmap KWindowSystemPrivateDummy::icon(WId win, int width, int height, bool scale, int flags)
{
    Q_UNUSED(win)
    Q_UNUSED(width)
    Q_UNUSED(height)
    Q_UNUSED(scale)
    Q_UNUSED(flags)
    return QPixmap();
}

void KWindowSystemPrivateDummy::setIcons(WId win, const QPixmap &icon, const QPixmap &miniIcon)
{
    Q_UNUSED(win)
    Q_UNUSED(icon)
    Q_UNUSED(miniIcon)
}

void KWindowSystemPrivateDummy::setType(WId win, NET::WindowType windowType)
{
    Q_UNUSED(win)
    Q_UNUSED(windowType)
}

void KWindowSystemPrivateDummy::setState(WId win, NET::States state)
{
    Q_UNUSED(win)
    Q_UNUSED(state)
}

void KWindowSystemPrivateDummy::clearState(WId win, NET::States state)
{
    Q_UNUSED(win)
    Q_UNUSED(state)
}

void KWindowSystemPrivateDummy::minimizeWindow(WId win)
{
    Q_UNUSED(win)
}

void KWindowSystemPrivateDummy::unminimizeWindow(WId win)
{
    Q_UNUSED(win)
}

void KWindowSystemPrivateDummy::raiseWindow(WId win)
{
    Q_UNUSED(win)
}

void KWindowSystemPrivateDummy::lowerWindow(WId win)
{
    Q_UNUSED(win)
}

bool KWindowSystemPrivateDummy::icccmCompliantMappingState()
{
    return false;
}

QRect KWindowSystemPrivateDummy::workArea(int desktop)
{
    Q_UNUSED(desktop)
    return QRect();
}

QRect KWindowSystemPrivateDummy::workArea(const QList<WId> &excludes, int desktop)
{
    Q_UNUSED(excludes)
    Q_UNUSED(desktop)
    return QRect();
}

QString KWindowSystemPrivateDummy::desktopName(int desktop)
{
    Q_UNUSED(desktop)
    return QString();
}

void KWindowSystemPrivateDummy::setDesktopName(int desktop, const QString &name)
{
    Q_UNUSED(desktop)
    Q_UNUSED(name)
}

bool KWindowSystemPrivateDummy::showingDesktop()
{
    return false;
}

void KWindowSystemPrivateDummy::setUserTime(WId win, long time)
{
    Q_UNUSED(win)
    Q_UNUSED(time)
}

void KWindowSystemPrivateDummy::setExtendedStrut(WId win, int left_width, int left_start, int left_end,
                                                 int right_width, int right_start, int right_end, int top_width, int top_start, int top_end,
                                                 int bottom_width, int bottom_start, int bottom_end)
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
}

void KWindowSystemPrivateDummy::setStrut(WId win, int left, int right, int top, int bottom)
{
    Q_UNUSED(win)
    Q_UNUSED(left)
    Q_UNUSED(right)
    Q_UNUSED(top)
    Q_UNUSED(bottom)
}

bool KWindowSystemPrivateDummy::allowedActionsSupported()
{
    return false;
}

QString KWindowSystemPrivateDummy::readNameProperty(WId window, unsigned long atom)
{
    Q_UNUSED(window)
    Q_UNUSED(atom)
    return QString();
}

void KWindowSystemPrivateDummy::allowExternalProcessWindowActivation(int pid)
{
    Q_UNUSED(pid)
}

void KWindowSystemPrivateDummy::setBlockingCompositing(WId window, bool active)
{
    Q_UNUSED(window)
    Q_UNUSED(active)
}

bool KWindowSystemPrivateDummy::mapViewport()
{
    return false;
}

int KWindowSystemPrivateDummy::viewportToDesktop(const QPoint &pos)
{
    Q_UNUSED(pos)
    return 0;
}

int KWindowSystemPrivateDummy::viewportWindowToDesktop(const QRect &r)
{
    Q_UNUSED(r)
    return 0;
}

QPoint KWindowSystemPrivateDummy::desktopToViewport(int desktop, bool absolute)
{
    Q_UNUSED(desktop)
    Q_UNUSED(absolute)
    return QPoint();
}

QPoint KWindowSystemPrivateDummy::constrainViewportRelativePosition(const QPoint &pos)
{
    Q_UNUSED(pos)
    return QPoint();
}

void KWindowSystemPrivateDummy::connectNotify(const QMetaMethod &signal)
{
    Q_UNUSED(signal)
}

KWindowSystem *KWindowSystem::self()
{
    return &(g_kwmInstanceContainer()->kwm);
}

KWindowSystemPrivate *KWindowSystem::d_func()
{
    return g_kwmInstanceContainer()->d.data();
}

void KWindowSystem::connectNotify(const QMetaMethod &signal)
{
    Q_D(KWindowSystem);
    d->connectNotify(signal);
    QObject::connectNotify(signal);
}

QList<WId> KWindowSystem::windows()
{
    Q_D(KWindowSystem);
    return d->windows();
}

#ifndef KWINDOWSYSTEM_NO_DEPRECATED
KWindowInfo KWindowSystem::windowInfo(WId win, NET::Properties properties, NET::Properties2 properties2)
{
    return KWindowInfo(win, properties, properties2);
}
#endif

bool KWindowSystem::hasWId(WId w)
{
    return windows().contains(w);
}

QList<WId> KWindowSystem::stackingOrder()
{
    Q_D(KWindowSystem);
    return d->stackingOrder();
}

int KWindowSystem::currentDesktop()
{
    Q_D(KWindowSystem);
    return d->currentDesktop();
}

int KWindowSystem::numberOfDesktops()
{
    Q_D(KWindowSystem);
    return d->numberOfDesktops();
}

void KWindowSystem::setCurrentDesktop(int desktop)
{
    Q_D(KWindowSystem);
    d->setCurrentDesktop(desktop);
}

void KWindowSystem::setOnAllDesktops(WId win, bool b)
{
    Q_D(KWindowSystem);
    d->setOnAllDesktops(win, b);
}

void KWindowSystem::setOnDesktop(WId win, int desktop)
{
    Q_D(KWindowSystem);
    d->setOnDesktop(win, desktop);
}

void KWindowSystem::setOnActivities(WId win, const QStringList &activities)
{
    Q_D(KWindowSystem);
    d->setOnActivities(win, activities);
}

WId KWindowSystem::activeWindow()
{
    Q_D(KWindowSystem);
    return d->activeWindow();
}

void KWindowSystem::activateWindow(WId win, long time)
{
    Q_D(KWindowSystem);
    d->activateWindow(win, time);
}

void KWindowSystem::forceActiveWindow(WId win, long time)
{
    Q_D(KWindowSystem);
    d->forceActiveWindow(win, time);
}

void KWindowSystem::demandAttention(WId win, bool set)
{
    Q_D(KWindowSystem);
    d->demandAttention(win, set);
}

#ifndef KWINDOWSYSTEM_NO_DEPRECATED
WId KWindowSystem::transientFor(WId win)
{
    Q_D(KWindowSystem);
    return d->transientFor(win);
}
#endif

void KWindowSystem::setMainWindow(QWidget *subWidget, WId mainWindowId)
{
    // Set the WA_NativeWindow attribute to force the creation of the QWindow.
    // Without this QWidget::windowHandle() returns 0.
    subWidget->setAttribute(Qt::WA_NativeWindow, true);
    QWindow *subWindow = subWidget->windowHandle();
    Q_ASSERT(subWindow);

    QWindow *mainWindow = QWindow::fromWinId(mainWindowId);
    // mainWindow is not the child of any object, so make sure it gets deleted at some point
    connect(subWidget, &QObject::destroyed, mainWindow, &QObject::deleteLater);
    subWindow->setTransientParent(mainWindow);
}

#ifndef KWINDOWSYSTEM_NO_DEPRECATED
WId KWindowSystem::groupLeader(WId win)
{
    Q_D(KWindowSystem);
    return d->groupLeader(win);
}
#endif

QPixmap KWindowSystem::icon(WId win, int width, int height, bool scale)
{
    return icon(win, width, height, scale, NETWM | WMHints | ClassHint | XApp);
}

QPixmap KWindowSystem::icon(WId win, int width, int height, bool scale, int flags)
{
    Q_D(KWindowSystem);
    return d->icon(win, width, height, scale, flags);
}

void KWindowSystem::setIcons(WId win, const QPixmap &icon, const QPixmap &miniIcon)
{
    Q_D(KWindowSystem);
    d->setIcons(win, icon, miniIcon);
}

void KWindowSystem::setType(WId win, NET::WindowType windowType)
{
    Q_D(KWindowSystem);
    d->setType(win, windowType);
}

void KWindowSystem::setState(WId win, NET::States state)
{
    Q_D(KWindowSystem);
    d->setState(win, state);
}

void KWindowSystem::clearState(WId win, NET::States state)
{
    Q_D(KWindowSystem);
    d->clearState(win, state);
}

void KWindowSystem::minimizeWindow(WId win)
{
    Q_D(KWindowSystem);
    d->minimizeWindow(win);
}

void KWindowSystem::unminimizeWindow(WId win)
{
    Q_D(KWindowSystem);
    d->unminimizeWindow(win);
}

#ifndef KWINDOWSYSTEM_NO_DEPRECATED
void KWindowSystem::minimizeWindow(WId win, bool animation)
{
    Q_UNUSED(animation)
    minimizeWindow(win);
}
#endif

#ifndef KWINDOWSYSTEM_NO_DEPRECATED
void KWindowSystem::unminimizeWindow(WId win, bool animation)
{
    Q_UNUSED(animation)
    unminimizeWindow(win);
}
#endif

void KWindowSystem::raiseWindow(WId win)
{
    Q_D(KWindowSystem);
    d->raiseWindow(win);
}

void KWindowSystem::lowerWindow(WId win)
{
    Q_D(KWindowSystem);
    d->lowerWindow(win);
}

bool KWindowSystem::compositingActive()
{
    Q_D(KWindowSystem);
    return d->compositingActive();
}

QRect KWindowSystem::workArea(int desktop)
{
    Q_D(KWindowSystem);
    return d->workArea(desktop);
}

QRect KWindowSystem::workArea(const QList<WId> &exclude, int desktop)
{
    Q_D(KWindowSystem);
    return d->workArea(exclude, desktop);
}

QString KWindowSystem::desktopName(int desktop)
{
    Q_D(KWindowSystem);
    return d->desktopName(desktop);
}

void KWindowSystem::setDesktopName(int desktop, const QString &name)
{
    Q_D(KWindowSystem);
    d->setDesktopName(desktop, name);
}

bool KWindowSystem::showingDesktop()
{
    Q_D(KWindowSystem);
    return d->showingDesktop();
}

void KWindowSystem::setUserTime(WId win, long time)
{
    Q_D(KWindowSystem);
    d->setUserTime(win, time);
}

void KWindowSystem::setExtendedStrut(WId win, int left_width, int left_start, int left_end,
                                     int right_width, int right_start, int right_end, int top_width, int top_start, int top_end,
                                     int bottom_width, int bottom_start, int bottom_end)
{
    Q_D(KWindowSystem);
    d->setExtendedStrut(win, left_width, left_start, left_end,
                        right_width, right_start, right_end, top_width, top_start, top_end,
                        bottom_width, bottom_start, bottom_end);
}

void KWindowSystem::setStrut(WId win, int left, int right, int top, int bottom)
{
    Q_D(KWindowSystem);
    d->setStrut(win, left, right, top, bottom);
}

bool KWindowSystem::icccmCompliantMappingState()
{
    Q_D(KWindowSystem);
    return d->icccmCompliantMappingState();
}

bool KWindowSystem::allowedActionsSupported()
{
    Q_D(KWindowSystem);
    return d->allowedActionsSupported();
}

QString KWindowSystem::readNameProperty(WId win, unsigned long atom)
{
    Q_D(KWindowSystem);
    return d->readNameProperty(win, atom);
}

void KWindowSystem::allowExternalProcessWindowActivation(int pid)
{
    Q_D(KWindowSystem);
    d->allowExternalProcessWindowActivation(pid);
}

void KWindowSystem::setBlockingCompositing(WId window, bool active)
{
    Q_D(KWindowSystem);
    d->setBlockingCompositing(window, active);
}

bool KWindowSystem::mapViewport()
{
    Q_D(KWindowSystem);
    return d->mapViewport();
}

int KWindowSystem::viewportToDesktop(const QPoint &p)
{
    Q_D(KWindowSystem);
    return d->viewportToDesktop(p);
}

int KWindowSystem::viewportWindowToDesktop(const QRect &r)
{
    Q_D(KWindowSystem);
    return d->viewportWindowToDesktop(r);
}

QPoint KWindowSystem::desktopToViewport(int desktop, bool absolute)
{
    Q_D(KWindowSystem);
    return d->desktopToViewport(desktop, absolute);
}

QPoint KWindowSystem::constrainViewportRelativePosition(const QPoint &pos)
{
    Q_D(KWindowSystem);
    return d->constrainViewportRelativePosition(pos);
}
