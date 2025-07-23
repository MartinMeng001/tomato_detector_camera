#include "frmimgsetting.h"
#include "ui_frmimgsetting.h"

FrmImgSetting::FrmImgSetting(int nid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmImgSetting)
{
    m_nid = nid;
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    // show rg setting midform
    frmrgSet[0] = new FrmRGSetting(m_nid, 0, ui->rgbtab1);
    frmrgSet[1] = new FrmRGSetting(m_nid, 1, ui->rgbtab2);
    frmrgSet[2] = new FrmRGSetting(m_nid, 2, ui->rgbtab3);
    ui->rgLayout1->addWidget(frmrgSet[0], 0, 0, 1, 1);
    ui->rgLayout2->addWidget(frmrgSet[1], 0, 0, 1, 1);
    ui->rgLayout3->addWidget(frmrgSet[2], 0, 0, 1, 1);
    for (int i = 0; i < MAX_COLOR_SETTING; ++i) {
        frmrgSet[i]->move(5, 5);
        connect(frmrgSet[i], &FrmRGSetting::updateName, this, &FrmImgSetting::updateName);
        connect(frmrgSet[i], &FrmRGSetting::updateRed, this, &FrmImgSetting::updateRed);
        connect(frmrgSet[i], &FrmRGSetting::updateGreen, this, &FrmImgSetting::updateGreen);
        connect(frmrgSet[i], &FrmRGSetting::updateHanni, this, &FrmImgSetting::updateHanni);
        connect(frmrgSet[i], &FrmRGSetting::updateWmin, this, &FrmImgSetting::updateWmin);
        connect(frmrgSet[i], &FrmRGSetting::updateWmax, this, &FrmImgSetting::updateWmax);
        connect(frmrgSet[i], &FrmRGSetting::updateRG_A1, this, &FrmImgSetting::updateRG_A1);
        connect(frmrgSet[i], &FrmRGSetting::updateRG_A2, this, &FrmImgSetting::updateRG_A2);
        connect(frmrgSet[i], &FrmRGSetting::updateRG_K, this, &FrmImgSetting::updateRG_K);
        connect(frmrgSet[i], &FrmRGSetting::updateRG_Rmin, this, &FrmImgSetting::updateRG_Rmin);
        connect(frmrgSet[i], &FrmRGSetting::updateDispR, this, &FrmImgSetting::updateDispR);
        connect(frmrgSet[i], &FrmRGSetting::updateDispG, this, &FrmImgSetting::updateDispG);
        connect(frmrgSet[i], &FrmRGSetting::updateDispB, this, &FrmImgSetting::updateDispB);
        connect(frmrgSet[i], &FrmRGSetting::updateUse, this, &FrmImgSetting::updateUse);

        // show rgb setting
        connect(this, &FrmImgSetting::ShowScreenRGB, frmrgSet[i], &FrmRGSetting::Screen_Show_Slot);
        //        frmrgSet[i]->show();
    }

    // show color picker
    colorPicker = ColorWidget::Instance();
    colorPicker->setObjectName(QString::fromUtf8("colorPicker"));
}

FrmImgSetting::~FrmImgSetting()
{
//    if (colorPicker != NULL) {
//        delete colorPicker;
//    }
    for (int i = 0; i < MAX_COLOR_SETTING; ++i) {
        delete frmrgSet[i];
    }
    delete ui;
}

void FrmImgSetting::showEvent(QShowEvent *)
{
    ui->colorPickerLayout->addWidget(colorPicker, 0, 0, 1, 1);
}

void FrmImgSetting::on_ebxBinarySepLine_valueChanged(int arg1)
{
    emit updateSepLine(m_nid, arg1);
}

void FrmImgSetting::on_ebxBinaryTopThr_valueChanged(int arg1)
{
    emit updateTopThr(m_nid, arg1);
}

void FrmImgSetting::on_ebxBinaryBotThr_valueChanged(int arg1)
{
    emit updateBotThr(m_nid, arg1);
}

void FrmImgSetting::on_ebxStripeThreshold_valueChanged(int arg1)
{
    emit updateStrpThreShold(m_nid, arg1);
}

void FrmImgSetting::on_ebxStripeAngleWt_valueChanged(double arg1)
{
    emit updateStrpAngleWt(m_nid, arg1);
}

void FrmImgSetting::on_ebxCenterLineAngleWt_valueChanged(double arg1)
{
    emit updateCentreLinepAngleWt(m_nid, arg1);
}

void FrmImgSetting::on_ebxGHosei_valueChanged(int arg1)
{
    emit updateGHosei(m_nid, arg1);
}

void FrmImgSetting::on_ebxHeightHosei_valueChanged(int arg1)
{
    emit updateHeightHosei(m_nid, arg1);
}

void FrmImgSetting::on_ebxShimaBlockSize_valueChanged(int arg1)
{
    emit updateShimaBlockSize(m_nid, arg1);
}

