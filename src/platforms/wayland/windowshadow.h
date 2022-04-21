/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef WINDOWSHADOW_H
#define WINDOWSHADOW_H

#include <KWayland/Client/buffer.h>
#include <KWayland/Client/shadow.h>
#include <private/kwindowshadow_p.h>

class WindowShadowTile final : public KWindowShadowTilePrivate
{
public:
    WindowShadowTile();
    ~WindowShadowTile();

    bool create() override;
    void destroy() override;

    static WindowShadowTile *get(const KWindowShadowTile *tile);

    KWayland::Client::Buffer::Ptr buffer;
    QScopedPointer<KWayland::Client::ShmPool> m_shmPool;
};

class WindowShadow final : public QObject, public KWindowShadowPrivate
{
public:
    bool create() override;
    void destroy() override;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool internalCreate();
    void internalDestroy();

    QPointer<KWayland::Client::Shadow> shadow;
};

#endif // WINDOWSHADOW_H
