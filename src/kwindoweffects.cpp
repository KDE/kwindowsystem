/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kwindoweffects_p.h"
#include "pluginwrapper_p.h"
#include <QGuiApplication>
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 62)
#include <QWidget>
#endif

KWindowEffectsPrivate::KWindowEffectsPrivate()
{
}

KWindowEffectsPrivate::~KWindowEffectsPrivate()
{
}

namespace KWindowEffects
{
bool isEffectAvailable(Effect effect)
{
    return KWindowSystemPluginWrapper::self().effects()->isEffectAvailable(effect);
}

void enableBlurBehind(WId window, bool enable, const QRegion &region)
{
    KWindowSystemPluginWrapper::self().effects()->enableBlurBehind(window, enable, region);
}

void enableBackgroundContrast(WId window, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    KWindowSystemPluginWrapper::self().effects()->enableBackgroundContrast(window, enable, contrast, intensity, saturation, region);
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void highlightWindows(WId controller, const QList<WId> &ids)
{
    KWindowSystemPluginWrapper::self().effects()->highlightWindows(controller, ids);
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 67)
void markAsDashboard(WId window)
{
    KWindowSystemPluginWrapper::self().effects()->markAsDashboard(window);
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void presentWindows(WId controller, const QList<WId> &ids)
{
    KWindowSystemPluginWrapper::self().effects()->presentWindows(controller, ids);
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void presentWindows(WId controller, int desktop)
{
    KWindowSystemPluginWrapper::self().effects()->presentWindows(controller, desktop);
}
#endif

void slideWindow(WId id, SlideFromLocation location, int offset)
{
    KWindowSystemPluginWrapper::self().effects()->slideWindow(id, location, offset);
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 62)
void slideWindow(QWidget *widget, SlideFromLocation location)
{
    slideWindow(widget->effectiveWinId(), location, -1);
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 81)
QList<QSize> windowSizes(const QList<WId> &ids)
{
    return KWindowSystemPluginWrapper::self().effects()->windowSizes(ids);
}
#endif

}
