/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Matthias Ettrich <ettrich@kde.org>
    SPDX-FileCopyrightText: 2007 Lubos Lunak <l.lunak@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kx11extras.h"

#include "kwindowsystem.h"
#include "kwindowsystem_p.h"

#include "kxutils_p.h"
#include "netwm.h"

#include <QGuiApplication>
#include <QRect>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <private/qtx11extras_p.h>
#else
#include <QX11Info>
#endif

// QPoint and QSize all have handy / operators which are useful for scaling, positions and sizes for high DPI support
// QRect does not, so we create one for internal purposes within this class
inline QRect operator/(const QRect &rectangle, qreal factor)
{
    return QRect(rectangle.topLeft() / factor, rectangle.size() / factor);
}

KX11Extras *KX11Extras::self()
{
    static KX11Extras instance;
    return &instance;
}

QList<WId> KX11Extras::windows()
{
    return KWindowSystem::d_func()->windows();
}

bool KX11Extras::hasWId(WId w)
{
    return windows().contains(w);
}

QList<WId> KX11Extras::stackingOrder()
{
    return KWindowSystem::d_func()->stackingOrder();
}

WId KX11Extras::activeWindow()
{
    return KWindowSystem::d_func()->activeWindow();
}

void KX11Extras::activateWindow(WId win, long time)
{
    KWindowSystem::d_func()->activateWindow(win, time);
}

void KX11Extras::forceActiveWindow(WId win, long time)
{
    KWindowSystem::d_func()->forceActiveWindow(win, time);
}

bool KX11Extras::compositingActive()
{
    return KWindowSystem::d_func()->compositingActive();
}

int KX11Extras::currentDesktop()
{
    return KWindowSystem::d_func()->currentDesktop();
}

int KX11Extras::numberOfDesktops()
{
    return KWindowSystem::d_func()->numberOfDesktops();
}

void KX11Extras::setCurrentDesktop(int desktop)
{
    KWindowSystem::d_func()->setCurrentDesktop(desktop);
}

void KX11Extras::setOnAllDesktops(WId win, bool b)
{
    KWindowSystem::d_func()->setOnAllDesktops(win, b);
}

void KX11Extras::setOnDesktop(WId win, int desktop)
{
    KWindowSystem::d_func()->setOnDesktop(win, desktop);
}

void KX11Extras::setOnActivities(WId win, const QStringList &activities)
{
    KWindowSystem::d_func()->setOnActivities(win, activities);
}

QPixmap KX11Extras::icon(WId win, int width, int height, bool scale)
{
    return icon(win, width, height, scale, NETWM | WMHints | ClassHint | XApp);
}

QPixmap KX11Extras::icon(WId win, int width, int height, bool scale, int flags)
{
    return KWindowSystem::d_func()->icon(win, width, height, scale, flags);
}

