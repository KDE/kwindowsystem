/*
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef WINDOWSYSTEM_H
#define WINDOWSYSTEM_H

#include "kwindowsystem_p.h"

#include <QObject>

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
    QPixmap icon(WId win, int width, int height, bool scale, int flags) override;
    void setType(WId win, NET::WindowType windowType) override;
    void setState(WId win, NET::States state) override;
    void clearState(WId win, NET::States state) override;
    void minimizeWindow(WId win) override;
    void unminimizeWindow(WId win) override;
    QRect workArea(int desktop) override;
    QRect workArea(const QList<WId> &excludes, int desktop) override;
    QString desktopName(int desktop) override;
    void setDesktopName(int desktop, const QString &name) override;
    bool showingDesktop() override;
    void setShowingDesktop(bool showing) override;
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
    QString readNameProperty(WId window, unsigned long atom) override;
    bool mapViewport() override;
    int viewportWindowToDesktop(const QRect &r) override;
    QPoint constrainViewportRelativePosition(const QPoint &pos) override;
    void connectNotify(const QMetaMethod &signal) override;

private:
    QString m_lastToken;
    WindowManagement *m_windowManagement;
};

#endif
