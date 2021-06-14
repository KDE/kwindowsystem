/*
    SPDX-FileCopyrightText: 2001-2003 Lubos Lunak <l.lunak@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KXMESSAGES_H
#define KXMESSAGES_H

#include <QObject>
#include <kwindowsystem_export.h>

#include <config-kwindowsystem.h> // KWINDOWSYSTEM_HAVE_X11
#if KWINDOWSYSTEM_HAVE_X11
#include <xcb/xcb.h>
typedef struct _XDisplay Display;

class QString;

class KXMessagesPrivate;

/**
 * Sending string messages to other applications using the X Client Messages.
 *
 * Used internally by KStartupInfo and kstart.
 * You usually don't want to use this, use D-Bus instead.
 *
 * @internal
 *
 * @author Lubos Lunak <l.lunak@kde.org>
 */
class KWINDOWSYSTEM_EXPORT KXMessages : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates an instance which will receive X messages.
     *
     * @param accept_broadcast if non-nullptr, all broadcast messages with
     *                         this message type will be received.
     * @param parent the parent of this widget
     */
    explicit KXMessages(const char *accept_broadcast = nullptr, QObject *parent = nullptr);

    /**
     * @overload
     * Overload passing in the xcb_connection_t to use instead relying on platform xcb.
     *
     * @param connection The xcb connection
     * @param rootWindow The rootWindow to use
     * @param accept_broadcast if non-nullptr, all broadcast messages with
     *                         this message type will be received.
     * @param parent the parent of this object
     * @since 5.8
     **/
    explicit KXMessages(xcb_connection_t *connection, xcb_window_t rootWindow, const char *accept_broadcast = nullptr, QObject *parent = nullptr);

    ~KXMessages() override;
    /**
     * Broadcasts the given message with the given message type.
     * @param msg_type the type of the message
     * @param message the message itself
     * @param screen X11 screen to use, -1 for the default
     */
    void broadcastMessage(const char *msg_type, const QString &message, int screen = -1);
// Not 5.0, as KStartupInfo::sendStartupX uses this, and is only deprecated for 5.18
#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 18)
    /**
     * Broadcasts the given message with the given message type.
     *
     * @param disp X11 connection which will be used instead of
     *             QX11Info::display()
     * @param msg_type the type of the message
     * @param message the message itself
     * @param screen X11 screen to use, -1 for the default
     * @return false when an error occurred, true otherwise
     * @deprecated since 5.0 use xcb variant
     */
    KWINDOWSYSTEM_DEPRECATED_VERSION(
        5,
        0,
        "Use KXMessages::broadcastMessageX(xcb_connection_t *, const char *,                                   const QString &, int)")
    static bool broadcastMessageX(Display *disp, const char *msg_type, const QString &message, int screen = -1);
#endif
    /**
     * Broadcasts the given message with the given message type.
     *
     * @param c X11 connection which will be used instead of
     *             QX11Info::connection()
     * @param msg_type the type of the message
     * @param message the message itself
     * @param screenNumber X11 screen to use
     * @return false when an error occurred, true otherwise
     */
    static bool broadcastMessageX(xcb_connection_t *c, const char *msg_type, const QString &message, int screenNumber);

#if 0 // currently unused
    /**
     * Sends the given message with the given message type only to given
         * window.
         *
         * @param w X11 handle for the destination window
     * @param msg_type the type of the message
     * @param message the message itself
     */
    void sendMessage(WId w, const char *msg_type, const QString &message);
    /**
     * Sends the given message with the given message type only to given
         * window.
         *
     * @param disp X11 connection which will be used instead of
     *             QX11Info::display()
         * @param w X11 handle for the destination window
     * @param msg_type the type of the message
     * @param message the message itself
     * @return false when an error occurred, true otherwise
     */
    static bool sendMessageX(Display *disp, WId w, const char *msg_type,
                             const QString &message);
#endif

Q_SIGNALS:
    /**
     * Emitted when a message was received.
     * @param message the message that has been received
     */
    void gotMessage(const QString &message);

private:
    friend class KXMessagesPrivate;
    KXMessagesPrivate *const d;
};

#endif
#endif
