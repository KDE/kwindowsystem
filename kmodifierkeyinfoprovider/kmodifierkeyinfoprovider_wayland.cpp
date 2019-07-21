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
    but WITHOUT ANY WARRAKModifierKeyInfoProviderNTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kmodifierkeyinfoprovider_wayland.h"
#include <KWayland/Client/registry.h>
#include <KWayland/Client/connection_thread.h>
#include <QDebug>
#include <QTimer>

using namespace KWayland::Client;

KModifierKeyInfoProviderWayland::KModifierKeyInfoProviderWayland()
{
    auto registry = new Registry(this);

    auto m_waylandConnection = ConnectionThread::fromApplication(this);
    if (!m_waylandConnection) {
        qWarning() << "Failed getting Wayland connection from QPA";
        return;
    }
    registry->create(m_waylandConnection);
    registry->setup();

    connect(registry, &Registry::keystateAnnounced, this, [this, registry](quint32 name, quint32 version) {
        m_keystate = registry->createKeystate(name, version, this);
        connect(m_keystate, &Keystate::stateChanged, this, &KModifierKeyInfoProviderWayland::updateModifiers);
        m_keystate->fetchStates();
    });

    stateUpdated(Qt::Key_CapsLock, KModifierKeyInfoProvider::Nothing);
    stateUpdated(Qt::Key_NumLock, KModifierKeyInfoProvider::Nothing);
    stateUpdated(Qt::Key_ScrollLock, KModifierKeyInfoProvider::Nothing);
}

KModifierKeyInfoProviderWayland::~KModifierKeyInfoProviderWayland() = default;

bool KModifierKeyInfoProviderWayland::setKeyLatched(Qt::Key /*key*/, bool /*latched*/)
{
    return false;
}

bool KModifierKeyInfoProviderWayland::setKeyLocked(Qt::Key /*key*/, bool /*locked*/)
{
    return false;
}

KModifierKeyInfoProvider::ModifierState toState(Keystate::State state)
{
    switch(state) {
        case Keystate::State::Unlocked:
            return KModifierKeyInfoProvider::Nothing;
        case Keystate::State::Latched:
            return KModifierKeyInfoProvider::Latched;
        case Keystate::State::Locked:
            return KModifierKeyInfoProvider::Locked;
    }
    Q_UNREACHABLE();
    return KModifierKeyInfoProvider::Nothing;
}

Qt::Key toKey(Keystate::Key key)
{
    switch(key) {
        case Keystate::Key::CapsLock:
            return Qt::Key_CapsLock;
        case Keystate::Key::NumLock:
            return Qt::Key_NumLock;
        case Keystate::Key::ScrollLock:
            return Qt::Key_ScrollLock;
    }
    Q_UNREACHABLE();
    return {};
}

void KModifierKeyInfoProviderWayland::updateModifiers(Keystate::Key key, Keystate::State state)
{
    stateUpdated(toKey(key), toState(state));
}
