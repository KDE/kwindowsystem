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
#ifndef KWINDOWSYSTEM_P_H
#define KWINDOWSYSTEM_P_H

#include "netwm_def.h"
#include <QWidgetList> //For WId

class KWindowSystemPrivate : public NET
{
public:
    virtual ~KWindowSystemPrivate();
    virtual QList<WId> windows() = 0;
    virtual QList<WId> stackingOrder() = 0;
    virtual WId activeWindow() = 0;
    virtual void activateWindow(WId win, long time = 0) = 0;
    virtual void forceActiveWindow(WId win, long time = 0) = 0;
    virtual void demandAttention(WId win, bool set) = 0;
    virtual bool compositingActive() = 0;
    virtual int currentDesktop() = 0;
    virtual int numberOfDesktops() = 0;
    virtual void setCurrentDesktop(int desktop) = 0;
    virtual void setOnAllDesktops(WId win, bool b) = 0;
    virtual void setOnDesktop(WId win, int desktop) = 0;
#ifndef KWINDOWSYSTEM_NO_DEPRECATED
    virtual WId transientFor(WId window) = 0;
    virtual WId groupLeader(WId window) = 0;
#endif
    virtual QPixmap icon(WId win, int width, int height, bool scale, int flags) = 0;
    virtual void setIcons(WId win, const QPixmap &icon, const QPixmap &miniIcon) = 0;
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
    virtual void setUserTime(WId win, long time) = 0;
    virtual void setExtendedStrut(WId win, int left_width, int left_start, int left_end,
                                  int right_width, int right_start, int right_end, int top_width, int top_start, int top_end,
                                  int bottom_width, int bottom_start, int bottom_end) = 0;
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

class KWindowSystemPrivateDummy : public KWindowSystemPrivate
{
public:
    QList<WId> windows() Q_DECL_OVERRIDE;
    QList<WId> stackingOrder() Q_DECL_OVERRIDE;
    WId activeWindow() Q_DECL_OVERRIDE;
    void activateWindow(WId win, long time) Q_DECL_OVERRIDE;
    void forceActiveWindow(WId win, long time) Q_DECL_OVERRIDE;
    void demandAttention(WId win, bool set) Q_DECL_OVERRIDE;
    bool compositingActive() Q_DECL_OVERRIDE;
    int currentDesktop() Q_DECL_OVERRIDE;
    int numberOfDesktops() Q_DECL_OVERRIDE;
    void setCurrentDesktop(int desktop) Q_DECL_OVERRIDE;
    void setOnAllDesktops(WId win, bool b) Q_DECL_OVERRIDE;
    void setOnDesktop(WId win, int desktop) Q_DECL_OVERRIDE;
#ifndef KWINDOWSYSTEM_NO_DEPRECATED
    WId transientFor(WId window) Q_DECL_OVERRIDE;
    WId groupLeader(WId window) Q_DECL_OVERRIDE;
#endif
    QPixmap icon(WId win, int width, int height, bool scale, int flags) Q_DECL_OVERRIDE;
    void setIcons(WId win, const QPixmap &icon, const QPixmap &miniIcon) Q_DECL_OVERRIDE;
    void setType(WId win, NET::WindowType windowType) Q_DECL_OVERRIDE;
    void setState(WId win, NET::States state) Q_DECL_OVERRIDE;
    void clearState(WId win, NET::States state) Q_DECL_OVERRIDE;
    void minimizeWindow(WId win) Q_DECL_OVERRIDE;
    void unminimizeWindow(WId win) Q_DECL_OVERRIDE;
    void raiseWindow(WId win) Q_DECL_OVERRIDE;
    void lowerWindow(WId win) Q_DECL_OVERRIDE;
    bool icccmCompliantMappingState() Q_DECL_OVERRIDE;
    QRect workArea(int desktop) Q_DECL_OVERRIDE;
    QRect workArea(const QList<WId> &excludes, int desktop) Q_DECL_OVERRIDE;
    QString desktopName(int desktop) Q_DECL_OVERRIDE;
    void setDesktopName(int desktop, const QString &name) Q_DECL_OVERRIDE;
    bool showingDesktop() Q_DECL_OVERRIDE;
    void setUserTime(WId win, long time) Q_DECL_OVERRIDE;
    void setExtendedStrut(WId win, int left_width, int left_start, int left_end,
                          int right_width, int right_start, int right_end, int top_width, int top_start, int top_end,
                          int bottom_width, int bottom_start, int bottom_end) Q_DECL_OVERRIDE;
    void setStrut(WId win, int left, int right, int top, int bottom) Q_DECL_OVERRIDE;
    bool allowedActionsSupported() Q_DECL_OVERRIDE;
    QString readNameProperty(WId window, unsigned long atom) Q_DECL_OVERRIDE;
    void allowExternalProcessWindowActivation(int pid) Q_DECL_OVERRIDE;
    void setBlockingCompositing(WId window, bool active) Q_DECL_OVERRIDE;
    bool mapViewport() Q_DECL_OVERRIDE;
    int viewportToDesktop(const QPoint &pos) Q_DECL_OVERRIDE;
    int viewportWindowToDesktop(const QRect &r) Q_DECL_OVERRIDE;
    QPoint desktopToViewport(int desktop, bool absolute) Q_DECL_OVERRIDE;
    QPoint constrainViewportRelativePosition(const QPoint &pos) Q_DECL_OVERRIDE;

    void connectNotify(const QMetaMethod &signal) Q_DECL_OVERRIDE;
};

#endif
