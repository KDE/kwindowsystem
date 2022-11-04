/*
    SPDX-FileCopyrightText: 2013 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <QSignalSpy>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <private/qtx11extras_p.h>
#else
#include <QX11Info>
#endif

#include <kmanagerselection.h>
#include <kwindoweffects.h>
#include <kwindowsystem.h>
#include <kx11extras.h>
#include <netwm.h>
#include <qtest_widgets.h>
#include <xcb/xcb.h>

#include "cptr_p.h"

Q_DECLARE_METATYPE(KWindowEffects::SlideFromLocation)
Q_DECLARE_METATYPE(KWindowEffects::Effect)

class KWindowEffectsTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testSlideWindow_data();
    void testSlideWindow();
    void testSlideWindowRemove();
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
    void testPresentWindows_data();
    void testPresentWindows();
    void testPresentWindowsEmptyGroup();
    void testPresentWindowsGroup_data();
    void testPresentWindowsGroup();
    void testHighlightWindows_data();
    void testHighlightWindows();
    void testHighlightWindowsEmpty();
#endif
    void testBlur_data();
    void testBlur();
    void testBlurDisable();
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 67)
    void testMarkAsDashboard();
#endif
    void testEffectAvailable_data();
    void testEffectAvailable();

private:
    int32_t locationToValue(KWindowEffects::SlideFromLocation location) const;
    void performSlideWindowTest(xcb_window_t window, int offset, KWindowEffects::SlideFromLocation location) const;
    void performSlideWindowRemoveTest(xcb_window_t window);
    void performWindowsOnPropertyTest(xcb_atom_t atom, const QList<WId> &windows);
    void performAtomIsRemoveTest(xcb_window_t window, xcb_atom_t atom);
    void getHelperAtom(const QByteArray &name, xcb_atom_t *atom) const;
    xcb_atom_t m_slide;
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
    xcb_atom_t m_presentWindows;
    xcb_atom_t m_presentWindowsGroup;
    xcb_atom_t m_highlightWindows;
#endif
    xcb_atom_t m_thumbnails;
    xcb_atom_t m_blur;
    std::unique_ptr<QWindow> m_window;
    std::unique_ptr<QWidget> m_widget;
};

void KWindowEffectsTest::initTestCase()
{
    m_window.reset(new QWindow());
    QVERIFY(m_window->winId() != XCB_WINDOW_NONE);
    m_widget.reset(new QWidget());
    m_widget->show();
    QVERIFY(m_widget->effectiveWinId() != XCB_WINDOW_NONE);

    getHelperAtom(QByteArrayLiteral("_KDE_SLIDE"), &m_slide);
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
    getHelperAtom(QByteArrayLiteral("_KDE_PRESENT_WINDOWS_DESKTOP"), &m_presentWindows);
    getHelperAtom(QByteArrayLiteral("_KDE_PRESENT_WINDOWS_GROUP"), &m_presentWindowsGroup);
    getHelperAtom(QByteArrayLiteral("_KDE_WINDOW_HIGHLIGHT"), &m_highlightWindows);
#endif
    getHelperAtom(QByteArrayLiteral("_KDE_WINDOW_PREVIEW"), &m_thumbnails);
    getHelperAtom(QByteArrayLiteral("_KDE_NET_WM_BLUR_BEHIND_REGION"), &m_blur);
}

void KWindowEffectsTest::getHelperAtom(const QByteArray &name, xcb_atom_t *atom) const
{
    xcb_connection_t *c = QX11Info::connection();
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, name.length(), name.constData());

    UniqueCPointer<xcb_intern_atom_reply_t> reply(xcb_intern_atom_reply(c, atomCookie, nullptr));
    QVERIFY(reply);
    *atom = reply->atom;
}

void KWindowEffectsTest::testSlideWindow_data()
{
    QTest::addColumn<int>("offset");
    QTest::addColumn<KWindowEffects::SlideFromLocation>("location");

    QTest::newRow("Left") << 10 << KWindowEffects::LeftEdge;
    QTest::newRow("Right") << 20 << KWindowEffects::RightEdge;
    QTest::newRow("Top") << 0 << KWindowEffects::TopEdge;
    QTest::newRow("Bottom") << -1 << KWindowEffects::BottomEdge;
}

void KWindowEffectsTest::testSlideWindow()
{
    QFETCH(int, offset);
    QFETCH(KWindowEffects::SlideFromLocation, location);

    KWindowEffects::slideWindow(m_window.get(), location, offset);
    performSlideWindowTest(m_window->winId(), offset, location);
}

void KWindowEffectsTest::testSlideWindowRemove()
{
    xcb_window_t window = m_window->winId();
    // first install the atom
    KWindowEffects::slideWindow(m_window.get(), KWindowEffects::TopEdge, 0);
    performSlideWindowTest(window, 0, KWindowEffects::TopEdge);

    // now delete it
    KWindowEffects::slideWindow(m_window.get(), KWindowEffects::NoEdge, 0);
    performSlideWindowRemoveTest(window);
}

void KWindowEffectsTest::performSlideWindowTest(xcb_window_t window, int offset, KWindowEffects::SlideFromLocation location) const
{
    xcb_connection_t *c = QX11Info::connection();
    xcb_get_property_cookie_t cookie = xcb_get_property_unchecked(c, false, window, m_slide, m_slide, 0, 100);
    UniqueCPointer<xcb_get_property_reply_t> reply(xcb_get_property_reply(c, cookie, nullptr));
    QVERIFY(reply);
    QCOMPARE(reply->format, uint8_t(32));
    QCOMPARE(reply->value_len, uint32_t(2));
    QCOMPARE(reply->type, m_slide);
    int32_t *data = static_cast<int32_t *>(xcb_get_property_value(reply.get()));
    QCOMPARE(data[0], offset);
    QCOMPARE(data[1], locationToValue(location));
}

void KWindowEffectsTest::performSlideWindowRemoveTest(xcb_window_t window)
{
    performAtomIsRemoveTest(window, m_slide);
}

void KWindowEffectsTest::performAtomIsRemoveTest(xcb_window_t window, xcb_atom_t atom)
{
    xcb_connection_t *c = QX11Info::connection();
    xcb_get_property_cookie_t cookie = xcb_get_property_unchecked(c, false, window, atom, atom, 0, 100);
    UniqueCPointer<xcb_get_property_reply_t> reply(xcb_get_property_reply(c, cookie, nullptr));
    QVERIFY(reply);
    QCOMPARE(reply->type, xcb_atom_t(XCB_ATOM_NONE));
}

int32_t KWindowEffectsTest::locationToValue(KWindowEffects::SlideFromLocation location) const
{
    switch (location) {
    case KWindowEffects::LeftEdge:
        return 0;
    case KWindowEffects::TopEdge:
        return 1;
    case KWindowEffects::RightEdge:
        return 2;
    case KWindowEffects::BottomEdge:
        return 3;
    default:
        return -1;
    }
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsTest::testPresentWindows_data()
{
    QTest::addColumn<int>("desktop");

    QTest::newRow("all desktops") << -1;
    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("3") << 3;
    QTest::newRow("4") << 4;
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsTest::testPresentWindows()
{
    QFETCH(int, desktop);

    KWindowEffects::presentWindows(m_window->winId(), desktop);

    xcb_connection_t *c = QX11Info::connection();
    xcb_get_property_cookie_t cookie = xcb_get_property_unchecked(c, false, m_window->winId(), m_presentWindows, m_presentWindows, 0, 100);
    UniqueCPointer<xcb_get_property_reply_t> reply(xcb_get_property_reply(c, cookie, nullptr));
    QVERIFY(reply);
    QCOMPARE(reply->format, uint8_t(32));
    QCOMPARE(reply->value_len, uint32_t(1));
    QCOMPARE(reply->type, m_presentWindows);
    int32_t *data = static_cast<int32_t *>(xcb_get_property_value(reply.get()));
    QCOMPARE(data[0], desktop);
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsTest::testPresentWindowsEmptyGroup()
{
    KWindowEffects::presentWindows(m_window->winId(), QList<WId>());

    xcb_connection_t *c = QX11Info::connection();
    xcb_get_property_cookie_t cookie = xcb_get_property_unchecked(c, false, m_window->winId(), m_presentWindowsGroup, m_presentWindowsGroup, 0, 100);
    UniqueCPointer<xcb_get_property_reply_t> reply(xcb_get_property_reply(c, cookie, nullptr));
    QVERIFY(reply);
    QCOMPARE(reply->type, xcb_atom_t(XCB_ATOM_NONE));
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsTest::testPresentWindowsGroup_data()
{
    QTest::addColumn<QList<WId>>("windows");

    QTest::newRow("one") << (QList<WId>() << m_window->winId());
    QTest::newRow("two") << (QList<WId>() << m_window->winId() << m_widget->effectiveWinId());
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsTest::testPresentWindowsGroup()
{
    QFETCH(QList<WId>, windows);
    KWindowEffects::presentWindows(m_window->winId(), windows);
    performWindowsOnPropertyTest(m_presentWindowsGroup, windows);
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsTest::testHighlightWindows_data()
{
    QTest::addColumn<QList<WId>>("windows");

    QTest::newRow("one") << (QList<WId>() << m_window->winId());
    QTest::newRow("two") << (QList<WId>() << m_window->winId() << m_widget->effectiveWinId());
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsTest::testHighlightWindows()
{
    QFETCH(QList<WId>, windows);
    KWindowEffects::highlightWindows(m_window->winId(), windows);
    performWindowsOnPropertyTest(m_highlightWindows, windows);
}
#endif

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
void KWindowEffectsTest::testHighlightWindowsEmpty()
{
    // ensure it's empty
    KWindowEffects::highlightWindows(m_window->winId(), QList<WId>());
    performAtomIsRemoveTest(m_window->winId(), m_highlightWindows);

    // install some windows on the atom
    QList<WId> windows;
    windows.append(m_window->winId());
    windows.append(m_widget->effectiveWinId());
    KWindowEffects::highlightWindows(m_window->winId(), windows);
    performWindowsOnPropertyTest(m_highlightWindows, windows);

    // and remove it again
    KWindowEffects::highlightWindows(m_window->winId(), QList<WId>());
    performAtomIsRemoveTest(m_window->winId(), m_highlightWindows);
}
#endif

void KWindowEffectsTest::performWindowsOnPropertyTest(xcb_atom_t atom, const QList<WId> &windows)
{
    xcb_connection_t *c = QX11Info::connection();
    xcb_get_property_cookie_t cookie = xcb_get_property_unchecked(c, false, m_window->winId(), atom, atom, 0, 100);
    UniqueCPointer<xcb_get_property_reply_t> reply(xcb_get_property_reply(c, cookie, nullptr));
    QVERIFY(reply);
    QCOMPARE(reply->type, atom);
    QCOMPARE(reply->format, uint8_t(32));
    QCOMPARE(reply->value_len, uint32_t(windows.size()));
    int32_t *data = static_cast<int32_t *>(xcb_get_property_value(reply.get()));
    for (int i = 0; i < windows.size(); ++i) {
        QCOMPARE(data[i], int32_t(windows.at(i)));
    }
}

void KWindowEffectsTest::testBlur_data()
{
    QTest::addColumn<QRegion>("blur");

    QRegion region(0, 0, 10, 10);
    QTest::newRow("one rect") << region;
    region = region.united(QRect(20, 20, 5, 5));
    QTest::newRow("two rects") << region;
    region = region.united(QRect(100, 100, 20, 20));
    QTest::newRow("three rects") << region;
    QTest::newRow("empty") << QRegion();
}

void KWindowEffectsTest::testBlur()
{
    QFETCH(QRegion, blur);

    KWindowEffects::enableBlurBehind(m_window.get(), true, blur);
    xcb_connection_t *c = QX11Info::connection();
    xcb_get_property_cookie_t cookie = xcb_get_property_unchecked(c, false, m_window->winId(), m_blur, XCB_ATOM_CARDINAL, 0, 100);
    UniqueCPointer<xcb_get_property_reply_t> reply(xcb_get_property_reply(c, cookie, nullptr));
    QVERIFY(reply);
    QCOMPARE(reply->type, xcb_atom_t(XCB_ATOM_CARDINAL));
    QCOMPARE(reply->format, uint8_t(32));
    QCOMPARE(reply->value_len, uint32_t(blur.rectCount() * 4));
    uint32_t *data = static_cast<uint32_t *>(xcb_get_property_value(reply.get()));
    int dataOffset = 0;
    for (const QRect &rect : blur) {
        QCOMPARE(data[dataOffset++], uint32_t(rect.x()));
        QCOMPARE(data[dataOffset++], uint32_t(rect.y()));
        QCOMPARE(data[dataOffset++], uint32_t(rect.width()));
        QCOMPARE(data[dataOffset++], uint32_t(rect.height()));
    }
}

void KWindowEffectsTest::testBlurDisable()
{
    KWindowEffects::enableBlurBehind(m_window.get(), false);
    performAtomIsRemoveTest(m_window->winId(), m_blur);

    KWindowEffects::enableBlurBehind(m_window.get(), true);
    // verify that it got added
    xcb_connection_t *c = QX11Info::connection();
    xcb_get_property_cookie_t cookie = xcb_get_property_unchecked(c, false, m_window->winId(), m_blur, XCB_ATOM_CARDINAL, 0, 100);
    UniqueCPointer<xcb_get_property_reply_t> reply(xcb_get_property_reply(c, cookie, nullptr));
    QVERIFY(reply);
    QCOMPARE(reply->type, xcb_atom_t(XCB_ATOM_CARDINAL));

    // and disable
    KWindowEffects::enableBlurBehind(m_window.get(), false);
    performAtomIsRemoveTest(m_window->winId(), m_blur);
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 67)
void KWindowEffectsTest::testMarkAsDashboard()
{
    const QByteArray className = QByteArrayLiteral("dashboard");
    // should not yet be set
    xcb_connection_t *c = QX11Info::connection();
    xcb_get_property_cookie_t cookie = xcb_get_property_unchecked(c, false, m_window->winId(), XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 0, 100);
    UniqueCPointer<xcb_get_property_reply_t> reply(xcb_get_property_reply(c, cookie, nullptr));
    QVERIFY(reply);
    QCOMPARE(reply->type, xcb_atom_t(XCB_ATOM_STRING));
    QCOMPARE(reply->format, uint8_t(8));
    char *data = static_cast<char *>(xcb_get_property_value(reply.get()));
    QVERIFY(QByteArray(data) != className);

    // now mark as dashboard
    KWindowEffects::markAsDashboard(m_window->winId());
    cookie = xcb_get_property_unchecked(c, false, m_window->winId(), XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 0, 100);
    reply.reset(xcb_get_property_reply(c, cookie, nullptr));
    QVERIFY(reply);
    QCOMPARE(reply->type, xcb_atom_t(XCB_ATOM_STRING));
    QCOMPARE(reply->format, uint8_t(8));
    QCOMPARE(reply->value_len, uint32_t(19));
    data = static_cast<char *>(xcb_get_property_value(reply.get()));
    QCOMPARE(QByteArray(data), className);
    data = data + 10;
    QCOMPARE(QByteArray(data), className);
}
#endif

void KWindowEffectsTest::testEffectAvailable_data()
{
    QTest::addColumn<KWindowEffects::Effect>("effect");
    QTest::addColumn<QByteArray>("propertyName");

    QTest::newRow("slide") << KWindowEffects::Slide << QByteArrayLiteral("_KDE_SLIDE");
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 82)
    QTest::newRow("PresentWindows") << KWindowEffects::PresentWindows << QByteArrayLiteral("_KDE_PRESENT_WINDOWS_DESKTOP");
    QTest::newRow("PresentWindowsGroup") << KWindowEffects::PresentWindowsGroup << QByteArrayLiteral("_KDE_PRESENT_WINDOWS_GROUP");
    QTest::newRow("HighlightWindows") << KWindowEffects::HighlightWindows << QByteArrayLiteral("_KDE_WINDOW_HIGHLIGHT");
#endif
    QTest::newRow("BlurBehind") << KWindowEffects::BlurBehind << QByteArrayLiteral("_KDE_NET_WM_BLUR_BEHIND_REGION");
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(5, 67)
    QTest::newRow("Dashboard") << KWindowEffects::Dashboard << QByteArrayLiteral("_WM_EFFECT_KDE_DASHBOARD");
#endif
    QTest::newRow("BackgroundContrast") << KWindowEffects::BackgroundContrast << QByteArrayLiteral("_KDE_NET_WM_BACKGROUND_CONTRAST_REGION");
}

void KWindowEffectsTest::testEffectAvailable()
{
    NETRootInfo rootInfo(QX11Info::connection(), NET::Supported | NET::SupportingWMCheck);
    if (qstrcmp(rootInfo.wmName(), "KWin") == 0) {
        QSKIP("KWin running, we don't want to interact with the running system");
    }
    // this test verifies whether an effect is available
    QFETCH(KWindowEffects::Effect, effect);
    // without a compositing manager it's not available
    // try-verify as there still might be the selection claimed from previous data run
    QTRY_VERIFY(!KX11Extras::compositingActive());
    QVERIFY(!KWindowEffects::isEffectAvailable(effect));

    // fake the compositor
    QSignalSpy compositingChangedSpy(KX11Extras::self(), &KX11Extras::compositingChanged);
    QVERIFY(compositingChangedSpy.isValid());
    KSelectionOwner compositorSelection("_NET_WM_CM_S0");
    QSignalSpy claimedSpy(&compositorSelection, &KSelectionOwner::claimedOwnership);
    QVERIFY(claimedSpy.isValid());
    compositorSelection.claim(true);
    QVERIFY(claimedSpy.wait());
    QCOMPARE(compositingChangedSpy.count(), 1);
    QCOMPARE(compositingChangedSpy.first().first().toBool(), true);
    QVERIFY(KX11Extras::compositingActive());

    // but not yet available
    QVERIFY(!KWindowEffects::isEffectAvailable(effect));

    // set the atom
    QFETCH(QByteArray, propertyName);
    xcb_connection_t *c = QX11Info::connection();
    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, propertyName.length(), propertyName.constData());
    UniqueCPointer<xcb_intern_atom_reply_t> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
    QVERIFY(atom);
    unsigned char dummy = 0;
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, QX11Info::appRootWindow(), atom->atom, atom->atom, 8, 1, &dummy);
    xcb_flush(c);

    // now the effect should be available
    QVERIFY(KWindowEffects::isEffectAvailable(effect));

    // delete the property again
    xcb_delete_property(c, QX11Info::appRootWindow(), atom->atom);
    xcb_flush(c);
    // which means it's no longer available
    QVERIFY(!KWindowEffects::isEffectAvailable(effect));

    // remove compositing selection
    compositorSelection.release();
    QVERIFY(compositingChangedSpy.wait());
    QCOMPARE(compositingChangedSpy.count(), 2);
    QCOMPARE(compositingChangedSpy.last().first().toBool(), false);
    QVERIFY(!KX11Extras::compositingActive());
}

QTEST_MAIN(KWindowEffectsTest)

#include "kwindoweffectstest.moc"
