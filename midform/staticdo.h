#ifndef STATICDO_H
#define STATICDO_H

#include <QDialog>
#include "DioPortUI.h"
#include "ui_staticdo.h"
#include "bdaqctrl.h"

using namespace Automation::BDaq;
class StaticDO : public QWidget
{
    Q_OBJECT

public:
    StaticDO(QWidget *parent = 0);
    ~StaticDO();

    void Initialize();
    void InitializePortPanel();
    void ConfigureDevice();
    void CheckError(ErrorCode errorCode);
    void SetDeviceName(QString str);
    void WriteIO(quint8 *);

private:
    Ui::StaticDOClass ui;
    InstantDoCtrl *instantDoCtrl;
    DioPortUI **m_portPanel;
    QVBoxLayout *layout;

    int portCount;
    int portPanelLength;
    QString images[3];
    QString deviceName;

private slots:
    void stateChanged(QObject *sender);

signals:
    void SigInit();
};

#endif // STATICDO_H
