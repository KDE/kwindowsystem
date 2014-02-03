/*
 *   Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef KWINDOWINFO_P_X11_H
#define KWINDOWINFO_P_X11_H
#include "kwindowinfo_p.h"
#include <QScopedPointer>

class NETWinInfo;

class KWindowInfoPrivateX11 : public KWindowInfoPrivate
{
public:
    KWindowInfoPrivateX11(WId window, NET::Properties properties, NET::Properties2 properties2);
    ~KWindowInfoPrivateX11();

    bool valid(bool withdrawn_is_valid) const;
    NET::States state() const;
    bool isMinimized() const;
    NET::MappingState mappingState() const;
    NETExtendedStrut extendedStrut() const;
    NET::WindowType windowType(NET::WindowTypes supported_types) const;
    QString visibleName() const;
    QString visibleNameWithState() const;
    QString name() const;
    QString visibleIconName() const;
    QString visibleIconNameWithState() const;
    QString iconName() const;
    bool onAllDesktops() const;
    bool isOnDesktop(int desktop) const;
    int desktop() const;
    QRect geometry() const;
    QRect frameGeometry() const;
    WId transientFor() const;
    WId groupLeader() const;
    QByteArray windowClassClass() const;
    QByteArray windowClassName() const;
    QByteArray windowRole() const;
    QByteArray clientMachine() const;
    bool actionSupported(NET::Action action) const;

private:
    QScopedPointer<NETWinInfo> m_info;
    QString m_name;
    QString m_iconic_name;
    QRect m_geometry;
    QRect m_frame_geometry;
    bool m_valid;
};

#endif
