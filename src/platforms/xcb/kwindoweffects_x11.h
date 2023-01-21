/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KWINDOWEFFECTS_X11_H
#define KWINDOWEFFECTS_X11_H
#include "kwindoweffects_p.h"

class KWindowEffectsPrivateX11 : public KWindowEffectsPrivateV2
{
public:
    KWindowEffectsPrivateX11();
    ~KWindowEffectsPrivateX11() override;
    bool isEffectAvailable(KWindowEffects::Effect effect) override;
    void slideWindow(WId id, KWindowEffects::SlideFromLocation location, int offset) override;
    void enableBlurBehind(WId window, bool enable = true, const QRegion &region = QRegion()) override;
    void enableBackgroundContrast(WId window,
                                  bool enable = true,
                                  qreal contrast = 1,
                                  qreal intensity = 1,
                                  qreal saturation = 1,
                                  const QRegion &region = QRegion()) override;
    void setBackgroundFrost(QWindow *window, QColor color, const QRegion &region = QRegion()) override;
};

#endif
