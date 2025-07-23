#pragma once

#include <QWidget>

#include <QtUiPlugin/QDesignerExportWidget>

namespace Ui {
class OMISlider;
}

class QDESIGNER_WIDGET_EXPORT OMISlider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int omiminValue READ getMinValue WRITE setMinValue)
    Q_PROPERTY(int omimaxValue READ getMaxValue WRITE setMaxValue)
    Q_PROPERTY(int omivalue READ getValue WRITE setValue)
    Q_PROPERTY(QString omicaption READ getCaption WRITE setCaption)
    Q_PROPERTY(int omistep READ getStep WRITE setStep)

private:
    int omiminValue;             //最小值
    int omimaxValue;             //最大值
    int omivalue;                //目标电量
    QString omicaption;
    int omistep;                 //每次移动的步长

public Q_SLOTS:
    //设置范围值
    void setRange(int minValue, int maxValue);

    //设置最大最小值
    void setMinValue(int minValue);
    void setMaxValue(int maxValue);

    //设置电池电量值
    void setValue(int value);

    //设置label
    void setCaption(QString caption);

    //设置步长
    void setStep(int step);

public:
    int getMinValue()         const;
    int getMaxValue()         const;
    int getValue()            const;
    QString getCaption()      const;
    int getStep()             const;

public:
    explicit OMISlider(QWidget *parent = nullptr);
    ~OMISlider();

signals:
    void updateSliderValue(int);

private:
    Ui::OMISlider *ui;
};

