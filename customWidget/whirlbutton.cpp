#include "whirlbutton.h"
#include <QPainter>
#include <QMouseEvent>
#include <math.h>
#include "def.h"

int RDegree[3] = {ANGLE_90, ANGLE_210, ANGLE_30 };
QString strTmp[3] = {"AUTO", "TEST", "MANUAL"};

whirlbutton::whirlbutton(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    iDex = 0;
    degree =  RDegree[iDex];
}

whirlbutton::~ whirlbutton()
{
}

void whirlbutton::setDegree(int deg)
{
    this->degree = deg;
    this->update();
}

int whirlbutton::getIndex()
{
    return iDex;
}

void whirlbutton::paintEvent(QPaintEvent *)
{
    //第一步画定点三角形
    //第二步画背景大圆
    //第三步画背小圆
    //第四步画旋转的棍子
    //第五步画刻度
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QTransform tranform0, tranform1, tranform2;
    int originY = whirlbuttonHeigth / 2;
    int originX = whirlbuttonWidth / 2;
    const int triangle[3][2] =
    {
        { originX - 3, 0 },
        { originX + 3, 0 },
        { originX, 7 }
    };
    tranform0.translate(originX, originY);
    tranform0.rotate(degree - 90);
    tranform0.translate(-(originX), -(originY));
    painter.setWorldTransform(tranform0);
    painter.setPen(QPen(Qt::green));
    //painter.setBrush(palette().foreground());
    painter.setBrush(QBrush(Qt::green));
    //draw polygon,triangle.
    painter.drawPolygon(QPolygon(3, &triangle[0][0]));
    //   painter.setWindow(0,0,150,150);
    tranform1.translate(originX, originY);
    tranform1.rotate(degree);
    tranform1.translate(-(originX), -(originY));
    painter.setWorldTransform(tranform1);
    QPen thickPen(palette().windowText(), 2);
    //    QPen midPen(palette().windowText(), 1.0);
    QPen thinPen(palette().windowText(), 0.5);
    QColor niceBlue(150, 150, 200);
    painter.setPen(thinPen);
    //gradually change with degree
    QConicalGradient coneGradient(originX, originY, -90.0);
    coneGradient.setColorAt(0.0, Qt::darkGray);
    coneGradient.setColorAt(0.2, niceBlue);
    coneGradient.setColorAt(1.0, Qt::white);
    coneGradient.setColorAt(1.0, Qt::darkGray);
    unsigned r = 70;
    painter.setBrush(coneGradient);
    painter.drawEllipse((originX - r), (originY - r), r * 2, r * 2);
    r = 26;
    QRadialGradient haloGradient(originX, originY, r, originX, originY);
    haloGradient.setColorAt(0.0, Qt::lightGray);
    haloGradient.setColorAt(0.8, Qt::darkGray);
    haloGradient.setColorAt(0.9, Qt::white);
    haloGradient.setColorAt(1.0, Qt::black);
    painter.setPen(Qt::NoPen);
    painter.setBrush(haloGradient);
    painter.drawEllipse(originX - r, originY - r, r * 2, 2 * r);
    unsigned rx = 30, ry = 8;
    QLinearGradient knobGradient(originX - rx, originY - ry, originX - rx, originY + ry);
    knobGradient.setColorAt(0.0, Qt::black);
    knobGradient.setColorAt(0.2, niceBlue);
    knobGradient.setColorAt(0.3, Qt::lightGray);
    knobGradient.setColorAt(0.8, Qt::white);
    knobGradient.setColorAt(1.0, Qt::black);
    painter.setBrush(knobGradient);
    painter.setPen(thinPen);
    painter.drawRoundedRect(originX - rx, originY - ry, rx * 2, ry * 2, 49, 99, Qt::RelativeSize);
    tranform2.translate(originX, originY);
    tranform2.rotate(90);
    tranform2.translate(-(originX), -(originY));
    painter.setWorldTransform(tranform2);

    for (int i = 0; i < 3; ++i)
    {
        painter.setPen(thickPen);
        painter.drawLine(5, originY, 13, originY);
        tranform2.translate(25, originY);
        tranform2.rotate(-90);
        tranform2.translate(-25, -originY);
        painter.setWorldTransform(tranform2);
        painter.drawText(12, originY, strTmp[i]);
        tranform2.translate(25, originY);
        tranform2.rotate(90);
        tranform2.translate(-25, -originY);
        painter.setWorldTransform(tranform2);
        tranform2.translate(originX, originY);
        tranform2.rotate(-120);
        tranform2.translate(-originX, -originY);
        painter.setWorldTransform(tranform2);
    }
}
QSize whirlbutton::sizeHint() const
{
    QSize size = QSize(whirlbuttonHeigth, whirlbuttonWidth);
    return size;
}
QSize whirlbutton::minimumSizeHint() const
{
    QSize size = QSize(whirlbuttonHeigth, whirlbuttonWidth);
    return size;
}
void whirlbutton::mousePressEvent(QMouseEvent *event)
{
    mouseToPosion(event);
    this->update();
}
int whirlbutton :: getDegree()
{
    return degree;
}

void whirlbutton:: mouseToPosion(QMouseEvent *event)
{
    //    QPoint currentPoint = event->pos();
    //    double pi = 3.1415927;
    //    double  x = currentPoint.x() - whirlbuttonHeigth / 2;
    //    double  y = currentPoint.y() - whirlbuttonWidth / 2;
    //    if ((x == 0) && (y <= 0))
    //        degree = 90;
    //    else if ((x == 0) && (y < 0))
    //        degree = 270;
    //    else if (x > 0)
    //        degree = atan(y / x) * 180 / pi + 180;
    //    else if (y > 0)
    //        degree = atan(y / x) * 180 / pi + 360;
    //    else
    //        degree = atan(y / x) * 180 / pi;
    QPoint currentPoint = event->pos();
    double  x = currentPoint.x() - whirlbuttonHeigth / 2;
    int iemitV = 0;

    if (x > 0)
    {
        iemitV = ++iDex % 3;
    }
    else
    {
        if (iDex - 1 < 0)
        {
            iDex = 3;
        }

        iemitV = --iDex % 3;
    }

    degree = RDegree[iemitV];
    emit(updateScanType(iemitV));
}
