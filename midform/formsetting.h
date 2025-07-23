#pragma once

#include <QWidget>
#include "def.h"

namespace Ui
{
    class FormSetting;
}

class FormSetting : public QWidget
{
    Q_OBJECT

public:
    explicit FormSetting(int nid, QWidget *parent = nullptr);
    ~FormSetting();

private slots:
    void on_areaLSpinbox_valueChanged(int arg1);

    void on_areaUSpinbox_valueChanged(int arg1);

    void on_areaRSpinbox_valueChanged(int arg1);

    void on_areaDSpinbox_valueChanged(int arg1);

    void on_pushButton_clicked();

    void on_stretchSpinBox_valueChanged(double arg1);

    void on_rotateCombox_currentIndexChanged(int index);

    void on_cmbTriggerMode_currentIndexChanged(int index);

    void on_ebxKaraThr_valueChanged(double arg1);// 空托盘判断阈值

    void on_ebxPixSize_valueChanged(double arg1);// 像素大小(mm)

    void on_ebxMinValue_valueChanged(int arg1);// 照明故障阈值

    void on_ebxMinLabelingSize_valueChanged(int arg1);// 连通域最小像素数

    void on_ebxMedianFilter_valueChanged(int arg1);

    void on_pushButton_2_clicked();

public slots:
    void Screen_Show_Slot(CamSetting **);
    void UpdateGainValue(int gain, int r_gain, int g_gain, int b_gain, int nid);

signals:
    void updatewhiteBalnChk(int, bool);
    void updateLeanChk(int, bool);
    void updateWidth(int, int);
    void updateHeight(int, int);
    void updateGain(int, int);
    void updateR_Gain(int, double, double);
    void updateG_Gain(int, double, double);
    void updateB_Gain(int, double, double);
    void updateTriggerSource(int, int);
    void updateLeft(int, int);
    void updateRight(int, int);
    void updateTop(int, int);
    void updateBottom(int, int);
    void updatePixSize(int, double);
    void updateMedianFilter(int, int);
    void updateKaraThr(int, double);
    void updateMinLabelingSize(int, int);
    void updateTanshuku(int, double);
    void updateMinValue(int, int);
    void updateRotateMode(int, bool);
    void updateAutowhiteBaln(int);
    void CamAutowhiteBaln(int);

private:
    Ui::FormSetting *ui;
    int m_nid;
};

