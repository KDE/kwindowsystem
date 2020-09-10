/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef WINDOWSHADOW_H
#define WINDOWSHADOW_H

#include <KWayland/Client/buffer.h>
#include <KWayland/Client/shadow.h>
#include <KWindowSystem/private/kwindowshadow_p.h>

class WindowShadowTile final : public KWindowShadowTilePrivate
{
public:
    bool create() override;
    void destroy() override;

    static WindowShadowTile *get(const KWindowShadowTile *tile);

    KWayland::Client::Buffer::Ptr buffer;
};

class WindowShadow final : public KWindowShadowPrivate
{
public:
    bool create() override;
    void destroy() override;

    QPointer<KWayland::Client::Shadow> shadow;
};

#endif // WINDOWSHADOW_H
