/*  This file is part of the KDE libraries

    SPDX-FileCopyrightText: 2012 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <kxmessages.h>
#include <QSignalSpy>
#include <QX11Info>
#include <qtest_widgets.h>

class KXMessages_UnitTest : public QObject
{
    Q_OBJECT
public:
    enum BroadcastType {
        BroadcastMessageObject,
#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 0)
        BroadcastStaticDisplay,
#endif
        BroadcastStaticConnection,
    };
    enum ReceiverType {
        ReceiverTypeDefault,
        ReceiverTypeConnection,
    };
    KXMessages_UnitTest()
        : m_msgs()
    {
    }

private Q_SLOTS:
    void testStart_data();
    void testStart();

private:
    KXMessages m_msgs;
};

Q_DECLARE_METATYPE(KXMessages_UnitTest::BroadcastType)
Q_DECLARE_METATYPE(KXMessages_UnitTest::ReceiverType)

void KXMessages_UnitTest::testStart_data()
{
    QTest::addColumn<KXMessages_UnitTest::BroadcastType>("broadcastType");
    QTest::addColumn<KXMessages_UnitTest::ReceiverType>("receiverType");

    QTest::newRow("object")     << BroadcastMessageObject << ReceiverTypeDefault;
#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 0)
    QTest::newRow("display")    << BroadcastStaticDisplay << ReceiverTypeDefault;
#endif
    QTest::newRow("connection") << BroadcastStaticConnection << ReceiverTypeDefault;
    QTest::newRow("object/xcb")     << BroadcastMessageObject << ReceiverTypeConnection;
#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 0)
    QTest::newRow("display/xcb")    << BroadcastStaticDisplay << ReceiverTypeConnection;
#endif
    QTest::newRow("connection/xcb") << BroadcastStaticConnection << ReceiverTypeConnection;
}

void KXMessages_UnitTest::testStart()
{
    QFETCH(KXMessages_UnitTest::BroadcastType, broadcastType);
    QFETCH(KXMessages_UnitTest::ReceiverType, receiverType);
    const QByteArray type = "kxmessage_unittest";
    QScopedPointer<KXMessages> receiver;
    switch (receiverType) {
    case KXMessages_UnitTest::ReceiverTypeDefault:
        receiver.reset(new KXMessages(type));
        break;
    case KXMessages_UnitTest::ReceiverTypeConnection:
        receiver.reset(new KXMessages(QX11Info::connection(), QX11Info::appRootWindow(), type));
        break;
    default:
        Q_UNREACHABLE();
        break;
    }

    // Check that all message sizes work, i.e. no bug when exactly 20 or 40 bytes,
    // despite the internal splitting.
    QString message;
    for (int i = 1; i < 50; ++i) {
        QSignalSpy spy(receiver.data(), SIGNAL(gotMessage(QString)));
        message += "a";
        switch (broadcastType) {
        case KXMessages_UnitTest::BroadcastMessageObject:
            m_msgs.broadcastMessage(type, message);
            break;
#if KWINDOWSYSTEM_ENABLE_DEPRECATED_SINCE(5, 0)
        case KXMessages_UnitTest::BroadcastStaticDisplay:
            QVERIFY(KXMessages::broadcastMessageX(QX11Info::display(), type.constData(), message));
            break;
#endif
        case KXMessages_UnitTest::BroadcastStaticConnection:
            QVERIFY(KXMessages::broadcastMessageX(QX11Info::connection(), type.constData(), message, QX11Info::appScreen()));
            break;
        }

        QVERIFY(spy.wait());
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), message);
    }
}

QTEST_MAIN(KXMessages_UnitTest)

#include "kxmessages_unittest.moc"
