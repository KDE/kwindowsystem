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
#include <kwindowinfo.h>
#include <kwindowsystem_export.h>
#include <netwm_def.h>

class KWindowSystemPrivate;
class NETWinInfo;

/**
 *
 * Convenience access to certain properties and features of the
 * window manager.
 *
 * The class KWindowSystem provides information about the state of the
 * window manager and allows asking the window manager to change them
 * using a more high-level interface than the NETWinInfo/NETRootInfo
 * lowlevel classes.
 *
 * Because of limitations of the way Qt is implemented on Mac OSX, the WId's
 * returned by methods in this class are not compatible with those expected
 * by other Qt methods. So while it should be fine to pass WId's retrieved by
 * for example calling the winId method on a QWidget to methods in this class
 * the reverse is not true. You should never pass a WId obtained from this class
 * to a Qt method accepting a WId parameter.
 *
 * @short Class for interaction with the window manager.
 * @author Matthias Ettrich (ettrich@kde.org)
 */
class KWINDOWSYSTEM_EXPORT KWindowSystem : public QObject, public NET
{
    Q_OBJECT
    Q_PROPERTY(bool isPlatformWayland READ isPlatformWayland CONSTANT)
    Q_PROPERTY(bool isPlatformX11 READ isPlatformX11 CONSTANT)

    /**
     * @brief Whether "show desktop" is currently active
     */
    Q_PROPERTY(bool showingDesktop READ showingDesktop WRITE setShowingDesktop NOTIFY showingDesktopChanged)

public:
    /**
     * Access to the singleton instance. Useful mainly for connecting to signals.
     */
    static KWindowSystem *self();

    /**
     * Requests that window @p win is activated.
     *
     * There are two ways how to activate a window, by calling
     * activateWindow() and forceActiveWindow(). Generally,
     * applications shouldn't make attempts to explicitly activate
     * their windows, and instead let the user to activate them.
     * In the special cases where this may be needed, applications
     * should use activateWindow(). Window manager may consider whether
     * this request wouldn't result in focus stealing, which
     * would be obtrusive, and may refuse the request.
     *
     * The usage of forceActiveWindow() is meant only for pagers
     * and similar tools, which represent direct user actions
     * related to window manipulation.
     * Except for rare cases, this request will be always honored,
     * and normal applications are forbidden to use it.
     *
     * In case of problems, consult the KWin README in the kdebase
     * package (kdebase/kwin/README), or ask on the kwin@kde.org
     * mailing list.
     *
     * @param window the window to make active
     * @param time X server timestamp of the user activity that
     *    caused this request
     *
     * @since 5.89
     */
    Q_INVOKABLE static void activateWindow(QWindow *window, long time = 0);

    /**
     * Sets the type of window @p win to @p windowType.
     *
     * @param win the id of the window
     * @param windowType the type of the window (see NET::WindowType)
     */
    static void setType(WId win, NET::WindowType windowType);
    /**
     * Sets the state of window @p win to @p state.
     *
     * Possible values are or'ed combinations of NET::Modal,
     * NET::Sticky, NET::MaxVert, NET::MaxHoriz, NET::Shaded,
     * NET::SkipTaskbar, NET::SkipPager, NET::Hidden,
     * NET::FullScreen, NET::KeepAbove, NET::KeepBelow,
     * NET::SkipSwitcher
     *
     * @param win the id of the window
     * @param state the new flags that will be set
     */
    static void setState(WId win, NET::States state);

    /**
     * Clears the state of window @p win from @p state.
     *
     * Possible values are or'ed combinations of NET::Modal,
     * NET::Sticky, NET::MaxVert, NET::MaxHoriz, NET::Shaded,
     * NET::SkipTaskbar, NET::SkipPager, NET::Hidden,
     * NET::FullScreen, NET::KeepAbove, NET::KeepBelow,
     * NET::SkipSwitcher
     *
     * @param win the id of the window
     * @param state the flags that will be cleared
     */
    static void clearState(WId win, NET::States state);

    /**
     * Raises the given window. This call is only for pagers and similar
     * tools that represent direct user actions. Applications should not
     * use it, they should keep using QWidget::raise() or XRaiseWindow()
     * if necessary.
     */
    static void raiseWindow(WId win);

    /**
     * Returns the state of showing the desktop.
     */
    static bool showingDesktop();

