#include "frmrgsetting.h"
#include "ui_frmrgsetting.h"

FrmRGSetting::FrmRGSetting(int itab, int nid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRGSetting)
{
    m_itab = itab;
    m_nid = nid;
    ui->setupUi(this);
    connect(ui->rgUseChk, &ImageSwitch::updateChecked, this, [ = ](bool bcheck) {
        emit updateUse(m_itab, m_nid, bcheck);
    });
}

FrmRGSetting::~FrmRGSetting()
{
    delete ui;
}

void FrmRGSetting::on_ebxName_textChanged(const QString &arg1)
{
    emit updateName(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxRed_valueChanged(int arg1)
{
    emit updateRed(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxGreen_valueChanged(int arg1)
{
    emit updateGreen(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxHanni_valueChanged(int arg1)
{
    emit updateHanni(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxWmin_valueChanged(int arg1)
{
    emit updateWmin(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxWmax_valueChanged(int arg1)
{
    emit updateWmax(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxRG_A1_valueChanged(int arg1)
{
    emit updateRG_A1(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxRG_A2_valueChanged(int arg1)
{
    emit updateRG_A2(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxRG_K_valueChanged(int arg1)
{
    emit updateRG_K(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxRG_Rmin_valueChanged(int arg1)
{
    emit updateRG_Rmin(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxDispR_valueChanged(int arg1)
{
    emit updateDispR(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxDispG_valueChanged(int arg1)
{
    emit updateDispG(m_itab, m_nid, arg1);
}

void FrmRGSetting::on_ebxDispB_valueChanged(int arg1)
{
    emit updateDispB(m_itab, m_nid, arg1);
}

void FrmRGSetting::Screen_Show_Slot(ColorSetting *pset)
{
    ColorSetting clrset = pset[m_nid];
    ui->ebxDispB->setValue(clrset.iDispB);
    ui->ebxDispG->setValue(clrset.iDispG);
    ui->ebxDispR->setValue(clrset.iDispR);
    ui->ebxGreen->setValue(clrset.iGreen);
    ui->ebxHanni->setValue(clrset.iHanni);
    ui->ebxName->setText(clrset.aName);
    ui->ebxRG_A1->setValue(clrset.iRG_A1);
    ui->ebxRG_A2->setValue(clrset.iRG_A2);
    ui->ebxRG_K->setValue(clrset.iRG_K);
    ui->ebxRG_Rmin->setValue(clrset.iRG_Rmin);
    ui->ebxRed->setValue(clrset.iRed);
    ui->ebxWmax->setValue(clrset.iWmax);
    ui->ebxWmin->setValue(clrset.iWmin);
    ui->rgUseChk->setChecked(clrset.bUse);
}

