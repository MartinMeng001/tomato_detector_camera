#pragma once

#include <QMainWindow>
#include <QMenu>
#include <QtGlobal>
#include "formsetting.h"
#include "def.h"
#include "frmimgsetting.h"
#include "camctrl.h"
#include "serialthread.h"
#include "imageprocess.h"
#include "inicommon.h"
#include "imagemeasure.h"
#include "frmio.h"
#include "MtIrq.h"
#include "saveimagethread.h"
#include <QVariant>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    FormSetting *frmSet[CAMERANUM];
    FrmImgSetting *frmImgSet[CAMERANUM];
    ImageSetting *m_imgSettings[CAMERANUM];
    CamSetting *m_camSettings[CAMERANUM];
    SYSetting m_sySetting;
    SaveImageThread *m_saveImgThread;
    CamCtrl *m_camCtrl;
    ImageProcess *m_imgProc;
    FrmIO *m_frmio;
    int iCamerasError; //
    int iLightError; //
    bool bResultOK; //
    bool bKaraCup; // >MinSize
    bool m_bIsTrigValid, m_bIsTrigValid2;                ///< 触发是否有效标志:当一次触发正在执行时，将该标志置为false
    bool m_bmanual;
    SerialThread *m_serThread;

    int GetCupIndex();
    QString GetSendCamData();
    QVector<Mat> m_matvec;

protected:
    void showEvent(QShowEvent *) override;
    //    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    void on_SigMonitorDisp(QVariant &qtmp);
    void on_camManuBtn_clicked();

private slots:
    void initForm();
    void initTableWidget();
    void initStyle();
    void ShowScanTypeSlot(int);
    //    void ShowScanTypeSlot2(int);
    void on_SigSysShutDown();
    void on_SigLogFruitDataTbl();
    void ShowLog(QtMsgType type, QString str);
    void SetImageToProcessImg(int, BYTE *);
    void ShowSrcImgFromCAMSlot();

    void on_actionDisp0_triggered(bool checked);
    void on_actionDisp1_triggered(bool checked);
    void on_actionDisp2_triggered(bool checked);
    void on_actionDisp3_triggered(bool checked);
    void on_actionDisp4_triggered(bool checked);
    void on_actionDisp5_triggered(bool checked);
    void on_actionDisp6_triggered(bool checked);
    void on_actionDisp7_triggered(bool checked);
    void on_actionDispClr4_triggered(bool checked);
    void on_actionDispClr3_triggered(bool checked);
    void on_actionDispClr2_triggered(bool checked);
    void on_actionDispClr1_triggered(bool checked);
    void on_actionDispClr0_triggered(bool checked);

    QT_DEPRECATED_X("not used") void on_camManuBtn_2_clicked();
    void on_ebxRsPort_valueChanged(int arg1);
    void on_ebxColorHin_valueChanged(int arg1);
    void on_ebxCaption_editingFinished();
    void ebxColorHin_valueSet(int arg1);

    // ----------------------image setting-------------------------------
    void on_updateSepLine(int, int);
    void on_updateTopThr(int, int);
    void on_updateBotThr(int, int);
    void on_updateStrpThreShold(int, int);
    void on_updateStrpAngleWt(int, double);
    void on_updateCentreLinepAngleWt(int, double);
    void on_updateGHosei(int, int);
    void on_updateHeightHosei(int, int);
    void on_updateShimaBlockSize(int, int);
    void on_updateShimaThreshold(int, int);
    void on_updateShimaMinLen(int, int);
    void on_updateShimaMinArea(int, int);
    void on_updateVolumeHoseiA(int, double);
    void on_updateVolumeHoseiB(int, double);
    void on_updateBinaryMaxB(int, int);
    void on_updateBinaryMinG(int, int);
    void on_updateBinaryThreshold(int, int);
    void on_updateErodeDilateNum(int, int);
    void on_updateBinaryShift(int, int);
    void on_updateSealGHoseiX(int, int);
    void on_updateSealGHoseiY(int, int);
    void on_updateSealWidth(int, int);
    void on_updateSealHeight(int, int);
    void on_updateSealMinSize(int, int);
    void on_updateExpoTime(int, int);
    // ----------------------image setting-------------------------------

    //-----------------------image rgb setting---------------------------
    void on_updateName(int, int, const QString &);
    void on_updateRed(int, int, int);
    void on_updateGreen(int, int, int);
    void on_updateHanni(int, int, int);
    void on_updateWmin(int, int, int);
    void on_updateWmax(int, int, int);
    void updateRG_A1_Slot(int, int, int);
    void updateRG_A2_Slot(int, int, int);
    void updateRG_K_Slot(int, int, int);
    void updateRG_Rmin_Slot(int, int, int);
    void on_updateDispR(int, int, int);
    void on_updateDispG(int, int, int);
    void on_updateDispB(int, int, int);
    void on_updateUse(int, int, bool);
    //-----------------------image rgb setting---------------------------

    //-----------------------cam setting---------------------------
    void on_camupdatewhiteBalnChk(int, bool);
    void on_camupdateLeanChk(int, bool);
    void on_camupdateWidth(int, int);
    void on_camupdateHeight(int, int);
    void on_camupdateGain(int, int);
    void camupdateR_Gain_Slot(int, double, double);
    void camupdateG_Gain_Slot(int, double, double);
    void camupdateB_Gain_Slot(int, double, double);
    void on_camupdateTriggerSource(int, int);
    void on_camupdateLeft(int, int);
    void on_camupdateRight(int, int);
    void on_camupdateTop(int, int);
    void on_camupdateBottom(int, int);
    void on_camupdatePixSize(int, double);
    void on_camupdateMedianFilter(int, int);
    void on_camupdateKaraThr(int, double);
    void on_camupdateMinLabelingSize(int, int);
    void on_camupdateTanshuku(int, double);
    void on_camupdateMinValue(int, int);
    void on_camupdateRotateMode(int, bool);
    void on_camupdateAutowhiteBaln(int);
    void on_camAutowhiteBaln(int);
    //-----------------------cam setting---------------------------

    void on_ebxHinsyuName_editingFinished();
    void on_chbSaveLog_stateChanged(int arg1);
    void on_picManuBtn_clicked();
    void on_spinBxDistance_valueChanged(int arg1);
    void on_CombIOIndexValuedChanged(int index);

    void on_tabWidget_currentChanged(int index);

    void on_spinBxFps_valueChanged(int arg1);

