#ifndef FRMIO_H
#define FRMIO_H

#include <QWidget>

extern bool m_bManual;
namespace Ui
{
    class FrmIO;
}

class FrmIO : public QWidget
{
    Q_OBJECT

public:
    explicit FrmIO(QWidget *parent = nullptr);
    ~FrmIO();
    void SetComIOTextIndex(int index);
    void InitDevice();
    void SearchDev();
    void SetTimerRunStop(bool);
    void *GetStaticIOIn();
    void *GetStaticIOOut();

signals:
    void updateCombIOText(int);

private slots:
    void on_chkIOManual_stateChanged(int arg1);

private:
    Ui::FrmIO *ui;

};

#endif // FRMIO_H
