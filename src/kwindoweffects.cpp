/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kwindoweffects_p.h"
#include "pluginwrapper_p.h"
#include <QWindow>

KWindowEffectsPrivate::KWindowEffectsPrivate()
{
}

KWindowEffectsPrivate::~KWindowEffectsPrivate()
{
}

KWindowEffectsPrivateV2::KWindowEffectsPrivateV2()
    : KWindowEffectsPrivate()
{
}

KWindowEffectsPrivateV2::~KWindowEffectsPrivateV2()
{
}

namespace KWindowEffects
{
bool isEffectAvailable(Effect effect)
{
    return KWindowSystemPluginWrapper::self().effects()->isEffectAvailable(effect);
}

void enableBlurBehind(QWindow *window, bool enable, const QRegion &region)
{
    KWindowSystemPluginWrapper::self().effects()->enableBlurBehind(window->winId(), enable, region);
}

void enableBackgroundContrast(QWindow *window, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    KWindowSystemPluginWrapper::self().effects()->enableBackgroundContrast(window->winId(), enable, contrast, intensity, saturation, region);
}

void setBackgroundFrost(QWindow *window, QColor frostColor, const QRegion &region)
{
    auto effects = KWindowSystemPluginWrapper::self().effects();
    if (auto effectsv2 = dynamic_cast<KWindowEffectsPrivateV2 *>(effects)) {
        effectsv2->setBackgroundFrost(window, frostColor, region);
    }
}

void slideWindow(QWindow *window, SlideFromLocation location, int offset)
{
    KWindowSystemPluginWrapper::self().effects()->slideWindow(window->winId(), location, offset);
}
}
