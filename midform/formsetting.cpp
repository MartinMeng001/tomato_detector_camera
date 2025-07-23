#include "formsetting.h"
#include "ui_formsetting.h"

FormSetting::FormSetting(int nid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSetting)
{
    m_nid = nid;
    ui->setupUi(this);
    //    ui->label_13->setVisible(false);
    //    ui->cmbTriggerMode->setVisible(false);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    connect(ui->whiteBalnChk, &ImageSwitch::updateChecked, this, [ = ](bool bcheck)
    {
        emit updatewhiteBalnChk(m_nid, bcheck);
    });
    connect(ui->leanChk, &ImageSwitch::updateChecked, this, [ = ](bool bcheck)
    {
        emit updateLeanChk(m_nid, bcheck);
    });
    connect(ui->cameraWidth, &OMISlider::updateSliderValue, this, [ = ](int val)
    {
        emit updateWidth(m_nid, val);
    });
    connect(ui->cameraHeight, &OMISlider::updateSliderValue, this, [ = ](int val)
    {
        emit updateHeight(m_nid, val);
    });
    connect(ui->cameraGain, &OMISlider::updateSliderValue, this, [ = ](int val)
    {
        emit updateGain(m_nid, val);
    });
    connect(ui->camBGain, &OMISlider::updateSliderValue, this, [ = ](int val)
    {
        emit updateB_Gain(m_nid, val, ui->camBGain->getMaxValue());
    });
    connect(ui->camRGain, &OMISlider::updateSliderValue, this, [ = ](int val)
    {
        emit updateR_Gain(m_nid, val, ui->camRGain->getMaxValue());
    });
    connect(ui->camGGain, &OMISlider::updateSliderValue, this, [ = ](int val)
    {
        emit updateG_Gain(m_nid, val, ui->camGGain->getMaxValue());
    });
}

FormSetting::~FormSetting()
{
    delete ui;
}

void FormSetting::on_areaLSpinbox_valueChanged(int arg1)
{
    emit updateLeft(m_nid, arg1);
}

void FormSetting::on_areaUSpinbox_valueChanged(int arg1)
{
    emit updateTop(m_nid, arg1);
}

void FormSetting::on_ebxPixSize_valueChanged(double arg1)
{
    emit updatePixSize(m_nid, arg1);
}

void FormSetting::on_ebxMinValue_valueChanged(int arg1)
{
    emit updateMinValue(m_nid, arg1);
}

void FormSetting::on_areaRSpinbox_valueChanged(int arg1)
{
    emit updateRight(m_nid, arg1);
}

void FormSetting::on_areaDSpinbox_valueChanged(int arg1)
{
    emit updateBottom(m_nid, arg1);
}

void FormSetting::on_ebxKaraThr_valueChanged(double arg1)
{
    emit updateKaraThr(m_nid, arg1);
}

void FormSetting::on_ebxMinLabelingSize_valueChanged(int arg1)
{
    emit updateMinLabelingSize(m_nid, arg1);
}

void FormSetting::on_pushButton_clicked()
{
    int dr = (int)((ui->camRGain->getValue() * ui->camRGain->getMaxValue()) / 4 + 0.05);
    ui->camRGain->setValue(dr);
    int dg = (int)((ui->camGGain->getValue() * ui->camGGain->getMaxValue()) / 4 + 0.05);
    ui->camGGain->setValue(dg);
    int db = (int)((ui->camBGain->getValue() * ui->camBGain->getMaxValue()) / 4 + 0.05);
    ui->camBGain->setValue(db);
    emit updateAutowhiteBaln(m_nid);
}

void FormSetting::on_stretchSpinBox_valueChanged(double arg1)
{
    emit updateTanshuku(m_nid, arg1);
}

void FormSetting::on_rotateCombox_currentIndexChanged(int index)
{
    emit updateRotateMode(m_nid, index);
}

void FormSetting::on_cmbTriggerMode_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    emit updateTriggerSource(m_nid, index);
}

void FormSetting::on_ebxMedianFilter_valueChanged(int arg1)
{
    emit updateMedianFilter(m_nid, arg1);
}

void FormSetting::Screen_Show_Slot(CamSetting **pcamSet)
{
    CamSetting *cam_setting = pcamSet[m_nid];
    ui->areaDSpinbox->setValue(cam_setting->iBottom);
    ui->areaLSpinbox->setValue(cam_setting->iLeft);
    ui->areaRSpinbox->setValue(cam_setting->iRight);
    ui->areaUSpinbox->setValue(cam_setting->iTop);
    ui->camBGain->setValue((int)cam_setting->dB_Gain);
    ui->camGGain->setValue((int)cam_setting->dG_Gain);
    ui->camRGain->setValue((int)cam_setting->dR_Gain);
    ui->cameraGain->setValue(cam_setting->iGain);
    ui->cameraHeight->setValue(cam_setting->iHeight);
    ui->cameraWidth->setValue(cam_setting->iWidth);

    if (cam_setting->bTriggerMode)
    {
        switch (cam_setting->iTriggerSource)
        {
            case 0: // 软触发
                ui->cmbTriggerMode->setCurrentIndex(1);
                break;

            case 1: // 触发源0
                ui->cmbTriggerMode->setCurrentIndex(2);
                break;

            case 2: // 触发源1
                ui->cmbTriggerMode->setCurrentIndex(3);
                break;

            case 3:// 触发源2
                ui->cmbTriggerMode->setCurrentIndex(4);
                break;

            case 4://  触发源3
                ui->cmbTriggerMode->setCurrentIndex(5);
                break;
        }
    }
    else
    {
        ui->cmbTriggerMode->setCurrentIndex(0);
    }

    ui->ebxKaraThr->setValue(cam_setting->dKaraThr);
    ui->ebxMedianFilter->setValue(cam_setting->iMedianFilter);
    ui->ebxMinLabelingSize->setValue(cam_setting->iMinLabelingSize);
    ui->ebxMinValue->setValue(cam_setting->iMinValue);
    ui->ebxPixSize->setValue(cam_setting->dPixSize);
    ui->leanChk->setChecked(cam_setting->bRotate);
    ui->rotateCombox->setCurrentIndex(cam_setting->iRotateMode);
    ui->stretchSpinBox->setValue(cam_setting->dTanshuku);
    ui->whiteBalnChk->setChecked(cam_setting->bAreaWhiteBalance);
}

void FormSetting::UpdateGainValue(int gain, int r_gain, int g_gain, int b_gain, int nid)
{
    if (nid != m_nid)
    {
        return;
    }

    ui->camBGain->setValue(b_gain);
    ui->camGGain->setValue(g_gain);
    ui->camRGain->setValue(r_gain);
    ui->cameraGain->setValue(gain);
}

void FormSetting::on_pushButton_2_clicked()
{
    emit CamAutowhiteBaln(m_nid);
}
