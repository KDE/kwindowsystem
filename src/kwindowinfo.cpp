/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kwindowinfo.h"
#include "kwindowinfo_p.h"
#include "kwindowsystem.h"
#include "pluginwrapper_p.h"

#include <config-kwindowsystem.h>

#include "kwindowinfo_dummy_p.h"

#include <QRect>

#if KWINDOWSYSTEM_HAVE_X11
#include "kx11extras.h"
#endif

// private
KWindowInfoPrivate *KWindowInfoPrivate::create(WId window, NET::Properties properties, NET::Properties2 properties2)
{
    return KWindowSystemPluginWrapper::self().createWindowInfo(window, properties, properties2);
}

KWindowInfoPrivateDesktopFileNameExtension::KWindowInfoPrivateDesktopFileNameExtension() = default;
KWindowInfoPrivateDesktopFileNameExtension::~KWindowInfoPrivateDesktopFileNameExtension() = default;

KWindowInfoPrivateGtkApplicationIdExtension::KWindowInfoPrivateGtkApplicationIdExtension() = default;
KWindowInfoPrivateGtkApplicationIdExtension::~KWindowInfoPrivateGtkApplicationIdExtension() = default;

KWindowInfoPrivatePidExtension::KWindowInfoPrivatePidExtension() = default;
KWindowInfoPrivatePidExtension::~KWindowInfoPrivatePidExtension() = default;

KWindowInfoPrivateAppMenuExtension::KWindowInfoPrivateAppMenuExtension() = default;
KWindowInfoPrivateAppMenuExtension::~KWindowInfoPrivateAppMenuExtension() = default;

class Q_DECL_HIDDEN KWindowInfoPrivate::Private
{
public:
    Private(WId window, NET::Properties properties, NET::Properties2 properties2);
    WId window;
    NET::Properties properties;
    NET::Properties2 properties2;
    KWindowInfoPrivateDesktopFileNameExtension *desktopFileNameExtension;
    KWindowInfoPrivateGtkApplicationIdExtension *gtkApplicationIdExtension;
    KWindowInfoPrivatePidExtension *pidExtension;
    KWindowInfoPrivateAppMenuExtension *appMenuExtension;
};

KWindowInfoPrivate::Private::Private(WId window, NET::Properties properties, NET::Properties2 properties2)
    : window(window)
    , properties(properties)
    , properties2(properties2)
    , desktopFileNameExtension(nullptr)
    , gtkApplicationIdExtension(nullptr)
    , pidExtension(nullptr)
    , appMenuExtension(nullptr)
{
}

KWindowInfoPrivate::KWindowInfoPrivate(WId window, NET::Properties properties, NET::Properties2 properties2)
    : d(new Private(window, properties, properties2))
{
}

KWindowInfoPrivate::~KWindowInfoPrivate()
{
}

WId KWindowInfoPrivate::win() const
{
    return d->window;
}

KWindowInfoPrivateDesktopFileNameExtension *KWindowInfoPrivate::desktopFileNameExtension() const
{
    return d->desktopFileNameExtension;
}

void KWindowInfoPrivate::installDesktopFileNameExtension(KWindowInfoPrivateDesktopFileNameExtension *extension)
{
    d->desktopFileNameExtension = extension;
}

KWindowInfoPrivateGtkApplicationIdExtension *KWindowInfoPrivate::gtkApplicationIdExtension() const
{
    return d->gtkApplicationIdExtension;
}

void KWindowInfoPrivate::installGtkApplicationIdExtension(KWindowInfoPrivateGtkApplicationIdExtension *extension)
{
    d->gtkApplicationIdExtension = extension;
}

KWindowInfoPrivatePidExtension *KWindowInfoPrivate::pidExtension() const
{
    return d->pidExtension;
}

void KWindowInfoPrivate::installPidExtension(KWindowInfoPrivatePidExtension *extension)
{
    d->pidExtension = extension;
}

