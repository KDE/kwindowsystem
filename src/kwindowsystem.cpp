/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "kwindowsystem.h"
#include "kwindowsystem_dummy_p.h"
#include "kwindowsystemplugininterface_p.h"
#include "pluginwrapper_p.h"

#include <config-kwindowsystem.h>

#if KWINDOWSYSTEM_HAVE_X11
#include "kstartupinfo.h"
#endif

#include <QGuiApplication>
#include <QMetaMethod>
#include <QPixmap>
#include <QPluginLoader>
#include <QTimer>
#include <QWindow>
#if KWINDOWSYSTEM_HAVE_X11
#include <private/qtx11extras_p.h>
#endif

// QPoint and QSize all have handy / operators which are useful for scaling, positions and sizes for high DPI support
// QRect does not, so we create one for internal purposes within this class
inline QRect operator/(const QRect &rectangle, qreal factor)
{
    return QRect(rectangle.topLeft() / factor, rectangle.size() / factor);
}

class KWindowSystemStaticContainer
{
public:
    KWindowSystemStaticContainer()
    {
        d.reset(KWindowSystemPluginWrapper::self().createWindowSystem());

        if (QCoreApplication::instance()) {
            kwm.moveToThread(QCoreApplication::instance()->thread());
        }
    }
    KWindowSystemPrivate *xcbPlugin()
    {
        if (!xcbPrivate) {
            QPluginLoader loader(QStringLiteral(XCB_PLUGIN_PATH));
            std::unique_ptr<KWindowSystemPluginInterface> xcbPlugin(qobject_cast<KWindowSystemPluginInterface *>(loader.instance()));
            if (xcbPlugin) {
                xcbPrivate.reset(xcbPlugin->createWindowSystem());
            }
        }
        return xcbPrivate.get();
    }
    KWindowSystem kwm;
    std::unique_ptr<KWindowSystemPrivate> d;
    std::unique_ptr<KWindowSystemPrivate> xcbPrivate;
};

Q_GLOBAL_STATIC(KWindowSystemStaticContainer, g_kwmInstanceContainer)

KWindowSystemPrivate::~KWindowSystemPrivate()
{
}

