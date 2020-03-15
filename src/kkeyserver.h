/*
    SPDX-FileCopyrightText: 2001 Ellis Whitehead <ellis@kde.org>

    Win32 port:
    SPDX-FileCopyrightText: 2004 Jarosław Staniek <staniek@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KKEYSERVER_H
#define _KKEYSERVER_H

#include <kwindowsystem_export.h>

#include <qglobal.h>

#include <config-kwindowsystem.h>
#if KWINDOWSYSTEM_HAVE_X11 /*or defined Q_OS_WIN*/
#include "kkeyserver_x11.h"
#elif defined Q_OS_MAC
#include "kkeyserver_mac.h"
#elif defined Q_OS_WIN
#include "kkeyserver_win.h"
#endif

class QString;

/**
 * A collection of functions for the conversion of key presses and
 * their modifiers from the window system specific format
 * to the generic format and vice-versa.
 */
namespace KKeyServer
{
/**
 * Converts the mask of ORed KKey::ModFlag modifiers to a
 * user-readable string.
 * @param mod the mask of ORed KKey::ModFlag modifiers
 * @return the user-readable string (in English)
 */
KWINDOWSYSTEM_EXPORT QString modToStringUser(uint mod);

/**
 * Converts the modifier given as user-readable string (in English)
 * to KKey::ModFlag modifier, or 0.
 * @internal
 */
KWINDOWSYSTEM_EXPORT uint stringUserToMod(const QString &mod);

/**
* Test if the shift modifier should be recorded for a given key.
*
* For example, if shift+5 produces '%' Qt wants ctrl+shift+5 recorded as ctrl+% and
* in that case this function would return false.
*
* @since 4.7.1
*/
KWINDOWSYSTEM_EXPORT bool isShiftAsModifierAllowed(int keyQt);

} // namespace KKeyServer

#endif // !_KKEYSERVER_H
