/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2021 Aleix Pol <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KWAYLANDEXTRAS_H
#define KWAYLANDEXTRAS_H

#include <QFuture>
#include <QObject>
#include <QWindow>

#include <kwindowsystem_export.h>

/*!
 * \class KWaylandExtras
 * \inmodule KWindowSystem
 * \brief A collection of functions to do Wayland things.
 * \since 6.0
 */
class KWINDOWSYSTEM_EXPORT KWaylandExtras : public QObject
{
    Q_OBJECT

public:
    /*!
     *
     */
    static KWaylandExtras *self();

    /*!
     * Requests an xdg_activation_v1 token for a specific window \a win with the given \a app_id.
     *
     * \a serial Serial of the event that triggered the request.
     *
     * \sa lastInputSerial
     */
    Q_INVOKABLE static void requestXdgActivationToken(QWindow *win, uint32_t serial, const QString &app_id);

    /*!
     * Offers the seat's current serial for the given \a window.
     */
    Q_INVOKABLE static quint32 lastInputSerial(QWindow *window);

    /*!
     * Requests to export the given \a window using xdg_foreign_v2.
     *
     * \sa windowExported
     * \since 6.0
     */
    Q_INVOKABLE static void exportWindow(QWindow *window);

    /*!
     * Unexports the \a window previously exported using xdg_foreign_v2.
     *
     * Asks the compositor to revoke the handle.
     *
     * \since 6.0
     */
    Q_INVOKABLE static void unexportWindow(QWindow *window);

    /*!
     * Requests an xdg_activation_v1 token for a specific window \a window with the given \a appId.
     * The \a serial indicates an event that triggered the request.
     *
     * \note No xdgActivationTokenArrived() signal will be emitted for this token.
     *
     * \since 6.19
     */
    static QFuture<QString> xdgActivationToken(QWindow *window, uint32_t serial, const QString &appId);

    /*!
     * Requests an xdg_activation_v1 token for a specific window \a window with the given \a appId.
     * The last received input serial will be used to request the token.
     *
     * \note No xdgActivationTokenArrived() signal will be emitted for this token.
     *
     * \since 6.19
     */
    static QFuture<QString> xdgActivationToken(QWindow *window, const QString &appId);

Q_SIGNALS:
    /*!
     * Activation \a token to pass to the client.
     *
     * \a serial Serial of the event that triggered the request
     *
     * \sa requestXdgActivationToken
     */
    void xdgActivationTokenArrived(int serial, const QString &token);

    /*!
     * The \a handle of the given \a window to pass to the client.
     * \sa exportWindow
     * \since 6.0
     */
    void windowExported(QWindow *window, const QString &handle);

private:
    KWaylandExtras();
    ~KWaylandExtras();

    void *d;
};

#endif
