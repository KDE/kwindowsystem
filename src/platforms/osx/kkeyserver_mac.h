/*
    SPDX-FileCopyrightText: 2006 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KKEYSERVER_MAC_H
#define _KKEYSERVER_MAC_H

#include <kwindowsystem_export.h>

#include <QList>

namespace KKeyServer
{
/**
 * Extracts the symbol from the given Qt key, and converts it to an OSX symbol.
 * @param keyQt the qt key code
 * @param sym if successful, the symbol will be written here
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool keyQtToSymMac(int keyQt, int &sym);

/**
 * Extracts all the scancodes from the given Qt key. The returned values can change if a different
 * keyboard layout is selected.
 * @param keyQt the qt key code
 * @param keyCodes if successful, a list of scancodes will be written here
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool keyQtToCodeMac(int keyQt, QList<uint> &keyCodes);

/**
 * Extracts the modifiers from the given Qt key and converts them in a mask of OSX modifiers.
 * @param keyQt the qt key code
 * @param mod if successful, the modifiers will be written here
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool keyQtToModMac(int keyQt, uint &mod);
}

#endif // !_KKEY_SERVER_MAC_H
