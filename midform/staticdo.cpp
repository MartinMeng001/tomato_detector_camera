#include "staticdo.h"
#include <QMessageBox>
#include "def.h"
quint8 g_portOutStates[4] = {0};

StaticDO::StaticDO(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    instantDoCtrl = InstantDoCtrl::Create();
    m_portPanel = NULL;
    portPanelLength = 0;
    this->layout = new QVBoxLayout(ui.scrollAreaWidgetContents);
    images[0] = "background:url(:/StaticDO/image/ButtonUp.png)";
    images[1] = "background:url(:/StaticDO/image/ButtonDown.png)";
    images[2] = "background:url(:/StaticDO/image/ButtonDisabled.png)";
    connect(this, &StaticDO::SigInit, this, &StaticDO::Initialize);
}

StaticDO::~StaticDO()
{
    if (m_portPanel != NULL)
    {
        for (int i = 0; i < portPanelLength; i++)
        {
            if (m_portPanel[i] != NULL)
            {
                delete m_portPanel[i];
                m_portPanel[i] = NULL;
            }
        }
    }

    instantDoCtrl->Dispose();
}

void StaticDO::Initialize()
{
    ConfigureDevice();
    InitializePortPanel();
}

void StaticDO::ConfigureDevice()
{
    std::wstring description = deviceName.toStdWString();
    DeviceInformation selected(description.c_str());
    ErrorCode errorCode = Success;
    errorCode = instantDoCtrl->setSelectedDevice(selected);
    CheckError(errorCode);
    portCount = 4; //instantDoCtrl->getPortCount();
}

void StaticDO::InitializePortPanel()
{
    //remove the old port panel
    if (m_portPanel != NULL)
    {
        int height = ui.scrollAreaWidgetContents->height();

        for (int i = 0; i < portPanelLength; i++)
        {
            this->layout->removeWidget(m_portPanel[i]);
            ui.scrollAreaWidgetContents->setGeometry(0, 0, 350, height - (i + 1) * 60);

            if (m_portPanel[i] != NULL)
            {
                delete m_portPanel[i];
                m_portPanel[i] = NULL;
            }
        }
    }

    //    DioFeatures *features = instantDoCtrl->getFeatures();
    // in manual book, it's fixed '0xff'
    //Array<uint8> *portMasks = features->getDoDataMask();//getDataMask();
    m_portPanel = new DioPortUI *[portCount];
    portPanelLength = portCount;

    for (int i = 0; i < portCount; i++)
    {
        ui.scrollAreaWidgetContents->setGeometry(0, 0, 350, (i + 1) * 60);
        m_portPanel[i] = new DioPortUI(0, i, (quint8)0, images);
        m_portPanel[i]->setMask(0xff);
        m_portPanel[i]->setIsEditable(true);
        m_portPanel[i]->setState(0);
        m_portPanel[i]->JudgeDirection(0xff);
        connect(m_portPanel[i], SIGNAL(stateChanged(QObject *)), this, SLOT(stateChanged(QObject *)));
        this->layout->addWidget(m_portPanel[i]);
    }
}
void StaticDO::CheckError(ErrorCode errorCode)
{
    if (BioFailed(errorCode))
    {
        QString message = tr("Sorry, there are some errors occurred in io's output, Error Code: 0x") +
                          QString::number(errorCode, 16).right(8).toUpper();
        qCritical() << message;
    }
}
void StaticDO::SetDeviceName(QString str)
{
    deviceName = str;
}

void StaticDO::WriteIO(quint8 *data)
{
    ErrorCode errorCode = Success;
    errorCode = instantDoCtrl->Write(0, portCount, data);
    CheckError(errorCode);
}
void StaticDO::stateChanged(QObject *sender)
{
    DioPortUI *panel = (DioPortUI *) sender;
    quint8 status =  panel->getState();
    ErrorCode errorCode = Success;
    errorCode = instantDoCtrl->Write(panel->getKey(), 1, &status);
    CheckError(errorCode);
}
