/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Matthias Ettrich <ettrich@kde.org>
    SPDX-FileCopyrightText: 2007 Lubos Lunak <l.lunak@kde.org>
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kwindowinfo_p_x11.h"
#include "kwindowsystem.h"

#include <QDebug>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <private/qtx11extras_p.h>
#else
#include <QX11Info>
#endif

#include <X11/Xatom.h>
#include <kxerrorhandler_p.h>
#include <netwm.h>

#include <xcb/res.h>

#include "cptr_p.h"

static bool haveXRes()
{
    static bool s_checked = false;
    static bool s_haveXRes = false;
    if (!s_checked) {
        auto cookie = xcb_res_query_version(QX11Info::connection(), XCB_RES_MAJOR_VERSION, XCB_RES_MINOR_VERSION);
        UniqueCPointer<xcb_res_query_version_reply_t> reply(xcb_res_query_version_reply(QX11Info::connection(), cookie, nullptr));
        s_haveXRes = reply != nullptr;
        s_checked = true;
    }
    return s_haveXRes;
}

// KWindowSystem::info() should be updated too if something has to be changed here
KWindowInfoPrivateX11::KWindowInfoPrivateX11(WId _win, NET::Properties properties, NET::Properties2 properties2)
    : KWindowInfoPrivate(_win, properties, properties2)
    , KWindowInfoPrivateDesktopFileNameExtension()
    , KWindowInfoPrivatePidExtension()
    , KWindowInfoPrivateAppMenuExtension()
{
    installDesktopFileNameExtension(this);
    installPidExtension(this);
    installAppMenuExtension(this);
    installGtkApplicationIdExtension(this);

    KXErrorHandler handler;
    if (properties & NET::WMVisibleIconName) {
        properties |= NET::WMIconName | NET::WMVisibleName; // force, in case it will be used as a fallback
    }
    if (properties & NET::WMVisibleName) {
        properties |= NET::WMName; // force, in case it will be used as a fallback
    }
    if (properties2 & NET::WM2ExtendedStrut) {
        properties |= NET::WMStrut; // will be used as fallback
    }
    if (properties & NET::WMWindowType) {
        properties2 |= NET::WM2TransientFor; // will be used when type is not set
    }
    if ((properties & NET::WMDesktop) && KWindowSystem::mapViewport()) {
        properties |= NET::WMGeometry; // for viewports, the desktop (workspace) is determined from the geometry
    }
    properties |= NET::XAWMState; // force to get error detection for valid()
    m_info.reset(new NETWinInfo(QX11Info::connection(), _win, QX11Info::appRootWindow(), properties, properties2));
    if (properties & NET::WMName) {
        if (m_info->name() && m_info->name()[0] != '\0') {
            m_name = QString::fromUtf8(m_info->name());
        } else {
            m_name = KWindowSystem::readNameProperty(_win, XA_WM_NAME);
        }
    }
    if (properties & NET::WMIconName) {
        if (m_info->iconName() && m_info->iconName()[0] != '\0') {
            m_iconic_name = QString::fromUtf8(m_info->iconName());
        } else {
            m_iconic_name = KWindowSystem::readNameProperty(_win, XA_WM_ICON_NAME);
        }
    }
    if (properties & (NET::WMGeometry | NET::WMFrameExtents)) {
        NETRect frame;
        NETRect geom;
        m_info->kdeGeometry(frame, geom);
        m_geometry.setRect(geom.pos.x, geom.pos.y, geom.size.width, geom.size.height);
        m_frame_geometry.setRect(frame.pos.x, frame.pos.y, frame.size.width, frame.size.height);
    }
    m_valid = !handler.error(false); // no sync - NETWinInfo did roundtrips

    if (haveXRes()) {
        xcb_res_client_id_spec_t specs;
        specs.client = win();
        specs.mask = XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID;
        auto cookie = xcb_res_query_client_ids(QX11Info::connection(), 1, &specs);

        UniqueCPointer<xcb_res_query_client_ids_reply_t> reply(xcb_res_query_client_ids_reply(QX11Info::connection(), cookie, nullptr));
        if (reply && xcb_res_query_client_ids_ids_length(reply.get()) > 0) {
            uint32_t pid = *xcb_res_client_id_value_value((xcb_res_query_client_ids_ids_iterator(reply.get()).data));
            m_pid = pid;
        }
    }
}

