/*
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef POLLER_H
#define POLLER_H

#include <KIdleTime/private/abstractsystempoller.h>

#include <QHash>

class QMutex;
class QWaitCondition;

namespace KWayland
{
namespace Client
{
class Seat;
class Idle;
class ConnectionThread;
class IdleTimeout;
class Registry;
}
}

class Poller : public AbstractSystemPoller
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kidletime.AbstractSystemPoller" FILE "kwayland.json")
    Q_INTERFACES(AbstractSystemPoller)

public:
    explicit Poller(QObject *parent = nullptr);
    ~Poller() override;

    bool isAvailable() override;
    bool setUpPoller() override;
    void unloadPoller() override;

public Q_SLOTS:
    void addTimeout(int nextTimeout) override;
    void removeTimeout(int nextTimeout) override;
    QList<int> timeouts() const override;
    int forcePollRequest() override;
    void catchIdleEvent() override;
    void stopCatchingIdleEvents() override;
    void simulateUserActivity() override;

private:
    bool initWayland();
    struct Seat {
        quint32 version = 0;
        quint32 name = 0;
        KWayland::Client::Seat *seat = nullptr;
    } m_seat;
    struct Idle {
        quint32 version = 0;
        quint32 name = 0;
        KWayland::Client::Idle *idle = nullptr;
    } m_idle;
    bool m_inited = false;
    QScopedPointer<QMutex> m_registryMutex;
    QScopedPointer<QWaitCondition> m_registryAnnouncedCondition;
    KWayland::Client::ConnectionThread *m_connectionThread = nullptr;
    KWayland::Client::Registry *m_registry = nullptr;
    KWayland::Client::IdleTimeout *m_catchResumeTimeout = nullptr;
    QHash<int, KWayland::Client::IdleTimeout *> m_timeouts;
};

#endif /* XSYNCBASEDPOLLER_H */
