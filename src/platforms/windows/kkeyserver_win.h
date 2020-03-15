/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KKEYSERVER_WIN_H
#define _KKEYSERVER_WIN_H

#include <kwindowsystem_export.h>
#include <QtGlobal>

namespace KKeyServer
{
/**
* Extracts the modifiers from the given Qt key and
* converts them in a mask of Windows modifiers.
* @param keyQt the qt key code
* @param mod if successful, the modifiers will be written here
* @return true if successful, false otherwise
*/
KWINDOWSYSTEM_EXPORT bool keyQtToModWin(int keyQt, uint *mod);

KWINDOWSYSTEM_EXPORT bool modWinToKeyQt(uint mod, int *keyQt);

/**
 * Extracts the symbol from the given Qt key and
 * converts it to a Windows symbol.
 * @param keyQt the qt key code
 * @param sym if successful, the symbol will be written here
 * @return true if successful, false otherwise
 */
KWINDOWSYSTEM_EXPORT bool keyQtToCodeWin(int keyQt, uint *sym);

KWINDOWSYSTEM_EXPORT bool codeWinToKeyQt(uint sym, int *keyQt);
}

#endif
