//#include "MtIrq.h"
#include <windows.h>
#include <MMSystem.h>
#include "frmio.h"
//#include "staticdo.h"
#include "def.h"
#include "mainwindow.h"
#include "staticdi.h"

#define _PH_BUFF 10
#define _MAX_SENSOR_LINE_TRG_PH 1

#ifdef __MINGW32__ //w32api bug
    #define TIME_KILL_SYNCHRONOUS 0x0100
#endif

static TIMECAPS tc;
static int IrqCount;

MtIrq::MtIrq(void *lpvoid)
{
    bRun = true;
    iTrayInterval = 0;
    m_mainwindow = lpvoid;
}

MtIrq::~MtIrq()
{
}

void MtIrq::stop()
{
    bRun = false;
}

static int diff64(__int64 t1, __int64 t0)
{
    __int64 t, tt;
    QueryPerformanceFrequency((LARGE_INTEGER *)&tt);
    t = t1 - t0;
    return (int)(t * 1000L / tt);
}

static void MTIRQ0(void *lpvoid)
{
    MtIrq *trdMtIrq = (MtIrq *)lpvoid;

    if (!trdMtIrq->bRun)
    {
        return;
    }

    static __int64 t0, t1;
    MainWindow *main = (MainWindow *)(trdMtIrq->m_mainwindow);
    StaticDI *ioinput = (StaticDI *)((FrmIO *)(main->m_frmio))->GetStaticIOIn();
    //    StaticDO *iooutput = (StaticDO *)((FrmIO *)(main->m_frmio))->GetStaticIOOut();
    // start work
    static int count = 0;
    static int ph_buff[_MAX_SENSOR_LINE_TRG_PH][_PH_BUFF];
    static bool last_ph_status[_MAX_SENSOR_LINE_TRG_PH];
    bool ph_status[_MAX_SENSOR_LINE_TRG_PH];
    ioinput->ReadIO();
    ph_buff[0][count] = ioinput->GetIOData(IO_BOARD_IN_PORT_ZERO); //传感器

    for (int i = 0; i < _MAX_SENSOR_LINE_TRG_PH; i++) //
    {
        int on_count = 0;

        for (int j = 0; j < _PH_BUFF; j++)
        {
            if (ph_buff[i][j])
            {
                on_count++;
            }
        }

        if (_PH_BUFF / 2 <= on_count)
        {
            ph_status[i] = true;    //PH传感器滤波
        }
        else
        {
            ph_status[i] = false;
        }

        if (ph_status[i] && !last_ph_status[i]) 	// 上升沿
        {
            switch (i)
            {
                case 0:
                    // camera light
                    QueryPerformanceCounter((LARGE_INTEGER *)&t1);
                    trdMtIrq->iTrayInterval = diff64(t1, t0);
                    QueryPerformanceCounter((LARGE_INTEGER *)&t0);
                    // 1.capture image
                    main->m_serThread->TriggerCamCapture();
                    break;
            }
        }
        else if (!ph_status[i] && last_ph_status[i]) // 下降沿
        {
            switch (i)
            {
                case 0:
                    break;
            }
        }

        last_ph_status[i] = ph_status[i];
    }

    count = (count + 1) % _PH_BUFF;
    //    iooutput->WriteIO(g_portOutStates);// don't need it
}

void WINAPI CALLBACK IntervalTimer(UINT wTimerID, UINT msg,
                                   DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    Q_UNUSED(wTimerID)
    Q_UNUSED(msg)
    Q_UNUSED(dw1)
    Q_UNUSED(dw2)
    //    static __int64 last_t0;
    __int64 t1, t0;
    //    __int64 pitch;
    QueryPerformanceCounter((LARGE_INTEGER *)&t0);
    MtIrq *trdMtIrq = (MtIrq *)dwUser;

    if (++IrqCount >= 100000000)
    {
        IrqCount = 0;
    }

    MTIRQ0((void *)dwUser);
    QueryPerformanceCounter((LARGE_INTEGER *)&t1);
    trdMtIrq->int_time = diff64(t1, t0);
    //    qInfo() << QString("%1:%2").arg("process time").arg(trdMtIrq->int_time);
    //    pitch = diff64(t0, last_t0);
    //    qDebug() << QString("%1:%2").arg("idle time").arg(pitch);
    //    last_t0 = t0;
}

void MtIrq::run()
{
    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
    {
        return;
    }

    if (tc.wPeriodMin > TARGET_RESOLUTION)
    {
        return;
    }

    timeBeginPeriod(TARGET_RESOLUTION);
    wTimerID = timeSetEvent(
                   TARGET_INTERVAL, // delay
                   TARGET_RESOLUTION, // resolution (global variable)
                   IntervalTimer, // callback function
                   (DWORD_PTR)this, // user data
                   TIME_PERIODIC); // multi timer event

    while (bRun)
    {
        Sleep(100);
    }

    if (wTimerID)   // is timer event pending?
    {
        timeKillEvent(wTimerID); // cancel the event
        wTimerID = 0;
    }

    timeEndPeriod(TARGET_RESOLUTION);
}