KWindowInfoPrivateAppMenuExtension *KWindowInfoPrivate::appMenuExtension() const
{
    return d->appMenuExtension;
}

void KWindowInfoPrivate::installAppMenuExtension(KWindowInfoPrivateAppMenuExtension *extension)
{
    d->appMenuExtension = extension;
}

KWindowInfoPrivateDummy::KWindowInfoPrivateDummy(WId window, NET::Properties properties, NET::Properties2 properties2)
    : KWindowInfoPrivate(window, properties, properties2)
{
}

KWindowInfoPrivateDummy::~KWindowInfoPrivateDummy()
{
}

bool KWindowInfoPrivateDummy::valid(bool withdrawn_is_valid) const
{
    Q_UNUSED(withdrawn_is_valid)
    return false;
}

NET::States KWindowInfoPrivateDummy::state() const
{
    return NET::States();
}

bool KWindowInfoPrivateDummy::isMinimized() const
{
    return false;
}

NET::MappingState KWindowInfoPrivateDummy::mappingState() const
{
    return NET::Visible;
}

NETExtendedStrut KWindowInfoPrivateDummy::extendedStrut() const
{
    return NETExtendedStrut();
}

NET::WindowType KWindowInfoPrivateDummy::windowType(NET::WindowTypes supported_types) const
{
    Q_UNUSED(supported_types)
    return NET::Unknown;
}

QString KWindowInfoPrivateDummy::visibleName() const
{
    return QString();
}

QString KWindowInfoPrivateDummy::visibleNameWithState() const
{
    return QString();
}

QString KWindowInfoPrivateDummy::name() const
{
    return QString();
}

QString KWindowInfoPrivateDummy::visibleIconName() const
{
    return QString();
}

QString KWindowInfoPrivateDummy::visibleIconNameWithState() const
{
    return QString();
}

QString KWindowInfoPrivateDummy::iconName() const
{
    return QString();
}

bool KWindowInfoPrivateDummy::onAllDesktops() const
{
    return false;
}

bool KWindowInfoPrivateDummy::isOnDesktop(int desktop) const
{
    Q_UNUSED(desktop)
    return false;
}

int KWindowInfoPrivateDummy::desktop() const
{
    return 0;
}

QStringList KWindowInfoPrivateDummy::activities() const
{
    return QStringList();
}

QRect KWindowInfoPrivateDummy::geometry() const
{
    return QRect();
}

QRect KWindowInfoPrivateDummy::frameGeometry() const
{
    return QRect();
}

WId KWindowInfoPrivateDummy::transientFor() const
{
    return 0;
}

WId KWindowInfoPrivateDummy::groupLeader() const
{
    return 0;
}

QByteArray KWindowInfoPrivateDummy::windowClassClass() const
{
    return QByteArray();
}

QByteArray KWindowInfoPrivateDummy::windowClassName() const
{
    return QByteArray();
}

QByteArray KWindowInfoPrivateDummy::windowRole() const
{
    return QByteArray();
}

QByteArray KWindowInfoPrivateDummy::clientMachine() const
{
    return QByteArray();
}

bool KWindowInfoPrivateDummy::actionSupported(NET::Action action) const
{
    Q_UNUSED(action)
    return false;
}

// public
KWindowInfo::KWindowInfo(WId window, NET::Properties properties, NET::Properties2 properties2)
    : d(KWindowInfoPrivate::create(window, properties, properties2))
{
}

KWindowInfo::KWindowInfo(const KWindowInfo &other)
    : d(other.d)
{
}

KWindowInfo::~KWindowInfo()
{
}

KWindowInfo &KWindowInfo::operator=(const KWindowInfo &other)
{
    if (d != other.d) {
        d = other.d;
    }
    return *this;
}

// clang-format off

#define DELEGATE(name, args) \
    return d->name(args);

