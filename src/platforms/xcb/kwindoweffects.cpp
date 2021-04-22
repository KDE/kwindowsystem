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
#include <QX11Info>
#include <xcb/xcb.h>

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

    QScopedPointer<xcb_list_properties_reply_t, QScopedPointerPodDeleter> props(xcb_list_properties_reply(c, propsCookie, nullptr));
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
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
void KWindowEffectsPrivateX11::slideWindow(WId id, SlideFromLocation location, int offset)
{
    slideWindowInternal(id, location, offset);
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
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
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
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
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
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
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

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateX11::enableBlurBehind(WId window, bool enable, const QRegion &region)
{
    enableBlurBehindInternal(window, enable, region);
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateX11::enableBackgroundContrast(WId window, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    enableBackgroundContrastInternal(window, enable, contrast, intensity, saturation, region);
}
#endif

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

void KWindowEffectsPrivateX11::slideWindow(QWindow *window, SlideFromLocation location, int offset)
{
    slideWindowInternal(window->winId(), location, offset);
}

void KWindowEffectsPrivateX11::slideWindowInternal(WId window, SlideFromLocation location, int offset)
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

    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    if (!atom) {
        return;
    }
    if (location == NoEdge) {
        xcb_delete_property(c, window, atom->atom);
    } else {
        xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, atom->atom, atom->atom, 32, size, data);
    }
}

void KWindowEffectsPrivateX11::enableBlurBehind(QWindow *window, bool enable, const QRegion &region)
{
    enableBlurBehindInternal(window->winId(), enable, region);
}

void KWindowEffectsPrivateX11::enableBlurBehindInternal(WId window, bool enable, const QRegion &region)
{
    xcb_connection_t *c = QX11Info::connection();
    if (!c) {
        return;
    }
    const QByteArray effectName = QByteArrayLiteral("_KDE_NET_WM_BLUR_BEHIND_REGION");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    if (!atom) {
        return;
    }

    if (enable) {
        QVector<uint32_t> data;
        data.reserve(region.rectCount() * 4);
        for (const QRect &r : region) {
            // kwin on X uses device pixels, convert from logical
            auto dpr = qApp->devicePixelRatio();
            data << r.x() * dpr << r.y() * dpr << r.width() * dpr << r.height() * dpr;
        }

        xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, atom->atom, XCB_ATOM_CARDINAL, 32, data.size(), data.constData());
    } else {
        xcb_delete_property(c, window, atom->atom);
    }
}

void KWindowEffectsPrivateX11::enableBackgroundContrast(QWindow *window, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    enableBackgroundContrastInternal(window->winId(), enable, contrast, intensity, saturation, region);
}

void KWindowEffectsPrivateX11::enableBackgroundContrastInternal(WId window,
                                                                bool enable,
                                                                qreal contrast,
                                                                qreal intensity,
                                                                qreal saturation,
                                                                const QRegion &region)
{
    xcb_connection_t *c = QX11Info::connection();
    const QByteArray effectName = QByteArrayLiteral("_KDE_NET_WM_BACKGROUND_CONTRAST_REGION");
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, effectName.length(), effectName.constData());
    QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    if (!atom) {
        return;
    }

    if (enable) {
        QVector<uint32_t> data;
        data.reserve(region.rectCount() * 4 + 16);
        for (const QRect &r : region) {
            auto dpr = qApp->devicePixelRatio();
            data << r.x() * dpr << r.y() * dpr << r.width() * dpr << r.height() * dpr;
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
