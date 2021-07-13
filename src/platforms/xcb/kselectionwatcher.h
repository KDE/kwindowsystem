/*
    SPDX-FileCopyrightText: 2003 Lubos Lunak <l.lunak@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KSELECTIONWATCHER_H
#define KSELECTIONWATCHER_H

#include <QObject>
#include <kwindowsystem_export.h>

#include <xcb/xcb.h>
#include <xcb/xproto.h>

/**
 This class implements watching manager selections, as described in the ICCCM
 section 2.8. It emits signal newOwner() when a new owner claim the selection,
 and emits lostOwner() when the selection ownership is given up. To find
 out current owner of the selection, owner() can be used.
 @short ICCCM manager selection watching

 This class is only useful on the xcb platform. On other platforms the code is only
 functional if the constructor overloads taking an xcb_connection_t are used. In case
 you inherit from this class ensure that you don't use xcb and/or XLib without verifying
 the platform.
*/
class KWINDOWSYSTEM_EXPORT KSelectionWatcher : public QObject
{
    Q_OBJECT
public:
    /**
     * This constructor initializes the object, but doesn't perform any
     * operation on the selection.
     *
     * @param selection atom representing the manager selection
     * @param screen X screen, or -1 for default
     * @param parent parent object, or nullptr if there is none
     */
    explicit KSelectionWatcher(xcb_atom_t selection, int screen = -1, QObject *parent = nullptr);
    /**
     * @overload
     * This constructor accepts the selection name and creates the appropriate atom
     * for it automatically.
     *
     * @param selection name of the manager selection
     * @param screen X screen, or -1 for default
     * @param parent parent object, or nullptr if there is none
     */
    explicit KSelectionWatcher(const char *selection, int screen = -1, QObject *parent = nullptr);
    /**
     * @overload
     * This constructor accepts the xcb_connection_t and root window and doesn't depend on
     * running on the xcb platform. Otherwise this constructor behaves like the similar one
     * without the xcb_connection_t.
     *
     * @param selection atom representing the manager selection
     * @param c the xcb connection this KSelectionWatcher should use
     * @param root the root window this KSelectionWatcher should use
     * @since 5.8
     **/
    explicit KSelectionWatcher(xcb_atom_t selection, xcb_connection_t *c, xcb_window_t root, QObject *parent = nullptr);
    /**
     * @overload
     * This constructor accepts the xcb_connection_t and root window and doesn't depend on
     * running on the xcb platform. Otherwise this constructor behaves like the similar one
     * without the xcb_connection_t.
     *
     * @param selection name of the manager selection
     * @param c the xcb connection this KSelectionWatcher should use
     * @param root the root window this KSelectionWatcher should use
     * @since 5.8
     **/
    explicit KSelectionWatcher(const char *selection, xcb_connection_t *c, xcb_window_t root, QObject *parent = nullptr);
    ~KSelectionWatcher() override;
    /**
     * Return the current owner of the manager selection, if any. Note that if the event
     * informing about the owner change is still in the input queue, newOwner() might
     * have been emitted yet.
     */
    xcb_window_t owner();
    /**
     * @internal
     */
    void filterEvent(void *ev_P); // internal
Q_SIGNALS:
    /**
     * This signal is emitted when the selection is successfully claimed by a new
     * owner.
     * @param owner the new owner of the selection
     */
    void newOwner(xcb_window_t owner);
    /**
     * This signal is emitted when the selection is given up, i.e. there's no
     * owner. Note that the selection may be immediately claimed again,
     * so the newOwner() signal may be emitted right after this one.
     * It's safe to delete the instance in a slot connected to this signal.
     */
    void lostOwner();

private:
    void init();

    class Private;
    Private *const d;
};

Q_DECLARE_METATYPE(xcb_window_t)

#endif
