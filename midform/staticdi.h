#ifndef STATICDI_H
#define STATICDI_H

#include <QDialog>
#include <QTimer>
#include "ui_staticdi.h"
#include "DioPortUI.h"
#include "bdaqctrl.h"

using namespace Automation::BDaq;
class StaticDI : public QWidget
{
    Q_OBJECT

public:
    StaticDI(QWidget *parent = 0);
    ~StaticDI();

    void Initialize();
    void InitializePortPanel();
    void ConfigureDevice();
    void CheckError(ErrorCode errorCode);
    void SetDeviceName(QString str);
    void ReadIO();
    bool GetIOData(int io, int port = 0);

private:
    Ui::StaticDIClass ui;
    InstantDiCtrl *instantDiCtrl;
    DioPortUI **m_portPanel;
    QVBoxLayout *layout;

    int portCount;
    int portPanelLength;
    QString images[2];
    QTimer *timer;
    QString deviceName;

private slots:
    void TimerTicked();
    void TimerRunStop(bool);

signals:
    void SigInit();
    void SigTimerRunStop(bool);
};

#endif // STATICDI_H
