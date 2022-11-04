/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "kwindowsystem.h"
#include "kstartupinfo.h"
#include "kwindowsystem_dummy_p.h"
#include "kwindowsystemplugininterface_p.h"
#include "pluginwrapper_p.h"

#include <config-kwindowsystem.h>

#include <QGuiApplication>
#include <QMetaMethod>
#include <QPixmap>
#include <QPluginLoader>
#include <QTimer>
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 62)
#include <QWidget>
#endif
#include <QWindow>
#if KWINDOWSYSTEM_HAVE_X11
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <private/qtx11extras_p.h>
#else
#include <QX11Info>
#endif
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

void KWindowSystemPrivateDummy::demandAttention(WId win, bool set)
{
    Q_UNUSED(win)
    Q_UNUSED(set)
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

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 0)
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

void KWindowSystemPrivateDummy::setShowingDesktop(bool showing)
{
    Q_UNUSED(showing);
}

void KWindowSystemPrivateDummy::setUserTime(WId win, long time)
{
    Q_UNUSED(win)
    Q_UNUSED(time)
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
    return g_kwmInstanceContainer()->d.get();
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

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 0)
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

void KWindowSystem::activateWindow(QWindow *win, long time)
{
    activateWindow(win->winId(), time);
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

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 0)
WId KWindowSystem::transientFor(WId win)
{
    Q_D(KWindowSystem);
    return d->transientFor(win);
}

void KWindowSystem::setMainWindow(QWidget *subWidget, WId mainWindowId)
{
    // Set the WA_NativeWindow attribute to force the creation of the QWindow.
    // Without this QWidget::windowHandle() returns 0.
    subWidget->setAttribute(Qt::WA_NativeWindow, true);
    QWindow *subWindow = subWidget->windowHandle();
    Q_ASSERT(subWindow);
    setMainWindow(subWindow, mainWindowId);
}
#endif

void KWindowSystem::setMainWindow(QWindow *subWindow, WId mainWindowId)
{
    QWindow *mainWindow = QWindow::fromWinId(mainWindowId);
    if (mainWindow) { // foreign windows not supported on all platforms
        subWindow->setTransientParent(mainWindow);

        // mainWindow is not the child of any object, so make sure it gets deleted at some point
        connect(subWindow, &QObject::destroyed, mainWindow, &QObject::deleteLater);
    }
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 0)
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

QPixmap KWindowSystem::icon(WId win, int width, int height, bool scale, int flags, NETWinInfo *info)
{
    Q_D(KWindowSystem);
    width *= qApp->devicePixelRatio();
    height *= qApp->devicePixelRatio();
#if KWINDOWSYSTEM_HAVE_X11
    if (info) {
        if (isPlatformX11()) {
            // this is the xcb plugin, we can just delegate
            return d->iconFromNetWinInfo(width, height, scale, flags, info);
        } else {
            // other platform plugin, load xcb plugin to delegate to it
            if (KWindowSystemPrivate *p = g_kwmInstanceContainer()->xcbPlugin()) {
                return p->iconFromNetWinInfo(width, height, scale, flags, info);
            }
        }
    }
#else
    Q_UNUSED(info)
#endif
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

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 0)
void KWindowSystem::minimizeWindow(WId win, bool animation)
{
    Q_UNUSED(animation)
    minimizeWindow(win);
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 0)
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

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 101)
void KWindowSystem::lowerWindow(WId win)
{
    Q_D(KWindowSystem);
    d->lowerWindow(win);
}
#endif

bool KWindowSystem::compositingActive()
{
    Q_D(KWindowSystem);
    return d->compositingActive();
}

QRect KWindowSystem::workArea(int desktop)
{
    Q_D(KWindowSystem);
    return d->workArea(desktop) / qApp->devicePixelRatio();
}

QRect KWindowSystem::workArea(const QList<WId> &exclude, int desktop)
{
    Q_D(KWindowSystem);
    return d->workArea(exclude, desktop) / qApp->devicePixelRatio();
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

void KWindowSystem::setShowingDesktop(bool showing)
{
    Q_D(KWindowSystem);
    return d->setShowingDesktop(showing);
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 101)
void KWindowSystem::setUserTime(WId win, long time)
{
    Q_D(KWindowSystem);
    d->setUserTime(win, time);
}
#endif

void KWindowSystem::setExtendedStrut(WId win,
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
    Q_D(KWindowSystem);
    const qreal dpr = qApp->devicePixelRatio();
    d->setExtendedStrut(win,
                        left_width * dpr,
                        left_start * dpr,
                        left_end * dpr,
                        right_width * dpr,
                        right_start * dpr,
                        right_end * dpr,
                        top_width * dpr,
                        top_start * dpr,
                        top_end * dpr,
                        bottom_width * dpr,
                        bottom_start * dpr,
                        bottom_end * dpr);
}

void KWindowSystem::setStrut(WId win, int left, int right, int top, int bottom)
{
    Q_D(KWindowSystem);
    const qreal dpr = qApp->devicePixelRatio();
    d->setStrut(win, left * dpr, right * dpr, top * dpr, bottom * dpr);
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 101)
bool KWindowSystem::icccmCompliantMappingState()
{
    Q_D(KWindowSystem);
    return d->icccmCompliantMappingState();
}
#endif

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

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 101)
void KWindowSystem::setBlockingCompositing(WId window, bool active)
{
    Q_D(KWindowSystem);
    d->setBlockingCompositing(window, active);
}
#endif

bool KWindowSystem::mapViewport()
{
    Q_D(KWindowSystem);
    return d->mapViewport();
}

int KWindowSystem::viewportToDesktop(const QPoint &p)
{
    Q_D(KWindowSystem);
    return d->viewportToDesktop(p / qApp->devicePixelRatio());
}

int KWindowSystem::viewportWindowToDesktop(const QRect &r)
{
    Q_D(KWindowSystem);
    return d->viewportWindowToDesktop(r / qApp->devicePixelRatio());
}

QPoint KWindowSystem::desktopToViewport(int desktop, bool absolute)
{
    Q_D(KWindowSystem);
    return d->desktopToViewport(desktop, absolute);
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 101)
QPoint KWindowSystem::constrainViewportRelativePosition(const QPoint &pos)
{
    Q_D(KWindowSystem);
    return d->constrainViewportRelativePosition(pos / qApp->devicePixelRatio());
}
#endif

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