KWindowInfoPrivateX11::~KWindowInfoPrivateX11()
{
}

bool KWindowInfoPrivateX11::valid(bool withdrawn_is_valid) const
{
    if (!m_valid) {
        return false;
    }
    if (!withdrawn_is_valid && mappingState() == NET::Withdrawn) {
        return false;
    }
    return true;
}

NET::States KWindowInfoPrivateX11::state() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMState)) {
        qWarning() << "Pass NET::WMState to KWindowInfo";
    }
#endif
    return m_info->state();
}

NET::MappingState KWindowInfoPrivateX11::mappingState() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::XAWMState)) {
        qWarning() << "Pass NET::XAWMState to KWindowInfo";
    }
#endif
    return m_info->mappingState();
}

NETExtendedStrut KWindowInfoPrivateX11::extendedStrut() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2ExtendedStrut)) {
        qWarning() << "Pass NET::WM2ExtendedStrut to KWindowInfo";
    }
#endif
    NETExtendedStrut ext = m_info->extendedStrut();
    NETStrut str = m_info->strut();
    if (ext.left_width == 0 && ext.right_width == 0 && ext.top_width == 0 && ext.bottom_width == 0
        && (str.left != 0 || str.right != 0 || str.top != 0 || str.bottom != 0)) {
        // build extended from simple
        if (str.left != 0) {
            ext.left_width = str.left;
            ext.left_start = 0;
            ext.left_end = XDisplayHeight(QX11Info::display(), DefaultScreen(QX11Info::display()));
        }
        if (str.right != 0) {
            ext.right_width = str.right;
            ext.right_start = 0;
            ext.right_end = XDisplayHeight(QX11Info::display(), DefaultScreen(QX11Info::display()));
        }
        if (str.top != 0) {
            ext.top_width = str.top;
            ext.top_start = 0;
            ext.top_end = XDisplayWidth(QX11Info::display(), DefaultScreen(QX11Info::display()));
        }
        if (str.bottom != 0) {
            ext.bottom_width = str.bottom;
            ext.bottom_start = 0;
            ext.bottom_end = XDisplayWidth(QX11Info::display(), DefaultScreen(QX11Info::display()));
        }
    }
    return ext;
}

NET::WindowType KWindowInfoPrivateX11::windowType(NET::WindowTypes supported_types) const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMWindowType)) {
        qWarning() << "Pass NET::WMWindowType to KWindowInfo";
    }
#endif
    if (!m_info->hasWindowType()) { // fallback, per spec recommendation
        if (transientFor() != XCB_WINDOW_NONE) { // dialog
            if (supported_types & NET::DialogMask) {
                return NET::Dialog;
            }
        } else {
            if (supported_types & NET::NormalMask) {
                return NET::Normal;
            }
        }
    }
    return m_info->windowType(supported_types);
}

QString KWindowInfoPrivateX11::visibleNameWithState() const
{
    QString s = visibleName();
    if (isMinimized()) {
        s.prepend(QLatin1Char('('));
        s.append(QLatin1Char(')'));
    }
    return s;
}

QString KWindowInfoPrivateX11::visibleName() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMVisibleName)) {
        qWarning() << "Pass NET::WMVisibleName to KWindowInfo";
    }
#endif
    return m_info->visibleName() && m_info->visibleName()[0] != '\0' ? QString::fromUtf8(m_info->visibleName()) : name();
}

QString KWindowInfoPrivateX11::name() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMName)) {
        qWarning() << "Pass NET::WMName to KWindowInfo";
    }
#endif
    return m_name;
}

