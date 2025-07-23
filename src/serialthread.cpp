//#include "serialthread.h"
#include "mainwindow.h"
#include <QtCore>

SerialThread::SerialThread(QObject *parent): QThread(parent)
{
    m_bOpen = false;
    m_mainWindow = parent;
    rs_port = new RS();

    if (!rs_port->OpenPort(((MainWindow *)m_mainWindow)->m_sySetting.iRsPort, 115200, 8, 2, 0, 100, 100))
    {
        QString atmp;
        atmp = QString("COM%1 open failed!").arg(((MainWindow *)m_mainWindow)->m_sySetting.iRsPort);
        qCritical() << atmp;
        delete rs_port;
        return;
    }

    m_bStop = false;
    cnt = 0;
    iSendCnt = 0;
    m_bOpen = true;
}

SerialThread::~SerialThread()
{
}

bool SerialThread::CompleteCheck()
{
    bool ret = false;
    DWORD nIndex = WaitForSingleObject(g_SerSendHnd, 400);

    if (nIndex == WAIT_OBJECT_0)   //所有对象都发出信号
    {
        ret = true;
        //        emit SigMonitorDisp();
    }
    else if (nIndex == WAIT_TIMEOUT)     //超时400毫秒
    {
        //超时可作定时用
        if (m_mainWindow != NULL)
        {
            ((MainWindow *)m_mainWindow)->m_bIsTrigValid = true;
            ((MainWindow *)m_mainWindow)->m_bIsTrigValid2 = true;
        }
    }
    else if (nIndex == WAIT_FAILED)
    {
        //表示函数执行失败
        DWORD dw = GetLastError();
        qCritical() << QString("serial thread, CompleteCheck failed.last error code:%1").arg(dw);
    }

    return ret;
}

void SerialThread::StopThread()
{
    m_bStop = true;
}

void SerialThread::run()
{
    char buffer[20] = {0};
    int iCnt = 0;

    if (!m_bOpen)
    {
        return;
    }

    rs_port->LinePurge();

    while (!m_bStop)
    {
        if (rs_port->rs_loc())
        {
            char rchar = rs_port->rdchr();

            if (MY_STX == rchar)
            {
                memset(buffer, 0, sizeof(buffer));
                iCnt = 0;
            }
            else if (MY_ETX == rchar)
            {
                if (buffer[0] == 'E')
                {
                    emit SigSysShutDown();
                }
                else if (buffer[0] >= '0' && buffer[0] < '0' + MAX_COLOR_HINSYU)
                {
                    emit SigUpdateHinNo(buffer[0] - '0');
                    CHAR chin[5] = {0};
                    sprintf(chin, "%cK%c", MY_STX, MY_ETX);
                    rs_port->Send(chin);
                }
                else if (buffer[0] == 'T')     //上面5侧相机
                {
#ifdef TRIGGER_WITH_COMM
                    TriggerCamCapture();
#endif
                }/* else if (buffer[0] == 't') { //下面相机

                    emit SigBotCameraMeasure();
                }*/
            }
            else if (iCnt < 20)
            {
                buffer[iCnt] = rchar;
                iCnt++;
            }
        }
        else
        {
            usleep(500);// release cpu at free times
        }
    }

    //    future.waitForFinished();
    rs_port->ClosePort();
    delete rs_port;
    m_bOpen = false;
}

void SerialThread::SendDataBySerial()
{
    QString SendBuffer = ((MainWindow *)m_mainWindow)->GetSendCamData();
    std::string strtmp = SendBuffer.toStdString();
    rs_port->Send(strtmp.c_str());
    // insert into data trace table
    // emit SigLogFruitDataTbl();
}

void SerialThread::TriggerCamCapture()
{
    if (!((MainWindow *)m_mainWindow)->m_bmanual)
    {
        // 1.capture image
        //        emit SigTopCameraMeasure();// maybe cause error miss
        ((MainWindow *)m_mainWindow)->on_camManuBtn_clicked();
    }
}
