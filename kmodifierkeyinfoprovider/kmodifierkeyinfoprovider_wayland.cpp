/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kmodifierkeyinfoprovider_wayland.h"
#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/registry.h>
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
    switch (state) {
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
    switch (key) {
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
