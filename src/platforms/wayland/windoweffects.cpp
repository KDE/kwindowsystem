/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "windoweffects.h"
#include "waylandintegration.h"

#include <QDebug>
#include <QExposeEvent>
#include <QGuiApplication>
#include <QWidget>

#include <KWayland/Client/compositor.h>
#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/plasmashell.h>
#include <KWayland/Client/plasmawindowmanagement.h>
#include <KWayland/Client/region.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/slide.h>
#include <KWayland/Client/surface.h>
#include <private/qwaylandwindow_p.h>

#include <QWaylandClientExtensionTemplate>
#include <qwaylandclientextension.h>

#include "qwayland-blur.h"
#include "qwayland-contrast.h"

#include "surfacehelper.h"

class BlurManager : public QWaylandClientExtensionTemplate<BlurManager>, public QtWayland::org_kde_kwin_blur_manager
{
public:
    BlurManager()
        : QWaylandClientExtensionTemplate<BlurManager>(1)
    {
    }
};

class Blur : public QObject, public QtWayland::org_kde_kwin_blur
{
public:
    Blur(struct ::org_kde_kwin_blur *object, QObject *parent)
        : QObject(parent)
        , QtWayland::org_kde_kwin_blur(object)
    {
    }

    ~Blur() override
    {
        release();
    }
};

class ContrastManager : public QWaylandClientExtensionTemplate<ContrastManager>, public QtWayland::org_kde_kwin_contrast_manager
{
public:
    ContrastManager()
        : QWaylandClientExtensionTemplate<ContrastManager>(2)
    {
    }
};

class Contrast : public QObject, public QtWayland::org_kde_kwin_contrast
{
public:
    Contrast(struct ::org_kde_kwin_contrast *object, QObject *parent)
        : QObject(parent)
        , QtWayland::org_kde_kwin_contrast(object)
    {
    }

    ~Contrast() override
    {
        release();
    }
};

WindowEffects::WindowEffects()
    : QObject()
    , KWindowEffectsPrivateV2()
{
    auto registry = WaylandIntegration::self()->registry();

    m_blurManager = new BlurManager();
    m_contrastManager = new ContrastManager();

    // The KWindowEffects API doesn't provide any signals to notify that the particular
    // effect has become unavailable. So we re-install effects when the corresponding globals
    // are added.

    connect(m_blurManager, &BlurManager::activeChanged, this, [this] {
        for (auto it = m_blurRegions.constBegin(); it != m_blurRegions.constEnd(); ++it) {
            installBlur(it.key(), m_blurManager->isActive(), *it);
        }
    });

    connect(m_contrastManager, &ContrastManager::activeChanged, this, [this] {
        for (auto it = m_backgroundConstrastRegions.constBegin(); it != m_backgroundConstrastRegions.constEnd(); ++it) {
            if (m_contrastManager->isActive()) {
                installContrast(it.key(), true, it->contrast, it->intensity, it->saturation, it->region);
            } else {
                installContrast(it.key(), false);
            }
        }
    });

    connect(registry, &KWayland::Client::Registry::slideAnnounced, this, [this]() {
        for (auto it = m_slideMap.constBegin(); it != m_slideMap.constEnd(); ++it) {
            installSlide(it.key(), it->location, it->offset);
        }
    });
    connect(registry, &KWayland::Client::Registry::slideRemoved, this, [this]() {
        for (auto it = m_slideMap.constBegin(); it != m_slideMap.constEnd(); ++it) {
            installSlide(it.key(), KWindowEffects::SlideFromLocation::NoEdge, 0);
        }
    });
}

WindowEffects::~WindowEffects()
{
    delete m_blurManager;
    delete m_contrastManager;
}

QWindow *WindowEffects::windowForId(WId wid)
{
    QWindow *window = nullptr;

    for (auto win : qApp->allWindows()) {
        if (win->winId() == wid) {
            window = win;
            break;
        }
    }
    return window;
}

void WindowEffects::trackWindow(QWindow *window)
{
    if (!m_windowWatchers.contains(window)) {
        window->installEventFilter(this);
        auto conn = connect(window, &QObject::destroyed, this, [this, window]() {
            resetBlur(window);
            m_blurRegions.remove(window);
            resetContrast(window);
            m_backgroundConstrastRegions.remove(window);
            m_slideMap.remove(window);
            m_windowWatchers.remove(window);
        });
        m_windowWatchers[window] << conn;
        auto waylandWindow = dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle());
        if (waylandWindow) {
            auto conn = connect(waylandWindow, &QtWaylandClient::QWaylandWindow::wlSurfaceDestroyed, this, [this, window]() {
                resetBlur(window);
                resetContrast(window);
            });
            m_windowWatchers[window] << conn;
        }
    }
}

