/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KWINDOWINFO_DUMMY_P_H
#define KWINDOWINFO_DUMMY_P_H
#include "kwindowinfo_p.h"

/**
 * Dummy implementation for KWindowInfoPrivate. This is used as a fallback if there is no
 * implementation for the currently used windowing system platform.
 */
class KWindowInfoPrivateDummy : public KWindowInfoPrivate
{
public:
    KWindowInfoPrivateDummy(WId window, NET::Properties properties, NET::Properties2 properties2);
    ~KWindowInfoPrivateDummy() override;

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
};

#endif
