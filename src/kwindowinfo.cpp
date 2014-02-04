/*
 *   Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "kwindowinfo.h"
#include "kwindowinfo_p.h"
#include "kwindowsystem.h"

#include <config-kwindowsystem.h>

#if KWINDOWSYSTEM_HAVE_X11
#include "kwindowinfo_p_x11.h"
#else
typedef KWindowInfoPrivateDummy KWindowInfoPrivateX11;
#endif

#include <QRect>
#include <QGuiApplication>

// private
KWindowInfoPrivate *KWindowInfoPrivate::create(WId window, NET::Properties properties, NET::Properties2 properties2)
{
    KWindowInfoPrivate *d = Q_NULLPTR;
#if KWINDOWSYSTEM_HAVE_X11
    if (QGuiApplication::platformName() == QStringLiteral("xcb")) {
        d = new KWindowInfoPrivateX11(window, properties, properties2);
    }
#endif
    if (!d) {
        d = new KWindowInfoPrivateDummy(window, properties, properties2);
    }
    return d;
}

KWindowInfoPrivate::KWindowInfoPrivate(PlatformImplementation platform, WId window, NET::Properties properties, NET::Properties2 properties2)
    : m_window(window)
    , m_properties(properties)
    , m_properties2(properties2)
    , m_platform(platform)
{
}

KWindowInfoPrivate::~KWindowInfoPrivate()
{
}

KWindowInfoPrivateDummy::KWindowInfoPrivateDummy(WId window, NET::Properties properties, NET::Properties2 properties2)
    : KWindowInfoPrivate(KWindowInfoPrivate::DummyPlatform, window, properties, properties2)
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
    return 0;
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

#define DELEGATE(name, args) \
    switch (d->platform()) { \
    case KWindowInfoPrivate::XcbPlatform: \
        return d->name<KWindowInfoPrivateX11>( args ); \
    default: \
        return d->name<KWindowInfoPrivateDummy>( args ); \
    }

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
    return isOnDesktop(KWindowSystem::currentDesktop());
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

#undef DELEGATE