void WindowEffects::releaseWindow(QWindow *window)
{
    if (!m_blurRegions.contains(window) && !m_backgroundConstrastRegions.contains(window) && !m_slideMap.contains(window)) {
        for (const auto &conn : m_windowWatchers[window]) {
            disconnect(conn);
        }
        window->removeEventFilter(this);
        m_windowWatchers.remove(window);
    }
}

// Helper function to replace a QObject value in the map and delete the old one.
template<typename MapType>
void replaceValue(MapType &map, typename MapType::key_type key, typename MapType::mapped_type value)
{
    if (auto oldValue = map.take(key)) {
        oldValue->deleteLater();
    }
    if (value) {
        map[key] = value;
    }
}

void WindowEffects::resetBlur(QWindow *window, Blur *blur)
{
    replaceValue(m_blurs, window, blur);
}

void WindowEffects::resetContrast(QWindow *window, Contrast *contrast)
{
    replaceValue(m_contrasts, window, contrast);
}

bool WindowEffects::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Expose) {
        auto ee = static_cast<QExposeEvent *>(event);

        if ((ee->region().isNull())) {
            return false;
        }

        auto window = qobject_cast<QWindow *>(watched);
        if (!window) {
            return false;
        }

        {
            auto it = m_blurRegions.constFind(window);
            if (it != m_blurRegions.constEnd()) {
                installBlur(window, true, *it);
            }
        }
        {
            auto it = m_backgroundConstrastRegions.constFind(window);
            if (it != m_backgroundConstrastRegions.constEnd()) {
                installContrast(window, true, it->contrast, it->intensity, it->saturation, it->region);
            }
        }
    }
    return false;
}

bool WindowEffects::isEffectAvailable(KWindowEffects::Effect effect)
{
    switch (effect) {
    case KWindowEffects::BackgroundContrast:
        return m_contrastManager->isActive();
    case KWindowEffects::BlurBehind:
        return m_blurManager->isActive();
    case KWindowEffects::Slide:
        return WaylandIntegration::self()->waylandSlideManager() != nullptr;
    default:
        return false;
    }
}

void WindowEffects::slideWindow(WId id, KWindowEffects::SlideFromLocation location, int offset)
{
    auto window = windowForId(id);
    if (!window) {
        return;
    }
    if (location != KWindowEffects::SlideFromLocation::NoEdge) {
        m_slideMap[window] = SlideData{
            .location = location,
            .offset = offset,
        };
        trackWindow(window);
    } else {
        m_slideMap.remove(window);
        releaseWindow(window);
    }

    installSlide(window, location, offset);
}

void WindowEffects::installSlide(QWindow *window, KWindowEffects::SlideFromLocation location, int offset)
{
    if (!WaylandIntegration::self()->waylandSlideManager()) {
        return;
    }
    KWayland::Client::Surface *surface = KWayland::Client::Surface::fromWindow(window);
    if (surface) {
        if (location != KWindowEffects::SlideFromLocation::NoEdge) {
            auto slide = WaylandIntegration::self()->waylandSlideManager()->createSlide(surface, surface);

            KWayland::Client::Slide::Location convertedLoc;
            switch (location) {
            case KWindowEffects::SlideFromLocation::TopEdge:
                convertedLoc = KWayland::Client::Slide::Location::Top;
                break;
            case KWindowEffects::SlideFromLocation::LeftEdge:
                convertedLoc = KWayland::Client::Slide::Location::Left;
                break;
            case KWindowEffects::SlideFromLocation::RightEdge:
                convertedLoc = KWayland::Client::Slide::Location::Right;
                break;
            case KWindowEffects::SlideFromLocation::BottomEdge:
            default:
                convertedLoc = KWayland::Client::Slide::Location::Bottom;
                break;
            }

            slide->setLocation(convertedLoc);
            slide->setOffset(offset);
            slide->commit();
        } else {
            WaylandIntegration::self()->waylandSlideManager()->removeSlide(surface);
        }

        WaylandIntegration::self()->waylandConnection()->flush();
    }
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)

