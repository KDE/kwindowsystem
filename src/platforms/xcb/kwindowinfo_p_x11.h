/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KWINDOWINFO_P_X11_H
#define KWINDOWINFO_P_X11_H
#include "kwindowinfo_p.h"
#include <memory>

class NETWinInfo;

class KWindowInfoPrivateX11 : public KWindowInfoPrivate,
                              public KWindowInfoPrivateDesktopFileNameExtension,
                              public KWindowInfoPrivatePidExtension,
                              public KWindowInfoPrivateAppMenuExtension,
                              public KWindowInfoPrivateGtkApplicationIdExtension
{
public:
    KWindowInfoPrivateX11(WId window, NET::Properties properties, NET::Properties2 properties2);
    ~KWindowInfoPrivateX11() override;

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

    QByteArray desktopFileName() const override;
    QByteArray applicationMenuObjectPath() const override;
    QByteArray applicationMenuServiceName() const override;
    QByteArray gtkApplicationId() const override;

    int pid() const override;

private:
    bool icccmCompliantMappingState() const;

    std::unique_ptr<NETWinInfo> m_info;
    QString m_name;
    QString m_iconic_name;
    QRect m_geometry;
    QRect m_frame_geometry;
    int m_pid = -1; // real PID from XResources. Valid if > 0
    bool m_valid;
};

#endif
