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

public:
    /**
     * Access to the singleton instance. Useful mainly for connecting to signals.
     */
    static KWindowSystem *self();

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Returns the list of all toplevel windows currently managed by the
     * window manager in the order of creation. Please do not rely on
     * indexes of this list: Whenever you enter Qt's event loop in your
     * application, it may happen that entries are removed or added.
     * Your module should perhaps work on a copy of this list and verify a
     * window with hasWId() before any operations.
     *
     * Iteration over this list can be done easily with
     * \code
     *  QList<WId> windows = KWindowSystem::windows();
     *  for (auto it = windows.cbegin(), end = windows.cend(); it != end; ++it) {
     *     ... do something here,  (*it) is the current WId.
     *  }
     * \endcode
     * @return the list of all toplevel windows
     * @deprecated since 5.101, use KX11Extras::windows()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::windows() instead")
    static QList<WId> windows();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Test to see if @p id still managed at present.
     * @param id the window id to test
     * @return true if the window id is still managed
     * @deprecated since 5.101, use KX11Extras::hasWId()
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::hasWId() instead")
    static bool hasWId(WId id);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Returns information about window @p win. It is recommended to check
     * whether the returned info is valid by calling the valid() method.
     * @param win the id of the window
     * @param properties all properties that should be retrieved (see NET::Property
     *    enum for details). Unlisted properties cause related information to be invalid
     *    in the returned data, but make this function faster when not all data is needed.
     * @param properties2 additional properties (see NET::Property2 enum)
     * @return the window information
     * @deprecated Since 5.0, use KWindowInfo directly
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 0, "Use KWindowInfo(WId, NET::Properties, NET::Properties2")
    static KWindowInfo windowInfo(WId win, NET::Properties properties, NET::Properties2 properties2 = NET::Properties2());
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Returns the list of all toplevel windows currently managed by the
     * window manager in the current stacking order (from lower to
     * higher). May be useful for pagers.
     * @return the list of all toplevel windows in stacking order
     * @deprecated since 5.101, use KX11Extras::stackingOrder()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::stackingOrder() instead")
    static QList<WId> stackingOrder();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Returns the currently active window, or 0 if no window is active.
     * @return the window id of the active window, or 0 if no window is
     *  active
     * @deprecated since 5.101, use KX11Extras::activeWindow()
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::activeWindow() instead")
    static WId activeWindow();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
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
     * @param win the id of the window to make active
     * @param time X server timestamp of the user activity that
     *    caused this request
     * @deprecated since 5.101, use KX11Extras::activateWindow() or KWindowSystem::activateWindow(QWindow *) instead
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::activateWindow() or KWindowSystem::activateWindow(QWindow *) instead")
    static void activateWindow(WId win, long time = 0);
#endif

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

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Sets window @p win to be the active window. Note that this
     * should be called only in special cases, applications
     * shouldn't force themselves or other windows to be the active
     * window. Generally, this call should used only by pagers
     * and similar tools. See the explanation in description
     * of activateWindow().
     *
     * @param win the id of the window to make active
     * @param time X server timestamp of the user activity that
     *    caused this request
     * @deprecated since 5.101, use KX11Extras::forceActiveWindow()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::forceActiveWindow() instead")
    static void forceActiveWindow(WId win, long time = 0);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * When application finishes some operation and wants to notify
     * the user about it, it can call demandAttention(). Instead
     * of activating the window, which could be obtrusive, the window
     * will be marked specially as demanding user's attention.
     * See also explanation in description of activateWindow().
     *
     * Note that it's usually better to use KNotifyClient.
     * @deprecated since 5.101, use QWindow::alert() instead().
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use QWindow::alert()")
    static void demandAttention(WId win, bool set = true);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Returns true if a compositing manager is running (i.e. ARGB windows
     * are supported, effects will be provided, etc.).
     * @deprecated since 5.101, use KX11Extras::compositingActive()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::compositingActive() instead")
    static bool compositingActive();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Returns the current virtual desktop.
     * @return the current virtual desktop
     * @deprecated since 5.101, use KX11Extras::currentDesktop()
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::currentDesktop() instead")
    static int currentDesktop();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Returns the number of virtual desktops.
     * @return the number of virtual desktops
     * @deprecated since 5.101, use KX11Extras::numberOfDesktops()
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::numberOfDesktops() instead")
    static int numberOfDesktops();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Convenience function to set the current desktop to @p desktop.
     * See NETRootInfo.
     * @param desktop the number of the new desktop
     * @deprecated since 5.101, use KX11Extras::setCurrentDesktop()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::setCurrentDesktop() instead")
    static void setCurrentDesktop(int desktop);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Sets window @p win to be present on all virtual desktops if @p
     * is true. Otherwise the window lives only on one single desktop.
     *
     * @param win the id of the window
     * @param b true to show the window on all desktops, false
     *          otherwise
     * @deprecated since 5.101, use KX11Extras::setOnAllDesktops()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::setOnAllDesktops() instead")
    static void setOnAllDesktops(WId win, bool b);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Moves window @p win to desktop @p desktop.
     *
     * @param win the id of the window
     * @param desktop the number of the new desktop
     * @deprecated since 5.101, use KX11Extras::setOnDesktop()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::setOnDesktop() instead")
    static void setOnDesktop(WId win, int desktop);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Moves window @p win to activities @p activities.
     *
     * @param win the id of the window
     * @param activities the list of activity UUIDs
     *
     * @since 5.1
     * @see KWindowInfo::activities
     * @deprecated since 5.101, use KX11Extras::setOnActivities()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::setOnActivities() instead")
    static void setOnActivities(WId win, const QStringList &activities);
