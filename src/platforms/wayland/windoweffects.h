/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef WINDOWEFFECTS_H
#define WINDOWEFFECTS_H
#include <KWindowSystem/private/kwindoweffects_p.h>
#include <kwindowsystem_version.h>

#include <QHash>

namespace KWayland
{
namespace Client
{
class BlurManager;
class ContrastManager;
class Compositor;
class ConnectionThread;
}
}

class WindowEffects : public QObject, public KWindowEffectsPrivateV2
{
    Q_OBJECT
public:
    WindowEffects();
    ~WindowEffects() override;

    static QWindow *windowForId(WId);

    bool eventFilter(QObject *watched, QEvent *event) override;
    void trackWindow(QWindow *window);
    void releaseWindow(QWindow *window);

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
    void enableBlurBehind(WId winId, bool enable = true, const QRegion &region = QRegion()) override;
    void enableBlurBehind(QWindow *window, bool enable, const QRegion &region);
    void enableBackgroundContrast(WId winId,
                                  bool enable = true,
                                  qreal contrast = 1,
                                  qreal intensity = 1,
                                  qreal saturation = 1,
                                  const QRegion &region = QRegion()) override;
    void enableBackgroundContrast(QWindow *window,
                                  bool enable = true,
                                  qreal contrast = 1,
                                  qreal intensity = 1,
                                  qreal saturation = 1,
                                  const QRegion &region = QRegion());
    void setBackgroundFrost(QWindow *window, QColor color, const QRegion &region = QRegion()) override;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 67)
    void markAsDashboard(WId window) override;
#endif
private:
    QHash<QWindow *, QMetaObject::Connection> m_windowWatchers;
    QHash<QWindow *, QRegion> m_blurRegions;
    struct BackgroundContrastData {
        qreal contrast = 1;
        qreal intensity = 1;
        qreal saturation = 1;
        QRegion region;
    };
    QHash<QWindow *, BackgroundContrastData> m_backgroundConstrastRegions;
};

#endif