QString KWindowInfoPrivateX11::visibleIconNameWithState() const
{
    QString s = visibleIconName();
    if (isMinimized()) {
        s.prepend(QLatin1Char('('));
        s.append(QLatin1Char(')'));
    }
    return s;
}

QString KWindowInfoPrivateX11::visibleIconName() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMVisibleIconName)) {
        qWarning() << "Pass NET::WMVisibleIconName to KWindowInfo";
    }
#endif
    if (m_info->visibleIconName() && m_info->visibleIconName()[0] != '\0') {
        return QString::fromUtf8(m_info->visibleIconName());
    }
    if (m_info->iconName() && m_info->iconName()[0] != '\0') {
        return QString::fromUtf8(m_info->iconName());
    }
    if (!m_iconic_name.isEmpty()) {
        return m_iconic_name;
    }
    return visibleName();
}

QString KWindowInfoPrivateX11::iconName() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMIconName)) {
        qWarning() << "Pass NET::WMIconName to KWindowInfo";
    }
#endif
    if (m_info->iconName() && m_info->iconName()[0] != '\0') {
        return QString::fromUtf8(m_info->iconName());
    }
    if (!m_iconic_name.isEmpty()) {
        return m_iconic_name;
    }
    return name();
}

bool KWindowInfoPrivateX11::isOnDesktop(int _desktop) const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMDesktop)) {
        qWarning() << "Pass NET::WMDesktop to KWindowInfo";
    }
#endif
    if (KWindowSystem::mapViewport()) {
        if (onAllDesktops()) {
            return true;
        }
        return KWindowSystem::viewportWindowToDesktop(m_geometry) == _desktop;
    }
    return m_info->desktop() == _desktop || m_info->desktop() == NET::OnAllDesktops;
}

bool KWindowInfoPrivateX11::onAllDesktops() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMDesktop)) {
        qWarning() << "Pass NET::WMDesktop to KWindowInfo";
    }
#endif
    if (KWindowSystem::mapViewport()) {
        if (m_info->passedProperties() & NET::WMState) {
            return m_info->state() & NET::Sticky;
        }
        NETWinInfo info(QX11Info::connection(), win(), QX11Info::appRootWindow(), NET::WMState, NET::Properties2());
        return info.state() & NET::Sticky;
    }
    return m_info->desktop() == NET::OnAllDesktops;
}

int KWindowInfoPrivateX11::desktop() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMDesktop)) {
        qWarning() << "Pass NET::WMDesktop to KWindowInfo";
    }
#endif
    if (KWindowSystem::mapViewport()) {
        if (onAllDesktops()) {
            return NET::OnAllDesktops;
        }
        return KWindowSystem::viewportWindowToDesktop(m_geometry);
    }
    return m_info->desktop();
}

QStringList KWindowInfoPrivateX11::activities() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2Activities)) {
        qWarning() << "Pass NET::WM2Activities to KWindowInfo";
    }
#endif

    const QStringList result = QString::fromLatin1(m_info->activities()).split(QLatin1Char(','), Qt::SkipEmptyParts);

    return result.contains(QStringLiteral(KDE_ALL_ACTIVITIES_UUID)) ? QStringList() : result;
}

QRect KWindowInfoPrivateX11::geometry() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMGeometry)) {
        qWarning() << "Pass NET::WMGeometry to KWindowInfo";
    }
#endif
    return m_geometry;
}

QRect KWindowInfoPrivateX11::frameGeometry() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMFrameExtents)) {
        qWarning() << "Pass NET::WMFrameExtents to KWindowInfo";
    }
#endif
    return m_frame_geometry;
}

WId KWindowInfoPrivateX11::transientFor() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2TransientFor)) {
        qWarning() << "Pass NET::WM2TransientFor to KWindowInfo";
    }
#endif
    return m_info->transientFor();
}

WId KWindowInfoPrivateX11::groupLeader() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2GroupLeader)) {
        qWarning() << "Pass NET::WM2GroupLeader to KWindowInfo";
    }
#endif
    return m_info->groupLeader();
}