QList<QSize> WindowEffects::windowSizes(const QList<WId> &ids)
{
    Q_UNUSED(ids)
    QList<QSize> sizes;
    return sizes;
}
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void WindowEffects::presentWindows(WId controller, const QList<WId> &ids)
{
    Q_UNUSED(controller)
    Q_UNUSED(ids)
}
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void WindowEffects::presentWindows(WId controller, int desktop)
{
    Q_UNUSED(controller)
    Q_UNUSED(desktop)
}
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void WindowEffects::highlightWindows(WId controller, const QList<WId> &ids)
{
    Q_UNUSED(controller)
    Q_UNUSED(ids)
}
#endif

void WindowEffects::enableBlurBehind(WId winId, bool enable, const QRegion &region)
{
    auto window = windowForId(winId);
    if (!window) {
        return;
    }
    if (enable) {
        trackWindow(window);
        m_blurRegions[window] = region;
    } else {
        resetBlur(window);
        m_blurRegions.remove(window);
        releaseWindow(window);
    }

    installBlur(window, enable, region);
}

void WindowEffects::installBlur(QWindow *window, bool enable, const QRegion &region)
{
    if (!m_blurManager->isActive()) {
        return;
    }

    wl_surface *surface = surfaceForWindow(window);

    if (surface) {
        if (enable) {
            auto blur = new Blur(m_blurManager->create(surface), window);
            std::unique_ptr<KWayland::Client::Region> wlRegion(WaylandIntegration::self()->waylandCompositor()->createRegion(region, nullptr));
            blur->set_region(*(wlRegion.get()));
            blur->commit();
            resetBlur(window, blur);
        } else {
            resetBlur(window);
            m_blurManager->unset(surface);
        }

        WaylandIntegration::self()->waylandConnection()->flush();
    }
}

void WindowEffects::enableBackgroundContrast(WId winId, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    auto window = windowForId(winId);
    if (!window) {
        return;
    }
    if (enable) {
        trackWindow(window);
        m_backgroundConstrastRegions[window].contrast = contrast;
        m_backgroundConstrastRegions[window].intensity = intensity;
        m_backgroundConstrastRegions[window].saturation = saturation;
        m_backgroundConstrastRegions[window].region = region;
    } else {
        resetContrast(window);
        m_backgroundConstrastRegions.remove(window);
        releaseWindow(window);
    }

    installContrast(window, enable, contrast, intensity, saturation, region);
}

void WindowEffects::installContrast(QWindow *window, bool enable, qreal contrast, qreal intensity, qreal saturation, const QRegion &region)
{
    if (!m_contrastManager->isActive()) {
        return;
    }

    wl_surface *surface = surfaceForWindow(window);
    if (surface) {
        if (enable) {
            auto backgroundContrast = new Contrast(m_contrastManager->create(surface), window);
            std::unique_ptr<KWayland::Client::Region> wlRegion(WaylandIntegration::self()->waylandCompositor()->createRegion(region, nullptr));
            backgroundContrast->set_region(*(wlRegion.get()));
            backgroundContrast->set_contrast(wl_fixed_from_double(contrast));
            backgroundContrast->set_intensity(wl_fixed_from_double(intensity));
            backgroundContrast->set_saturation(wl_fixed_from_double(saturation));
            backgroundContrast->commit();
            resetContrast(window, backgroundContrast);
        } else {
            resetContrast(window);
            m_contrastManager->unset(surface);
        }

        WaylandIntegration::self()->waylandConnection()->flush();
    }
}

void WindowEffects::setBackgroundFrost(QWindow *window, QColor color, const QRegion &region)
{
    if (!m_contrastManager->isActive()) {
        return;
    }

    wl_surface *surface = surfaceForWindow(window);
    if (!surface) {
        return;
    }
    if (!color.isValid()) {
        resetContrast(window);
        m_contrastManager->unset(surface);
        return;
    }

    auto backgroundContrast = new Contrast(m_contrastManager->create(surface), window);
    std::unique_ptr<KWayland::Client::Region> wlRegion(WaylandIntegration::self()->waylandCompositor()->createRegion(region, nullptr));
    backgroundContrast->set_region(*(wlRegion.get()));
    backgroundContrast->set_frost(color.red(), color.green(), color.blue(), color.alpha());
    backgroundContrast->commit();
    resetContrast(window, backgroundContrast);

    WaylandIntegration::self()->waylandConnection()->flush();
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void WindowEffects::markAsDashboard(WId window)
{
    Q_UNUSED(window)
}
#endif