#endif

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

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 62)
    /**
     * Sets the parent window of @p subwindow to be @p mainwindow.
     * This overrides the parent set the usual way as the QWidget parent,
     * but only for the window manager - e.g. stacking order and window grouping
     * will be affected, but features like automatic deletion of children
     * when the parent is deleted are unaffected and normally use
     * the QWidget parent.
     *
     * This function should be used before a dialog is shown for a window
     * that belongs to another application.
     * @deprecated since 5.62, use setMainWindow(QWindow *). If all you have is a QWidget*,
     * you might need to call setAttribute(Qt::WA_NativeWindow, true); before calling
     * >window()->windowHandle().
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 62, "Use KWindowSystem::setMainWindow(QWindow *)")
    static void setMainWindow(QWidget *subwindow, WId mainwindow);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Returns the WM_TRANSIENT_FOR property for the given window, i.e. the mainwindow
     * for this window.
     *
     * @param window the id of the window
     * @deprecated Since 5.0, use KWindowInfo::transientFor
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 0, "Use KWindowInfo::transientFor()")
    static WId transientFor(WId window);

    /**
     * Returns the leader window for the group the given window is in, if any.
     * @param window the id of the window
     * @deprecated Since 5.0, use KWindowInfo::groupLeader
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 0, "Use KWindowInfo::groupLeader()")
    static WId groupLeader(WId window);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Returns an icon for window @p win.
     *
     * If  @p width and @p height are specified, the best icon for the requested
     * size is returned.
     *
     * If @p scale is true, the icon is smooth-scaled to have exactly
     * the requested size.
     *
     * @param win the id of the window
     * @param width the desired width, or -1
     * @param height the desired height, or -1
     * @param scale if true the icon will be scaled to the desired size. Otherwise the
     *        icon will not be modified.
     * @return the icon of the window
     * @deprecated since 5.101, use KX11Extras::icon()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::icon() instead")
    static QPixmap icon(WId win, int width = -1, int height = -1, bool scale = false);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Masks specifying from which sources to read an icon. They are tried from the best
     * until an icon is found.
     * @li NETWM from property from the window manager specification
     * @li WMHints from WMHints property
     * @li ClassHint load icon after getting name from the classhint
     * @li XApp load the standard X icon (last fallback)
     */
    enum IconSource {
        NETWM = 1, //!< read from property from the window manager specification
        WMHints = 2, //!< read from WMHints property
        ClassHint = 4, //!< load icon after getting name from the classhint
        XApp = 8, //!< load the standard X icon (last fallback)
    };
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * @overload
     *
     * Overloaded variant that allows specifying from which sources the icon should be read.
     * You should usually prefer the simpler variant which tries all possibilities to get
     * an icon.
     *
     * @param win the id of the window
     * @param width the desired width, or -1
     * @param height the desired height, or -1
     * @param scale if true the icon will be scaled to the desired size. Otherwise the
     *        icon will not be modified.
     * @param flags OR-ed flags from the IconSource enum
     * @deprecated since 5.101, use KX11Extras::icon()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::icon() instead")
    static QPixmap icon(WId win, int width, int height, bool scale, int flags);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * @overload
     *
     * Overloaded variant that allows passing in the NETWinInfo to use for reading the
     * information. This variant is only useful on the X11 platform, other platforms do not
     * use NETWinInfo and delegate to the variant without NETWinInfo. Though if compiled with
     * X11 support the X11 variant is used on other platforms if info is not @c nullptr.
     * This can be used by applications using e.g. platform wayland but also connecting to an
     * XServer.
     *
     * The NETWinInfo must be constructed with property NET::WMIcon in order to use the
     * IconSource flag NETWM. NET::WM2IconPixmap for IconSource flag WMHints and
     * NET::WM2WindowClass for IconSource flag ClassHint.
     *
     * @param win the id of the window
     * @param width the desired width, or -1
     * @param height the desired height, or -1
     * @param scale if true the icon will be scaled to the desired size. Otherwise the
     *        icon will not be modified.
     * @param flags OR-ed flags from the IconSource enum
     * @param into the NETWinInfo to use for reading properties.
     * @since 5.7
     * @deprecated since 5.101, use KX11Extras::icon()
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::icon() instead")
    static QPixmap icon(WId win, int width, int height, bool scale, int flags, NETWinInfo *info);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Sets an @p icon and a  @p miniIcon on window @p win
     * @param win the id of the window
     * @param icon the new icon
     * @param miniIcon the new mini icon
     * @deprecated since 5.101, use QWindow::setIcon() or QGuiApplication::setWindowIcon()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use QWindow::setIcon() or QGuiApplication::setWindowIcon()")
    static void setIcons(WId win, const QPixmap &icon, const QPixmap &miniIcon);
