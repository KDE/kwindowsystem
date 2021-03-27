/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KWINDOWEFFECTS_P_H
#define KWINDOWEFFECTS_P_H
#include "kwindoweffects.h"

class KWINDOWSYSTEM_EXPORT KWindowEffectsPrivate
{
public:
    virtual ~KWindowEffectsPrivate();
    virtual bool isEffectAvailable(KWindowEffects::Effect effect) = 0;
    virtual void slideWindow(WId id, KWindowEffects::SlideFromLocation location, int offset) = 0;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 81)
    virtual QList<QSize> windowSizes(const QList<WId> &ids) = 0;
#endif
    virtual void presentWindows(WId controller, const QList<WId> &ids) = 0;
    virtual void presentWindows(WId controller, int desktop = NET::OnAllDesktops) = 0;
    virtual void highlightWindows(WId controller, const QList<WId> &ids) = 0;
    virtual void enableBlurBehind(WId window, bool enable = true, const QRegion &region = QRegion()) = 0;
    virtual void enableBackgroundContrast(WId window, bool enable = true, qreal contrast = 1, qreal intensity = 1, qreal saturation = 1, const QRegion &region = QRegion()) = 0;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 67)
    virtual void markAsDashboard(WId window) = 0;
#endif
protected:
    KWindowEffectsPrivate();
};

#endif
