/*
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef WINDOWSYSTEM_H
#define WINDOWSYSTEM_H

#include "kwindowsystem_p.h"

#include <QObject>

class WindowManagement;

class WindowSystem : public QObject, public KWindowSystemPrivateV2
{
    Q_OBJECT
public:
    WindowSystem();
    ~WindowSystem() override;
    void activateWindow(QWindow *win, long time) override;
    void requestToken(QWindow *win, uint32_t serial, const QString &app_id) override;
    quint32 lastInputSerial(QWindow *window) override;
    void setCurrentToken(const QString &token) override;
    bool showingDesktop() override;
    void setShowingDesktop(bool showing) override;

private:
    QString m_lastToken;
    WindowManagement *m_windowManagement;
};

#endif