signals:
    void StopSeiral();
    void ShowCamSetting(CamSetting **);
    void ShowImageSetting(ImageSetting **);
    void SigUpdateGainValue(int gain, int r_gain, int g_gain, int b_gain, int nid);

private:
    Ui::MainWindow *ui;
    QWidget *tab_ioctl;
    QGridLayout *layout_io;
    QMenu *m_Qmenu;
    HDC m_hdcmain, m_hdcsub, m_hdc[CAMERANUM];
    QLabel *m_lblCaller;
    int m_menuCallerIdx;
    int m_menuUpValue[LBL_CAM_NUM][1];
    int m_menuDownValue[LBL_CAM_NUM][1];
    InICommon m_inicommon;
    bool m_bFirstShow, m_bAutoSaveImg;
    QString m_strFilePath;

    bool bFlash[2];
    bool bLightOn[2];
    bool m_ShutDown;
    int m_nTimerID500, m_nTimerID1000, m_nTimerID5000;
    ULONGLONG m_dataIndex;
    int m_cupIndex;
    bool m_bReg;
    MtIrq *m_trdMtIrq;
    bool m_bShowImgRT;

    void initMenu();
    void displaySplitUpMenu(int);
    void displaySplitDownMenu(int);
    void SetSplitUpMenu();
    void SetSplitDownMenu();
    void StartSerialThread();
    void ScreenShow();
    void DrawImg(CamDevice *, HDC, BYTE *, int, QRect, int width, int height, bool bcolor = true);
    void InsertTableData(int);
    double AvePartResult(int iResultID);
    double SumPartResult(int iResultID);
    double SumSideResult(int iResultID);
    double AveSideResult(int iResultID);
    void ImageLoad();
    QT_DEPRECATED_X("not used") void ImgInitLoad();
    Mat m_appleRedRatio, m_appleRedType;

protected:
    // QObject interface
    void timerEvent(QTimerEvent *event) override;
    // QWidget interface
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};
