/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KWINDOWSYSTEM_DUMMY_P_H
#define KWINDOWSYSTEM_DUMMY_P_H

#include "kwindowsystem_p.h"

class KWindowSystemPrivateDummy : public KWindowSystemPrivate
{
public:
    QList<WId> windows() override;
    QList<WId> stackingOrder() override;
    WId activeWindow() override;
    void activateWindow(WId win, long time) override;
    void forceActiveWindow(WId win, long time) override;
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
    void raiseWindow(WId win) override;
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
};

#endif
