/*
 *   Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "kwindowinfo.h"
#include "kwindowsystem.h"
#include "netwm.h"
#include "nettesthelper.h"

#include <qtest_widgets.h>
#include <QScreen>
#include <QSignalSpy>
#include <QX11Info>
Q_DECLARE_METATYPE(WId)
Q_DECLARE_METATYPE(NET::State)
Q_DECLARE_METATYPE(NET::WindowType)
Q_DECLARE_METATYPE(NET::WindowTypeMask)
Q_DECLARE_METATYPE(NET::WindowTypes)


class KWindowInfoX11Test : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init();
    void cleanup();

    void testState_data();
    void testState();
    void testMinimized();
    void testMappingState();
    void testWindowType_data();
    void testWindowType();
    void testDesktop();
    void testWindowClass();
    void testWindowRole();
    void testClientMachine();
    void testName();
    void testTransientFor();
    void testGroupLeader();
    void testExtendedStrut();
    void testGeometry();

    // actionSupported is not tested as it's too window manager specific
    // we could write a test against KWin's behavior, but that would fail on
    // build.kde.org as we use OpenBox there.

private:
    bool waitForWindow(QSignalSpy &spy, WId winId, NET::Property property) const;
    bool verifyMinimized(WId window) const;

    QScopedPointer<QWidget> window;
};

bool KWindowInfoX11Test::waitForWindow(QSignalSpy& spy, WId winId, NET::Property property) const
{
    // we need to wait, window manager has to react and update the property.
    bool foundOurWindow = false;
    for (int i = 0; i < 10; ++i) {
        spy.wait(50);
        if (spy.isEmpty()) {
            continue;
        }
        for (auto it = spy.constBegin(); it != spy.constEnd(); ++it) {
            if (it->first().value<WId>() != winId) {
                continue;
            }
            if (it->last().toInt() != property) {
                continue;
            }
            foundOurWindow = true;
            break;
        }
        if (foundOurWindow) {
            break;
        }
        spy.clear();
    }
    return foundOurWindow;
}

bool KWindowInfoX11Test::verifyMinimized(WId window) const
{
    KWindowInfo info(window, NET::WMState | NET::XAWMState);
    return info.isMinimized();
}

void KWindowInfoX11Test::init()
{
    // create the window and ensure it has been managed
    window.reset(new QWidget());
    qRegisterMetaType<WId>("WId");
    QSignalSpy spy(KWindowSystem::self(), SIGNAL(windowAdded(WId)));
    window->show();
    bool foundOurWindow = false;
    for (int i = 0; i < 50; ++i) {
        spy.wait(50);
        if (spy.isEmpty()) {
            continue;
        }
        for (auto it = spy.constBegin(); it != spy.constEnd(); ++it) {
            if (it->isEmpty()) {
                continue;
            }
            if (it->first().value<WId>() == window->winId()) {
                foundOurWindow = true;
                break;
            }
        }
        if (foundOurWindow) {
            break;
        }
        spy.clear();
    }
}

void KWindowInfoX11Test::cleanup()
{
    // we hide the window and wait till it is gone so that we have a clean state in next test
    if (!window.isNull() && window->isVisible()) {
        WId id = window->winId();
        QSignalSpy spy(KWindowSystem::self(), SIGNAL(windowRemoved(WId)));
        window->hide();
        bool foundOurWindow = false;
        for (int i = 0; i < 50; ++i) {
            spy.wait(50);
            if (spy.isEmpty()) {
                continue;
            }
            for (auto it = spy.constBegin(); it != spy.constEnd(); ++it) {
                if (it->first().value<WId>() == id) {
                    foundOurWindow = true;
                    break;
                }
            }
            if (foundOurWindow) {
                break;
            }
            spy.clear();
        }
    }
    window.reset();
}

void KWindowInfoX11Test::testState_data()
{
    QTest::addColumn<unsigned long>("state");

    QTest::newRow("max")         << static_cast<unsigned long>(NET::Max);
    QTest::newRow("maxHoriz")    << static_cast<unsigned long>(NET::MaxHoriz);
    QTest::newRow("shaded")      << static_cast<unsigned long>(NET::Shaded);
    QTest::newRow("skipTaskbar") << static_cast<unsigned long>(NET::SkipTaskbar);
    QTest::newRow("skipPager")   << static_cast<unsigned long>(NET::SkipPager);
    QTest::newRow("keep above")  << static_cast<unsigned long>(NET::KeepAbove);
    QTest::newRow("keep below")  << static_cast<unsigned long>(NET::KeepBelow);
    QTest::newRow("fullscreen")  << static_cast<unsigned long>(NET::FullScreen);

    // NOTE: modal, sticky and hidden cannot be tested with this variant
    // demands attention is not tested as that's already part of the first run adjustments
}

void KWindowInfoX11Test::testState()
{
    QFETCH(unsigned long, state);
    QX11Info::getTimestamp();

    KWindowInfo info(window->winId(), NET::WMState);
    QVERIFY(info.valid());
    // all states except demands attention
    for (int i = 0; i < 12; ++i) {
        QVERIFY(!info.hasState(1 << i));
    }

    QSignalSpy spy(KWindowSystem::self(), SIGNAL(windowChanged(WId,unsigned int)));
    // now we have a clean window and can do fun stuff
    KWindowSystem::setState(window->winId(), state);

    QVERIFY(waitForWindow(spy, window->winId(), NET::WMState));

    KWindowInfo info3(window->winId(), NET::WMState);
    QVERIFY(info3.valid());
    QCOMPARE(info3.state(), static_cast<unsigned long>(state));
    QVERIFY(info3.hasState(state));
}

void KWindowInfoX11Test::testMinimized()
{
    // should not be minimized, now
    QVERIFY(!verifyMinimized(window->winId()));

    window->showMinimized();
    // TODO: improve by using signalspy?
    QTest::qWait(100);

    // should be minimized, now
    QVERIFY(verifyMinimized(window->winId()));

    // back to normal
    window->showNormal();
    // TODO: improve by using signalspy?
    QTest::qWait(100);

    // should no longer be minimized
    QVERIFY(!verifyMinimized(window->winId()));
}

void KWindowInfoX11Test::testMappingState()
{
    KWindowInfo info(window->winId(), NET::XAWMState);
    QCOMPARE(info.mappingState(), NET::Visible);

    window->showMinimized();
    // TODO: improve by using signalspy?
    QTest::qWait(100);
    KWindowInfo info2(window->winId(), NET::XAWMState);
    QCOMPARE(info2.mappingState(), NET::Iconic);

    window->hide();
    // TODO: improve by using signalspy?
    QTest::qWait(100);
    KWindowInfo info3(window->winId(), NET::XAWMState);
    QCOMPARE(info3.mappingState(), NET::Withdrawn);
}

void KWindowInfoX11Test::testWindowType_data()
{
    QTest::addColumn<NET::WindowTypeMask>("mask");
    QTest::addColumn<NET::WindowType>("type");
    QTest::addColumn<NET::WindowType>("expectedType");

    QTest::newRow("desktop")            << NET::DesktopMask      << NET::Desktop      << NET::Desktop;
    QTest::newRow("dock")               << NET::DockMask         << NET::Dock         << NET::Dock;
    QTest::newRow("toolbar")            << NET::ToolbarMask      << NET::Toolbar      << NET::Toolbar;
    QTest::newRow("menu")               << NET::MenuMask         << NET::Menu         << NET::Menu;
    QTest::newRow("dialog")             << NET::DialogMask       << NET::Dialog       << NET::Dialog;
    QTest::newRow("override")           << NET::OverrideMask     << NET::Override     << NET::Override;
    QTest::newRow("override as normal") << NET::NormalMask       << NET::Override     << NET::Normal;
    QTest::newRow("topmenu")            << NET::TopMenuMask      << NET::TopMenu      << NET::TopMenu;
    QTest::newRow("topmenu as dock")    << NET::DockMask         << NET::TopMenu      << NET::Dock;
    QTest::newRow("utility")            << NET::UtilityMask      << NET::Utility      << NET::Utility;
    QTest::newRow("utility as dialog")  << NET::DialogMask       << NET::Utility      << NET::Dialog;
    QTest::newRow("splash")             << NET::SplashMask       << NET::Splash       << NET::Splash;
    QTest::newRow("splash as dock")     << NET::DockMask         << NET::Splash       << NET::Dock;
    QTest::newRow("dropdownmenu")       << NET::DropdownMenuMask << NET::DropdownMenu << NET::DropdownMenu;
    QTest::newRow("popupmenu")          << NET::PopupMenuMask    << NET::PopupMenu    << NET::PopupMenu;
    QTest::newRow("popupmenu as menu")  << NET::MenuMask         << NET::Menu         << NET::Menu;
    QTest::newRow("tooltip")            << NET::TooltipMask      << NET::Tooltip      << NET::Tooltip;
    QTest::newRow("notification")       << NET::NotificationMask << NET::Notification << NET::Notification;
    QTest::newRow("ComboBox")           << NET::ComboBoxMask     << NET::ComboBox     << NET::ComboBox;
    QTest::newRow("DNDIcon")            << NET::DNDIconMask      << NET::DNDIcon      << NET::DNDIcon;

    // incorrect masks
    QTest::newRow("desktop-unknown")      << NET::NormalMask << NET::Desktop      << NET::Unknown;
    QTest::newRow("dock-unknown")         << NET::NormalMask << NET::Dock         << NET::Unknown;
    QTest::newRow("toolbar-unknown")      << NET::NormalMask << NET::Toolbar      << NET::Unknown;
    QTest::newRow("menu-unknown")         << NET::NormalMask << NET::Menu         << NET::Unknown;
    QTest::newRow("dialog-unknown")       << NET::NormalMask << NET::Dialog       << NET::Unknown;
    QTest::newRow("override-unknown")     << NET::DialogMask << NET::Override     << NET::Unknown;
    QTest::newRow("topmenu-unknown")      << NET::NormalMask << NET::TopMenu      << NET::Unknown;
    QTest::newRow("utility-unknown")      << NET::NormalMask << NET::Utility      << NET::Unknown;
    QTest::newRow("splash-unknown")       << NET::NormalMask << NET::Splash       << NET::Unknown;
    QTest::newRow("dropdownmenu-unknown") << NET::NormalMask << NET::DropdownMenu << NET::Unknown;
    QTest::newRow("popupmenu-unknown")    << NET::NormalMask << NET::PopupMenu    << NET::Unknown;
    QTest::newRow("tooltip-unknown")      << NET::NormalMask << NET::Tooltip      << NET::Unknown;
    QTest::newRow("notification-unknown") << NET::NormalMask << NET::Notification << NET::Unknown;
    QTest::newRow("ComboBox-unknown")     << NET::NormalMask << NET::ComboBox     << NET::Unknown;
    QTest::newRow("DNDIcon-unknown")      << NET::NormalMask << NET::DNDIcon      << NET::Unknown;
}

void KWindowInfoX11Test::testWindowType()
{
    KWindowInfo info(window->winId(), NET::WMWindowType);
    QCOMPARE(info.windowType(NET::NormalMask), NET::Normal);

    QFETCH(NET::WindowTypeMask, mask);
    QFETCH(NET::WindowType, type);
    QFETCH(NET::WindowType, expectedType);

    KWindowSystem::setType(window->winId(), type);
    // setWindowType just changes an xproperty, so a roundtrip waiting for another property ensures we are updated
    QX11Info::getTimestamp();
    KWindowInfo info2(window->winId(), NET::WMWindowType);
    QCOMPARE(info2.windowType(mask), expectedType);
}

void KWindowInfoX11Test::testDesktop()
{
    if (KWindowSystem::numberOfDesktops() < 2) {
        QSKIP("We need at least two virtual desktops to perform proper virtual desktop testing");
    }
    KWindowInfo info(window->winId(), NET::WMDesktop);
    QVERIFY(info.isOnCurrentDesktop());
    QVERIFY(!info.onAllDesktops());
    QCOMPARE(info.desktop(), KWindowSystem::currentDesktop());
    for (int i = 1; i < KWindowSystem::numberOfDesktops(); i++) {
        if (i == KWindowSystem::currentDesktop()) {
            QVERIFY(info.isOnDesktop(i));
        } else {
            QVERIFY(!info.isOnDesktop(i));
        }
    }

    // set on all desktop
    QSignalSpy spy(KWindowSystem::self(), SIGNAL(windowChanged(WId,unsigned int)));
    KWindowSystem::setOnAllDesktops(window->winId(), true);
    QVERIFY(waitForWindow(spy, window->winId(), NET::WMDesktop));

    KWindowInfo info2(window->winId(), NET::WMDesktop);
    QVERIFY(info2.isOnCurrentDesktop());
    QVERIFY(info2.onAllDesktops());
    QCOMPARE(info2.desktop(), int(NET::OnAllDesktops));
    for (int i = 1; i < KWindowSystem::numberOfDesktops(); i++) {
        QVERIFY(info2.isOnDesktop(i));
    }

    const int desktop = (KWindowSystem::currentDesktop() % KWindowSystem::numberOfDesktops()) + 1;
    spy.clear();
    KWindowSystem::setOnDesktop(window->winId(), desktop);
    QX11Info::getTimestamp();
    QVERIFY(waitForWindow(spy, window->winId(), NET::WMDesktop));

    KWindowInfo info3(window->winId(), NET::WMDesktop);
    QVERIFY(!info3.isOnCurrentDesktop());
    QVERIFY(!info3.onAllDesktops());
    QCOMPARE(info3.desktop(), desktop);
    for (int i = 1; i < KWindowSystem::numberOfDesktops(); i++) {
        if (i == desktop) {
            QVERIFY(info3.isOnDesktop(i));
        } else {
            QVERIFY(!info3.isOnDesktop(i));
        }
    }
}

void KWindowInfoX11Test::testWindowClass()
{
    KWindowInfo info(window->winId(), 0, NET::WM2WindowClass);
    QCOMPARE(info.windowClassName(), QByteArrayLiteral("kwindowinfox11test"));
    QCOMPARE(info.windowClassClass(), QByteArrayLiteral("kwindowinfox11test"));

    // window class needs to be changed using xcb
    xcb_change_property(QX11Info::connection(), XCB_PROP_MODE_REPLACE, window->winId(),
                        XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8, 7, "foo\0bar");
    xcb_flush(QX11Info::connection());

    // it's just a property change so we can easily refresh
    QX11Info::getTimestamp();

    KWindowInfo info2(window->winId(), 0, NET::WM2WindowClass);
    QCOMPARE(info2.windowClassName(), QByteArrayLiteral("foo"));
    QCOMPARE(info2.windowClassClass(), QByteArrayLiteral("bar"));
}

void KWindowInfoX11Test::testWindowRole()
{
    KWindowInfo info(window->winId(), 0, NET::WM2WindowRole);
    QVERIFY(info.windowRole().isNull());

    // window role needs to be changed using xcb
    KXUtils::Atom atom(QX11Info::connection(), QByteArrayLiteral("WM_WINDOW_ROLE"));
    xcb_change_property(QX11Info::connection(), XCB_PROP_MODE_REPLACE, window->winId(),
                        atom, XCB_ATOM_STRING, 8, 3, "bar");
    xcb_flush(QX11Info::connection());

    // it's just a property change so we can easily refresh
    QX11Info::getTimestamp();

    KWindowInfo info2(window->winId(), 0, NET::WM2WindowRole);
    QCOMPARE(info2.windowRole(), QByteArrayLiteral("bar"));
}

void KWindowInfoX11Test::testClientMachine()
{
    KWindowInfo info(window->winId(), 0, NET::WM2ClientMachine);
    QVERIFY(info.clientMachine().isNull());

    // client machine needs to be set through xcb
    xcb_change_property(QX11Info::connection(), XCB_PROP_MODE_REPLACE, window->winId(),
                        XCB_ATOM_WM_CLIENT_MACHINE, XCB_ATOM_STRING, 8, 9, "localhost");
    xcb_flush(QX11Info::connection());

    // it's just a property change so we can easily refresh
    QX11Info::getTimestamp();

    KWindowInfo info2(window->winId(), 0, NET::WM2ClientMachine);
    QCOMPARE(info2.clientMachine(), QByteArrayLiteral("localhost"));
}

void KWindowInfoX11Test::testName()
{
    KWindowInfo info(window->winId(), NET::WMName | NET::WMVisibleName | NET::WMIconName | NET::WMVisibleIconName | NET::WMState | NET::XAWMState);
    QCOMPARE(info.name(),                     QStringLiteral("kwindowinfox11test"));
    QCOMPARE(info.visibleName(),              QStringLiteral("kwindowinfox11test"));
    QCOMPARE(info.visibleNameWithState(),     QStringLiteral("kwindowinfox11test"));
    QCOMPARE(info.iconName(),                 QStringLiteral("kwindowinfox11test"));
    QCOMPARE(info.visibleIconName(),          QStringLiteral("kwindowinfox11test"));
    QCOMPARE(info.visibleIconNameWithState(), QStringLiteral("kwindowinfox11test"));

    window->showMinimized();
    // TODO: improve by using signalspy?
    QTest::qWait(100);
    // should be minimized, now
    QVERIFY(verifyMinimized(window->winId()));

    // that should have changed the visible name
    KWindowInfo info2(window->winId(), NET::WMName | NET::WMVisibleName | NET::WMIconName | NET::WMVisibleIconName | NET::WMState | NET::XAWMState);
    QCOMPARE(info2.name(),                     QStringLiteral("kwindowinfox11test"));
    QCOMPARE(info2.visibleName(),              QStringLiteral("kwindowinfox11test"));
    QCOMPARE(info2.visibleNameWithState(),     QStringLiteral("(kwindowinfox11test)"));
    QCOMPARE(info2.iconName(),                 QStringLiteral("kwindowinfox11test"));
    QCOMPARE(info2.visibleIconName(),          QStringLiteral("kwindowinfox11test"));
    QCOMPARE(info2.visibleIconNameWithState(), QStringLiteral("(kwindowinfox11test)"));

    NETRootInfo rootInfo(QX11Info::connection(), NET::Supported | NET::SupportingWMCheck);
    if (qstrcmp(rootInfo.wmName(), "Openbox") == 0) {
        QSKIP("setting name test fails on openbox");
    }

    // create a low level NETWinInfo to manipulate the name
    NETWinInfo winInfo(QX11Info::connection(), window->winId(), QX11Info::appRootWindow(), NET::WMName);
    winInfo.setName("foobar");

    QX11Info::getTimestamp();

    KWindowInfo info3(window->winId(), NET::WMName | NET::WMVisibleName | NET::WMIconName | NET::WMVisibleIconName | NET::WMState | NET::XAWMState);
    QCOMPARE(info3.name(),                     QStringLiteral("foobar"));
    QCOMPARE(info3.visibleName(),              QStringLiteral("foobar"));
    QCOMPARE(info3.visibleNameWithState(),     QStringLiteral("(foobar)"));
    QCOMPARE(info3.iconName(),                 QStringLiteral("foobar"));
    QCOMPARE(info3.visibleIconName(),          QStringLiteral("foobar"));
    QCOMPARE(info3.visibleIconNameWithState(), QStringLiteral("(foobar)"));
}

void KWindowInfoX11Test::testTransientFor()
{
    KWindowInfo info(window->winId(), 0, NET::WM2TransientFor);
    QCOMPARE(info.transientFor(), WId(0));

    // let's create a second window
    QScopedPointer<QWidget> window2(new QWidget());
    window2->show();
    QTest::qWaitForWindowExposed(window2.data());

    // update the transient for of window1 to window2
    const uint32_t id = window2->winId();
    xcb_change_property(QX11Info::connection(), XCB_PROP_MODE_REPLACE, window->winId(),
                        XCB_ATOM_WM_TRANSIENT_FOR, XCB_ATOM_WINDOW, 32, 1, &id);
    xcb_flush(QX11Info::connection());

    KWindowInfo info2(window->winId(), 0, NET::WM2TransientFor);
    QCOMPARE(info2.transientFor(), window2->winId());
}

void KWindowInfoX11Test::testGroupLeader()
{
    KWindowInfo info(window->winId(), 0, NET::WM2GroupLeader);
    QCOMPARE(info.groupLeader(), WId(0));

    // TODO: here we should try to set a group leader and re-read it
    // this needs setting and parsing the WMHints
}

void KWindowInfoX11Test::testExtendedStrut()
{
    KWindowInfo info(window->winId(), 0, NET::WM2ExtendedStrut);
    NETExtendedStrut strut = info.extendedStrut();
    QCOMPARE(strut.bottom_end, 0);
    QCOMPARE(strut.bottom_start, 0);
    QCOMPARE(strut.bottom_width, 0);
    QCOMPARE(strut.left_end, 0);
    QCOMPARE(strut.left_start, 0);
    QCOMPARE(strut.left_width, 0);
    QCOMPARE(strut.right_end, 0);
    QCOMPARE(strut.right_start, 0);
    QCOMPARE(strut.right_width, 0);
    QCOMPARE(strut.top_end, 0);
    QCOMPARE(strut.top_start, 0);
    QCOMPARE(strut.top_width, 0);

    KWindowSystem::setExtendedStrut(window->winId(), 10, 20, 30, 40, 5, 15, 25, 35, 2, 12, 22, 32);

    // it's just an xprop, so one roundtrip is good enough
    QX11Info::getTimestamp();

    KWindowInfo info2(window->winId(), 0, NET::WM2ExtendedStrut);
    strut = info2.extendedStrut();
    QCOMPARE(strut.bottom_end, 32);
    QCOMPARE(strut.bottom_start, 22);
    QCOMPARE(strut.bottom_width, 12);
    QCOMPARE(strut.left_end, 30);
    QCOMPARE(strut.left_start, 20);
    QCOMPARE(strut.left_width, 10);
    QCOMPARE(strut.right_end, 15);
    QCOMPARE(strut.right_start, 5);
    QCOMPARE(strut.right_width, 40);
    QCOMPARE(strut.top_end, 2);
    QCOMPARE(strut.top_start, 35);
    QCOMPARE(strut.top_width, 25);
}

void KWindowInfoX11Test::testGeometry()
{
    KWindowInfo info(window->winId(), NET::WMGeometry | NET::WMFrameExtents);
    QCOMPARE(info.geometry(), window->geometry());
    QCOMPARE(info.frameGeometry(), window->frameGeometry());

    QSignalSpy spy(KWindowSystem::self(), SIGNAL(windowChanged(WId,unsigned int)));

    // this is tricky, KWin is smart and doesn't allow all geometries we pass in
    // setting to center of screen should work, though
    QRect geo(window->windowHandle()->screen()->geometry().center() - QPoint(window->width()/2-5, window->height()/2-5),
              window->size() + QSize(10, 10));
    window->setGeometry(geo);
    waitForWindow(spy, window->winId(), NET::WMGeometry);

    KWindowInfo info2(window->winId(), NET::WMGeometry | NET::WMFrameExtents);
    QCOMPARE(info2.geometry(), window->geometry());
    QCOMPARE(info2.geometry(), geo);
    QCOMPARE(info2.frameGeometry(), window->frameGeometry());
}

QTEST_MAIN(KWindowInfoX11Test)

#include "kwindowinfox11test.moc"
