/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