QPixmap iconFromNetWinInfo(int width, int height, bool scale, int flags, NETWinInfo *info)
{
    QPixmap result;
    if (!info) {
        return result;
    }
    if (flags & KX11Extras::NETWM) {
        NETIcon ni = info->icon(width, height);
        if (ni.data && ni.size.width > 0 && ni.size.height > 0) {
            QImage img((uchar *)ni.data, (int)ni.size.width, (int)ni.size.height, QImage::Format_ARGB32);
            if (scale && width > 0 && height > 0 && img.size() != QSize(width, height) && !img.isNull()) {
                img = img.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            }
            if (!img.isNull()) {
                result = QPixmap::fromImage(img);
            }
            return result;
        }
    }

    if (flags & KX11Extras::WMHints) {
        xcb_pixmap_t p = info->icccmIconPixmap();
        xcb_pixmap_t p_mask = info->icccmIconPixmapMask();

        if (p != XCB_PIXMAP_NONE) {
            QPixmap pm = KXUtils::createPixmapFromHandle(info->xcbConnection(), p, p_mask);
            if (scale && width > 0 && height > 0 && !pm.isNull() //
                && (pm.width() != width || pm.height() != height)) {
                result = QPixmap::fromImage(pm.toImage().scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            } else {
                result = pm;
            }
        }
    }

    // Since width can be any arbitrary size, but the icons cannot,
    // take the nearest value for best results (ignoring 22 pixel
    // icons as they don't exist for apps):
    int iconWidth;
    if (width < 24) {
        iconWidth = 16;
    } else if (width < 40) {
        iconWidth = 32;
    } else if (width < 56) {
        iconWidth = 48;
    } else if (width < 96) {
        iconWidth = 64;
    } else if (width < 192) {
        iconWidth = 128;
    } else {
        iconWidth = 256;
    }

    if (flags & KX11Extras::ClassHint) {
        // Try to load the icon from the classhint if the app didn't specify
        // its own:
        if (result.isNull()) {
            const QIcon icon = QIcon::fromTheme(QString::fromUtf8(info->windowClassClass()).toLower());
            const QPixmap pm = icon.isNull() ? QPixmap() : icon.pixmap(iconWidth, iconWidth);
            if (scale && !pm.isNull()) {
                result = QPixmap::fromImage(pm.toImage().scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            } else {
                result = pm;
            }
        }
    }

    if (flags & KX11Extras::XApp) {
        // If the icon is still a null pixmap, load the icon for X applications
        // as a last resort:
        if (result.isNull()) {
            const QIcon icon = QIcon::fromTheme(QStringLiteral("xorg"));
            const QPixmap pm = icon.isNull() ? QPixmap() : icon.pixmap(iconWidth, iconWidth);
            if (scale && !pm.isNull()) {
                result = QPixmap::fromImage(pm.toImage().scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            } else {
                result = pm;
            }
        }
    }
    return result;
}

QPixmap KX11Extras::icon(WId win, int width, int height, bool scale, int flags, NETWinInfo *info)
{
    width *= qGuiApp->devicePixelRatio();
    height *= qGuiApp->devicePixelRatio();

    if (info) {
        return iconFromNetWinInfo(width, height, scale, flags, info);
    }

    NETWinInfo newInfo(QX11Info::connection(), win, QX11Info::appRootWindow(), NET::WMIcon, NET::WM2WindowClass | NET::WM2IconPixmap);

    return iconFromNetWinInfo(width, height, scale, flags, &newInfo);
}

void KX11Extras::minimizeWindow(WId win)
{
    KWindowSystem::d_func()->minimizeWindow(win);
}

void KX11Extras::unminimizeWindow(WId win)
{
    KWindowSystem::d_func()->unminimizeWindow(win);
}

QRect KX11Extras::workArea(int desktop)
{
    return KWindowSystem::d_func()->workArea(desktop) / qApp->devicePixelRatio();
}

QRect KX11Extras::workArea(const QList<WId> &exclude, int desktop)
{
    return KWindowSystem::d_func()->workArea(exclude, desktop) / qApp->devicePixelRatio();
}

QString KX11Extras::desktopName(int desktop)
{
    return KWindowSystem::d_func()->desktopName(desktop);
}

void KX11Extras::setDesktopName(int desktop, const QString &name)
{
    KWindowSystem::d_func()->setDesktopName(desktop, name);
}

QString KX11Extras::readNameProperty(WId win, unsigned long atom)
{
    return KWindowSystem::d_func()->readNameProperty(win, atom);
}

bool KX11Extras::mapViewport()
{
    return KWindowSystem::d_func()->mapViewport();
}

void KX11Extras::setExtendedStrut(WId win,
                                  int left_width,
                                  int left_start,
                                  int left_end,
                                  int right_width,
                                  int right_start,
                                  int right_end,
                                  int top_width,
                                  int top_start,
                                  int top_end,
                                  int bottom_width,
                                  int bottom_start,
                                  int bottom_end)
{
    const qreal dpr = qApp->devicePixelRatio();
    KWindowSystem::d_func()->setExtendedStrut(win,
                                              left_width * dpr,
                                              left_start * dpr,
                                              left_end * dpr,
                                              right_width * dpr,
                                              right_start * dpr,
                                              right_end * dpr,
                                              top_width * dpr,
                                              top_start * dpr,
                                              top_end * dpr,
                                              bottom_width * dpr,
                                              bottom_start * dpr,
                                              bottom_end * dpr);
}

void KX11Extras::setStrut(WId win, int left, int right, int top, int bottom)
{
    const qreal dpr = qApp->devicePixelRatio();
    KWindowSystem::d_func()->setStrut(win, left * dpr, right * dpr, top * dpr, bottom * dpr);
}

void KX11Extras::connectNotify(const QMetaMethod &signal)
{
    KWindowSystem::self()->d_func()->connectNotify(signal);
    QObject::connectNotify(signal);
}

#include "moc_kx11extras.cpp"