#endif

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

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Minimizes the window with id @p win.
     * On X11 this follows the protocol described in ICCCM section 4.1.4.
     *
     * @param win The window to minimize
     * @see unminimizeWindow()
     * @deprecated since 5.101, use KX11Extras::minimizeWindow() or QWindow::setState()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::minimizeWindow() or QWindow::setState() instead")
    static void minimizeWindow(WId win);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Unminimizes the window with id @p win.
     * On X11 this follows the protocol described in ICCCM section 4.1.4.
     *
     * @param win The window to unminimize
     * @see minimizeWindow()
     * @deprecated since 5.101, use KX11Extras::unminimizeWindow() or QWindow::setState()
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::unminimizeWindow() or QWindow::setState() instead")
    static void unminimizeWindow(WId win);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * @deprecated since 5.0 the @p animation is ignored.
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 0, "Use KWindowSystem::minimizeWindow(WId)")
    static void minimizeWindow(WId win, bool animation);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * @deprecated since 5.0 the @p animation is ignored.
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 0, "Use KWindowSystem::unminimizeWindow(WId)")
    static void unminimizeWindow(WId win, bool animation);
#endif

    /**
     * Raises the given window. This call is only for pagers and similar
     * tools that represent direct user actions. Applications should not
     * use it, they should keep using QWidget::raise() or XRaiseWindow()
     * if necessary.
     */
    static void raiseWindow(WId win);

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Lowers the given window. This call is only for pagers and similar
     * tools that represent direct user actions. Applications should not
     * use it, they should keep using QWidget::lower() or XLowerWindow()
     * if necessary.
     *
     * @deprecated since 5.101, no known users.
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "No known users.")
    static void lowerWindow(WId win);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * @internal
     * Returns true if the WM uses IconicState also for windows
     * on inactive virtual desktops.
     *
     * @deprecated since 5.101, internal.
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Internal")
    static bool icccmCompliantMappingState();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Returns the workarea for the specified desktop, or the current
     * work area if no desktop has been specified.
     * @param desktop the number of the desktop to check, -1 for the
     *        current desktop
     * @return the size and position of the desktop
     * @deprecated since 5.101, use KX11Extras::workArea()
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::workArea() instead")
    static QRect workArea(int desktop = -1);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Returns the workarea for the specified desktop, or the current
     * work area if no desktop has been specified. Excludes struts of
     * clients in the exclude List.
     *
     * @param excludes the list of clients whose struts will be excluded
     * @param desktop the number of the desktop to check, -1 for the
     *        current desktop
     * @return the size and position of the desktop
     * @deprecated since 5.101, use KX11Extras::workArea()
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::workArea() instead")
    static QRect workArea(const QList<WId> &excludes, int desktop = -1);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Returns the name of the specified desktop.
     * @param desktop the number of the desktop
     * @return the name of the desktop
     * @deprecated since 5.101, use KX11Extras::desktopName()
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::desktopName() instead")
    static QString desktopName(int desktop);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Sets the name of the specified desktop.
     * @param desktop the number of the desktop
     * @param name the new name for the desktop
     * @deprecated since 5.101, use KX11Extras::setDesktopName()
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::setDesktopName() instead")
    static void setDesktopName(int desktop, const QString &name);