QPixmap KWindowSystemPrivate::iconFromNetWinInfo(int width, int height, bool scale, int flags, NETWinInfo *info)
{
    Q_UNUSED(width)
    Q_UNUSED(height)
    Q_UNUSED(scale)
    Q_UNUSED(flags)
    Q_UNUSED(info)
    return QPixmap();
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

bool KWindowSystemPrivateDummy::compositingActive()
{
    return KWindowSystem::isPlatformWayland();
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

QPixmap KWindowSystemPrivateDummy::icon(WId win, int width, int height, bool scale, int flags)
{
    Q_UNUSED(win)
    Q_UNUSED(width)
    Q_UNUSED(height)
    Q_UNUSED(scale)
    Q_UNUSED(flags)
    return QPixmap();
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

void KWindowSystemPrivateDummy::setShowingDesktop(bool showing)
{
    Q_UNUSED(showing);
}

void KWindowSystemPrivateDummy::setExtendedStrut(WId win,
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
}

void KWindowSystemPrivateDummy::setStrut(WId win, int left, int right, int top, int bottom)
{
    Q_UNUSED(win)
    Q_UNUSED(left)
    Q_UNUSED(right)
    Q_UNUSED(top)
    Q_UNUSED(bottom)
}

QString KWindowSystemPrivateDummy::readNameProperty(WId window, unsigned long atom)
{
    Q_UNUSED(window)
    Q_UNUSED(atom)
    return QString();
}

bool KWindowSystemPrivateDummy::mapViewport()
{
    return false;
}

int KWindowSystemPrivateDummy::viewportWindowToDesktop(const QRect &r)
{
    Q_UNUSED(r)
    return 0;
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
    return g_kwmInstanceContainer()->d.get();
}

void KWindowSystem::connectNotify(const QMetaMethod &signal)
{
    Q_D(KWindowSystem);
    d->connectNotify(signal);
    QObject::connectNotify(signal);
}

void KWindowSystem::activateWindow(QWindow *win, long time)
{
    Q_D(KWindowSystem);
    d->activateWindow(win->winId(), time);
}

void KWindowSystem::setMainWindow(QWindow *subWindow, WId mainWindowId)
{
    QWindow *mainWindow = QWindow::fromWinId(mainWindowId);
    if (mainWindow) { // foreign windows not supported on all platforms
        subWindow->setTransientParent(mainWindow);

        // mainWindow is not the child of any object, so make sure it gets deleted at some point
        connect(subWindow, &QObject::destroyed, mainWindow, &QObject::deleteLater);
    }
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

void KWindowSystem::raiseWindow(WId win)
{
    Q_D(KWindowSystem);
    d->raiseWindow(win);
}

bool KWindowSystem::showingDesktop()
{
    Q_D(KWindowSystem);
    return d->showingDesktop();
}

void KWindowSystem::setShowingDesktop(bool showing)
{
    Q_D(KWindowSystem);
    return d->setShowingDesktop(showing);
}

int KWindowSystem::viewportWindowToDesktop(const QRect &r)
{
    Q_D(KWindowSystem);
    return d->viewportWindowToDesktop(r / qApp->devicePixelRatio());
}

static inline KWindowSystem::Platform initPlatform()
{
    auto platformName = QGuiApplication::platformName();
    if (platformName == QLatin1String("flatpak")) {
        // here we cannot know what is the actual windowing system, let's try it's env variable
        const auto flatpakPlatform = QString::fromLocal8Bit(qgetenv("QT_QPA_FLATPAK_PLATFORM"));
        if (!flatpakPlatform.isEmpty()) {
            platformName = flatpakPlatform;
        }
    }
#if KWINDOWSYSTEM_HAVE_X11
    if (platformName == QLatin1String("xcb")) {
        return KWindowSystem::Platform::X11;
    }
#endif
    if (platformName.startsWith(QLatin1String("wayland"), Qt::CaseInsensitive)) {
        return KWindowSystem::Platform::Wayland;
    }
    return KWindowSystem::Platform::Unknown;
}

KWindowSystem::Platform KWindowSystem::platform()
{
    static Platform s_platform = initPlatform();
    return s_platform;
}

bool KWindowSystem::isPlatformX11()
{
    return platform() == Platform::X11;
}

bool KWindowSystem::isPlatformWayland()
{
    return platform() == Platform::Wayland;
}

void KWindowSystem::updateStartupId(QWindow *window)
{
    // clang-format off
    // TODO: move to a new KWindowSystemPrivate interface
#if KWINDOWSYSTEM_HAVE_X11
    if (isPlatformX11()) {
        const QByteArray startupId = QX11Info::nextStartupId();
        if (!startupId.isEmpty()) {
            KStartupInfo::setNewStartupId(window, startupId);
        }
    } else
#else
    Q_UNUSED(window);
#endif
    if (isPlatformWayland()) {
        const QString token = qEnvironmentVariable("XDG_ACTIVATION_TOKEN");
        if (!token.isEmpty()) {
            setCurrentXdgActivationToken(token);
            qunsetenv("XDG_ACTIVATION_TOKEN");
        }
    }
    // clang-format on
}

void KWindowSystem::requestXdgActivationToken(QWindow *win, uint32_t serial, const QString &app_id)
{
    Q_D(KWindowSystem);
    auto dv2 = dynamic_cast<KWindowSystemPrivateV2 *>(d);
    if (!dv2) {
        // Ensure that xdgActivationTokenArrived is always emitted asynchronously
        QTimer::singleShot(0, [serial] {
            Q_EMIT KWindowSystem::self()->xdgActivationTokenArrived(serial, {});
        });

        return;
    }
    dv2->requestToken(win, serial, app_id);
}

void KWindowSystem::setCurrentXdgActivationToken(const QString &token)
{
    Q_D(KWindowSystem);
    auto dv2 = dynamic_cast<KWindowSystemPrivateV2 *>(d);
    if (!dv2) {
        return;
    }
    dv2->setCurrentToken(token);
}

quint32 KWindowSystem::lastInputSerial(QWindow *window)
{
    Q_D(KWindowSystem);
    auto dv2 = dynamic_cast<KWindowSystemPrivateV2 *>(d);
    if (!dv2) {
        return 0;
    }
    return dv2->lastInputSerial(window);
}

#include "moc_kwindowsystem.cpp"
