/*
    SPDX-FileCopyrightText: 2016 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/
#include <KWindowSystem>
#include <kmanagerselection.h>
#include <QtTest>

class CompositingEnabledTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testRecreatingNetEventFilter();
};

void CompositingEnabledTest::testRecreatingNetEventFilter()
{
    // this test simulates the condition that the compositor gets enabled while the NetEventFilter gets recreated
    QVERIFY(!KWindowSystem::compositingActive());

    // fake the compositor
    QSignalSpy compositingChangedSpy(KWindowSystem::self(), &KWindowSystem::compositingChanged);
    QVERIFY(compositingChangedSpy.isValid());
    KSelectionOwner compositorSelection("_NET_WM_CM_S0");
    QSignalSpy claimedSpy(&compositorSelection, &KSelectionOwner::claimedOwnership);
    QVERIFY(claimedSpy.isValid());
    compositorSelection.claim(true);
    connect(&compositorSelection, &KSelectionOwner::claimedOwnership,
        [] {
            // let's connect to a signal which will cause a re-creation of NetEventFilter
            QSignalSpy workAreaChangedSpy(KWindowSystem::self(), &KWindowSystem::workAreaChanged);
            QVERIFY(workAreaChangedSpy.isValid());
        }
    );

    QVERIFY(claimedSpy.wait());
    QTRY_VERIFY(KWindowSystem::compositingActive());
    QCOMPARE(compositingChangedSpy.count(), 1);
}

QTEST_MAIN(CompositingEnabledTest)
#include "compositingenabled_test.moc"
