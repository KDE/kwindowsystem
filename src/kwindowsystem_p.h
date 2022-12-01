/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef KWINDOWSYSTEM_P_H
#define KWINDOWSYSTEM_P_H

#include "netwm_def.h"
#include <QStringList>
#include <QWidgetList> //For WId
#include <kwindowsystem_export.h>

class NETWinInfo;

class KWINDOWSYSTEM_EXPORT KWindowSystemPrivate : public NET
{
public:
    virtual ~KWindowSystemPrivate();
    virtual QList<WId> windows() = 0;
    virtual QList<WId> stackingOrder() = 0;
    virtual WId activeWindow() = 0;
    virtual void activateWindow(WId win, long time = 0) = 0;
    virtual void forceActiveWindow(WId win, long time = 0) = 0;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 101)
    virtual void demandAttention(WId win, bool set) = 0;
#endif
    virtual bool compositingActive() = 0;
    virtual int currentDesktop() = 0;
    virtual int numberOfDesktops() = 0;
    virtual void setCurrentDesktop(int desktop) = 0;
    virtual void setOnAllDesktops(WId win, bool b) = 0;
    virtual void setOnDesktop(WId win, int desktop) = 0;
    virtual void setOnActivities(WId win, const QStringList &activities) = 0;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 0)
    virtual WId transientFor(WId window) = 0;
    virtual WId groupLeader(WId window) = 0;
#endif
    virtual QPixmap icon(WId win, int width, int height, bool scale, int flags) = 0;
    virtual QPixmap iconFromNetWinInfo(int width, int height, bool scale, int flags, NETWinInfo *info);
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 101)
    virtual void setIcons(WId win, const QPixmap &icon, const QPixmap &miniIcon) = 0;
#endif
    virtual void setType(WId win, NET::WindowType windowType) = 0;
    virtual void setState(WId win, NET::States state) = 0;
    virtual void clearState(WId win, NET::States state) = 0;
    virtual void minimizeWindow(WId win) = 0;
    virtual void unminimizeWindow(WId win) = 0;
    virtual void raiseWindow(WId win) = 0;
    virtual void lowerWindow(WId win) = 0;
    virtual bool icccmCompliantMappingState() = 0;
    virtual QRect workArea(int desktop) = 0;
    virtual QRect workArea(const QList<WId> &excludes, int desktop) = 0;
    virtual QString desktopName(int desktop) = 0;
    virtual void setDesktopName(int desktop, const QString &name) = 0;
    virtual bool showingDesktop() = 0;
    virtual void setShowingDesktop(bool showing) = 0;
    virtual void setUserTime(WId win, long time) = 0;
    virtual void setExtendedStrut(WId win,
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
                                  int bottom_end) = 0;
    virtual void setStrut(WId win, int left, int right, int top, int bottom) = 0;
    virtual bool allowedActionsSupported() = 0;
    virtual QString readNameProperty(WId window, unsigned long atom) = 0;
    virtual void allowExternalProcessWindowActivation(int pid) = 0;
    virtual void setBlockingCompositing(WId window, bool active) = 0;
    virtual bool mapViewport() = 0;
    virtual int viewportToDesktop(const QPoint &pos) = 0;
    virtual int viewportWindowToDesktop(const QRect &r) = 0;
    virtual QPoint desktopToViewport(int desktop, bool absolute) = 0;
    virtual QPoint constrainViewportRelativePosition(const QPoint &pos) = 0;

    virtual void connectNotify(const QMetaMethod &signal) = 0;
};

class KWINDOWSYSTEM_EXPORT KWindowSystemPrivateV2 : public KWindowSystemPrivate
{
public:
    virtual void requestToken(QWindow *win, uint32_t serial, const QString &app_id) = 0;
    virtual void setCurrentToken(const QString &token) = 0;
    virtual quint32 lastInputSerial(QWindow *window) = 0;
};

#endif
