/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2016 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "../src/platforms/xcb/netwm.h"
#include <kwindowsystem.h>
#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QX11Info>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("WId"),
                                 QStringLiteral("window id for the window to take the icon from"),
                                 QStringLiteral("[WId]"));
    parser.addHelpOption();
    parser.process(app);

    QWidget window;
    QVBoxLayout *vbox = new QVBoxLayout(&window);

    bool ok = false;
    qulonglong id = parser.positionalArguments().first().toULongLong(&ok);
    if (!ok) {
        // try hex
        id = parser.positionalArguments().first().toULongLong(&ok, 16);
    }
    if (!ok) {
        return 1;
    }
    NETWinInfo info(QX11Info::connection(), id, QX11Info::appRootWindow(), NET::WMIcon, NET::WM2WindowClass | NET::WM2IconPixmap);
    auto addIcons = [&window, vbox, &id, &info] (const QString &name, int flag) {
        QLabel *title = new QLabel(name, &window);
        vbox->addWidget(title);
        QIcon icons;
        if (flag & KWindowSystem::NETWM) {
            const int *iconSizes = info.iconSizes();
            int index = 0;
            while (iconSizes[index] != 0 && iconSizes[index + 1] != 0) {
                const int width = iconSizes[index++];
                const int height = iconSizes[index++];
                NETIcon ni = info.icon(width, height);
                if (ni.data) {
                    QImage img((uchar *) ni.data, (int) ni.size.width, (int) ni.size.height, QImage::Format_ARGB32);
                    if (!img.isNull()) {
                        icons.addPixmap(QPixmap::fromImage(img));
                    }
                }
            }
        }

        if (flag & KWindowSystem::WMHints) {
            icons.addPixmap(KWindowSystem::icon(id, 0, 0, false, KWindowSystem::WMHints, &info));
        }

        if (flag & KWindowSystem::ClassHint) {
            icons = QIcon::fromTheme(QString::fromUtf8(info.windowClassClass()).toLower());
        }
        if (flag & KWindowSystem::XApp) {
            icons = QIcon::fromTheme(QLatin1String("xorg"));
        }
        if (icons.isNull()) {
            return;
        }
        QHBoxLayout *layout = new QHBoxLayout();
        const auto sizes = icons.availableSizes();
        for (auto it = sizes.begin(); it != sizes.end(); ++it) {
            const QSize &s = *it;
            QVBoxLayout *v = new QVBoxLayout();
            QLabel *l = new QLabel(QStringLiteral("%1/%2").arg(s.width()).arg(s.height()), &window);
            v->addWidget(l);
            QLabel *p = new QLabel(&window);
            p->setPixmap(icons.pixmap(s));
            v->addWidget(p);
            layout->addLayout(v);
        }
        vbox->addLayout(layout);
    };
    addIcons(QStringLiteral("NetWM"), KWindowSystem::NETWM);
    addIcons(QStringLiteral("WMHints"), KWindowSystem::WMHints);
    addIcons(QStringLiteral("ClassHint"), KWindowSystem::ClassHint);
    addIcons(QStringLiteral("XApp"), KWindowSystem::XApp);

    window.setLayout(vbox);
    window.show();

    return app.exec();
}
