#pragma once

#include <QWidget>
#include "frmrgsetting.h"
#include "def.h"
#include "colorwidget.h"

namespace Ui {
class FrmImgSetting;
}

class FrmImgSetting : public QWidget
{
    Q_OBJECT

public:
    explicit FrmImgSetting(int, QWidget *parent = nullptr);
    ~FrmImgSetting();
    FrmRGSetting *frmrgSet[MAX_COLOR_SETTING];
    ColorWidget *colorPicker;

protected:
    void showEvent(QShowEvent *) override;

private slots:
    void on_ebxBinarySepLine_valueChanged(int arg1);

    void on_ebxBinaryTopThr_valueChanged(int arg1);

    void on_ebxBinaryBotThr_valueChanged(int arg1);

    void on_ebxStripeThreshold_valueChanged(int arg1);

    void on_ebxStripeAngleWt_valueChanged(double arg1);

    void on_ebxCenterLineAngleWt_valueChanged(double arg1);

    void on_ebxGHosei_valueChanged(int arg1);

    void on_ebxHeightHosei_valueChanged(int arg1);

    void on_ebxShimaBlockSize_valueChanged(int arg1);

    void on_ebxShimaThreshold_valueChanged(int arg1);

    void on_ebxShimaMinLen_valueChanged(int arg1);

    void on_ebxShimaMinArea_valueChanged(int arg1);

    void on_ebxVolumeHoseiA_valueChanged(double arg1);

    void on_ebxVolumeHoseiB_valueChanged(double arg1);

    void on_ebxBinaryMaxB_valueChanged(int arg1);

    void on_ebxBinaryMinG_valueChanged(int arg1);

    void on_ebxBinaryThreshold_valueChanged(int arg1);

    void on_ebxErodeDilateNum_valueChanged(int arg1);

    void on_ebxBinaryShift_valueChanged(int arg1);

    void on_ebxSealGHoseiX_valueChanged(int arg1);

    void on_ebxSealGHoseiY_valueChanged(int arg1);

    void on_ebxSealWidth_valueChanged(int arg1);

    void on_ebxSealHeight_valueChanged(int arg1);

    void on_ebxSealMinSize_valueChanged(int arg1);

    void on_ebxExpoTime_valueChanged(int arg1);

signals:
    void updateSepLine(int, int);
    void updateTopThr(int, int);
    void updateBotThr(int, int);
    void updateStrpThreShold(int, int);
    void updateStrpAngleWt(int, double);
    void updateCentreLinepAngleWt(int, double);
    void updateGHosei(int, int);
    void updateHeightHosei(int, int);
    void updateShimaBlockSize(int, int);
    void updateShimaThreshold(int, int);
    void updateShimaMinLen(int, int);
    void updateShimaMinArea(int, int);
    void updateVolumeHoseiA(int, double);
    void updateVolumeHoseiB(int, double);
    void updateBinaryMaxB(int, int);
    void updateBinaryMinG(int, int);
    void updateBinaryThreshold(int, int);
    void updateErodeDilateNum(int, int);
    void updateBinaryShift(int, int);
    void updateSealGHoseiX(int, int);
    void updateSealGHoseiY(int, int);
    void updateSealWidth(int, int);
    void updateSealHeight(int, int);
    void updateSealMinSize(int, int);
    void updateExpoTime(int, int);

    void updateName(int, int, const QString &);
    void updateUse(int, int, bool);
    void updateRed(int, int, int);
    void updateGreen(int, int, int);
    void updateHanni(int, int, int);
    void updateWmin(int, int, int);
    void updateWmax(int, int, int);
    void updateRG_A1(int, int, int);
    void updateRG_A2(int, int, int);
    void updateRG_K(int, int, int);
    void updateRG_Rmin(int, int, int);
    void updateDispR(int, int, int);
    void updateDispG(int, int, int);
    void updateDispB(int, int, int);

    void ShowScreenRGB(ColorSetting *);

public slots:
    void Screen_Show_Slot(ImageSetting **);

private:
    Ui::FrmImgSetting *ui;
    int m_nid;
};

