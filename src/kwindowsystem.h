/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Matthias Ettrich <ettrich@kde.org>
    SPDX-FileCopyrightText: 2007 Lubos Lunak <l.lunak@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/
/*
 * kwindowsystem.h. Part of the KDE project.
 */

#ifndef KWINDOWSYSTEM_H
#define KWINDOWSYSTEM_H

#include <QObject>
#include <QWidgetList> //For WId
#include <kwindowsystem_export.h>

class KWindowSystemPrivate;

/*!
 * \qmltype KWindowSystem
 * \inqmlmodule org.kde.kwindowsystem
 * \nativetype KWindowSystem
 * \brief Convenience access to certain properties and features of window systems.
 */
/*!
 * \class KWindowSystem
 * \inmodule KWindowSystem
 * \brief Convenience access to certain properties and features of window systems.
 */
class KWINDOWSYSTEM_EXPORT KWindowSystem : public QObject
{
    Q_OBJECT
    /*!
     * \qmlproperty bool KWindowSystem::isPlatformWayland
     */
    /*!
     * \property KWindowSystem::isPlatformWayland
     */
    Q_PROPERTY(bool isPlatformWayland READ isPlatformWayland CONSTANT)
    /*!
     * \qmlproperty bool KWindowSystem::isPlatformX11
     */
    /*!
     * \property KWindowSystem::isPlatformX11
     */
    Q_PROPERTY(bool isPlatformX11 READ isPlatformX11 CONSTANT)

    /*!
     * \qmlproperty bool KWindowSystem::showingDesktop
     *
     * \brief Whether "show desktop" is currently active.
     */
    /*!
     * \property KWindowSystem::showingDesktop
     *
     * \brief Whether "show desktop" is currently active.
     */
    Q_PROPERTY(bool showingDesktop READ showingDesktop WRITE setShowingDesktop NOTIFY showingDesktopChanged)

public:
    /*
     * Access to the singleton instance. Useful mainly for connecting to signals.
     */
    static KWindowSystem *self();

    /*!
     * Requests that window \a window is activated.
     *
     * Applications shouldn't make attempts to explicitly activate
     * their windows, and instead let the user activate them.
     * In the special cases where this may be needed, applications
     * can use activateWindow(). The window manager may consider whether
     * this request wouldn't result in focus stealing, which
     * would be obtrusive, and may refuse the request.
     *
     * In case of problems, consult KWin's README.md file, or ask on the kwin@kde.org
     * mailing list.
     *
     * \a window the window to make active
     * \a time X server timestamp of the user activity that
     *    caused this request
     *
     * \since 5.89
     */
    Q_INVOKABLE static void activateWindow(QWindow *window, long time = 0);

    /*!
     * Returns the state of showing the desktop.
     */
    static bool showingDesktop();

    /*!
     * Sets the state of the "showing desktop" mode of the window manager. If on,
     * windows are hidden and desktop background is shown and focused.
     *
     * \a showing if true, the window manager is put in "showing desktop" mode.
     * If false, the window manager is put out of that mode.
     *
     * \since 5.7.0
     */
    static void setShowingDesktop(bool showing);

    /*!
     * Sets the parent window of \a subwindow to be \a mainwindow.
     * This overrides the parent set the usual way as the QWidget or QWindow parent,
     * but only for the window manager - e.g. stacking order and window grouping
     * will be affected, but features like automatic deletion of children
     * when the parent is deleted are unaffected and normally use
     * the QObject parent.
     *
     * This function should be used before a dialog is shown for a window
     * that belongs to another application.
     *
     * On Wayland, use the QString overload to provide an XDG Foreign token.
     */
    static void setMainWindow(QWindow *subwindow, WId mainwindow);

    /*!
     * Sets the parent window of \a subwindow to be \a mainwindow.
     *
     * This function should be used before a dialog is shown for a window
     * that belongs to another application.
     *
     * \a subwindow the sub window
     * \a mainwindow The main window ID or XDG Foreign token
     *
     * \since 6.0
     */
    static void setMainWindow(QWindow *subwindow, const QString &mainwindow);

    /*!
     * Updates the platform-specific startup id, if any.
     *
     * This method is to be called when a running application instance
     * is reused for handling the request to start this application.
     * A typical use would be in the handler of the KDBusService activation signal.
     *
     * For X11, this updates the id for the Startup Notification protocol,
     * taking the id from QX11Info::nextStartupId(), if not empty.
     * For Wayland, this updates the token for the XDG Activation protocol,
     * taking the token from the "XDG_ACTIVATION_TOKEN" environment variable
     * and then unsetting it, if not empty.
     *
     * \a window the main window (needed by X11 platform)
     *
     * \since 5.91
     */
    static void updateStartupId(QWindow *window);

    /*!
     * \enum KWindowSystem::Platform
     * \brief Enum describing the windowing system platform used by the QGuiApplication.
     * \value Unknown
     *        A platform unknown to the application is used
     * \value X11
     *        The X11 window system.
     * \value Wayland
     *        The Wayland window system.
     * \sa platform()
     * \since 5.25
     **/
    enum class Platform {
        Unknown,
        X11,
        Wayland,
    };
    Q_ENUM(Platform)
    /*!
     * Returns the Platform used by the QGuiApplication.
     * The Platform gets resolved the first time the method is invoked and cached for further
     * usages.
     * Returns The Platform used by the QGuiApplication.
     * \since 5.25
     **/
    static Platform platform();

    /*!
     * Returns whether the Platform is X11.
     * \sa platform
     * \sa isPlatformWayland
     * \since 5.25
     **/
    static bool isPlatformX11();

    /*!
     * Returns whether the Platform is Wayland.
     * \sa platform
     * \sa isPlatformX11
     * \since 5.25
     **/
    static bool isPlatformWayland();

    /*!
     * Sets the \a token that will be used when activateWindow is called next
     *
     * \since 5.83
     */
    Q_INVOKABLE static void setCurrentXdgActivationToken(const QString &token);

Q_SIGNALS:
    /*!
     * The state of \a showing the desktop has changed.
     */
    void showingDesktopChanged(bool showing);

private:
    friend class KWindowSystemStaticContainer;
    friend class KX11Extras;
    friend class KWaylandExtras;

    KWindowSystem()
    {
    }
    static KWindowSystemPrivate *d_func();
};

#endif
