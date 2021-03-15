/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KWINDOWINFO_P_H
#define KWINDOWINFO_P_H
#include "netwm_def.h"
#include <kwindowsystem_export.h>

#include <QByteArray>
#include <QRect>
#include <QScopedPointer>
#include <QSharedData>
#include <QString>
#include <QStringList>
#include <QWidgetList> //For WId

class KWindowInfoPrivateDesktopFileNameExtension;
class KWindowInfoPrivatePidExtension;
class KWindowInfoPrivateAppMenuExtension;

class KWINDOWSYSTEM_EXPORT KWindowInfoPrivate : public QSharedData
{
public:
    virtual ~KWindowInfoPrivate();

    WId win() const;

    virtual bool valid(bool withdrawn_is_valid) const = 0;
    virtual NET::States state() const = 0;
    virtual bool isMinimized() const = 0;
    virtual NET::MappingState mappingState() const = 0;
    virtual NETExtendedStrut extendedStrut() const = 0;
    virtual NET::WindowType windowType(NET::WindowTypes supported_types) const = 0;
    virtual QString visibleName() const = 0;
    virtual QString visibleNameWithState() const = 0;
    virtual QString name() const = 0;
    virtual QString visibleIconName() const = 0;
    virtual QString visibleIconNameWithState() const = 0;
    virtual QString iconName() const = 0;
    virtual bool onAllDesktops() const = 0;
    virtual bool isOnDesktop(int desktop) const = 0;
    virtual int desktop() const = 0;
    virtual QStringList activities() const = 0;
    virtual QRect geometry() const = 0;
    virtual QRect frameGeometry() const = 0;
    virtual WId transientFor() const = 0;
    virtual WId groupLeader() const = 0;
    virtual QByteArray windowClassClass() const = 0;
    virtual QByteArray windowClassName() const = 0;
    virtual QByteArray windowRole() const = 0;
    virtual QByteArray clientMachine() const = 0;
    virtual bool actionSupported(NET::Action action) const = 0;

    KWindowInfoPrivateDesktopFileNameExtension *desktopFileNameExtension() const;
    KWindowInfoPrivatePidExtension *pidExtension() const;
    KWindowInfoPrivateAppMenuExtension *appMenuExtension() const;

    static KWindowInfoPrivate *create(WId window, NET::Properties properties, NET::Properties2 properties2);

protected:
    KWindowInfoPrivate(WId window, NET::Properties properties, NET::Properties2 properties2);

    void installDesktopFileNameExtension(KWindowInfoPrivateDesktopFileNameExtension *extension);
    void installPidExtension(KWindowInfoPrivatePidExtension *extension);
    void installAppMenuExtension(KWindowInfoPrivateAppMenuExtension *extension);

private:
    class Private;
    const QScopedPointer<Private> d;
};

class KWINDOWSYSTEM_EXPORT KWindowInfoPrivateDesktopFileNameExtension
{
public:
    virtual ~KWindowInfoPrivateDesktopFileNameExtension();

    virtual QByteArray desktopFileName() const = 0;

protected:
    explicit KWindowInfoPrivateDesktopFileNameExtension();
};

class KWINDOWSYSTEM_EXPORT KWindowInfoPrivatePidExtension
{
public:
    virtual ~KWindowInfoPrivatePidExtension();

    virtual int pid() const = 0;

protected:
    explicit KWindowInfoPrivatePidExtension();
};

class KWINDOWSYSTEM_EXPORT KWindowInfoPrivateAppMenuExtension
{
public:
    virtual ~KWindowInfoPrivateAppMenuExtension();

    virtual QByteArray applicationMenuServiceName() const = 0;
    virtual QByteArray applicationMenuObjectPath() const = 0;

protected:
    explicit KWindowInfoPrivateAppMenuExtension();
};

#endif
