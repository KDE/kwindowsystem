/*
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "windowsystem.h"
#include "logging.h"
#include "surfacehelper.h"
#include "waylandxdgactivationv1_p.h"

#include <KWaylandExtras>
#include <KWindowSystem>

#include "qwayland-plasma-window-management.h"
#include <QGuiApplication>
#include <QPixmap>
#include <QPoint>
#include <QString>
#include <QWaylandClientExtensionTemplate>
#include <QWindow>
#include <private/qwaylanddisplay_p.h>
#include <private/qwaylandinputdevice_p.h>
#include <private/qwaylandwindow_p.h>
#include <qpa/qplatformnativeinterface.h>
#include <qwaylandclientextension.h>

class WindowManagement : public QWaylandClientExtensionTemplate<WindowManagement>, public QtWayland::org_kde_plasma_window_management
{
public:
    WindowManagement()
        : QWaylandClientExtensionTemplate<WindowManagement>(16)
    {
    }

    void org_kde_plasma_window_management_show_desktop_changed(uint32_t state) override
    {
        showingDesktop = state == show_desktop_enabled;
    }

    bool showingDesktop = false;
};

WindowSystem::WindowSystem()
    : QObject()
    , KWindowSystemPrivateV2()
    , m_lastToken(qEnvironmentVariable("XDG_ACTIVATION_TOKEN"))
{
    m_windowManagement = new WindowManagement;
}

WindowSystem::~WindowSystem()
{
    delete m_windowManagement;
}

void WindowSystem::activateWindow(WId win, long int time)
{
    Q_UNUSED(time);
    auto s = surfaceForWindow(QWindow::fromWinId(win));
    if (!s) {
        return;
    }
    WaylandXdgActivationV1 *activation = WaylandXdgActivationV1::self();
    if (!activation->isActive()) {
        return;
    }
    activation->activate(m_lastToken, s);
}

void WindowSystem::forceActiveWindow(WId win, long int time)
{
    activateWindow(win, time);
}

void WindowSystem::requestToken(QWindow *window, uint32_t serial, const QString &app_id)
{
    wl_surface *wlSurface = [](QWindow *window) -> wl_surface * {
        if (!window) {
            return nullptr;
        }

        QPlatformNativeInterface *native = qGuiApp->platformNativeInterface();
        if (!native) {
            return nullptr;
        }
        window->create();
        return reinterpret_cast<wl_surface *>(native->nativeResourceForWindow(QByteArrayLiteral("surface"), window));
    }(window);

    WaylandXdgActivationV1 *activation = WaylandXdgActivationV1::self();
    if (!activation->isActive()) {
        // Ensure that xdgActivationTokenArrived is always emitted asynchronously
        QTimer::singleShot(0, [serial] {
            Q_EMIT KWaylandExtras::self()->xdgActivationTokenArrived(serial, {});
        });
        return;
    }

    auto waylandWindow = window ? dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle()) : nullptr;
    auto seat = waylandWindow ? waylandWindow->display()->defaultInputDevice()->wl_seat() : nullptr;
    auto tokenReq = activation->requestXdgActivationToken(seat, wlSurface, serial, app_id);
    connect(tokenReq, &WaylandXdgActivationTokenV1::failed, KWindowSystem::self(), [serial, app_id]() {
        Q_EMIT KWaylandExtras::self()->xdgActivationTokenArrived(serial, {});
    });
    connect(tokenReq, &WaylandXdgActivationTokenV1::done, KWindowSystem::self(), [serial](const QString &token) {
        Q_EMIT KWaylandExtras::self()->xdgActivationTokenArrived(serial, token);
    });
}

void WindowSystem::setCurrentToken(const QString &token)
{
    m_lastToken = token;
}

quint32 WindowSystem::lastInputSerial(QWindow *window)
{
    auto waylandWindow = window ? dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle()) : nullptr;
    if (!waylandWindow) {
        // Should never get here
        return 0;
    }
    return waylandWindow->display()->lastInputSerial();
}

WId WindowSystem::activeWindow()
{
    qCDebug(KWAYLAND_KWS) << "This plugin does not support querying the active window";
    return 0;
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

QString WindowSystem::desktopName(int desktop)
{
    Q_UNUSED(desktop)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support virtual desktops";
    return QString();
}

QPixmap WindowSystem::icon(WId win, int width, int height, bool scale, int flags)
{
    Q_UNUSED(win)
    Q_UNUSED(width)
    Q_UNUSED(height)
    Q_UNUSED(scale)
    Q_UNUSED(flags)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support getting window icons";
    return QPixmap();
}

bool WindowSystem::mapViewport()
{
    qCDebug(KWAYLAND_KWS) << "This plugin does not support viewport positions";
    return false;
}

void WindowSystem::minimizeWindow(WId win)
{
    Q_UNUSED(win)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support minimizing windows";
}

void WindowSystem::unminimizeWindow(WId win)
{
    Q_UNUSED(win)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support unminimizing windows";
}

int WindowSystem::numberOfDesktops()
{
    qCDebug(KWAYLAND_KWS) << "This plugin does not support virtual desktops";
    return 1;
}

QString WindowSystem::readNameProperty(WId window, long unsigned int atom)
{
    Q_UNUSED(window)
    Q_UNUSED(atom)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support reading X11 properties";
    return QString();
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

void WindowSystem::setExtendedStrut(WId win,
                                    int left_width,
                                    int left_start,
                                    int left_end,
                                    int right_width,
                                    int right_start,
                                    int right_end,
                                    int top_width,
                                    int top_start,
                                    int top_end,
                                    int bottom_width,
                                    int bottom_start,
                                    int bottom_end)
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
    Q_UNUSED(win)
    Q_UNUSED(desktop)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support setting window on a specific desktop";
}

void WindowSystem::setShowingDesktop(bool showing)
{
    if (!m_windowManagement->isActive()) {
        return;
    }
    m_windowManagement->show_desktop(showing ? WindowManagement::show_desktop_enabled : WindowManagement::show_desktop_disabled);
}

void WindowSystem::clearState(WId win, NET::States state)
{
    Q_UNUSED(win)
    Q_UNUSED(state)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support changing window state";
}

void WindowSystem::setState(WId win, NET::States state)
{
    Q_UNUSED(win)
    Q_UNUSED(state)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support changing window state";
}

void WindowSystem::setType(WId win, NET::WindowType windowType)
{
    Q_UNUSED(win)
    Q_UNUSED(windowType)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support setting window type";
}

bool WindowSystem::showingDesktop()
{
    if (!m_windowManagement->isActive()) {
        return false;
    }
    return m_windowManagement->showingDesktop;
}

QList<WId> WindowSystem::stackingOrder()
{
    qCDebug(KWAYLAND_KWS) << "This plugin does not support getting windows";
    return QList<WId>();
}

int WindowSystem::viewportWindowToDesktop(const QRect &r)
{
    Q_UNUSED(r)
    qCDebug(KWAYLAND_KWS) << "This plugin does not support viewports";
    return 0;
}

QList<WId> WindowSystem::windows()
{
    return stackingOrder();
}

QRect WindowSystem::workArea(const QList<WId> &excludes, int desktop)
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
