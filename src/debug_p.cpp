/*
 *   Copyright 2015 Christoph Cullmann <cullmann@kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "debug_p.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
// logging category for this framework, default: log stuff >= warning
Q_LOGGING_CATEGORY(LOG_KWINDOWSYSTEM, "org.kde.kwindowsystem", QtWarningMsg)
Q_LOGGING_CATEGORY(LOG_KKEYSERVER_X11, "org.kde.kwindowsystem.keyserver.x11", QtWarningMsg)
#else
Q_LOGGING_CATEGORY(LOG_KWINDOWSYSTEM, "org.kde.kwindowsystem")
Q_LOGGING_CATEGORY(LOG_KKEYSERVER_X11, "org.kde.kwindowsystem.keyserver.x11")
#endif