bool KWindowInfo::valid(bool withdrawn_is_valid) const
{
    DELEGATE(valid, withdrawn_is_valid)
}

WId KWindowInfo::win() const
{
    return d->win();
}

NET::States KWindowInfo::state() const
{
    DELEGATE(state, )
}

bool KWindowInfo::hasState(NET::States s) const
{
    return (state() & s) == s;
}

bool KWindowInfo::isMinimized() const
{
    DELEGATE(isMinimized, )
}

NET::MappingState KWindowInfo::mappingState() const
{
    DELEGATE(mappingState, )
}

NETExtendedStrut KWindowInfo::extendedStrut() const
{
    DELEGATE(extendedStrut, )
}

NET::WindowType KWindowInfo::windowType(NET::WindowTypes supported_types) const
{
    DELEGATE(windowType, supported_types)
}

QString KWindowInfo::visibleName() const
{
    DELEGATE(visibleName, )
}

QString KWindowInfo::visibleNameWithState() const
{
    DELEGATE(visibleNameWithState, )
}

QString KWindowInfo::name() const
{
    DELEGATE(name, )
}

QString KWindowInfo::visibleIconName() const
{
    DELEGATE(visibleIconName, )
}

QString KWindowInfo::visibleIconNameWithState() const
{
    DELEGATE(visibleIconNameWithState, )
}

QString KWindowInfo::iconName() const
{
    DELEGATE(iconName, )
}

bool KWindowInfo::isOnCurrentDesktop() const
{
#if KWINDOWSYSTEM_HAVE_X11
    return isOnDesktop(KX11Extras::currentDesktop());
#else
    return true;
#endif
}

bool KWindowInfo::isOnDesktop(int desktop) const
{
    DELEGATE(isOnDesktop, desktop)
}

bool KWindowInfo::onAllDesktops() const
{
    DELEGATE(onAllDesktops, )
}

int KWindowInfo::desktop() const
{
    DELEGATE(desktop, )
}

QStringList KWindowInfo::activities() const
{
    DELEGATE(activities, )
}

QRect KWindowInfo::geometry() const
{
    DELEGATE(geometry, )
}

QRect KWindowInfo::frameGeometry() const
{
    DELEGATE(frameGeometry, )
}

WId KWindowInfo::transientFor() const
{
    DELEGATE(transientFor, )
}

WId KWindowInfo::groupLeader() const
{
    DELEGATE(groupLeader, )
}

QByteArray KWindowInfo::windowClassClass() const
{
    DELEGATE(windowClassClass, )
}

QByteArray KWindowInfo::windowClassName() const
{
    DELEGATE(windowClassName, )
}

QByteArray KWindowInfo::windowRole() const
{
    DELEGATE(windowRole, )
}

QByteArray KWindowInfo::clientMachine() const
{
    DELEGATE(clientMachine, )
}

bool KWindowInfo::actionSupported(NET::Action action) const
{
    DELEGATE(actionSupported, action)
}

QByteArray KWindowInfo::desktopFileName() const
{
    if (auto extension = d->desktopFileNameExtension()) {
        return extension->desktopFileName();
    }
    return QByteArray();
}

QByteArray KWindowInfo::gtkApplicationId() const
{
    if (auto extension = d->gtkApplicationIdExtension()) {
        return extension->gtkApplicationId();
    }
    return QByteArray();
}

QByteArray KWindowInfo::applicationMenuServiceName() const
{
    if (auto extension = d->appMenuExtension()) {
        return extension->applicationMenuServiceName();
    }
    return QByteArray();
}

QByteArray KWindowInfo::applicationMenuObjectPath() const
{
    if (auto extension = d->appMenuExtension()) {
        return extension->applicationMenuObjectPath();
    }
    return QByteArray();
}

int KWindowInfo::pid() const
{
    if (auto extension = d->pidExtension()) {
        return extension->pid();
    }
    return 0;
}

#undef DELEGATE
