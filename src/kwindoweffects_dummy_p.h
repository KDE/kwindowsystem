/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KWINDOWEFFECTS_DUMMY_P_H
#define KWINDOWEFFECTS_DUMMY_P_H
#include "kwindoweffects_p.h"

class KWindowEffectsPrivateDummy : public KWindowEffectsPrivateV2
{
public:
    KWindowEffectsPrivateDummy();
    ~KWindowEffectsPrivateDummy() override;
    bool isEffectAvailable(KWindowEffects::Effect effect) override;
    void slideWindow(WId id, KWindowEffects::SlideFromLocation location, int offset) override;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 81)
    QList<QSize> windowSizes(const QList<WId> &ids) override;
#endif
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
    void presentWindows(WId controller, const QList<WId> &ids) override;
    void presentWindows(WId controller, int desktop = NET::OnAllDesktops) override;
    void highlightWindows(WId controller, const QList<WId> &ids) override;
#endif
    void enableBlurBehind(WId window, bool enable = true, const QRegion &region = QRegion()) override;
    void enableBackgroundContrast(WId window,
                                  bool enable = true,
                                  qreal contrast = 1,
                                  qreal intensity = 1,
                                  qreal saturation = 1,
                                  const QRegion &region = QRegion()) override;
    void setBackgroundFrost(QWindow *window, std::optional<QColor> color, const QRegion &region = QRegion()) override;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 67)
    void markAsDashboard(WId window) override;
#endif
};

#endif
