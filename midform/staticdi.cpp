#include "staticdi.h"
#include <QMessageBox>
#include "def.h"
quint8 g_portInStates[4] = {0};

StaticDI::StaticDI(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    instantDiCtrl = InstantDiCtrl::Create();
    m_portPanel = NULL;
    portPanelLength = 0;
    timer = new QTimer(this);
    this->layout = new QVBoxLayout(ui.scrollAreaWidgetContents);
    images[0] = "background:url(:/StaticDI/image/ledLow.png)";
    images[1] = "background:url(:/StaticDI/image/ledHigh.png)";
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerTicked()));
    connect(this, &StaticDI::SigInit, this, &StaticDI::Initialize);
    connect(this, &StaticDI::SigTimerRunStop, this, &StaticDI::TimerRunStop);
}

StaticDI::~StaticDI()
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

    instantDiCtrl->Dispose();
}

void StaticDI::Initialize()
{
    timer->stop();
    ConfigureDevice();
    InitializePortPanel();
}

void StaticDI::ConfigureDevice()
{
    std::wstring description = deviceName.toStdWString();
    DeviceInformation selected(description.c_str());
    ErrorCode errorCode = Success;
    errorCode = instantDiCtrl->setSelectedDevice(selected);
    CheckError(errorCode);
    portCount = 4; //instantDiCtrl->getPortCount();
}

void StaticDI::InitializePortPanel()
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

    m_portPanel = new DioPortUI*[portCount];
    portPanelLength = portCount;

    for (int i = 0; i < portCount; i++)
    {
        ui.scrollAreaWidgetContents->setGeometry(0, 0, 350, (i + 1) * 60);
        m_portPanel[i] = new DioPortUI(0, i, (quint8)0, images);
        m_portPanel[i]->setIsEditable(false);
        this->layout->addWidget(m_portPanel[i]);
    }
}

void StaticDI::CheckError(ErrorCode errorCode)
{
    if (BioFailed(errorCode))
    {
        QString message = tr("Sorry, there are some errors occurred in io's input, Error Code: 0x") +
                          QString::number(errorCode, 16).right(8).toUpper();
        qCritical() << message;
    }
}

void StaticDI::SetDeviceName(QString str)
{
    deviceName = str;
}

void StaticDI::ReadIO()
{
    //read DI data
    instantDiCtrl->Read(0, portCount, g_portInStates);
}

bool StaticDI::GetIOData(int io, int port)
{
    return ((g_portInStates[port] >> io) & 0x1);
}

void StaticDI::TimerTicked()
{
    //update the UI
    for (int i = 0; i < portCount; i++)
    {
        this->m_portPanel[i]->setState(g_portInStates[i]);
    }
}

void StaticDI::TimerRunStop(bool flag)
{
    if (flag)
    {
        //start the timer to read DI ports status
        timer->start(50);
    }
    else
    {
        timer->stop();
    }
}
