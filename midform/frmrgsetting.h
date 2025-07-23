#pragma once

#include <QWidget>
#include "def.h"

namespace Ui {
class FrmRGSetting;
}

class FrmRGSetting : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRGSetting(int, int, QWidget *parent = nullptr);
    ~FrmRGSetting();

private slots:
    void on_ebxName_textChanged(const QString &arg1);

    void on_ebxRed_valueChanged(int arg1);

    void on_ebxGreen_valueChanged(int arg1);

    void on_ebxHanni_valueChanged(int arg1);

    void on_ebxWmin_valueChanged(int arg1);

    void on_ebxWmax_valueChanged(int arg1);

    void on_ebxRG_A1_valueChanged(int arg1);

    void on_ebxRG_A2_valueChanged(int arg1);

    void on_ebxRG_K_valueChanged(int arg1);

    void on_ebxRG_Rmin_valueChanged(int arg1);

    void on_ebxDispR_valueChanged(int arg1);

    void on_ebxDispG_valueChanged(int arg1);

    void on_ebxDispB_valueChanged(int arg1);

public slots:
    void Screen_Show_Slot(ColorSetting *);

signals:
    void updateName(int, int, const QString &);
    void updateUse(int, int, bool);
    void updateRed(int, int, int);
    void updateGreen(int, int, int);
    void updateHanni(int, int, int);
    void updateWmin(int, int, int);
    void updateWmax(int, int, int);
    void updateRG_A1(int, int, int);
    void updateRG_A2(int, int, int);
    void updateRG_K(int, int, int);
    void updateRG_Rmin(int, int, int);
    void updateDispR(int, int, int);
    void updateDispG(int, int, int);
    void updateDispB(int, int, int);

private:
    Ui::FrmRGSetting *ui;
    int m_itab;
    int m_nid;
};

