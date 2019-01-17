/* This file is part of the KDE libraries

    Copyright 2013 Nicolás Alvarez <nicolas.alvarez@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QImage>
#include <QDebug>

#include <kwindowshadow.h>

class ShadowTestWindow: public QWidget
{
public:
    ShadowTestWindow();
    ~ShadowTestWindow() override;
private:
    QPushButton *m_btnNothing;
    QPushButton *m_btnFullWindow;
    QWidget *m_area;

    void disableShadows();
    void enableShadows();

    QScopedPointer<ShadowData> m_shadow;
};

ShadowTestWindow::ShadowTestWindow()
{
    m_btnNothing    = new QPushButton("Nothing");
    m_btnFullWindow = new QPushButton("Shadows");
    setWindowFlag(Qt::FramelessWindowHint);
    move(300,300);

    m_area = new QWidget;
    m_area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_btnNothing,    &QPushButton::clicked, this, &ShadowTestWindow::disableShadows);
    connect(m_btnFullWindow, &QPushButton::clicked, this, &ShadowTestWindow::enableShadows);

    auto createShadowPiece = [](int width, int height, const QColor &color) -> QImage {
        QImage img(QSize(width, height), QImage::Format_ARGB32_Premultiplied);
        img.fill(color);
        return img;
    };

    QImage top = createShadowPiece(5, 5, Qt::red);
    QImage left = createShadowPiece(15, 5, Qt::green);
    QImage right = createShadowPiece(5, 5, Qt::yellow);
    QImage bottom = createShadowPiece(5, 5, Qt::blue);
    QImage topLeft = createShadowPiece(15, 5, Qt::blue);
    QImage topRight = createShadowPiece(5, 5, Qt::black);
    QImage bottomLeft = createShadowPiece(15, 5, Qt::white);
    QImage bottomRight = createShadowPiece(5, 5, Qt::green);

    QMargins margins(15,5,5,5);

    m_shadow.reset(new ShadowData(top,
                          left,
                          right,
                          bottom,
                          topLeft,
                          topRight,
                          bottomLeft,
                          bottomRight,
                          margins
                         ));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_btnNothing);
    layout->addWidget(m_btnFullWindow);
    setLayout(layout);
}

ShadowTestWindow::~ShadowTestWindow()
{}

void ShadowTestWindow::disableShadows()
{
    m_shadow->clearWindow(windowHandle());
}

void ShadowTestWindow::enableShadows()
{
    m_shadow->decorateWindow(windowHandle());
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    ShadowTestWindow wnd;
    wnd.show();

    return app.exec();
}
