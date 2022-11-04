/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KWINDOWSYSTEM_P_X11_H
#define KWINDOWSYSTEM_P_X11_H

#include "kwindowsystem_p.h"
#include "netwm.h"

#include <QAbstractNativeEventFilter>
#include <memory>

class NETEventFilter;

class KWindowSystemPrivateX11 : public KWindowSystemPrivate
{
public:
    QList<WId> windows() override;
    QList<WId> stackingOrder() override;
    WId activeWindow() override;
    void activateWindow(WId win, long time) override;
    void forceActiveWindow(WId win, long time) override;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 101)
    void demandAttention(WId win, bool set) override;
#endif
    bool compositingActive() override;
    int currentDesktop() override;
    int numberOfDesktops() override;
    void setCurrentDesktop(int desktop) override;
    void setOnAllDesktops(WId win, bool b) override;
    void setOnDesktop(WId win, int desktop) override;
    void setOnActivities(WId win, const QStringList &activities) override;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 0)
    WId transientFor(WId window) override;
    WId groupLeader(WId window) override;
#endif
    QPixmap icon(WId win, int width, int height, bool scale, int flags) override;
    QPixmap iconFromNetWinInfo(int width, int height, bool scale, int flags, NETWinInfo *info) override;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 101)
    void setIcons(WId win, const QPixmap &icon, const QPixmap &miniIcon) override;
#endif
    void setType(WId win, NET::WindowType windowType) override;
    void setState(WId win, NET::States state) override;
    void clearState(WId win, NET::States state) override;
    void minimizeWindow(WId win) override;
    void unminimizeWindow(WId win) override;
    void raiseWindow(WId win) override;
    void lowerWindow(WId win) override;
    bool icccmCompliantMappingState() override;
    QRect workArea(int desktop) override;
    QRect workArea(const QList<WId> &excludes, int desktop) override;
    QString desktopName(int desktop) override;
    void setDesktopName(int desktop, const QString &name) override;
    bool showingDesktop() override;
    void setShowingDesktop(bool showing) override;
    void setUserTime(WId win, long time) override;
    void setExtendedStrut(WId win,
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
                          int bottom_end) override;
    void setStrut(WId win, int left, int right, int top, int bottom) override;
    bool allowedActionsSupported() override;
    QString readNameProperty(WId window, unsigned long atom) override;
    void allowExternalProcessWindowActivation(int pid) override;
    void setBlockingCompositing(WId window, bool active) override;
    bool mapViewport() override;
    int viewportToDesktop(const QPoint &pos) override;
    int viewportWindowToDesktop(const QRect &r) override;
    QPoint desktopToViewport(int desktop, bool absolute) override;
    QPoint constrainViewportRelativePosition(const QPoint &pos) override;

    void connectNotify(const QMetaMethod &signal) override;

    enum FilterInfo {
        INFO_BASIC = 1, // desktop info, not per-window
        INFO_WINDOWS = 2, // also per-window info
    };

private:
    void init(FilterInfo info);
    NETEventFilter *s_d_func()
    {
        return d.get();
    }
    std::unique_ptr<NETEventFilter> d;
};

class MainThreadInstantiator : public QObject
{
    Q_OBJECT

public:
    MainThreadInstantiator(KWindowSystemPrivateX11::FilterInfo _what);
    Q_INVOKABLE NETEventFilter *createNETEventFilter();

private:
    KWindowSystemPrivateX11::FilterInfo m_what;
};

class NETEventFilter : public NETRootInfo, public QAbstractNativeEventFilter
{
public:
    NETEventFilter(KWindowSystemPrivateX11::FilterInfo _what);
    ~NETEventFilter() override;
    void activate();
    QList<WId> windows;
    QList<WId> stackingOrder;

    struct StrutData {
        StrutData(WId window_, const NETStrut &strut_, int desktop_)
            : window(window_)
            , strut(strut_)
            , desktop(desktop_)
        {
        }
        WId window;
        NETStrut strut;
        int desktop;
    };
    QList<StrutData> strutWindows;
    QList<WId> possibleStrutWindows;
    bool strutSignalConnected;
    bool compositingEnabled;
    bool haveXfixes;
    KWindowSystemPrivateX11::FilterInfo what;
    int xfixesEventBase;
    bool mapViewport();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *) override;
#else
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *) override;
#endif

    void updateStackingOrder();
    bool removeStrutWindow(WId);

protected:
    void addClient(xcb_window_t) override;
    void removeClient(xcb_window_t) override;

private:
    bool nativeEventFilter(xcb_generic_event_t *event);
    xcb_window_t winId;
    xcb_window_t m_appRootWindow;
};

#endif
