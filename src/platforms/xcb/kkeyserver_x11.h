/*
    SPDX-FileCopyrightText: 2001 Ellis Whitehead <ellis@kde.org>

    Win32 port:
    SPDX-FileCopyrightText: 2004 Jarosław Staniek <staniek@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KKEYSERVER_X11_H
#define _KKEYSERVER_X11_H

#include <X11/Xlib.h>
#include <fixx11h.h>
#include <kwindowsystem_export.h>
#include <xcb/xcb.h>

namespace KKeyServer
{
static const int MODE_SWITCH = 0x2000;

/**
 * Initialises the values to return for the mod*() functions below.
 * Called automatically by those functions if not already initialized.
 */
KWINDOWSYSTEM_EXPORT bool initializeMods();

/**
 * Returns true if the current keyboard layout supports the Meta key.
 * Specifically, whether the Super or Meta keys are assigned to an X modifier.
 * @return true if the keyboard has a Meta key
 * @see modXMeta()
 */
KWINDOWSYSTEM_EXPORT bool keyboardHasMetaKey();

/**
 * Returns the X11 Shift modifier mask/flag.
 * @return the X11 Shift modifier mask/flag.
 * @see accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXShift();

/**
 * Returns the X11 Lock modifier mask/flag.
 * @return the X11 Lock modifier mask/flag.
 * @see accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXLock();

/**
 * Returns the X11 Ctrl modifier mask/flag.
 * @return the X11 Ctrl modifier mask/flag.
 * @see accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXCtrl();

/**
 * Returns the X11 Alt (Mod1) modifier mask/flag.
 * @return the X11 Alt (Mod1) modifier mask/flag.
 * @see accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXAlt();

/**
 * Returns the X11 Win (Mod3) modifier mask/flag.
 * @return the X11 Win (Mod3) modifier mask/flag.
 * @see keyboardHasWinKey()
 * @see accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXMeta();

/**
 * Returns the X11 NumLock modifier mask/flag.
 * @return the X11 NumLock modifier mask/flag.
 * @see accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXNumLock();

/**
 * Returns the X11 ScrollLock modifier mask/flag.
 * @return the X11 ScrollLock modifier mask/flag.
 * @see accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXScrollLock();

/**
 * Returns the X11 Mode_switch modifier mask/flag.
 * @return the X11 Mode_switch modifier mask/flag.
 * @see accelModMaskX()
 */
KWINDOWSYSTEM_EXPORT uint modXModeSwitch();

/**
 * Returns bitwise OR'ed mask containing Shift, Ctrl, Alt, and
 * Win (if available).
 * @see modXShift()
 * @see modXLock()
 * @see modXCtrl()
 * @see modXAlt()
 * @see modXNumLock()
 * @see modXWin()
 * @see modXScrollLock()
 */
KWINDOWSYSTEM_EXPORT uint accelModMaskX();

/**
 * Extracts the symbol from the given Qt key and
 * converts it to an X11 symbol + modifiers.
 * @param keyQt the qt key code
 * @param sym if successful, the symbol will be written here
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool keyQtToSymX(int keyQt, int *sym);

/**
 * Extracts the code from the given Qt key.
 * @param keyQt the qt key code
 * @param keyCode if successful, the symbol will be written here
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool keyQtToCodeX(int keyQt, int *keyCode);

/**
 * Extracts the modifiers from the given Qt key and
 * converts them in a mask of X11 modifiers.
 * @param keyQt the qt key code
 * @param mod if successful, the modifiers will be written here
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool keyQtToModX(int keyQt, uint *mod);

#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 38)
/**
 * Converts the given symbol to a Qt key code.
 * @param sym the symbol
 * @param keyQt if successful, the qt key code will be written here
 * @return true if successful, false otherwise
 * @deprecated since 5.38, use symXModXToKeyQt in order to support numeric keypad keys too
 */
KWINDOWSYSTEM_EXPORT
KWINDOWSYSTEM_DEPRECATED_VERSION(5, 38, "Use KKeyServer::symXModXToKeyQt(uint32_t, uint16_t, int *)")
bool symXToKeyQt(uint sym, int *keyQt);
#endif

/**
 * Converts the given symbol and modifier combination to a Qt key code.
 * @param keySym the X key symbol
 * @param modX the mask of X11 modifiers
 * @param keyQt if successful, the qt key code will be written here
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool symXModXToKeyQt(uint32_t keySym, uint16_t modX, int *keyQt);

/**
 * Converts the mask of ORed X11 modifiers to
 * a mask of ORed Qt key code modifiers.
 * @param modX the mask of X11 modifiers
 * @param modQt the mask of Qt key code modifiers will be written here
 *        if successful
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool modXToQt(uint modX, int *modQt);

/**
 * Converts an X keypress event into a Qt key + modifier code
 * @param e the X11 keypress event
 * @param keyModQt the Qt keycode and mask of Qt key code modifiers will be written here
 *        if successful
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool xEventToQt(XEvent *e, int *keyModQt);

/**
 * Converts an XCB keypress event into a Qt key + modifier code
 * @param e the XCB keypress event
 * @param keyModQt the Qt keycode and mask of Qt key code modifiers will be written here
 *        if successful
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool xcbKeyPressEventToQt(xcb_generic_event_t *e, int *keyModQt);
/**
 * Overloaded method for convenience.
 */
KWINDOWSYSTEM_EXPORT bool xcbKeyPressEventToQt(xcb_key_press_event_t *e, int *keyModQt);

}

#endif // !_KKEYSERVER_X11_H
