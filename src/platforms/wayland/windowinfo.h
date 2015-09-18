/*
 *   Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
 *   Copyright 2015 Marco Martin <mart@kde.org>
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
#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <kwindowinfo.h>
#include <KWindowSystem/private/kwindowinfo_p.h>


namespace KWayland
{
    namespace Client
    {
        class Surface;
        class PlasmaShellSurface;
    }
}

class WindowInfo : public KWindowInfoPrivate
{
public:
    WindowInfo(WId window, NET::Properties properties, NET::Properties2 properties2);
    ~WindowInfo();

    bool valid(bool withdrawn_is_valid) const override;
    NET::States state() const override;
    bool isMinimized() const override;
    NET::MappingState mappingState() const override;
    NETExtendedStrut extendedStrut() const override;
    NET::WindowType windowType(NET::WindowTypes supported_types) const override;
    QString visibleName() const override;
    QString visibleNameWithState() const override;
    QString name() const override;
    QString visibleIconName() const override;
    QString visibleIconNameWithState() const override;
    QString iconName() const override;
    bool onAllDesktops() const override;
    bool isOnDesktop(int desktop) const override;
    int desktop() const override;
    QStringList activities() const override;
    QRect geometry() const override;
    QRect frameGeometry() const override;
    WId transientFor() const override;
    WId groupLeader() const override;
    QByteArray windowClassClass() const override;
    QByteArray windowClassName() const override;
    QByteArray windowRole() const override;
    QByteArray clientMachine() const override;
    bool actionSupported(NET::Action action) const override;

private:
    bool m_valid;
    NET::Properties m_properties;
    NET::Properties2 m_properties2;
    KWayland::Client::Surface *m_surface;
    KWayland::Client::PlasmaShellSurface *m_plasmaShellSurface;
};

#endif
