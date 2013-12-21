/*
 *   Copyright 2009 Marco Martin <notmart@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "kwindoweffects.h"

#include <QVarLengthArray>

#include "kwindowsystem.h"
#include <config-kwindowsystem.h>

#include <xcb/xcb.h>
#include <QX11Info>
#include <QMatrix4x4>

static const char *DASHBOARD_WIN_CLASS = "dashboard\0dashboard";

namespace KWindowEffects
{

bool isEffectAvailable(Effect effect)
{
    if (!KWindowSystem::self()->compositingActive()) {
        return false;
    }
    QByteArray effectName;

    switch (effect) {
    case Slide:
        effectName = QByteArrayLiteral("_KDE_SLIDE");
        break;
    case WindowPreview:
        effectName = QByteArrayLiteral("_KDE_WINDOW_PREVIEW");
        break;
    case PresentWindows:
        effectName = QByteArrayLiteral("_KDE_PRESENT_WINDOWS_DESKTOP");
        break;
    case PresentWindowsGroup:
        effectName = QByteArrayLiteral("_KDE_PRESENT_WINDOWS_GROUP");
        break;
    case HighlightWindows:
        effectName = QByteArrayLiteral("_KDE_WINDOW_HIGHLIGHT");
        break;
    case BlurBehind:
        effectName = QByteArrayLiteral("_KDE_NET_WM_BLUR_BEHIND_REGION");
        break;
    case Dashboard:
        // TODO: Better namespacing for atoms
        effectName = QByteArrayLiteral("_WM_EFFECT_KDE_DASHBOARD");
        break;
    default:
        return false;
    }

    // hackish way to find out if KWin has the effect enabled,
    // TODO provide proper support
    xcb_connection_t *c = QX11Info::connection();
    xcb_list_properties_cookie_t propsCookie = xcb_list_properties_unchecked(c, QX11Info::appRootWindow());
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());

    QScopedPointer<xcb_list_properties_reply_t, QScopedPointerPodDeleter> props(xcb_list_properties_reply(c, propsCookie, NULL));
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, NULL));
    if (!atom || !props) {
        return false;
    }
    xcb_atom_t *atoms = xcb_list_properties_atoms(props.data());
    for (int i = 0; i < props->atoms_len; ++i) {
        if (atoms[i] == atom->atom) {
            return true;
        }
    }
    return false;
}

void slideWindow(WId id, SlideFromLocation location, int offset)
{
    xcb_connection_t *c = QX11Info::connection();
    const QByteArray effectName = QByteArrayLiteral("_KDE_SLIDE");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());

    const int size = 2;
    int32_t data[size];
    data[0] = offset;

    switch (location) {
    case LeftEdge:
        data[1] = 0;
        break;
    case TopEdge:
        data[1] = 1;
        break;
    case RightEdge:
        data[1] = 2;
        break;
    case BottomEdge:
        data[1] = 3;
    default:
        break;
    }

    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, NULL));
    if (!atom) {
        return;
    }
    if (location == NoEdge) {
        xcb_delete_property(c, id, atom->atom);
    } else {
        xcb_change_property(c, XCB_PROP_MODE_REPLACE, id, atom->atom, atom->atom, 32, size, data);
    }
}

void slideWindow(QWidget *widget, SlideFromLocation location)
{
    slideWindow(widget->effectiveWinId(), location, -1);
}

QList<QSize> windowSizes(const QList<WId> &ids)
{
    QList<QSize> windowSizes;
    Q_FOREACH (WId id, ids) {
        if (id > 0) {
            KWindowInfo info = KWindowSystem::windowInfo(id, NET::WMGeometry | NET::WMFrameExtents);
            windowSizes.append(info.frameGeometry().size());
        } else {
            windowSizes.append(QSize());
        }
    }
    return windowSizes;
}

void showWindowThumbnails(WId parent, const QList<WId> &windows, const QList<QRect> &rects)
{
    if (windows.size() != rects.size()) {
        return;
    }
    xcb_connection_t *c = QX11Info::connection();
    const QByteArray effectName = QByteArrayLiteral("_KDE_WINDOW_PREVIEW");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, NULL));
    if (!atom) {
        return;
    }
    if (windows.isEmpty()) {
        xcb_delete_property(c, parent, atom->atom);
        return;
    }

    int numWindows = windows.size();

    // 64 is enough for 10 windows and is a nice base 2 number
    QVarLengthArray<int32_t, 64> data(1 + (6 * numWindows));
    data[0] = numWindows;

    QList<WId>::const_iterator windowsIt;
    QList<QRect>::const_iterator rectsIt = rects.constBegin();
    int i = 0;
    for (windowsIt = windows.constBegin(); windowsIt != windows.constEnd(); ++windowsIt) {

        const int start = (i * 6) + 1;
        const QRect thumbnailRect = (*rectsIt);

        data[start] = 5;
        data[start + 1] = (*windowsIt);
        data[start + 2] = thumbnailRect.x();
        data[start + 3] = thumbnailRect.y();
        data[start + 4] = thumbnailRect.width();
        data[start + 5] = thumbnailRect.height();
        ++rectsIt;
        ++i;
    }

    xcb_change_property(c, XCB_PROP_MODE_REPLACE, parent, atom->atom, atom->atom,
                        32, data.size(), data.constData());
}

void presentWindows(WId controller, const QList<WId> &ids)
{
    const int numWindows = ids.count();
    QVarLengthArray<int32_t, 32> data(numWindows);
    int actualCount = 0;

    for (int i = 0; i < numWindows; ++i) {
        data[i] = ids.at(i);
        ++actualCount;

    }

    if (actualCount != numWindows) {
        data.resize(actualCount);
    }

    if (data.isEmpty()) {
        return;
    }
    xcb_connection_t *c = QX11Info::connection();
    const QByteArray effectName = QByteArrayLiteral("_KDE_PRESENT_WINDOWS_GROUP");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, NULL));
    if (!atom) {
        return;
    }
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, controller, atom->atom, atom->atom, 32, data.size(), data.constData());
}

void presentWindows(WId controller, int desktop)
{
    xcb_connection_t *c = QX11Info::connection();
    const QByteArray effectName = QByteArrayLiteral("_KDE_PRESENT_WINDOWS_DESKTOP");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, NULL));
    if (!atom) {
        return;
    }

    int32_t data = desktop;
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, controller, atom->atom, atom->atom, 32, 1, &data);
}

void highlightWindows(WId controller, const QList<WId> &ids)
{
    xcb_connection_t *c = QX11Info::connection();
    const QByteArray effectName = QByteArrayLiteral("_KDE_WINDOW_HIGHLIGHT");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, NULL));
    if (!atom) {
        return;
    }

    const int numWindows = ids.count();
    if (numWindows == 0) {
        xcb_delete_property(c, controller, atom->atom);
        return;
    }

    QVarLengthArray<int32_t, 32> data(numWindows);
    int actualCount = 0;

    for (int i = 0; i < numWindows; ++i) {
        data[i] = ids.at(i);
        ++actualCount;
    }

    if (actualCount != numWindows) {
        data.resize(actualCount);
    }

    if (data.isEmpty()) {
        return;
    }
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, controller, atom->atom, atom->atom,
                        32, data.size(), data.constData());
}

void enableBlurBehind(WId window, bool enable, const QRegion &region)
{
    xcb_connection_t *c = QX11Info::connection();
    const QByteArray effectName = QByteArrayLiteral("_KDE_NET_WM_BLUR_BEHIND_REGION");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, NULL));
    if (!atom) {
        return;
    }

    if (enable) {
        QVector<QRect> rects = region.rects();
        QVector<uint32_t> data;
        Q_FOREACH (const QRect &r, rects) {
            data << r.x() << r.y() << r.width() << r.height();
        }

        xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, atom->atom, XCB_ATOM_CARDINAL,
                            32, data.size(), data.constData());
    } else {
        xcb_delete_property(c, window, atom->atom);
    }
}

void enableBackgroundContrast(WId window, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    qDebug() << contrast << intensity << saturation;
    xcb_connection_t *c = QX11Info::connection();
    const QByteArray effectName = QByteArrayLiteral("_KDE_NET_WM_BACKGROUND_CONTRAST_REGION");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, NULL));
    if (!atom) {
        return;
    }

    if (enable) {
        QVector<QRect> rects = region.rects();
        QVector<long> data;
        Q_FOREACH (const QRect &r, rects) {
            data << r.x() << r.y() << r.width() << r.height();
        }

        QMatrix4x4 satMatrix; //saturation
        QMatrix4x4 intMatrix; //intensity
        QMatrix4x4 contMatrix; //contrast

        //Saturation matrix
        if (!qFuzzyCompare(saturation, 1.0)) {
            const qreal rval = (1.0 - saturation) * .2126;
            const qreal gval = (1.0 - saturation) * .7152;
            const qreal bval = (1.0 - saturation) * .0722;

            satMatrix = QMatrix4x4(rval + saturation, rval,     rval,     0.0,
                gval,     gval + saturation, gval,     0.0,
                bval,     bval,     bval + saturation, 0.0,
                0,        0,        0,        1.0);
        }

        //IntensityMatrix
        if (!qFuzzyCompare(intensity, 1.0)) {
            intMatrix.scale(intensity, intensity, intensity);
        }

        //Contrast Matrix
        if (!qFuzzyCompare(contrast, 1.0)) {
            const float transl = (1.0 - contrast) / 2.0;

            contMatrix = QMatrix4x4(contrast, 0,        0,        0.0,
                0,        contrast, 0,        0.0,
                0,        0,        contrast, 0.0,
                transl,   transl,   transl,   1.0);
        }

        QMatrix4x4 colorMatrix = contMatrix * satMatrix * intMatrix;
        colorMatrix = colorMatrix * 100;
        colorMatrix = colorMatrix.transposed();

        for (int i = 0; i < 16; ++i) {
            data << (uint32_t)colorMatrix.data()[i];
        }

        xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, atom->atom, XCB_ATOM_CARDINAL,
                            32, data.size(), data.constData());
    } else {
        xcb_delete_property(c, window, atom->atom);
    }
}

void markAsDashboard(WId window)
{
    xcb_change_property(QX11Info::connection(), XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_CLASS,
                        XCB_ATOM_STRING, 8, 19, DASHBOARD_WIN_CLASS);
}

}

