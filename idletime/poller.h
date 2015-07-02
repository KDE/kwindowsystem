/*
Copyright (C) 2015 Martin Gräßlin <mgraesslin@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
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
    Poller(QObject *parent = 0);
    virtual ~Poller();

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
    KWayland::Client::Registry *m_registry = nullptr;
    KWayland::Client::IdleTimeout *m_catchResumeTimeout = nullptr;
    QHash<int, KWayland::Client::IdleTimeout*> m_timeouts;
};

#endif /* XSYNCBASEDPOLLER_H */
