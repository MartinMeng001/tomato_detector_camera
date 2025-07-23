#pragma once

#include <QWidget>
#include <QColor>
#include <QImage>

#define whirlbuttonHeigth   160
#define whirlbuttonWidth    150

class whirlbutton : public QWidget
{
    Q_OBJECT
public:
    explicit whirlbutton(QWidget *parent = nullptr);
    ~whirlbutton();

    void  paintEvent(QPaintEvent *event);
    void setDegree(int deg);
    int getDegree();
    int getIndex();

signals:
    void updateDegree();
    void updateScanType(int);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseToPosion(QMouseEvent *event);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

private:
    int degree;
    int iDex;
};
