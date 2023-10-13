/*
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef WINDOWSYSTEM_H
#define WINDOWSYSTEM_H

#include <private/kwindowsystem_p.h>

#include <QObject>

namespace KWayland
{
namespace Client
{
class PlasmaShell;
}
}

class WindowManagement;

class WindowSystem : public QObject, public KWindowSystemPrivateV2
{
    Q_OBJECT
public:
    WindowSystem();
    ~WindowSystem() override;
    QList<WId> windows() override;
    QList<WId> stackingOrder() override;
    WId activeWindow() override;
    void activateWindow(WId win, long time) override;
    void forceActiveWindow(WId win, long time) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void demandAttention(WId win, bool set) override;
#endif
    void requestToken(QWindow *win, uint32_t serial, const QString &app_id) override;
    quint32 lastInputSerial(QWindow *window) override;
    void setCurrentToken(const QString &token) override;
    bool compositingActive() override;
    int currentDesktop() override;
    int numberOfDesktops() override;
    void setCurrentDesktop(int desktop) override;
    void setOnAllDesktops(WId win, bool b) override;
    void setOnDesktop(WId win, int desktop) override;
    void setOnActivities(WId win, const QStringList &activities) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    WId transientFor(WId window) override;
    WId groupLeader(WId window) override;
#endif
    QPixmap icon(WId win, int width, int height, bool scale, int flags) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void setIcons(WId win, const QPixmap &icon, const QPixmap &miniIcon) override;
#endif
    void setType(WId win, NET::WindowType windowType) override;
    void setState(WId win, NET::States state) override;
    void clearState(WId win, NET::States state) override;
    void minimizeWindow(WId win) override;
    void unminimizeWindow(WId win) override;
    void raiseWindow(WId win) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void lowerWindow(WId win) override;
    bool icccmCompliantMappingState() override;
#endif
    QRect workArea(int desktop) override;
    QRect workArea(const QList<WId> &excludes, int desktop) override;
    QString desktopName(int desktop) override;
    void setDesktopName(int desktop, const QString &name) override;
    bool showingDesktop() override;
    void setShowingDesktop(bool showing) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void setUserTime(WId win, long time) override;
#endif
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    bool allowedActionsSupported() override;
#endif
    QString readNameProperty(WId window, unsigned long atom) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void allowExternalProcessWindowActivation(int pid) override;
    void setBlockingCompositing(WId window, bool active) override;
#endif
    bool mapViewport() override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    int viewportToDesktop(const QPoint &pos) override;
#endif
    int viewportWindowToDesktop(const QRect &r) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QPoint desktopToViewport(int desktop, bool absolute) override;
#endif
    QPoint constrainViewportRelativePosition(const QPoint &pos) override;
    void connectNotify(const QMetaMethod &signal) override;

private:
    KWayland::Client::PlasmaShell *m_waylandPlasmaShell = nullptr;
    QString m_lastToken;
    WindowManagement *m_windowManagement;
};

#endif
