/*
    Copyright 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KMODIFIERKEYINFOPROVIDERWAYLAND_H
#define KMODIFIERKEYINFOPROVIDERWAYLAND_H

#include <kmodifierkeyinfoprovider_p.h>
#include <KWayland/Client/keystate.h>
#include <QPointer>

class KModifierKeyInfoProviderWayland : public KModifierKeyInfoProvider
{
Q_OBJECT
Q_PLUGIN_METADATA(IID "org.kde.kguiaddons.KModifierKeyInfoProvider.Wayland")
public:
    KModifierKeyInfoProviderWayland();
    ~KModifierKeyInfoProviderWayland();

    bool setKeyLatched(Qt::Key key, bool latched) override;
    bool setKeyLocked(Qt::Key key, bool locked) override;

private:
    void updateModifiers(KWayland::Client::Keystate::Key key, KWayland::Client::Keystate::State state);

    QPointer<KWayland::Client::Keystate> m_keystate;
};

#endif
