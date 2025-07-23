#pragma once

#include <QThread>
#include "ApiRsLib.h"
#include "def.h"
#include <QtConcurrent/QtConcurrent>

class SerialThread : public QThread
{
    Q_OBJECT
public:
    SerialThread(QObject *parent = 0);
    ~SerialThread();
    void SendDataBySerial();
    void TriggerCamCapture();

    int cnt;
    int iSendCnt;

    QT_DEPRECATED bool CompleteCheck();
    bool m_bOpen;

signals:
    void SigMonitorDisp(QVariant &qtmp);
    void SigSysShutDown();
    void SigTopCameraMeasure();
    QT_DEPRECATED_X("not used") void SigBotCameraMeasure();
    void SigLogFruitDataTbl();
    void SigUpdateHinNo(int);

public slots:
    void StopThread();

    // QThread interface
protected:
    void run();

private:
    RS *rs_port; //
    HANDLE m_handles[CAMERANUM];
    void *m_mainWindow;
    bool m_bStop;
    QFuture<bool> future;

};

