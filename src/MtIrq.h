#pragma once

#include <QThread>
#include <QtCore/QObject>

#define TARGET_RESOLUTION 1    // 1-millisecond target resolution
#define TARGET_INTERVAL 2      // 2-millisecond target interval

class MtIrq : public QThread
{
    Q_OBJECT
protected:
    void run();
private:
    int wTimerID;
public:
    MtIrq(void *);
    ~MtIrq();

    bool bRun;
    int iTrayInterval;

    void stop();
    int int_time;
    void *m_mainwindow;

signals:
    //    void SigTopCameraMeasure();
};

