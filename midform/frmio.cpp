#include "frmio.h"
#include <QComboBox>
#include "ui_frmio.h"
#include "def.h"
bool m_bManual = false;

FrmIO::FrmIO(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmIO)
{
    ui->setupUi(this);
    connect(ui->combIOName, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FrmIO::updateCombIOText);
    SearchDev();
}

FrmIO::~FrmIO()
{
    delete ui;
}

void FrmIO::SetComIOTextIndex(int index)
{
    ui->combIOName->setCurrentIndex(index);
    InitDevice();
}

void FrmIO::InitDevice()
{
    QString devNm = ui->combIOName->currentText().trimmed();
    ui->inwidget->SetDeviceName(devNm);
    ui->outwidget->SetDeviceName(devNm);
    emit ui->inwidget->SigInit();
    emit ui->outwidget->SigInit();
}

void FrmIO::on_chkIOManual_stateChanged(int arg1)
{
    if (arg1 == Qt::CheckState::Checked)
    {
        m_bManual = true;
    }
    else
    {
        m_bManual = false;
    }
}

void FrmIO::SearchDev()
{
    InstantDiCtrl *instantDiCtrl = InstantDiCtrl::Create();
    Array<DeviceTreeNode> *supportedDevices = instantDiCtrl->getSupportedDevices();

    if (supportedDevices->getCount() == 0)
    {
        qCritical() << tr("No device to support the currently demonstrated function!");
        return;
    }
    else
    {
        for (int i = 0; i < supportedDevices->getCount(); i++)
        {
            DeviceTreeNode const &node = supportedDevices->getItem(i);

            if (wcscmp(node.Description, L"DemoDevice") == 0)
            {
                continue;
            }

            ui->combIOName->addItem(QString::fromWCharArray(node.Description));
        }
    }

    instantDiCtrl->Dispose();
    supportedDevices->Dispose();
}

void FrmIO::SetTimerRunStop(bool flag)
{
    emit ui->inwidget->SigTimerRunStop(flag);
}

void *FrmIO::GetStaticIOIn()
{
    return ui->inwidget;
}

void *FrmIO::GetStaticIOOut()
{
    return ui->outwidget;
}