void FrmImgSetting::on_ebxShimaThreshold_valueChanged(int arg1)
{
    emit updateShimaThreshold(m_nid, arg1);
}

void FrmImgSetting::on_ebxShimaMinLen_valueChanged(int arg1)
{
    emit updateShimaMinLen(m_nid, arg1);
}

void FrmImgSetting::on_ebxShimaMinArea_valueChanged(int arg1)
{
    emit updateShimaMinArea(m_nid, arg1);
}

void FrmImgSetting::on_ebxVolumeHoseiA_valueChanged(double arg1)
{
    emit updateVolumeHoseiA(m_nid, arg1);
}

void FrmImgSetting::on_ebxVolumeHoseiB_valueChanged(double arg1)
{
    emit updateVolumeHoseiB(m_nid, arg1);
}

void FrmImgSetting::on_ebxBinaryMaxB_valueChanged(int arg1)
{
    emit updateBinaryMaxB(m_nid, arg1);
}

void FrmImgSetting::on_ebxBinaryMinG_valueChanged(int arg1)
{
    emit updateBinaryMinG(m_nid, arg1);
}

void FrmImgSetting::on_ebxBinaryThreshold_valueChanged(int arg1)
{
    emit updateBinaryThreshold(m_nid, arg1);
}

void FrmImgSetting::on_ebxErodeDilateNum_valueChanged(int arg1)
{
    emit updateErodeDilateNum(m_nid, arg1);
}

void FrmImgSetting::on_ebxBinaryShift_valueChanged(int arg1)
{
    emit updateBinaryShift(m_nid, arg1);
}

void FrmImgSetting::on_ebxSealGHoseiX_valueChanged(int arg1)
{
    emit updateSealGHoseiX(m_nid, arg1);
}

void FrmImgSetting::on_ebxSealGHoseiY_valueChanged(int arg1)
{
    emit updateSealGHoseiY(m_nid, arg1);
}

void FrmImgSetting::on_ebxSealWidth_valueChanged(int arg1)
{
    emit updateSealWidth(m_nid, arg1);
}

void FrmImgSetting::on_ebxSealHeight_valueChanged(int arg1)
{
    emit updateSealHeight(m_nid, arg1);
}

void FrmImgSetting::on_ebxSealMinSize_valueChanged(int arg1)
{
    emit updateSealMinSize(m_nid, arg1);
}

void FrmImgSetting::on_ebxExpoTime_valueChanged(int arg1)
{
    emit updateExpoTime(m_nid, arg1);
}

void FrmImgSetting::Screen_Show_Slot(ImageSetting **pimageSet)
{
    ImageSetting *img_Set = pimageSet[m_nid];
    ui->ebxBinaryBotThr->setValue(img_Set->iBinaryBotThr);
    ui->ebxBinaryMaxB->setValue(img_Set->iBinaryMaxB);
    ui->ebxBinaryMinG->setValue(img_Set->iBinaryMinG);
    ui->ebxBinarySepLine->setValue(img_Set->iBinarySepLine);
    ui->ebxBinaryShift->setValue(img_Set->iBinaryShift);
    ui->ebxBinaryThreshold->setValue(img_Set->iBinaryThreshold);
    ui->ebxBinaryTopThr->setValue(img_Set->iBinaryTopThr);
    ui->ebxCenterLineAngleWt->setValue(img_Set->dCenterLineAngleWt);
    ui->ebxErodeDilateNum->setValue(img_Set->iErodeDilateNum);
    ui->ebxExpoTime->setValue(img_Set->iExpoTime);
    ui->ebxGHosei->setValue(img_Set->iGHosei);
    ui->ebxHeightHosei->setValue(img_Set->iHeightHosei);
    ui->ebxSealGHoseiX->setValue(img_Set->iSealGHoseiX);
    ui->ebxSealGHoseiY->setValue(img_Set->iSealGHoseiY);
    ui->ebxSealHeight->setValue(img_Set->iSealHeight);
    ui->ebxSealMinSize->setValue(img_Set->iSealMinSize);
    ui->ebxSealWidth->setValue(img_Set->iSealWidth);
    ui->ebxShimaBlockSize->setValue(img_Set->iShimaBlockSize);
    ui->ebxShimaMinArea->setValue(img_Set->iShimaMinArea);
    ui->ebxShimaMinLen->setValue(img_Set->iShimaMinLen);
    ui->ebxShimaThreshold->setValue(img_Set->iShimaThreshold);
    ui->ebxStripeAngleWt->setValue(img_Set->dStripeAngleWt);
    ui->ebxStripeThreshold->setValue(img_Set->iStripeThreshold);
    ui->ebxVolumeHoseiA->setValue(img_Set->dVolumeHoseiA);
    ui->ebxVolumeHoseiB->setValue(img_Set->dVolumeHoseiB);

    emit ShowScreenRGB(img_Set->colorSetting);
}
