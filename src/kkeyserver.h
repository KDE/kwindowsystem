/*
    SPDX-FileCopyrightText: 2001 Ellis Whitehead <ellis@kde.org>

    Win32 port:
    SPDX-FileCopyrightText: 2004 Jarosław Staniek <staniek@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KKEYSERVER_H
#define _KKEYSERVER_H

#include <kwindowsystem_export.h>

#include <X11/Xlib.h>
#include <fixx11h.h>
#include <qglobal.h>
#include <xcb/xcb.h>

class QString;

/*!
 * \namespace KKeyServer
 * \inmodule KWindowSystem
 *
 * \brief A collection of functions for the conversion of key presses and
 * their modifiers from the window system specific format
 * to the generic format and vice-versa.
 */
namespace KKeyServer
{
/*!
 * Converts the mask of ORed KKey::ModFlag modifiers to a
 * user-readable string.
 *
 * \a mod the mask of ORed KKey::ModFlag modifiers
 *
 * Returns the user-readable string (in English)
 */
KWINDOWSYSTEM_EXPORT QString modToStringUser(uint mod);

/*!
 * Converts the modifier given as user-readable string (in English)
 * to KKey::ModFlag modifier, or 0.
 * \internal
 */
KWINDOWSYSTEM_EXPORT uint stringUserToMod(const QString &mod);

/*!
 * Test if the shift modifier should be recorded for a given key.
 *
 * For example, if shift+5 produces '%' Qt wants ctrl+shift+5 recorded as ctrl+% and
 * in that case this function would return false.
 *
 * \since 4.7.1
 */
KWINDOWSYSTEM_EXPORT bool isShiftAsModifierAllowed(int keyQt);

static const int MODE_SWITCH = 0x2000;

/*!
 * Initialises the values to return for the mod*() functions below.
 * Called automatically by those functions if not already initialized.
 */
KWINDOWSYSTEM_EXPORT bool initializeMods();

/*!
 * Returns true if the current keyboard layout supports the Meta key.
 * Specifically, whether the Super or Meta keys are assigned to an X modifier.
 * Returns true if the keyboard has a Meta key
 * \sa modXMeta()
 */
KWINDOWSYSTEM_EXPORT bool keyboardHasMetaKey();

/*!
 * Returns the X11 Shift modifier mask/flag.
 * \sa accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXShift();

/*!
 * Returns the X11 Lock modifier mask/flag.
 * \sa accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXLock();

/*!
 * Returns the X11 Ctrl modifier mask/flag.
 * \sa accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXCtrl();

/*!
 * Returns the X11 Alt (Mod1) modifier mask/flag.
 * \sa accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXAlt();

/*!
 * Returns the X11 Win (Mod3) modifier mask/flag.
 * \sa keyboardHasWinKey()
 * \sa accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXMeta();

/*!
 * Returns the X11 NumLock modifier mask/flag.
 * \sa accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXNumLock();

/*!
 * Returns the X11 ScrollLock modifier mask/flag.
 * \sa accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXScrollLock();

/*!
 * Returns the X11 Mode_switch modifier mask/flag.
 * \sa accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXModeSwitch();

/*!
 * Returns bitwise OR'ed mask containing Shift, Ctrl, Alt, and
 * Win (if available).
 * \sa modXShift()
 * \sa modXLock()
 * \sa modXCtrl()
 * \sa modXAlt()
 * \sa modXNumLock()
 * \sa modXWin()
 * \sa modXScrollLock()
 */
KWINDOWSYSTEM_EXPORT uint accelModMaskX();

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(6, 0)
/*!
 * Extracts the symbol from the given Qt key and
 * converts it to an X11 symbol + modifiers.
 *
 * \a keyQt the qt key code
 *
 * \a sym if successful, the symbol will be written here
 *
 * Returns true if successful, false otherwise
 *
 * \deprecated[6.0]
 *
 * Use keyQtToSymXs(keyQt)
 */
KWINDOWSYSTEM_EXPORT
KWINDOWSYSTEM_DEPRECATED_VERSION(6, 0, "Use keyQtToSymXs(int keyQt)")
bool keyQtToSymX(int keyQt, int *sym);
#endif

/*!
 * Extracts the symbols from the given Qt key and
 * converts it to an X11 symbol + modifiers.
 *
 * \a keyQt the qt key code
 *
 * Returns the symbols; emtpy if unsuccessful
 */
KWINDOWSYSTEM_EXPORT QList<int> keyQtToSymXs(int keyQt);

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(6, 0)
/*!
 * Extracts the code from the given Qt key.
 *
 * \a keyQt the qt key code
 *
 * \a keyCode if successful, the symbol will be written here
 *
 * Returns true if successful, false otherwise
 *
 * \deprecated[6.0] Use keyQtToCodeXs(keyQt)
 */
KWINDOWSYSTEM_EXPORT
KWINDOWSYSTEM_DEPRECATED_VERSION(6, 0, "Use keyQtToCodeXs(int keyQt)")
bool keyQtToCodeX(int keyQt, int *keyCode);
#endif

/*!
 * Extracts the codes from the given Qt key.
 *
 * \a keyQt the qt key code
 *
 * \a return the codes; empty if unsuccessful
 */
KWINDOWSYSTEM_EXPORT QList<int> keyQtToCodeXs(int keyQt);

/*!
 * Extracts the modifiers from the given Qt key and
 * converts them in a mask of X11 modifiers.
 *
 * \a keyQt the qt key code
 *
 * \a mod if successful, the modifiers will be written here
 *
 * Returns true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool keyQtToModX(int keyQt, uint *mod);

/*!
 * Converts the given symbol and modifier combination to a Qt key code.
 *
 * \a keySym the X key symbol
 *
 * \a modX the mask of X11 modifiers
 *
 * \a keyQt if successful, the qt key code will be written here
 *
 * Returns true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool symXModXToKeyQt(uint32_t keySym, uint16_t modX, int *keyQt);

/*!
 * Converts the mask of ORed X11 modifiers to
 * a mask of ORed Qt key code modifiers.
 *
 * \a modX the mask of X11 modifiers
 *
 * \a modQt the mask of Qt key code modifiers will be written here
 *        if successful
 *
 * Returns true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool modXToQt(uint modX, int *modQt);

/*!
 * Converts an X keypress event into a Qt key + modifier code
 *
 * \a e the X11 keypress event
 *
 * \a keyModQt the Qt keycode and mask of Qt key code modifiers will be written here
 *        if successful
 *
 * Returns true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool xEventToQt(XEvent *e, int *keyModQt);

/*!
 * Converts an XCB keypress event into a Qt key + modifier code
 *
 * \a e the XCB keypress event
 *
 * \a keyModQt the Qt keycode and mask of Qt key code modifiers will be written here
 *        if successful
 *
 * Returns true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool xcbKeyPressEventToQt(xcb_generic_event_t *e, int *keyModQt);
/*!
 * Overloaded method for convenience.
 */
KWINDOWSYSTEM_EXPORT bool xcbKeyPressEventToQt(xcb_key_press_event_t *e, int *keyModQt);

}; // namespace KKeyServer

#endif // !_KKEYSERVER_H