QByteArray KWindowInfoPrivateX11::windowClassClass() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2WindowClass)) {
        qWarning() << "Pass NET::WM2WindowClass to KWindowInfo";
    }
#endif
    return m_info->windowClassClass();
}

QByteArray KWindowInfoPrivateX11::windowClassName() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2WindowClass)) {
        qWarning() << "Pass NET::WM2WindowClass to KWindowInfo";
    }
#endif
    return m_info->windowClassName();
}

QByteArray KWindowInfoPrivateX11::windowRole() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2WindowRole)) {
        qWarning() << "Pass NET::WM2WindowRole to KWindowInfo";
    }
#endif
    return m_info->windowRole();
}

QByteArray KWindowInfoPrivateX11::clientMachine() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2ClientMachine)) {
        qWarning() << "Pass NET::WM2ClientMachine to KWindowInfo";
    }
#endif
    return m_info->clientMachine();
}

bool KWindowInfoPrivateX11::actionSupported(NET::Action action) const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2AllowedActions)) {
        qWarning() << "Pass NET::WM2AllowedActions to KWindowInfo";
    }
#endif
    if (KWindowSystem::allowedActionsSupported()) {
        return m_info->allowedActions() & action;
    } else {
        return true; // no idea if it's supported or not -> pretend it is
    }
}

bool KWindowInfoPrivateX11::icccmCompliantMappingState() const
{
    static enum { noidea, yes, no } wm_is_1_2_compliant = noidea;
    if (wm_is_1_2_compliant == noidea) {
        NETRootInfo info(QX11Info::connection(), NET::Supported, NET::Properties2(), QX11Info::appScreen());
        wm_is_1_2_compliant = info.isSupported(NET::Hidden) ? yes : no;
    }
    return wm_is_1_2_compliant == yes;
}

// see NETWM spec section 7.6
bool KWindowInfoPrivateX11::isMinimized() const
{
    if (mappingState() != NET::Iconic) {
        return false;
    }
    // NETWM 1.2 compliant WM - uses NET::Hidden for minimized windows
    if ((state() & NET::Hidden) != 0 && (state() & NET::Shaded) == 0) { // shaded may have NET::Hidden too
        return true;
    }
    // older WMs use WithdrawnState for other virtual desktops
    // and IconicState only for minimized
    return icccmCompliantMappingState() ? false : true;
}

QByteArray KWindowInfoPrivateX11::desktopFileName() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2DesktopFileName)) {
        qWarning() << "Pass NET::WM2DesktopFileName to KWindowInfo";
    }
#endif
    return QByteArray(m_info->desktopFileName());
}

QByteArray KWindowInfoPrivateX11::gtkApplicationId() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2DesktopFileName)) {
        qWarning() << "Pass NET::WM2DesktopFileName to KWindowInfo";
    }
#endif
    return QByteArray(m_info->gtkApplicationId());
}

QByteArray KWindowInfoPrivateX11::applicationMenuObjectPath() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2AppMenuObjectPath)) {
        qWarning() << "Pass NET::WM2AppMenuObjectPath to KWindowInfo";
    }
#endif
    return QByteArray(m_info->appMenuObjectPath());
}

QByteArray KWindowInfoPrivateX11::applicationMenuServiceName() const
{
#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties2() & NET::WM2AppMenuServiceName)) {
        qWarning() << "Pass NET::WM2AppMenuServiceName to KWindowInfo";
    }
#endif
    return QByteArray(m_info->appMenuServiceName());
}

int KWindowInfoPrivateX11::pid() const
{
    // If pid is found using the XRes extension use that instead.
    // It is more reliable than the app reporting it's own PID as apps
    // within an app namespace are unable to do so correctly
    if (m_pid > 0) {
        return m_pid;
    }

#if !defined(KDE_NO_WARNING_OUTPUT)
    if (!(m_info->passedProperties() & NET::WMPid)) {
        qWarning() << "Pass NET::WMPid to KWindowInfo";
    }
#endif

    return m_info->pid();
}