#endif

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

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Sets user timestamp @p time on window @p win. The timestamp
     * is expressed as XServer time. If a window
     * is shown with user timestamp older than the time of the last
     * user action, it won't be activated after being shown.
     * The most common case is the special value 0 which means
     * not to activate the window after being shown.
     *
     * @deprecated since 5.101, use QX11Info::setAppUserTime().
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use QX11Info::setAppUserTime()")
    static void setUserTime(WId win, long time);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Sets the strut of window @p win to @p left_width
     * ranging from @p left_start to @p left_end on the left edge,
     * and simiarly for the other edges. For not reserving a strut, pass 0 as the width.
     * E.g. to reserve 10x10 square in the topleft corner, use e.g.
     * setExtendedStrut( w, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 ).
     *
     * @param win the id of the window
     * @param left_width width of the strut at the left edge
     * @param left_start starting y coordinate of the strut at the left edge
     * @param left_end ending y coordinate of the strut at the left edge
     * @param right_width width of the strut at the right edge
     * @param right_start starting y coordinate of the strut at the right edge
     * @param right_end ending y coordinate of the strut at the right edge
     * @param top_width width of the strut at the top edge
     * @param top_start starting x coordinate of the strut at the top edge
     * @param top_end ending x coordinate of the strut at the top edge
     * @param bottom_width width of the strut at the bottom edge
     * @param bottom_start starting x coordinate of the strut at the bottom edge
     * @param bottom_end ending x coordinate of the strut at the bottom edge
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::setExtendedStrut() instead")
    static void setExtendedStrut(WId win,
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
                                 int bottom_end);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Convenience function for setExtendedStrut() that automatically makes struts
     * as wide/high as the screen width/height.
     * Sets the strut of window @p win to @p left, @p right, @p top, @p bottom.
     *
     * @param win the id of the window
     * @param left the left strut
     * @param right the right strut
     * @param top the top strut
     * @param bottom the bottom strut
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::setStrut() instead")
    static void setStrut(WId win, int left, int right, int top, int bottom);
#endif

    /**
     * Returns true if the WM announces which actions it allows for windows.
     */
    static bool allowedActionsSupported();

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Function that reads and returns the contents of the given text
     * property (WM_NAME, WM_ICON_NAME,...).
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::readNameProperty() instead")
    static QString readNameProperty(WId window, unsigned long atom);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 104)
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
     *
     * @deprecated since 5.104, not implemented.
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 104, "Not implemented");
    static void allowExternalProcessWindowActivation(int pid = -1);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Sets whether the client wishes to block compositing (for better performance)
     * @since 4.7
     * @deprecated since 5.101, no known users.
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "No known users")
    static void setBlockingCompositing(WId window, bool active);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * @internal
     * Returns true if viewports are mapped to virtual desktops.
     */
    static bool mapViewport();
#endif

    /**
     * @internal
     * Returns mapped virtual desktop for the given position in the viewport.
     */
    static int viewportToDesktop(const QPoint &pos);
    /**
     * @internal
     * Returns mapped virtual desktop for the given window geometry.
     */
    static int viewportWindowToDesktop(const QRect &r);
    /**
     * @internal
     * Returns topleft corner of the viewport area for the given mapped virtual desktop.
     */
    static QPoint desktopToViewport(int desktop, bool absolute);

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * @internal
     * @since 4.0.1
     * Checks the relative difference used to move a window will still be inside
     * valid desktop area.
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "No known users")
    static QPoint constrainViewportRelativePosition(const QPoint &pos);