    /**
     * Sets the state of the "showing desktop" mode of the window manager. If on,
     * windows are hidden and desktop background is shown and focused.
     *
     * @param showing if true, the window manager is put in "showing desktop" mode.
     * If false, the window manager is put out of that mode.
     *
     * @since 5.7.0
     */
    static void setShowingDesktop(bool showing);

    /**
     * Sets the parent window of @p subwindow to be @p mainwindow.
     * This overrides the parent set the usual way as the QWidget or QWindow parent,
     * but only for the window manager - e.g. stacking order and window grouping
     * will be affected, but features like automatic deletion of children
     * when the parent is deleted are unaffected and normally use
     * the QObject parent.
     *
     * This function should be used before a dialog is shown for a window
     * that belongs to another application.
     */
    static void setMainWindow(QWindow *subwindow, WId mainwindow);

    /**
     * Allows a window from another process to raise and activate itself.
     * Depending on the window manager, the grant may only be temporary,
     * or for a single activation, and it may require the current process
     * to be the "foreground" one" (ie. the process with the input focus).
     *
     * You should call this function before executing actions that may trigger
     * the showing of a window or dialog in another process, e.g. a dbus signal
     * or function call, or any other inter-process notification mechanism.
     *
     * This is mostly used on Windows, where windows are not allowed to be raised
     * and activated if their process is not the foreground one, but it may also
     * apply to other window managers.
     *
     * @param pid if specified, the grant only applies to windows belonging to the
     *            specific process. By default, a value of -1 means all processes.
     */
    static void allowExternalProcessWindowActivation(int pid = -1);

    /**
     * @internal
     * Returns mapped virtual desktop for the given window geometry.
     */
    static int viewportWindowToDesktop(const QRect &r);

    /**
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
     * @param window the main window (needed by X11 platform)
     *
     * @since 5.91
     */
    static void updateStartupId(QWindow *window);

    /**
     * Enum describing the windowing system platform used by the QGuiApplication.
     * @see platform
     * @since 5.25
     **/
    enum class Platform {
        /**
         * A platform unknown to the application is used
         **/
        Unknown,
        /**
         * The xcb/X11 windowing system platorm.
         **/
        X11,
        /**
         * The Wayland windowing system platform.
         **/
        Wayland,
    };
    Q_ENUM(Platform)
    /**
     * Returns the Platform used by the QGuiApplication.
     * This method allows to check for the used windowing system in a cheap and reliable way.
     * The Platform gets resolved the first time the method is invoked and cached for further
     * usages.
     * @returns The Platform used by the QGuiApplication.
     * @since 5.25
     **/
    static Platform platform();

    /**
     * Convenience method to check whether the Platform is X11.
     * @see platform
     * @see isPlatformWayland
     * @since 5.25
     **/
    static bool isPlatformX11();

    /**
     * Convenience method to check whether the Platform is Wayland.
     * @see platform
     * @see isPlatformX11
     * @since 5.25
     **/
    static bool isPlatformWayland();

    /**
     * Requests an xdg_activation_v1 token for a specific window.
     *
     * @param win window in behalf this request is made
     * @param serial of the event that triggered the request
     * @param app_id identifier of the application that we are launching
     *
     * @see lastInputSerial
     * @since 5.83
     */
    Q_INVOKABLE static void requestXdgActivationToken(QWindow *win, uint32_t serial, const QString &app_id);

    /**
     * Sets the @p token that will be used when activateWindow is called next
     *
     * @since 5.83
     */
    Q_INVOKABLE static void setCurrentXdgActivationToken(const QString &token);

    /**
     * Offers the seat's current serial
     *
     * This will mostly be useful on wayland sessions.
     *
     * @since 5.83
     */
    Q_INVOKABLE static quint32 lastInputSerial(QWindow *window);

Q_SIGNALS:
    /**
     * The state of showing the desktop has changed.
     */
    void showingDesktopChanged(bool showing);

    /**
     * Activation @p token to pass to the client.
     *
     * @see requestXdgActivationToken
     * @see setCurrentXdgActivationToken
     * @since 5.83
     */
    void xdgActivationTokenArrived(int serial, const QString &token);

protected:
    void connectNotify(const QMetaMethod &signal) override;

private:
    friend class KWindowSystemStaticContainer;
    friend class KX11Extras;

    KWindowSystem()
    {
    }
    static KWindowSystemPrivate *d_func();
};

#endif
