/* This file is part of the KDE libraries
    Copyright (c) 2017 David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License or ( at
    your option ) version 3 or, at the discretion of KDE e.V. ( which shall
    act as a proxy as in section 14 of the GPLv3 ), any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <QTest>
#include "kkeyserver_x11.h"
#include <X11/keysym.h>

class KKeyServerTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void keyQtToSymX_data()
    {
        QTest::addColumn<int>("keyQt");
        QTest::addColumn<uint>("modX");
        QTest::addColumn<int>("keySymX");

        QTest::newRow("a") << int(Qt::Key_A) << uint(0) << XK_A;
        QTest::newRow("CTRL_F1") << int(Qt::ControlModifier|Qt::Key_F1) << KKeyServer::modXCtrl() << XK_F1;
        QTest::newRow("CTRL_1") << int(Qt::ControlModifier|Qt::Key_1) << KKeyServer::modXCtrl() << XK_1;
        QTest::newRow("CTRL_keypad_1") << int(Qt::ControlModifier|Qt::KeypadModifier|Qt::Key_1) << KKeyServer::modXCtrl() << XK_KP_1;
        QTest::newRow("CTRL_keypad_slash") << int(Qt::ControlModifier|Qt::KeypadModifier|Qt::Key_Slash) << KKeyServer::modXCtrl() << XK_KP_Divide;
        QTest::newRow("CTRL_ampersand") << int(Qt::ControlModifier|Qt::Key_Ampersand) << KKeyServer::modXCtrl() << XK_ampersand;
        QTest::newRow("ALT_SHIFT_right") << int(Qt::AltModifier|Qt::ShiftModifier|Qt::Key_Right) << (KKeyServer::modXAlt() | KKeyServer::modXShift()) << XK_Right;
        QTest::newRow("META_SHIFT_print") << int(Qt::MetaModifier|Qt::ShiftModifier|Qt::Key_Print) << (KKeyServer::modXMeta() | KKeyServer::modXShift()) << XK_Print;
    }

    void keyQtToSymX()
    {
        QFETCH(int, keyQt);
        QFETCH(uint, modX);
        QFETCH(int, keySymX);
        int sym;
        QVERIFY(KKeyServer::keyQtToSymX(keyQt, &sym));
        QCOMPARE(sym, keySymX);
        uint mod;
        QVERIFY(KKeyServer::keyQtToModX(keyQt, &mod));
        QCOMPARE(mod, modX);
    }

    void symXToKeyQt_data()
    {
        keyQtToSymX_data();
    }

    void symXToKeyQt()
    {
        QFETCH(int, keyQt);
        QFETCH(uint, modX);
        QFETCH(int, keySymX);
        int keyCodeQt;

        //qDebug() << "modX=" << modX << "keySymX=0x" << QString::number(keySymX, 16);

        QVERIFY(KKeyServer::symXModXToKeyQt(keySymX, modX, &keyCodeQt));
        QCOMPARE(keyCodeQt, keyQt);
    }

};

QTEST_MAIN(KKeyServerTest)

#include "kkeyserver_x11_unittest.moc"
