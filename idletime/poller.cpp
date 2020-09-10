/*
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "poller.h"

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/idle.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/seat.h>

#include <QGuiApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

Q_DECLARE_LOGGING_CATEGORY(POLLER)
Q_LOGGING_CATEGORY(POLLER, "kf5idletime_kwayland")

Poller::Poller(QObject *parent)
    : AbstractSystemPoller(parent)
    , m_registryMutex(new QMutex())
    , m_registryAnnouncedCondition(new QWaitCondition())
{
    if (!initWayland()) {
        m_inited = true;
    }
}

Poller::~Poller() = default;

bool Poller::initWayland()
{
    using namespace KWayland::Client;
    m_connectionThread = ConnectionThread::fromApplication(this);
    if (!m_connectionThread) {
        return false;
    }
    // need to be able to cleanup prior to the Wayland connection being destroyed
    // otherwise we get a crash in libwayland
    connect(reinterpret_cast<QObject*>(qApp->platformNativeInterface()), &QObject::destroyed, this, &Poller::unloadPoller);
    m_registry = new Registry(this);
    m_registry->create(m_connectionThread);
    connect(m_registry, &Registry::seatAnnounced, this,
        [this] (quint32 name, quint32 version) {
            QMutexLocker locker(m_registryMutex.data());
            if (m_seat.name != 0) {
                // already have a seat
                return;
            }
            m_seat.name = name;
            m_seat.version = version;
        }, Qt::DirectConnection
    );
    connect(m_registry, &Registry::idleAnnounced, this,
        [this] (quint32 name, quint32 version) {
            QMutexLocker locker(m_registryMutex.data());
            if (m_idle.name != 0) {
                // already have a seat
                return;
            }
            m_idle.name = name;
            m_idle.version = version;
        }, Qt::DirectConnection
    );
    connect(m_registry, &Registry::interfacesAnnounced, this,
        [this] {
            m_registryMutex->lock();
            m_inited = true;
            m_registryMutex->unlock();
            m_registryAnnouncedCondition->wakeAll();
        }, Qt::DirectConnection
    );

    m_registry->setup();
    m_connectionThread->roundtrip();
    return true;
}

bool Poller::isAvailable()
{
    m_registryMutex->lock();
    while (!m_inited) {
        m_registryAnnouncedCondition->wait(m_registryMutex.data());
    }
    m_registryMutex->unlock();
    return m_idle.name != 0;
}

bool Poller::setUpPoller()
{
    if (!m_registry || !isAvailable()) {
        return false;
    }
    if (!m_seat.seat) {
        m_seat.seat = m_registry->createSeat(m_seat.name, m_seat.version, this);
    }
    if (!m_idle.idle) {
        m_idle.idle = m_registry->createIdle(m_idle.name, m_idle.version, this);
    }
    return m_seat.seat->isValid() && m_idle.idle->isValid();
}

void Poller::unloadPoller()
{
    qDeleteAll(m_timeouts);
    m_timeouts.clear();

    delete m_catchResumeTimeout;
    m_catchResumeTimeout = nullptr;

    delete m_registry;
    m_registry = nullptr;

    delete m_seat.seat;
    m_seat.seat = nullptr;
    delete m_idle.idle;
    m_idle.idle = nullptr;

    delete m_connectionThread;
    m_connectionThread = nullptr;
}

void Poller::addTimeout(int nextTimeout)
{
    if (m_timeouts.contains(nextTimeout)) {
        return;
    }
    if (!m_idle.idle) {
        return;
    }
    auto timeout = m_idle.idle->getTimeout(nextTimeout, m_seat.seat, this);
    m_timeouts.insert(nextTimeout, timeout);
    connect(timeout, &KWayland::Client::IdleTimeout::idle, this,
        [this, nextTimeout] {
            emit timeoutReached(nextTimeout);
        }
    );
    connect(timeout, &KWayland::Client::IdleTimeout::resumeFromIdle, this, &Poller::resumingFromIdle);
}

void Poller::removeTimeout(int nextTimeout)
{
    auto it = m_timeouts.find(nextTimeout);
    if (it == m_timeouts.end()) {
        return;
    }
    delete it.value();
    m_timeouts.erase(it);
}

QList< int > Poller::timeouts() const
{
    return QList<int>();
}

void Poller::catchIdleEvent()
{
    if (m_catchResumeTimeout) {
        // already setup
        return;
    }
    if (!m_idle.idle) {
        return;
    }
    m_catchResumeTimeout = m_idle.idle->getTimeout(0, m_seat.seat, this);
    connect(m_catchResumeTimeout, &KWayland::Client::IdleTimeout::resumeFromIdle, this,
        [this] {
            stopCatchingIdleEvents();
            emit resumingFromIdle();
        }
    );
}

void Poller::stopCatchingIdleEvents()
{
    delete m_catchResumeTimeout;
    m_catchResumeTimeout = nullptr;
}

int Poller::forcePollRequest()
{
    qCWarning(POLLER) << "This plugin does not support polling idle time";
    return 0;
}

void Poller::simulateUserActivity()
{
    for (auto it = m_timeouts.constBegin(); it != m_timeouts.constEnd(); ++it) {
        it.value()->simulateUserActivity();
    }
}
