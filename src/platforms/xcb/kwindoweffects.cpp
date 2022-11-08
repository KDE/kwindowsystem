/*
    SPDX-FileCopyrightText: 2009 Marco Martin <notmart@gmail.com>
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kwindoweffects_x11.h"

#include <QGuiApplication>
#include <QVarLengthArray>

#include "kwindowsystem.h"
#include <config-kwindowsystem.h>

#include <QMatrix4x4>
#include <QWindow>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <private/qtx11extras_p.h>
#else
#include <QX11Info>
#endif

#include <xcb/xcb.h>

#include "cptr_p.h"
#include <cmath>

using namespace KWindowEffects;

KWindowEffectsPrivateX11::KWindowEffectsPrivateX11()
{
}

KWindowEffectsPrivateX11::~KWindowEffectsPrivateX11()
{
}

bool KWindowEffectsPrivateX11::isEffectAvailable(Effect effect)
{
    if (!KWindowSystem::self()->compositingActive()) {
        return false;
    }
    QByteArray effectName;

    switch (effect) {
    case Slide:
        effectName = QByteArrayLiteral("_KDE_SLIDE");
        break;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
    case PresentWindows:
        effectName = QByteArrayLiteral("_KDE_PRESENT_WINDOWS_DESKTOP");
        break;
    case PresentWindowsGroup:
        effectName = QByteArrayLiteral("_KDE_PRESENT_WINDOWS_GROUP");
        break;
    case HighlightWindows:
        effectName = QByteArrayLiteral("_KDE_WINDOW_HIGHLIGHT");
        break;
#endif
    case BlurBehind:
        effectName = QByteArrayLiteral("_KDE_NET_WM_BLUR_BEHIND_REGION");
        break;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 67)
    case Dashboard:
        // TODO: Better namespacing for atoms
        effectName = QByteArrayLiteral("_WM_EFFECT_KDE_DASHBOARD");
        break;
#endif
    case BackgroundContrast:
        effectName = QByteArrayLiteral("_KDE_NET_WM_BACKGROUND_CONTRAST_REGION");
        break;
    default:
        return false;
    }

    // hackish way to find out if KWin has the effect enabled,
    // TODO provide proper support
    xcb_connection_t *c = QX11Info::connection();
    xcb_list_properties_cookie_t propsCookie = xcb_list_properties_unchecked(c, QX11Info::appRootWindow());
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());

    UniqueCPointer<xcb_list_properties_reply_t> props(xcb_list_properties_reply(c, propsCookie, nullptr));
    UniqueCPointer<xcb_intern_atom_reply_t> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    if (!atom || !props) {
        return false;
    }
    xcb_atom_t *atoms = xcb_list_properties_atoms(props.get());
    for (int i = 0; i < props->atoms_len; ++i) {
        if (atoms[i] == atom->atom) {
            return true;
        }
    }
    return false;
}

void KWindowEffectsPrivateX11::slideWindow(WId id, SlideFromLocation location, int offset)
{
    xcb_connection_t *c = QX11Info::connection();
    if (!c) {
        return;
    }

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

    UniqueCPointer<xcb_intern_atom_reply_t> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    if (!atom) {
        return;
    }
    if (location == NoEdge) {
        xcb_delete_property(c, id, atom->atom);
    } else {
        xcb_change_property(c, XCB_PROP_MODE_REPLACE, id, atom->atom, atom->atom, 32, size, data);
    }
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 81)
QList<QSize> KWindowEffectsPrivateX11::windowSizes(const QList<WId> &ids)
{
    QList<QSize> windowSizes;
    for (WId id : ids) {
        if (id > 0) {
            KWindowInfo info(id, NET::WMGeometry | NET::WMFrameExtents);
            windowSizes.append(info.frameGeometry().size());
        } else {
            windowSizes.append(QSize());
        }
    }
    return windowSizes;
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateX11::presentWindows(WId controller, const QList<WId> &ids)
{
    xcb_connection_t *c = QX11Info::connection();
    if (!c) {
        return;
    }

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

    const QByteArray effectName = QByteArrayLiteral("_KDE_PRESENT_WINDOWS_GROUP");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    UniqueCPointer<xcb_intern_atom_reply_t> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    if (!atom) {
        return;
    }
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, controller, atom->atom, atom->atom, 32, data.size(), data.constData());
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateX11::presentWindows(WId controller, int desktop)
{
    xcb_connection_t *c = QX11Info::connection();
    if (!c) {
        return;
    }
    const QByteArray effectName = QByteArrayLiteral("_KDE_PRESENT_WINDOWS_DESKTOP");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    UniqueCPointer<xcb_intern_atom_reply_t> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    if (!atom) {
        return;
    }

    int32_t data = desktop;
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, controller, atom->atom, atom->atom, 32, 1, &data);
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateX11::highlightWindows(WId controller, const QList<WId> &ids)
{
    xcb_connection_t *c = QX11Info::connection();
    if (!c) {
        return;
    }
    const QByteArray effectName = QByteArrayLiteral("_KDE_WINDOW_HIGHLIGHT");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    UniqueCPointer<xcb_intern_atom_reply_t> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
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
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, controller, atom->atom, atom->atom, 32, data.size(), data.constData());
}
#endif

void KWindowEffectsPrivateX11::enableBlurBehind(WId window, bool enable, const QRegion &region)
{
    xcb_connection_t *c = QX11Info::connection();
    if (!c) {
        return;
    }
    const QByteArray effectName = QByteArrayLiteral("_KDE_NET_WM_BLUR_BEHIND_REGION");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    UniqueCPointer<xcb_intern_atom_reply_t> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    if (!atom) {
        return;
    }

    if (enable) {
        QVector<uint32_t> data;
        data.reserve(region.rectCount() * 4);
        for (const QRect &r : region) {
            // kwin on X uses device pixels, convert from logical
            auto dpr = qApp->devicePixelRatio();
            data << std::floor(r.x() * dpr) << std::floor(r.y() * dpr) << std::ceil(r.width() * dpr) << std::ceil(r.height() * dpr);
        }

        xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, atom->atom, XCB_ATOM_CARDINAL, 32, data.size(), data.constData());
    } else {
        xcb_delete_property(c, window, atom->atom);
    }
}

void KWindowEffectsPrivateX11::setBackgroundFrost(QWindow *window, QColor color, const QRegion &region)
{
    auto id = window->winId();

    xcb_connection_t *c = QX11Info::connection();
    const QByteArray effectName = QByteArrayLiteral("_KDE_NET_WM_BACKGROUND_FROST_REGION");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    UniqueCPointer<xcb_intern_atom_reply_t> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    if (!atom) {
        return;
    }

    if (!color.isValid()) {
        xcb_delete_property(c, id, atom->atom);
        return;
    }

    enableBackgroundContrast(id, false);

    QVector<uint32_t> data;
    data.reserve(region.rectCount() * 4 + 4);
    for (const QRect &r : region) {
        auto dpr = qApp->devicePixelRatio();
        data << std::floor(r.x() * dpr) << std::floor(r.y() * dpr) << std::ceil(r.width() * dpr) << std::ceil(r.height() * dpr);
    }

    data << color.red();
    data << color.green();
    data << color.blue();
    data << color.alpha();

    xcb_change_property(c, XCB_PROP_MODE_REPLACE, id, atom->atom, atom->atom, 32, data.size(), data.constData());
}

static QWindow *getWindowFromWinId(WId winId)
{
    const QWindowList windows = qGuiApp->topLevelWindows();
    for (auto window : windows) {
        if (window->handle() && window->winId() == winId) {
            return window;
        }
    }
    return nullptr;
}

void KWindowEffectsPrivateX11::enableBackgroundContrast(WId window, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    xcb_connection_t *c = QX11Info::connection();
    const QByteArray effectName = QByteArrayLiteral("_KDE_NET_WM_BACKGROUND_CONTRAST_REGION");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    UniqueCPointer<xcb_intern_atom_reply_t> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    if (!atom) {
        return;
    }

    if (enable) {
        if (QWindow *windowObject = getWindowFromWinId(window)) {
            setBackgroundFrost(windowObject, {});
        }
        QVector<uint32_t> data;
        data.reserve(region.rectCount() * 4 + 16);
        for (const QRect &r : region) {
            auto dpr = qApp->devicePixelRatio();
            data << std::floor(r.x() * dpr) << std::floor(r.y() * dpr) << std::ceil(r.width() * dpr) << std::ceil(r.height() * dpr);
        }

        QMatrix4x4 satMatrix; // saturation
        QMatrix4x4 intMatrix; // intensity
        QMatrix4x4 contMatrix; // contrast

        // clang-format off

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

        // clang-format on

        QMatrix4x4 colorMatrix = contMatrix * satMatrix * intMatrix;
        colorMatrix = colorMatrix.transposed();

        uint32_t *rawData = reinterpret_cast<uint32_t *>(colorMatrix.data());

        for (int i = 0; i < 16; ++i) {
            data << rawData[i];
        }

        xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, atom->atom, atom->atom, 32, data.size(), data.constData());
    } else {
        xcb_delete_property(c, window, atom->atom);
    }
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 67)
void KWindowEffectsPrivateX11::markAsDashboard(WId window)
{
    static const char DASHBOARD_WIN_CLASS[] = "dashboard\0dashboard";
    xcb_connection_t *c = QX11Info::connection();
    if (!c) {
        return;
    }
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8, 19, DASHBOARD_WIN_CLASS);
}
#endif
