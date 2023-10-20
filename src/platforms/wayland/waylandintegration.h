/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef WAYLANDINTEGRATION_H
#define WAYLANDINTEGRATION_H
#include "kwindoweffects_p.h"

#include <QObject>
#include <QPointer>

class WaylandXdgActivationV1;

class WaylandIntegration : public QObject
{
public:
    explicit WaylandIntegration();
    ~WaylandIntegration();

    static WaylandIntegration *self();

    WaylandXdgActivationV1 *activation();

private:
    std::unique_ptr<WaylandXdgActivationV1> m_activation;
    struct {
        quint32 name = 0;
        quint32 version = 0;
    } m_activationInterface;
};

#endif
