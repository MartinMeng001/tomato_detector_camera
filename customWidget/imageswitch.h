﻿#ifndef IMAGESWITCH_H
#define IMAGESWITCH_H

#include <QWidget>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT ImageSwitch : public QWidget
#else
class ImageSwitch : public QWidget
#endif

{
    Q_OBJECT
    Q_ENUMS(ButtonStyle)

    Q_PROPERTY(bool isChecked READ getChecked WRITE setChecked)
    Q_PROPERTY(ButtonStyle buttonStyle READ getButtonStyle WRITE setButtonStyle)

public:
    enum ButtonStyle {
        ButtonStyle_1 = 0,  //开关样式1
        ButtonStyle_2 = 1,  //开关样式2
        ButtonStyle_3 = 2   //开关样式3
    };

    explicit ImageSwitch(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *event);

private:
    bool isChecked;
    ButtonStyle buttonStyle;

    QString imgOffFile;
    QString imgOnFile;
    QString imgFile;

public:
    bool getChecked()               const;
    ButtonStyle getButtonStyle()    const;
    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public Q_SLOTS:
    void setChecked(bool isChecked);
    void setButtonStyle(const ImageSwitch::ButtonStyle &buttonStyle);

signals:
    void updateChecked(bool isChecked);
};

#endif // IMAGESWITCH_H
