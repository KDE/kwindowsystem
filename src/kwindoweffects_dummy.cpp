/*
    SPDX-FileCopyrightText: 2009 Marco Martin <notmart@gmail.com>
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kwindoweffects_dummy_p.h"

#include <QList>

KWindowEffectsPrivateDummy::KWindowEffectsPrivateDummy()
{
}

KWindowEffectsPrivateDummy::~KWindowEffectsPrivateDummy()
{
}

bool KWindowEffectsPrivateDummy::isEffectAvailable(KWindowEffects::Effect effect)
{
    Q_UNUSED(effect)
    return false;
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateDummy::slideWindow(WId id, KWindowEffects::SlideFromLocation location, int offset)
{
    Q_UNUSED(id)
    Q_UNUSED(location)
    Q_UNUSED(offset)
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 81)
QList<QSize> KWindowEffectsPrivateDummy::windowSizes(const QList<WId> &ids)
{
    QList<QSize> windowSizes;
    windowSizes.reserve(ids.size());
    for (int i = 0; i < ids.size(); ++i) {
        windowSizes.append(QSize());
    }
    return windowSizes;
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateDummy::presentWindows(WId controller, const QList<WId> &ids)
{
    Q_UNUSED(controller)
    Q_UNUSED(ids)
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateDummy::presentWindows(WId controller, int desktop)
{
    Q_UNUSED(controller)
    Q_UNUSED(desktop)
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateDummy::highlightWindows(WId controller, const QList<WId> &ids)
{
    Q_UNUSED(controller)
    Q_UNUSED(ids)
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateDummy::enableBlurBehind(WId window, bool enable, const QRegion &region)
{
    Q_UNUSED(window)
    Q_UNUSED(enable)
    Q_UNUSED(region)
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsPrivateDummy::enableBackgroundContrast(WId window, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    Q_UNUSED(window)
    Q_UNUSED(enable)
    Q_UNUSED(contrast)
    Q_UNUSED(intensity)
    Q_UNUSED(saturation)
    Q_UNUSED(region)
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 67)
void KWindowEffectsPrivateDummy::markAsDashboard(WId window)
{
    Q_UNUSED(window)
}
#endif

void KWindowEffectsPrivateDummy::slideWindow(QWindow *window, KWindowEffects::SlideFromLocation location, int offset)
{
    Q_UNUSED(window)
    Q_UNUSED(location)
    Q_UNUSED(offset)
}

void KWindowEffectsPrivateDummy::enableBlurBehind(QWindow *window, bool enable, const QRegion &region)
{
    Q_UNUSED(window)
    Q_UNUSED(enable)
    Q_UNUSED(region)
}

void KWindowEffectsPrivateDummy::enableBackgroundContrast(QWindow *window,
                                                          bool enable,
                                                          qreal contrast,
                                                          qreal intensity,
                                                          qreal saturation,
                                                          const QRegion &region)
{
    Q_UNUSED(window)
    Q_UNUSED(enable)
    Q_UNUSED(contrast)
    Q_UNUSED(intensity)
    Q_UNUSED(saturation)
    Q_UNUSED(region)
}
