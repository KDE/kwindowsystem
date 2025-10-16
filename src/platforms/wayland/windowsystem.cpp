/*
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2023 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "windowsystem.h"
#include "logging.h"
#include "surfacehelper.h"
#include "waylandxdgactivationv1_p.h"
#include "waylandxdgdialogv1_p.h"
#include "waylandxdgforeignv2_p.h"

#include <KWaylandExtras>
#include <KWindowSystem>

#include "qwayland-plasma-window-management.h"
#include <QEvent>
#include <QGuiApplication>
#include <QLibraryInfo>
#include <QPixmap>
#include <QPoint>
#include <QString>
#include <QTimer>
#include <QVersionNumber>
#include <QWaylandClientExtensionTemplate>
#include <QWindow>
#include <qpa/qplatformnativeinterface.h>
#include <qpa/qplatformwindow_p.h>

constexpr const char *c_kdeXdgForeignExportedProperty("_kde_xdg_foreign_exported_v2");
constexpr const char *c_kdeXdgForeignImportedProperty("_kde_xdg_foreign_imported_v2");
constexpr const char *c_kdeXdgForeignPendingHandleProperty("_kde_xdg_foreign_pending_handle");

class WindowManagement : public QWaylandClientExtensionTemplate<WindowManagement>, public QtWayland::org_kde_plasma_window_management
{
public:
    WindowManagement()
        : QWaylandClientExtensionTemplate<WindowManagement>(17)
    {
    }

    void org_kde_plasma_window_management_show_desktop_changed(uint32_t state) override
    {
        showingDesktop = state == show_desktop_enabled;
        KWindowSystem::self()->showingDesktopChanged(showingDesktop);
    }

    bool showingDesktop = false;
};

WindowSystem::WindowSystem()
    : m_lastToken(qEnvironmentVariable("XDG_ACTIVATION_TOKEN"))
{
    m_windowManagement = new WindowManagement;
}

WindowSystem::~WindowSystem()
{
    delete m_windowManagement;
}

void WindowSystem::activateWindow(QWindow *win, long int time)
{
    Q_UNUSED(time);
    auto s = surfaceForWindow(win);
    if (!s) {
        return;
    }
    WaylandXdgActivationV1 *activation = WaylandXdgActivationV1::self();
    if (!activation->isActive()) {
        return;
    }
    activation->activate(m_lastToken, s);
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(6, 19)
void WindowSystem::requestToken(QWindow *window, uint32_t serial, const QString &app_id)
{
    if (window) {
        window->create();
    }
    wl_surface *wlSurface = surfaceForWindow(window);

    WaylandXdgActivationV1 *activation = WaylandXdgActivationV1::self();
    if (!activation->isActive()) {
        // Ensure that xdgActivationTokenArrived is always emitted asynchronously
        QTimer::singleShot(0, [serial] {
            Q_EMIT KWaylandExtras::self()->xdgActivationTokenArrived(serial, {});
        });
        return;
    }

    auto waylandApp = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    auto seat = waylandApp ? waylandApp->lastInputSeat() : nullptr;
    auto tokenReq = activation->requestXdgActivationToken(seat, wlSurface, serial, app_id);
    connect(tokenReq, &WaylandXdgActivationTokenV1::done, KWindowSystem::self(), [serial](const QString &token) {
        Q_EMIT KWaylandExtras::self()->xdgActivationTokenArrived(serial, token);
    });
}
#endif

void WindowSystem::setCurrentToken(const QString &token)
{
    m_lastToken = token;
}

quint32 WindowSystem::lastInputSerial(QWindow *window)
{
    Q_UNUSED(window)
    if (auto waylandApp = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>()) {
        return waylandApp->lastInputSerial();
    }
    return 0;
}

void WindowSystem::setShowingDesktop(bool showing)
{
    if (!m_windowManagement->isActive()) {
        return;
    }
    m_windowManagement->show_desktop(showing ? WindowManagement::show_desktop_enabled : WindowManagement::show_desktop_disabled);
}

bool WindowSystem::showingDesktop()
{
    if (!m_windowManagement->isActive()) {
        return false;
    }
    return m_windowManagement->showingDesktop;
}

void WindowSystem::exportWindow(QWindow *window)
{
    auto emitHandle = [window](const QString &handle) {
        // Ensure that windowExported is always emitted asynchronously.
        QMetaObject::invokeMethod(
            window,
            [window, handle] {
                Q_EMIT KWaylandExtras::self()->windowExported(window, handle);
            },
            Qt::QueuedConnection);
    };

    if (!window) {
        return;
    }

    window->create();

    auto waylandWindow = window->nativeInterface<QNativeInterface::Private::QWaylandWindow>();
    if (!waylandWindow) {
        emitHandle({});
        return;
    }

    auto &exporter = WaylandXdgForeignExporterV2::self();
    if (!exporter.isActive()) {
        emitHandle({});
        return;
    }

    // We want to use QObject::property(char*) and use dynamic properties on the object rather than
    // call QWaylandWindow::property(QString) and send it around.
    WaylandXdgForeignExportedV2 *exported = waylandWindow->property(c_kdeXdgForeignExportedProperty).value<WaylandXdgForeignExportedV2 *>();
    if (!exported) {
        exported = exporter.exportToplevel(surfaceForWindow(window));
        exported->setParent(waylandWindow);

        waylandWindow->setProperty(c_kdeXdgForeignExportedProperty, QVariant::fromValue(exported));
        connect(exported, &QObject::destroyed, waylandWindow, [waylandWindow] {
            waylandWindow->setProperty(c_kdeXdgForeignExportedProperty, QVariant());
        });

        connect(exported, &WaylandXdgForeignExportedV2::handleReceived, window, [window](const QString &handle) {
            Q_EMIT KWaylandExtras::self()->windowExported(window, handle);
        });
    }

    if (!exported->handle().isEmpty()) {
        emitHandle(exported->handle());
    }
}

void WindowSystem::unexportWindow(QWindow *window)
{
    auto waylandWindow = window ? window->nativeInterface<QNativeInterface::Private::QWaylandWindow>() : nullptr;
    if (!waylandWindow) {
        return;
    }

    WaylandXdgForeignExportedV2 *exported = waylandWindow->property(c_kdeXdgForeignExportedProperty).value<WaylandXdgForeignExportedV2 *>();
    delete exported;
    Q_ASSERT(!waylandWindow->property(c_kdeXdgForeignExportedProperty).isValid());
}

void WindowSystem::setMainWindow(QWindow *window, const QString &handle)
{
    if (!window) {
        return;
    }

    window->create();
    auto waylandWindow = window->nativeInterface<QNativeInterface::Private::QWaylandWindow>();
    if (!waylandWindow) {
        return;
    }

    // We want to use QObject::property(char*) and use dynamic properties on the object rather than
    // call QWaylandWindow::property(QString) and send it around.
    auto *imported = waylandWindow->property(c_kdeXdgForeignImportedProperty).value<WaylandXdgForeignImportedV2 *>();
    // Window already parented with a different handle? Delete imported so we import the new one later.
    if (imported && imported->handle() != handle) {
        delete imported;
        imported = nullptr;
        Q_ASSERT(!waylandWindow->property(c_kdeXdgForeignImportedProperty).isValid());
    }

    // Don't bother.
    if (handle.isEmpty()) {
        return;
    }

    if (window->isExposed()) {
        doSetMainWindow(window, handle);
    } else {
        // We can only import an XDG toplevel.
        // QWaylandWindow::surfaceRoleCreated is only in Qt 6.8,
        // in earlier versions wait for the window be exposed,
        // since QWaylandWindow::wlSurfaceCreated is too early.
        connect(waylandWindow, &QNativeInterface::Private::QWaylandWindow::surfaceRoleCreated, window, [window, handle] {
            doSetMainWindow(window, handle);
        });
    }
}

bool WindowSystem::eventFilter(QObject *watched, QEvent *event)
{
    return QObject::eventFilter(watched, event);
}

void WindowSystem::doSetMainWindow(QWindow *window, const QString &handle)
{
    Q_ASSERT(window);
    Q_ASSERT(!handle.isEmpty());

    auto waylandWindow = window->nativeInterface<QNativeInterface::Private::QWaylandWindow>();
    if (!waylandWindow) {
        return;
    }

    auto &importer = WaylandXdgForeignImporterV2::self();
    if (!importer.isActive()) {
        return;
    }

    Q_ASSERT(!waylandWindow->property(c_kdeXdgForeignImportedProperty).isValid());

    WaylandXdgForeignImportedV2 *imported = importer.importToplevel(handle);
    imported->set_parent_of(surfaceForWindow(window)); // foreign parent.
    imported->setParent(waylandWindow); // memory owner.

    waylandWindow->setProperty(c_kdeXdgForeignImportedProperty, QVariant::fromValue(imported));
    connect(imported, &QObject::destroyed, waylandWindow, [waylandWindow] {
        waylandWindow->setProperty(c_kdeXdgForeignImportedProperty, QVariant());
    });

    // Before Qt 6.10, Qt sets XDG Dialog modal only when it has a transient parent.
    if (QLibraryInfo::version() < QVersionNumber(6, 10, 0)) {
        auto *oldDialog = waylandWindow->findChild<WaylandXdgDialogV1 *>();
        if (window->modality() != Qt::NonModal && !oldDialog) {
            auto &xdgDialog = WaylandXdgDialogWmV1::self();
            if (xdgDialog.isActive()) {
                if (auto *xdgToplevel = xdgToplevelForWindow(window)) {
                    auto *dialog = xdgDialog.getDialog(xdgToplevel);
                    dialog->set_modal();
                    dialog->setParent(waylandWindow);
                }
            }
        } else {
            delete oldDialog;
        }
    }
}

QFuture<QString> WindowSystem::xdgActivationToken(QWindow *window, uint32_t serial, const QString &appId)
{
    WaylandXdgActivationV1 *activation = WaylandXdgActivationV1::self();
    if (!activation->isActive()) {
        return QtFuture::makeReadyValueFuture(QString());
    }

    auto waylandApp = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if (!waylandApp) {
        return QtFuture::makeReadyValueFuture(QString());
    }

    if (window) {
        window->create();
    }
    wl_surface *wlSurface = surfaceForWindow(window);

    auto token = activation->requestXdgActivationToken(waylandApp->lastInputSeat(), wlSurface, serial, appId);
    return token->future();
}

#include "moc_windowsystem.cpp"