#endif

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

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Switched to another virtual desktop.
     * @param desktop the number of the new desktop
     * @deprecated since 5.101, use KX11Extras::currentDesktopChanged()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::currentDesktopChanged()")
    void currentDesktopChanged(int desktop);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * A window has been added.
     * @param id the id of the window
     * @deprecated since 5.101, use KX11Extras::windowAdded()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::windowAdded()")
    void windowAdded(WId id);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * A window has been removed.
     * @param id the id of the window that has been removed
     * @deprecated since 5.101, use KX11Extras::windowRemoved()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::windowRemoved()")
    void windowRemoved(WId id);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Hint that \<Window> is active (= has focus) now.
     * @param id the id of the window that is active
     * @deprecated since 5.101, use KX11Extras::activeWindowChanged()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::activeWindowChanged()")
    void activeWindowChanged(WId id);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Desktops have been renamed.
     * @deprecated since 5.101, use KX11Extras::desktopNamesChanged()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::desktopNamesChanged()")
    void desktopNamesChanged();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * The number of desktops changed.
     * @param num the new number of desktops
     * @deprecated since 5.101, use KX11Extras::numberOfDesktopsChanged()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::numberOfDesktopsChanged()")
    void numberOfDesktopsChanged(int num);
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * The workarea has changed.
     * @deprecated since 5.101, use KX11Extras::workAreaChanged()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::workAreaChanged()")
    void workAreaChanged();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Something changed with the struts, may or may not have changed
     * the work area. Usually just using the workAreaChanged() signal
     * is sufficient.
     * @deprecated since 5.101, use KX11Extras::strutChanged()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::strutChanged()")
    void strutChanged();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Emitted when the stacking order of the window changed. The new order
     * can be obtained with stackingOrder().
     * @deprecated since 5.101, use KX11Extras::stackingOrderChanged()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::stackingOrderChanged()")
    void stackingOrderChanged();
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * The window changed.
     *
     * Carries the NET::Properties and NET::Properties2 that were changed.
     *
     * @param id the id of the window
     * @param properties the properties that were modified
     * @param properties2 the properties2 that were modified
     *
     * @since 5.0
     * @deprecated since 5.101, use KX11Extras::windowChanged()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::windowChanged")
    void windowChanged(WId id, NET::Properties properties, NET::Properties2 properties2); // clazy:exclude=overloaded-signal
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * The window changed.
     *
     * The properties parameter contains the NET properties that
     * were modified (see netwm_def.h). First element are NET::Property
     * values, second element are NET::Property2 values (i.e. the format
     * is the same like for the NETWinInfo class constructor).
     * @param id the id of the window
     * @param properties the properties that were modified
     *
     * @deprecated since 5.0 use windowChanged(WId, NET::Properties, NET::Properties2)
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 0, "Use KWindowSystem::windowChanged(WId, NET::Properties, NET::Properties2)")
    QT_MOC_COMPAT void windowChanged(WId id, const unsigned long *properties); // clazy:exclude=overloaded-signal
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * The window changed.
     *
     * The unsigned int parameter contains the NET properties that
     * were modified (see netwm_def.h).
     * @param id the id of the window
     * @param properties the properties that were modified
     * @deprecated Since 5.0
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 0, "Use KWindowSystem::windowChanged(WId, NET::Properties, NET::Properties2)")
    QT_MOC_COMPAT void windowChanged(WId id, unsigned int properties); // clazy:exclude=overloaded-signal
#endif

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 80)
    /**
     * The window changed somehow.
     * @param id the id of the window
     *
     * @deprecated since 5.80, use windowChanged(WId, NET::Properties, NET::Properties2);
     **/
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 80, "Use KWindowSystem::windowChanged(WId, NET::Properties, NET::Properties2)")
    void windowChanged(WId id); // clazy:exclude=overloaded-signal
#endif

    /**
     * The state of showing the desktop has changed.
     */
    void showingDesktopChanged(bool showing);

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 101)
    /**
     * Compositing was enabled or disabled.
     *
     * Note that this signal may be emitted before any compositing plugins
     * have been initialized in the window manager.
     *
     * If you need to check if a specific compositing plugin such as the
     * blur effect is enabled, you should track that separately rather
     * than test for it in a slot connected to this signal.
     *
     * @since 4.7.1
     * @deprecated since 5.101, use KX11Extras::compositingChanged()
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(5, 101, "Use KX11Extras::compositingChanged()")
    void compositingChanged(bool enabled);
#endif

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
