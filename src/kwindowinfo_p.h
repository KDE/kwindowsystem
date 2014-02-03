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
#ifndef KWINDOWINFO_P_H
#define KWINDOWINFO_P_H
#include "netwm_def.h"

#include <QByteArray>
#include <QRect>
#include <QString>
#include <QSharedData>
#include <QWidgetList> //For WId

/**
 * To write a new platform specific implementation inherit from this class and extend the
 * PlatformImplementation enum. One needs to provide all the methods which are templated in this
 * class. The template delegates to the method in the implementation class. If a method is not
 * provided compilation will fail.
 *
 * The header of the platform specific implementations needs to be included in kwindowinfo.cpp
 * after the include of this header. As it's platform specific this should be ifdef'ed and the else
 * branch must typedef the name of the new platform specific class to KWindowInfoPrivateDummy.
 * Otherwise compilation will fail on other platforms.
 * Also the DELEGATE macro in kwindowinfo.cpp needs to be adjusted so that the specific
 * implementation is picked for the new platform.
 *
 * Last but not least the platform specific implementations are registered in the create() method.
 */
class KWindowInfoPrivate  : public QSharedData
{
public:
    enum PlatformImplementation {
        XcbPlatform,
        DummyPlatform
    };
    virtual ~KWindowInfoPrivate();

    WId win() const;
    PlatformImplementation platform() const;

    template <class T>
    bool valid(bool withdrawn_is_valid) const;
    template <class T>
    unsigned long state() const;
    template <class T>
    bool isMinimized() const;
    template <class T>
    NET::MappingState mappingState() const;
    template <class T>
    NETExtendedStrut extendedStrut() const;
    template <class T>
    NET::WindowType windowType(NET::WindowTypes supported_types) const;
    template <class T>
    QString visibleName() const;
    template <class T>
    QString visibleNameWithState() const;
    template <class T>
    QString name() const;
    template <class T>
    QString visibleIconName() const;
    template <class T>
    QString visibleIconNameWithState() const;
    template <class T>
    QString iconName() const;
    template <class T>
    bool onAllDesktops() const;
    template <class T>
    bool isOnDesktop(int desktop) const;
    template <class T>
    int desktop() const;
    template <class T>
    QRect geometry() const;
    template <class T>
    QRect frameGeometry() const;
    template <class T>
    WId transientFor() const;
    template <class T>
    WId groupLeader() const;
    template <class T>
    QByteArray windowClassClass() const;
    template <class T>
    QByteArray windowClassName() const;
    template <class T>
    QByteArray windowRole() const;
    template <class T>
    QByteArray clientMachine() const;
    template <class T>
    bool actionSupported(NET::Action action) const;

    static KWindowInfoPrivate *create(WId window, NET::Properties properties, NET::Properties2 properties2);

protected:
    KWindowInfoPrivate(PlatformImplementation platform, WId window, NET::Properties properties, NET::Properties2 properties2);
    WId m_window;
    NET::Properties m_properties;
    NET::Properties2 m_properties2;

private:
    PlatformImplementation m_platform;
};

/**
 * Dummy implementation for KWindowInfoPrivate. This is used as a fallback if there is no
 * implementation for the currently used windowing system platform.
 */
class KWindowInfoPrivateDummy : public KWindowInfoPrivate
{
public:
    KWindowInfoPrivateDummy(WId window, NET::Properties properties, NET::Properties2 properties2);
    ~KWindowInfoPrivateDummy();

    bool valid(bool withdrawn_is_valid) const;
    unsigned long state() const;
    bool isMinimized() const;
    NET::MappingState mappingState() const;
    NETExtendedStrut extendedStrut() const;
    NET::WindowType windowType(NET::WindowTypes supported_types) const;
    QString visibleName() const;
    QString visibleNameWithState() const;
    QString name() const;
    QString visibleIconName() const;
    QString visibleIconNameWithState() const;
    QString iconName() const;
    bool onAllDesktops() const;
    bool isOnDesktop(int desktop) const;
    int desktop() const;
    QRect geometry() const;
    QRect frameGeometry() const;
    WId transientFor() const;
    WId groupLeader() const;
    QByteArray windowClassClass() const;
    QByteArray windowClassName() const;
    QByteArray windowRole() const;
    QByteArray clientMachine() const;
    bool actionSupported(NET::Action action) const;
};

inline
WId KWindowInfoPrivate::win() const
{
    return m_window;
}

inline
KWindowInfoPrivate::PlatformImplementation KWindowInfoPrivate::platform() const
{
    return m_platform;
}

#define DELGATE(type, name) \
template <class T> \
inline \
type KWindowInfoPrivate::name() const \
{ \
    return static_cast<const T*>(this)->name(); \
}

DELGATE(unsigned long, state)
DELGATE(bool, isMinimized)
DELGATE(NET::MappingState, mappingState)
DELGATE(NETExtendedStrut, extendedStrut)
DELGATE(QString, visibleName)
DELGATE(QString, visibleNameWithState)
DELGATE(QString, name)
DELGATE(QString, visibleIconName)
DELGATE(QString, visibleIconNameWithState)
DELGATE(QString, iconName)
DELGATE(bool, onAllDesktops)
DELGATE(int, desktop)
DELGATE(QRect, geometry)
DELGATE(QRect, frameGeometry)
DELGATE(WId, transientFor)
DELGATE(WId, groupLeader)
DELGATE(QByteArray, windowClassClass)
DELGATE(QByteArray, windowClassName)
DELGATE(QByteArray, windowRole)
DELGATE(QByteArray, clientMachine)

#undef DELGATE

#define DELGATE(type, name, argType, arg) \
template <class T> \
inline \
type KWindowInfoPrivate::name( argType arg) const \
{ \
    return static_cast<const T*>(this)->name(arg); \
}

DELGATE(bool, valid, bool, withdrawn_is_valid)
DELGATE(NET::WindowType, windowType, NET::WindowTypes, supported_types)
DELGATE(bool, isOnDesktop, int, desktop)
DELGATE(bool, actionSupported, NET::Action, action)

#undef DELGATE

#endif
