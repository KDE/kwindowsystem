/* This file is part of the KDE libraries

    Copyright 2012 David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
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
        BroadcastStaticDisplay,
        BroadcastStaticConnection
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

void KXMessages_UnitTest::testStart_data()
{
    QTest::addColumn<KXMessages_UnitTest::BroadcastType>("broadcastType");

    QTest::newRow("object")     << BroadcastMessageObject;
    QTest::newRow("display")    << BroadcastStaticDisplay;
    QTest::newRow("connection") << BroadcastStaticConnection;
}

void KXMessages_UnitTest::testStart()
{
    QFETCH(KXMessages_UnitTest::BroadcastType, broadcastType);
    const QByteArray type = "kxmessage_unittest";
    KXMessages receiver(type);

    // Check that all message sizes work, i.e. no bug when exactly 20 or 40 bytes,
    // despite the internal splitting.
    QString message;
    for (int i = 1; i < 50; ++i) {
        QSignalSpy spy(&receiver, SIGNAL(gotMessage(QString)));
        message += "a";
        switch (broadcastType) {
        case KXMessages_UnitTest::BroadcastMessageObject:
            m_msgs.broadcastMessage(type, message);
            break;
        case KXMessages_UnitTest::BroadcastStaticDisplay:
            QVERIFY(KXMessages::broadcastMessageX(QX11Info::display(), type.constData(), message));
            break;
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
