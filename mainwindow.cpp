#if _MSC_VER >=1600 //VS2010版本号是1600
    #pragma execution_character_set("utf-8")
#endif

#include <QScreen>
#include <QDateTime>
#include <QDebug>
#include <QMouseEvent>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrentRun>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdebug.h"
#include "camctrl.h"
#include "savelog.h"
#include "omireg.h"

//#ifdef _MSC_VER
//    #pragma comment (lib,"Advapi32.lib")
//    #pragma comment(lib, "user32.lib")

//    #ifdef _DEBUG
//        #pragma comment(lib, "lib\\omisliderplugind.lib")
//        #pragma comment (lib, "opencv_world345d.lib")
//    #else
//        #pragma comment(lib, "lib\\omisliderplugin.lib")
//        #pragma comment (lib, "opencv_world345.lib")
//    #endif

//    #if _MSC_VER >= 1600 //VS2010版本号为1600
//        #pragma execution_character_set("utf-8")
//    #endif
//#endif

//--------------------global variable init-----------------------
HANDLE g_camComplHnds[CAMERANUM] = {CreateEvent(NULL, FALSE, FALSE, NULL)/*, CreateEvent(NULL, FALSE, FALSE, NULL), CreateEvent(NULL, FALSE, FALSE, NULL),
                                    CreateEvent(NULL, FALSE, FALSE, NULL), CreateEvent(NULL, FALSE, FALSE, NULL), CreateEvent(NULL, FALSE, FALSE, NULL)*/
                                   };
HANDLE g_SerSendHnd = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE g_KaraCupHnd = CreateEvent(NULL, TRUE, FALSE, NULL);
CStopWatch g_stopWatch;
double g_dCaptureTime = 0.0;
double g_dCalTime = 0.0;
double g_dDrawTime = 0.0;
ApeDataSt g_appleData;
QString g_sResultName[SERIAL_NUM_RESULT_DATA] =
{
    "病变",
    "干疤",
    "畸形率",
    "裂果",
    "直径mm",
    "转色",
    "-",
    "-",
    "-",
    "-"
};

const char *CAM_DEFINE_USERID[CAMERANUM] =
{
    "TOP_CAMERA"/*,
    "SIDE1_CAMERA",
    "SIDE2_CAMERA",
    "SIDE3_CAMERA",
    "SIDE4_CAMERA",
    "BOTTOM_CAMERA"*/
};

int g_iSaveCount = 1;
int g_iLoadCount = 1;
static bool bLoadImageFlag[CAMERANUM]; //

bool g_btrimode = false;
bool g_bTopCamKaraCup = true;

QString SECTION_NAME[CAMERANUM] =
{
    "CAM0"/*,
    "CAM1",
    "CAM2",
    "CAM3",
    "CAM4",
    "CAM5"*/
};

//--------------------global variable init-----------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_Qmenu(NULL)
{
    ui->setupUi(this);
    m_serThread = NULL;
    char    szName[MAX_PATH] = {0};
    QString strAppFullName = "";
    GetModuleFileNameA(NULL, szName, MAX_PATH);
    strAppFullName = szName;
    int nPos = strAppFullName.lastIndexOf('\\');
    m_strFilePath += strAppFullName.leftRef(nPos);
    m_strFilePath += IMG_SAVE_PATH;

    for (int i = 0; i < CAMERANUM; ++i)
    {
        frmSet[i] = NULL;
        frmImgSet[i] = NULL;
        // init setting
        m_imgSettings[i] = new ImageSetting();
        m_camSettings[i] = new CamSetting();
        bLoadImageFlag[i] = false;
    }

    m_frmio = NULL;
    tab_ioctl = NULL;
    m_camCtrl = new CamCtrl(this);
    m_hdcmain = GetDC((HWND)ui->labCh00->winId());
    m_hdcsub = GetDC((HWND)ui->labCh01->winId());
    m_hdc[0] = GetDC((HWND)ui->labCh1->winId());
#ifndef CAM_NUM_ONLY_ONE
    m_hdc[1] = GetDC((HWND)ui->labCh2->winId());
    m_hdc[2] = GetDC((HWND)ui->labCh3->winId());
    m_hdc[3] = GetDC((HWND)ui->labCh4->winId());
    m_hdc[4] = GetDC((HWND)ui->labCh5->winId());
    m_hdc[5] = GetDC((HWND)ui->labCh6->winId());
#endif
    m_menuCallerIdx = 0;
    m_lblCaller = NULL;
    m_bShowImgRT = false;

    for (int i = 0; i < LBL_CAM_NUM; i++)
    {
        m_menuUpValue[i][0] = 0x1;
        m_menuDownValue[i][0] = 0x1;
    }

    m_inicommon.ReadINI(m_camSettings, m_imgSettings, &m_sySetting);
    this->initForm();
    // menu init
    ui->labCh00->installEventFilter(this);
    ui->labCh01->installEventFilter(this);
    ui->labCh1->installEventFilter(this);
    ui->labCh2->installEventFilter(this);
    ui->labCh3->installEventFilter(this);
    ui->labCh4->installEventFilter(this);
    ui->labCh5->installEventFilter(this);
    ui->labCh6->installEventFilter(this);
    ui->lblReg->installEventFilter(this);
    ui->txtError->setStyleSheet("border:none");
    ui->txtEvent->setStyleSheet("border:none");
    ui->lblRSPort->setStyleSheet("color:red");
    connect(ui->camMeasBtn, QOverload<int>::of(&whirlbutton::updateScanType), this, &MainWindow::ShowScanTypeSlot);
    connect(ui->camMeasBtn_2, QOverload<int>::of(&whirlbutton::updateScanType), this, &MainWindow::ShowScanTypeSlot);
    connect(ui->chkAutoSave, &ImageSwitch::updateChecked, this, [ = ](bool bcheck)
    {
        m_bAutoSaveImg = bcheck;
        emit m_camCtrl->UpdateAutoSave(bcheck);
    });
    //单独控制
    //    connect(ui->cbxLED_ON, &ImageSwitch::updateChecked, this, [ = ](bool bcheck) {
    //        m_camCtrl->LEDCtrl(TOP_CAMERA, bcheck);
    //        ui->lcdLightType->Clear();
    //        if (bcheck || ui->cbxLED_ON_2->getChecked()) {
    //            ui->lcdLightType->SetCursor(3, 1);
    //            ui->lcdLightType->SetColorPixel(QColor(245, 245, 245));
    //            ui->lcdLightType->String(QString("LED: ON"));
    //        }
    //    });
    //    connect(ui->cbxLED_ON_2, &ImageSwitch::updateChecked, this, [ = ](bool bcheck) {
    //        m_camCtrl->LEDCtrl(BOTTOM_CAMERA, bcheck);
    //        ui->lcdLightType->Clear();
    //        if (bcheck || ui->cbxLED_ON->getChecked()) {
    //            ui->lcdLightType->SetCursor(3, 1);
    //            ui->lcdLightType->SetColorPixel(QColor(245, 245, 245));
    //            ui->lcdLightType->String(QString("LED: ON"));
    //        }
    //    });
    connect(ui->cbxLED_ON, &ImageSwitch::updateChecked, this, [ & ](bool bcheck)
    {
        if (bLightOn[0] == bcheck)
        {
            return;
        }

        m_camCtrl->LEDCtrl(TOP_CAMERA, bcheck);
#ifndef CAM_NUM_ONLY_ONE
        m_camCtrl->LEDCtrl(BOTTOM_CAMERA, bcheck);
#endif
        ui->cbxLED_ON_2->setChecked(bcheck);
        ui->lcdLightType->Clear();

        if (bcheck || ui->cbxLED_ON_2->getChecked())
        {
            ui->lcdLightType->SetCursor(3, 1);
            ui->lcdLightType->SetColorPixel(QColor(245, 245, 245));
            ui->lcdLightType->String(QString("LED: ON"));
        }

        bLightOn[0] = bcheck;
        bLightOn[1] = bcheck;
    });
    connect(ui->cbxLED_ON_2, &ImageSwitch::updateChecked, this, [ & ](bool bcheck)
    {
        if (bLightOn[1] == bcheck)
        {
            return;
        }

        m_camCtrl->LEDCtrl(TOP_CAMERA, bcheck);
#ifndef CAM_NUM_ONLY_ONE
        m_camCtrl->LEDCtrl(BOTTOM_CAMERA, bcheck);
#endif
        ui->cbxLED_ON->setChecked(bcheck);
        ui->lcdLightType->Clear();

        if (bcheck || ui->cbxLED_ON->getChecked())
        {
            ui->lcdLightType->SetCursor(3, 1);
            ui->lcdLightType->SetColorPixel(QColor(245, 245, 245));
            ui->lcdLightType->String(QString("LED: ON"));
        }

        bLightOn[0] = bcheck;
        bLightOn[1] = bcheck;
    });
    connect(ui->cbxIMGShow_OFF, &ImageSwitch::updateChecked, this, [ & ](bool bcheck)
    {
        m_bShowImgRT = bcheck;
    });
    connect(SaveLog::Instance(), &SaveLog::LogSignal, this, &MainWindow::ShowLog);
    connect(m_camCtrl, &CamCtrl::SetImageData, this, &MainWindow::SetImageToProcessImg);
    connect(m_camCtrl, &CamCtrl::ShowSrcImgFromCAMSig, this, &MainWindow::ShowSrcImgFromCAMSlot);
    m_ShutDown = false;
    m_bAutoSaveImg = false;
    m_nTimerID500 = 0;
    m_nTimerID1000 = 0;
    m_nTimerID5000 = 0;
    bFlash[0] = false;
    bFlash[1] = false;
    bLightOn[0] = false;
    bLightOn[1] = false;
    m_bIsTrigValid = true; // 触发是否有效标志
    m_bIsTrigValid2 = true;
    m_dataIndex = 0;
    m_cupIndex = 0;
    m_bmanual = false;
    // release's status, save log default
#ifndef QT_DEBUG
    ui->chbSaveLog->setChecked(true);
#endif
    m_bFirstShow = true;
    //    ImgInitLoad();
    m_trdMtIrq = NULL;
#ifdef CAM_NUM_ONLY_ONE
    ui->label_9->setVisible(false);
    ui->camMeasBtn_2->setVisible(false);
    ui->label_6->setVisible(false);
    ui->cbxLED_ON->setVisible(false);
    ui->label_10->setVisible(false);
    ui->cbxLED_ON_2->setVisible(false);
#endif
}

MainWindow::~MainWindow()
{
    //close cam
    m_camCtrl->CloseAllCamera();
#ifndef TRIGGER_WITH_COMM
    m_trdMtIrq->stop();
    m_trdMtIrq->quit();
    m_trdMtIrq->wait();
    delete m_trdMtIrq;
#endif

    if (m_camCtrl != nullptr)
    {
        delete m_camCtrl;
    }

    if (m_frmio != nullptr)
    {
        delete m_frmio;
    }

    for (int i = 0; i < CAMERANUM; ++i)
    {
        delete frmSet[i];
        delete frmImgSet[i];
        delete m_imgSettings[i];
        delete m_camSettings[i];
        CloseHandle(g_camComplHnds[i]);
    }

    CloseHandle(g_SerSendHnd);
    CloseHandle(g_KaraCupHnd);
    delete ui;
    //在结束的时候调用GXCLoseLib()释放资源
    GXCloseLib();
}

void MainWindow::ImgInitLoad()
{
    QString strPath, filenamerat, filenametyp, errstr;
    // 获取当前程序执行路径,设置图像保存的默认路径
    char    szName[MAX_PATH] = {0};
    QString strAppFullName = "";
    GetModuleFileNameA(NULL, szName, MAX_PATH);
    strAppFullName = szName;
    int nPos = strAppFullName.lastIndexOf('\\');
    strPath += strAppFullName.leftRef(nPos);
    strPath += IMG_CONTRAST_PATH;
    filenamerat = QString("%1\\%2_%3_%4.bmp").arg(strPath, IMG_LOAD_NAME, QString(m_sySetting.iHinNo), IMG_CONTRAST_RAT);
    filenametyp = QString("%1\\%2_%3_%4.bmp").arg(strPath, IMG_LOAD_NAME, QString(m_sySetting.iHinNo), IMG_CONTRAST_TYPE);
    m_appleRedRatio = imread(filenamerat.toStdString());

    if (m_appleRedRatio.empty())
    {
        errstr += QString("<font size='10' color='black'>redratio图片缺失。</font><br/>");
    }

    m_appleRedType = imread(filenametyp.toStdString());

    if (m_appleRedType.empty())
    {
        errstr += QString("<font size='10' color='black'>redtype图片缺失。</font>");
    }

    if (!errstr.isEmpty())
    {
        QMessageBox::critical(NULL, "Error", errstr);
    }
}

void MainWindow::initStyle()
{
    //加载样式表
    QString qss;
    QFile file(":/qss/psblack.css");

    if (file.open(QFile::ReadOnly))
    {
        //用QTextStream读取样式文件不用区分文件编码 带bom也行
        QStringList list;
        QTextStream in(&file);

        //in.setCodec("utf-8");
        while (!in.atEnd())
        {
            QString line;
            in >> line;
            list << line;
        }

        qss = list.join("\n");
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

void MainWindow::ScreenShow()
{
    ui->ebxRsPort->setValue(m_sySetting.iRsPort);
    ui->ebxCaption->setText(m_sySetting.aCaption);
    ui->ebxColorHin->setValue(m_sySetting.iHinNo);
    ui->ebxHinsyuName->setText(m_sySetting.aHinName);
    ui->spinBxDistance->setValue(m_sySetting.idistance);
    ui->spinBxFps->setValue(m_sySetting.iFpsTest);
    setWindowTitle(m_sySetting.aCaption);
}

void MainWindow::showEvent(QShowEvent *)
{
    // if hidden, when show again,this function will play again.
    // so add this flag to avoid this situation
    if (!m_bFirstShow)
    {
        return;
    }

    // 注册机
    QString str = QApplication::applicationName().append(".exe");
    std::string strTmp = str.toStdString();
    char *name = const_cast<char *>(strTmp.c_str());
    int status = GetLicenseStatusVC(name);

    if (status != 0)
    {
        ShowRegModalDialogVC(name);
        status = GetLicenseStatusVC(name);

        if (status != 0)
        {
            exit(-1);
        }
    }

#ifndef TRIGGER_WITH_COMM
    // show io
    tab_ioctl = new QWidget();
    tab_ioctl->setObjectName(QString::fromUtf8("tab_ioctl"));
    layout_io = new QGridLayout(tab_ioctl);
    layout_io->setObjectName(QString::fromUtf8("layout_io"));
    ui->tabWidget->insertTab(ui->tabWidget->indexOf(ui->tab_commonSet), tab_ioctl, QString());
    ui->tabWidget->setTabIcon(ui->tabWidget->indexOf(tab_ioctl), QIcon(QPixmap(":/image/electronics.ico")));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(tab_ioctl), QCoreApplication::translate("MainWindow", "IO\346\216\247\345\210\266", nullptr));
    m_frmio = new FrmIO(this);
    m_frmio->move(5, 5);
    m_frmio->SetComIOTextIndex(m_sySetting.iIOCTLIdx);
    connect(m_frmio, &FrmIO::updateCombIOText, this, &MainWindow::on_CombIOIndexValuedChanged);
    layout_io->addWidget(m_frmio, 0, 0, 1, 1);
#endif//
#ifdef CAM_NUM_ONLY_ONE
    ui->tabWidget->removeTab(0);
    // setting
    ui->tabWidget_2->removeTab(5);
    ui->tabWidget_2->removeTab(4);
    ui->tabWidget_2->removeTab(3);
    ui->tabWidget_2->removeTab(2);
    ui->tabWidget_2->removeTab(1);
    // image setting
    ui->tabWidget_4->removeTab(5);
    ui->tabWidget_4->removeTab(4);
    ui->tabWidget_4->removeTab(3);
    ui->tabWidget_4->removeTab(2);
    ui->tabWidget_4->removeTab(1);
#endif
    ui->tabWidget->setCurrentIndex(4);
    // show camera setting midform
    frmSet[0] = new FormSetting(0, ui->setting1);
    ui->setLay1->addWidget(frmSet[0], 0, 0, 1, 1);
    frmSet[0]->move(5, 5);
#ifndef CAM_NUM_ONLY_ONE
    frmSet[1] = new FormSetting(1, ui->setting2);
    ui->setLay2->addWidget(frmSet[1], 0, 0, 1, 1);
    frmSet[1]->move(5, 5);
    frmSet[2] = new FormSetting(2, ui->setting3);
    ui->setLay3->addWidget(frmSet[2], 0, 0, 1, 1);
    frmSet[2]->move(5, 5);
    frmSet[3] = new FormSetting(3, ui->setting4);
    ui->setLay4->addWidget(frmSet[3], 0, 0, 1, 1);
    frmSet[3]->move(5, 5);
    frmSet[4] = new FormSetting(4, ui->setting5);
    ui->setLay5->addWidget(frmSet[4], 0, 0, 1, 1);
    frmSet[4]->move(5, 5);
    frmSet[5] = new FormSetting(5, ui->setting6);
    ui->setLay6->addWidget(frmSet[5], 0, 0, 1, 1);
    frmSet[5]->move(5, 5);
#endif

    for (int i = 0; i < CAMERANUM; i++)
    {
        connect(frmSet[i], &FormSetting::updatewhiteBalnChk, this, &MainWindow::on_camupdatewhiteBalnChk);
        connect(frmSet[i], &FormSetting::updateLeanChk, this, &MainWindow::on_camupdateLeanChk);
        connect(frmSet[i], &FormSetting::updateWidth, this, &MainWindow::on_camupdateWidth);
        connect(frmSet[i], &FormSetting::updateHeight, this, &MainWindow::on_camupdateHeight);
        connect(frmSet[i], &FormSetting::updateGain, this, &MainWindow::on_camupdateGain);
        connect(frmSet[i], &FormSetting::updateR_Gain, this, &MainWindow::camupdateR_Gain_Slot);
        connect(frmSet[i], &FormSetting::updateG_Gain, this, &MainWindow::camupdateG_Gain_Slot);
        connect(frmSet[i], &FormSetting::updateB_Gain, this, &MainWindow::camupdateB_Gain_Slot);
        connect(frmSet[i], &FormSetting::updateTriggerSource, this, &MainWindow::on_camupdateTriggerSource);
        connect(frmSet[i], &FormSetting::updateLeft, this, &MainWindow::on_camupdateLeft);
        connect(frmSet[i], &FormSetting::updateRight, this, &MainWindow::on_camupdateRight);
        connect(frmSet[i], &FormSetting::updateTop, this, &MainWindow::on_camupdateTop);
        connect(frmSet[i], &FormSetting::updateBottom, this, &MainWindow::on_camupdateBottom);
        connect(frmSet[i], &FormSetting::updatePixSize, this, &MainWindow::on_camupdatePixSize);
        connect(frmSet[i], &FormSetting::updateMedianFilter, this, &MainWindow::on_camupdateMedianFilter);
        connect(frmSet[i], &FormSetting::updateKaraThr, this, &MainWindow::on_camupdateKaraThr);
        connect(frmSet[i], &FormSetting::updateMinLabelingSize, this, &MainWindow::on_camupdateMinLabelingSize);
        connect(frmSet[i], &FormSetting::updateTanshuku, this, &MainWindow::on_camupdateTanshuku);
        connect(frmSet[i], &FormSetting::updateMinValue, this, &MainWindow::on_camupdateMinValue);
        connect(frmSet[i], &FormSetting::updateRotateMode, this, &MainWindow::on_camupdateRotateMode);
        connect(frmSet[i], &FormSetting::updateAutowhiteBaln, this, &MainWindow::on_camupdateAutowhiteBaln);
        connect(frmSet[i], &FormSetting::CamAutowhiteBaln, this, &MainWindow::on_camAutowhiteBaln);
        // show camera setting
        connect(this, &MainWindow::ShowCamSetting, frmSet[i], &FormSetting::Screen_Show_Slot);
        connect(this, &MainWindow::SigUpdateGainValue, frmSet[i], &FormSetting::UpdateGainValue);
        //        frmSet[i]->show();
    }

    emit ShowCamSetting(m_camSettings);
    // show image setting midform
    frmImgSet[0] = new FrmImgSetting(0, ui->imageSet1);
    ui->imgLayout1->addWidget(frmImgSet[0], 0, 0, 1, 1);
    frmImgSet[0]->move(5, 5);
#ifndef CAM_NUM_ONLY_ONE
    frmImgSet[1] = new FrmImgSetting(1, ui->imageSet2);
    ui->imgLayout2->addWidget(frmImgSet[1], 0, 0, 1, 1);
    frmImgSet[1]->move(5, 5);
    frmImgSet[2] = new FrmImgSetting(2, ui->imageSet3);
    ui->imgLayout3->addWidget(frmImgSet[2], 0, 0, 1, 1);
    frmImgSet[2]->move(5, 5);
    frmImgSet[3] = new FrmImgSetting(3, ui->imageSet4);
    ui->imgLayout4->addWidget(frmImgSet[3], 0, 0, 1, 1);
    frmImgSet[3]->move(5, 5);
    frmImgSet[4] = new FrmImgSetting(4, ui->imageSet5);
    ui->imgLayout5->addWidget(frmImgSet[4], 0, 0, 1, 1);
    frmImgSet[4]->move(5, 5);
    frmImgSet[5] = new FrmImgSetting(5, ui->imageSet6);
    ui->imgLayout6->addWidget(frmImgSet[5], 0, 0, 1, 1);
    frmImgSet[5]->move(5, 5);
#endif

    for (int i = 0; i < CAMERANUM ; i++)
    {
        connect(frmImgSet[i], &FrmImgSetting::updateSepLine, this, &MainWindow::on_updateSepLine);
        connect(frmImgSet[i], &FrmImgSetting::updateTopThr, this, &MainWindow::on_updateTopThr);
        connect(frmImgSet[i], &FrmImgSetting::updateBotThr, this, &MainWindow::on_updateBotThr);
        connect(frmImgSet[i], &FrmImgSetting::updateStrpThreShold, this, &MainWindow::on_updateStrpThreShold);
        connect(frmImgSet[i], &FrmImgSetting::updateStrpAngleWt, this, &MainWindow::on_updateStrpAngleWt);
        connect(frmImgSet[i], &FrmImgSetting::updateCentreLinepAngleWt, this, &MainWindow::on_updateCentreLinepAngleWt);
        connect(frmImgSet[i], &FrmImgSetting::updateGHosei, this, &MainWindow::on_updateGHosei);
        connect(frmImgSet[i], &FrmImgSetting::updateHeightHosei, this, &MainWindow::on_updateHeightHosei);
        connect(frmImgSet[i], &FrmImgSetting::updateShimaBlockSize, this, &MainWindow::on_updateShimaBlockSize);
        connect(frmImgSet[i], &FrmImgSetting::updateShimaThreshold, this, &MainWindow::on_updateShimaThreshold);
        connect(frmImgSet[i], &FrmImgSetting::updateShimaMinLen, this, &MainWindow::on_updateShimaMinLen);
        connect(frmImgSet[i], &FrmImgSetting::updateShimaMinArea, this, &MainWindow::on_updateShimaMinArea);
        connect(frmImgSet[i], &FrmImgSetting::updateVolumeHoseiA, this, &MainWindow::on_updateVolumeHoseiA);
        connect(frmImgSet[i], &FrmImgSetting::updateVolumeHoseiB, this, &MainWindow::on_updateVolumeHoseiB);
        connect(frmImgSet[i], &FrmImgSetting::updateBinaryMaxB, this, &MainWindow::on_updateBinaryMaxB);
        connect(frmImgSet[i], &FrmImgSetting::updateBinaryMinG, this, &MainWindow::on_updateBinaryMinG);
        connect(frmImgSet[i], &FrmImgSetting::updateBinaryThreshold, this, &MainWindow::on_updateBinaryThreshold);
        connect(frmImgSet[i], &FrmImgSetting::updateErodeDilateNum, this, &MainWindow::on_updateErodeDilateNum);
        connect(frmImgSet[i], &FrmImgSetting::updateBinaryShift, this, &MainWindow::on_updateBinaryShift);
        connect(frmImgSet[i], &FrmImgSetting::updateSealGHoseiX, this, &MainWindow::on_updateSealGHoseiX);
        connect(frmImgSet[i], &FrmImgSetting::updateSealGHoseiY, this, &MainWindow::on_updateSealGHoseiY);
        connect(frmImgSet[i], &FrmImgSetting::updateSealWidth, this, &MainWindow::on_updateSealWidth);
        connect(frmImgSet[i], &FrmImgSetting::updateSealHeight, this, &MainWindow::on_updateSealHeight);
        connect(frmImgSet[i], &FrmImgSetting::updateSealMinSize, this, &MainWindow::on_updateSealMinSize);
        connect(frmImgSet[i], &FrmImgSetting::updateExpoTime, this, &MainWindow::on_updateExpoTime);
        connect(frmImgSet[i], &FrmImgSetting::updateName, this, &MainWindow::on_updateName);
        connect(frmImgSet[i], &FrmImgSetting::updateRed, this, &MainWindow::on_updateRed);
        connect(frmImgSet[i], &FrmImgSetting::updateGreen, this, &MainWindow::on_updateGreen);
        connect(frmImgSet[i], &FrmImgSetting::updateHanni, this, &MainWindow::on_updateHanni);
        connect(frmImgSet[i], &FrmImgSetting::updateWmin, this, &MainWindow::on_updateWmin);
        connect(frmImgSet[i], &FrmImgSetting::updateWmax, this, &MainWindow::on_updateWmax);
        connect(frmImgSet[i], &FrmImgSetting::updateRG_A1, this, &MainWindow::updateRG_A1_Slot);
        connect(frmImgSet[i], &FrmImgSetting::updateRG_A2, this, &MainWindow::updateRG_A2_Slot);
        connect(frmImgSet[i], &FrmImgSetting::updateRG_K, this, &MainWindow::updateRG_K_Slot);
        connect(frmImgSet[i], &FrmImgSetting::updateRG_Rmin, this, &MainWindow::updateRG_Rmin_Slot);
        connect(frmImgSet[i], &FrmImgSetting::updateDispR, this, &MainWindow::on_updateDispR);
        connect(frmImgSet[i], &FrmImgSetting::updateDispG, this, &MainWindow::on_updateDispG);
        connect(frmImgSet[i], &FrmImgSetting::updateDispB, this, &MainWindow::on_updateDispB);
        connect(frmImgSet[i], &FrmImgSetting::updateUse, this, &MainWindow::on_updateUse);
        // show image setting
        connect(this, &MainWindow::ShowImageSetting, frmImgSet[i], &FrmImgSetting::Screen_Show_Slot);
        //        frmImgSet[i]->show();
    }

    emit ShowImageSetting(m_imgSettings);
    // serial thread init
    StartSerialThread();
#ifndef TRIGGER_WITH_COMM
    // start ph thread
    m_trdMtIrq = new MtIrq(this);
    //    connect(m_trdMtIrq, &MtIrq::SigTopCameraMeasure, this, &MainWindow::on_camManuBtn_clicked);
    m_trdMtIrq->start();
#endif
    // show data
    ScreenShow();
    // image process
    m_imgProc = new ImageProcess(m_imgSettings, m_camSettings, this);
    connect(m_imgProc, &ImageProcess::ShowSrcImgFromCAMSig, this, &MainWindow::ShowSrcImgFromCAMSlot);
    connect(m_imgProc, &ImageProcess::SigUpdateFruitData, this, &MainWindow::on_SigLogFruitDataTbl);
    m_imgProc->start();
    m_saveImgThread = new SaveImageThread(this);
    m_saveImgThread->start();

    // open cam
    if (m_camCtrl->m_nDeviceNum > 0)
    {
        m_camCtrl->OpenAllCamera();

        if (m_camCtrl->m_FailCamList.count() > 0)
        {
            QString strTmp;

            for (int i = 0; i < m_camCtrl->m_FailCamList.count(); i++)
            {
                strTmp.append(QString(CAM_DEFINE_USERID[m_camCtrl->m_FailCamList.at(i) - 1]) + " ");
            }

            strTmp = strTmp.trimmed();
            qCritical() << QString("相机打开失败ID：%1").arg(strTmp);
            QMessageBox::warning(this, "Error", QString("<font size='10' color='black'>相机打开失败ID：%1</font>").arg(strTmp));
        }

        m_camCtrl->LEDCtrl(TOP_CAMERA, false);//关灯
#ifndef CAM_NUM_ONLY_ONE
        m_camCtrl->LEDCtrl(BOTTOM_CAMERA, false);
#endif
    }
    else
    {
        qInfo() << QString("未检测到相机。");
    }

    // lastly start 1000ms timer,5000ms timer
    m_nTimerID1000 = startTimer(1000);
    m_nTimerID5000 = startTimer(5000);
    m_bFirstShow = false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // save something
    m_inicommon.WriteINI(m_camSettings, m_imgSettings, m_sySetting);
    killTimer(m_nTimerID5000);
    killTimer(m_nTimerID1000);
    killTimer(m_nTimerID500);
    ReleaseDC((HWND)ui->labCh00->winId(), m_hdcmain);
    ReleaseDC((HWND)ui->labCh01->winId(), m_hdcsub);
    ReleaseDC((HWND)ui->labCh1->winId(), m_hdc[0]);
#ifndef CAM_NUM_ONLY_ONE
    ReleaseDC((HWND)ui->labCh2->winId(), m_hdc[1]);
    ReleaseDC((HWND)ui->labCh3->winId(), m_hdc[2]);
    ReleaseDC((HWND)ui->labCh4->winId(), m_hdc[3]);
    ReleaseDC((HWND)ui->labCh5->winId(), m_hdc[4]);
    ReleaseDC((HWND)ui->labCh6->winId(), m_hdc[5]);
#endif

    if (m_imgProc)
    {
        m_imgProc->m_bStop = true;
        m_imgProc->wait();
        delete m_imgProc;
    }

    if (m_saveImgThread)
    {
        m_saveImgThread->m_exit = true;

        for (int i = 0; i < CAMERANUM; ++i)
        {
            SetEvent(g_camComplHnds[i]);
        }

        m_saveImgThread->wait();
        delete m_saveImgThread;
    }

    if (m_serThread)
    {
        emit StopSeiral();
        m_serThread->wait();
        delete m_serThread;
    }

    event->accept();//不会将事件传递给组件的父组件
    qDebug() << "Application exit";

    if (m_ShutDown)
    {
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;
        // Get a token for this process.
        ::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
        // Get the LUID for the shutdown privilege.
        ::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1; // one privilege to set
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        // Get the shutdown privilege for this process.
        ::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
        // Shut down the system and force all applications to close.
        ::ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    int height = ui->oneFrttable->size().height();

    if (height > 300)
    {
        int nrow = ui->oneFrttable->rowCount();
        int ih = (height - ui->oneFrttable->horizontalHeader()->size().height()) / nrow;

        for (int i = 0; i < nrow; i++)
        {
            ui->oneFrttable->setRowHeight(i, ih);
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (m_imgProc->pImageBuffers[0] != NULL)
    {
        ShowSrcImgFromCAMSlot();
    }
}

/*
** static function, async update screen.
*/
static void DeviceResetLink(LPVOID lp)
{
    CamCtrl *camcrtl = ((MainWindow *)lp)->m_camCtrl;
    unsigned int nDeviceNum = 0;
    GXUpdateDeviceList(&nDeviceNum, 2000);

    if (nDeviceNum < ((MainWindow *)lp)->m_camCtrl->m_nDeviceNum)
    {
        return;// no more cam take in
    }

    QList<int> tmplist;
    tmplist.swap(camcrtl->m_FailCamList);
    GX_STATUS  emStatus   = GX_STATUS_SUCCESS;

    for (int i = 0; i < tmplist.count(); i++)
    {
        int uid = tmplist.at(i) - 1;
        QLabel *lbl = ((MainWindow *)lp)->findChild<QLabel *>(QString("labCh%1").arg(tmplist.at(i)));
        lbl->setText(QString("相机%1启动失败").arg(tmplist.at(i)));
        GX_DEVICE_BASE_INFO *pBaseinfo = new GX_DEVICE_BASE_INFO[nDeviceNum];
        size_t nSize = nDeviceNum * sizeof(GX_DEVICE_BASE_INFO);
        //获取所有设备的基础信息
        emStatus = GXGetAllDeviceBaseInfo(pBaseinfo, &nSize);
        bool find = false;

        for (unsigned int i = 0; i < nDeviceNum; i++)
        {
            GX_DEVICE_IP_INFO *stIPInfo = new GX_DEVICE_IP_INFO();
            //获取设备的网络信息
            emStatus = GXGetDeviceIPInfo(i + 1, stIPInfo);

            if (strcmp(stIPInfo->szDeviceID, pBaseinfo[i].szDeviceID) == 0)
            {
                if (strcmp(pBaseinfo[i].szUserID, CAM_DEFINE_USERID[uid]) == 0)
                {
                    ((MainWindow *)lp)->m_camCtrl->m_stIPInfos[uid] = stIPInfo;
                    camcrtl->m_camDevice[uid]->m_stIPInfo = stIPInfo;
                    find = true;
                    break;
                }
                else
                {
                    delete stIPInfo;
                }
            }
        }

        delete[] pBaseinfo;

        // try to restart
        // 设备重连
        if (!find || camcrtl->m_camDevice[uid]->m_stIPInfo == NULL)
        {
            // fail again,insert it
            camcrtl->m_FailCamList << tmplist.at(i);
            continue;
        }

        emStatus = GXGigEResetDevice(camcrtl->m_camDevice[uid]->m_stIPInfo->szMAC, GX_MANUFACTURER_SPECIFIC_RECONNECT);

        if (emStatus != GX_STATUS_SUCCESS)
        {
            ShowErrorString(emStatus);
            // 设备复位
            GXGigEResetDevice(camcrtl->m_camDevice[uid]->m_stIPInfo->szMAC, GX_MANUFACTURER_SPECIFIC_RESET);
        }

        ((MainWindow *)lp)->m_camCtrl->m_nDeviceNum = nDeviceNum;
        camcrtl->m_camDevice[uid]->CloseCamera();
        camcrtl->m_camDevice[uid]->OpenCamera();

        if (camcrtl->m_camDevice[uid]->m_pstCam->bIsOpen)
        {
            lbl->setText(QString("相机%1").arg(tmplist.at(i)));
        }
        else
        {
            // fail again,insert it
            camcrtl->m_FailCamList << tmplist.at(i);
        }
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_nTimerID500)
    {
        //500ms timer
        // auto capture
        if (bFlash[0])
        {
            on_camManuBtn_clicked();
        }

        //        if (bFlash[1])
        //            on_camManuBtn_2_clicked();
    }
    else if (event->timerId() == m_nTimerID1000)
    {
        //        if (m_serThread && m_serThread->m_bOpen)
        //        {
        //            ui->lblRSPort->setStyleSheet("color:green;font:bold");
        //        }
        //        else
        //        {
        //            ui->lblRSPort->setStyleSheet("color:red");
        //        }

        // update fps
        for (unsigned int i = 0; i < CAMERANUM; i++)
        {
            if (m_camCtrl->m_camDevice[i])
            {
                m_camCtrl->m_camDevice[i]->m_pCamsFps->UpdateFps();
                m_camCtrl->m_camDevice[i]->m_pstCam->fFps = (float)m_camCtrl->m_camDevice[i]->m_pCamsFps->GetFps();
            }
        }

        // update balance ratio
        int64_t nWBEnumVal = 0;   //< 保存白平衡的枚举值
        GX_STATUS status = GX_STATUS_ERROR;

        for (int i = 0; i < CAMERANUM; i++)
        {
            if (m_camCtrl->m_nAutoWhiteBalance[i] == GX_BALANCE_WHITE_AUTO_ONCE && m_camCtrl->m_camDevice[i]->m_hDevice != NULL)
            {
                status = GXGetEnum(m_camCtrl->m_camDevice[i]->m_hDevice, GX_ENUM_BALANCE_WHITE_AUTO, &nWBEnumVal);

                if (status != GX_STATUS_SUCCESS)
                {
                    continue;
                }
                else
                {
                    m_camCtrl->m_nAutoWhiteBalance[i] = nWBEnumVal;

                    //判断设备自动白平衡是否变为OFF
                    if (nWBEnumVal == GX_BALANCE_WHITE_AUTO_OFF)
                    {
                        double dr, dg, db;

                        if (m_camCtrl->GetBalanceRatio(i, dr, dg, db))
                        {
                            // update rgb ratio
                            m_camSettings[i]->dR_Ratio = dr;
                            m_camSettings[i]->dG_Ratio = dg;
                            m_camSettings[i]->dB_Ratio = db;
                        }
                    }
                }
            }
        }
    }
    else if (event->timerId() == m_nTimerID5000)
    {
        // update cam status
        QtConcurrent::run(DeviceResetLink, this);
    }
}

void MainWindow::on_SigSysShutDown()
{
    m_ShutDown = true;
    this->close();
}

double MainWindow::AvePartResult(int iResultID)
{
    double dAve = 0;
    int iCnt = 0;

    for (unsigned int i = 0; i < CAMERANUM - 1; i++)
    {
        if (m_camCtrl->m_camDevice[i]->m_pstCam->bIsOpen || bLoadImageFlag[i])
        {
            dAve += m_imgProc->dResultData[iResultID];
            iCnt++;
        }
    }

    if (iCnt >= 1)
    {
        dAve /= iCnt;
    }

    return dAve;
}

double MainWindow::SumPartResult(int iResultID)
{
    double dSum = 0;

    for (int i = 0; i < CAMERANUM - 1; i++)
    {
        if (m_camCtrl->m_camDevice[i]->m_pstCam->bIsOpen || bLoadImageFlag[i])
        {
            dSum += m_imgProc->dResultData[iResultID];
        }
    }

    return dSum;
}

double MainWindow::SumSideResult(int iResultID)
{
    double dSum = 0;

    for (int i = 1; i < BOTTOM_CAMERA; i++)
    {
        if (m_camCtrl->m_camDevice[i]->m_pstCam->bIsOpen || bLoadImageFlag[i])
        {
            dSum += m_imgProc->dResultData[iResultID];
        }
    }

    return dSum;
}

double MainWindow::AveSideResult(int iResultID)
{
    double dAve = 0;
    int iCnt = 0;

    for (unsigned int i = 1; i < BOTTOM_CAMERA; i++)
    {
        if (m_camCtrl->m_camDevice[i]->m_pstCam->bIsOpen || bLoadImageFlag[i])
        {
            dAve += m_imgProc->dResultData[iResultID];
            iCnt++;
        }
    }

    if (iCnt >= 1)
    {
        dAve /= iCnt;
    }

    return dAve;
}

void MainWindow::on_SigMonitorDisp(QVariant &qtmp)
{
    //todo show something when serial port send something back to pc control
    // just count data,show data will do in next function [on_SigLogFruitDataTbl]
    g_appleData.Clear();
    RESULTDATA tmp = qtmp.value<RESULTDATA>();
    g_appleData.dCupData[SCRE_BAD_NUM] = tmp.dCupData[SCRE_BAD_NUM];
    g_appleData.dCupData[SCRE_BAD_RATIO] = tmp.dCupData[SCRE_BAD_RATIO];//AveResult(SCRE_BAD_RATIO);
    g_appleData.dCupData[SCRE_RED_RATIO] = tmp.dCupData[SCRE_RED_RATIO];
    g_appleData.dCupData[SCRE_APL_SHAPE] = tmp.dCupData[SCRE_APL_SHAPE];// 取一个相机数据，默认为side1
    g_appleData.dCupData[SCRE_APL_DIAMETER] = tmp.dCupData[SCRE_APL_DIAMETER];
    g_appleData.dCupData[SCRE_APL_REDTYPE] = tmp.dCupData[SCRE_APL_REDTYPE];
    g_appleData.dCupData[SCRE_APL_SCARE] = tmp.dCupData[SCRE_APL_SCARE];
    /*
    *************************************************************************************
    ******************************* special counting*************************************
    *************************************************************************************
    */
    bKaraCup = g_appleData.bKaraCup = tmp.bKaraCup;
    bResultOK = tmp.bResultOk;

    if (m_bAutoSaveImg)
    {
        g_iSaveCount++;
    }
}

void MainWindow::on_SigLogFruitDataTbl()
{
    QString aResult;
    iLightError = 0;
    iCamerasError = 0;

    if (bResultOK && !bKaraCup)   //
    {
        ui->oneFrttable->setItem(0, 1, new QTableWidgetItem(QString("检测OK")));
        ui->oneFrttable->item(0, 1)->setForeground(QBrush(QColor(0, 255, 0)));
    }
    else if (bKaraCup)
    {
        ui->oneFrttable->setItem(0, 1, new QTableWidgetItem(QString("空托盘")));
        ui->oneFrttable->item(0, 1)->setForeground(QBrush(QColor(245, 245, 10)));
    }
    else
    {
        ui->oneFrttable->setItem(0, 1, new QTableWidgetItem(QString("检测NG")));
        ui->oneFrttable->item(0, 1)->setForeground(QBrush(QColor(255, 0, 0)));
    }

    ui->oneFrttable->item(0, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    for (int i = 0; i < SERIAL_EFFICTIVE_NUM_RESULT_DATA; i++)
    {
        aResult = QString::asprintf("%0.2f", g_appleData.dCupData[i]);
        ui->oneFrttable->setItem(i + 1, 1, new QTableWidgetItem(aResult));
        ui->oneFrttable->item(i + 1, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    for (int i = SERIAL_EFFICTIVE_NUM_RESULT_DATA; i < SERIAL_NUM_RESULT_DATA; i++)
    {
        aResult = QString("-");
        ui->oneFrttable->setItem(i + 1, 1, new QTableWidgetItem(aResult));
        ui->oneFrttable->item(i + 1, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    InsertTableData(m_cupIndex++);// insert into data table

    if (m_cupIndex >= CUPDATA_BUFFER)
    {
        m_cupIndex = 0;
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick)
    {
        if (watched->metaObject()->className() == QString("QLabel"))
        {
            QLabel *widget = (QLabel *) watched;

            if (widget == ui->lblReg)
            {
                QString str = QApplication::applicationName().append(".exe");
                std::string strTmp = str.toStdString();
                char *name = const_cast<char *>(strTmp.c_str());
                ShowRegModalDialogVC(name);
                int status = GetLicenseStatusVC(name);

                if (status == 0)
                {
                    m_bReg = true;
                }
                else
                {
                    QMessageBox::information(this, "提示", "软件注册失败，程序退出。");
                    this->close();
                }
            }
            else
            {
                QRect       objRect;  //显示窗口的大小
                CamDevice *pf = NULL;
                int nid = -1;

                if (widget == ui->labCh1)  //cam0
                {
                    nid = 0;
                    pf  = m_camCtrl->m_camDevice[nid];
                }
                else if (widget == ui->labCh2)    //cam1
                {
                    nid = 1;
                    pf  = m_camCtrl->m_camDevice[nid];
                }
                else if (widget == ui->labCh3)    //cam2
                {
                    nid = 2;
                    pf  = m_camCtrl->m_camDevice[nid];
                }
                else if (widget == ui->labCh4)    //cam3
                {
                    nid = 3;
                    pf  = m_camCtrl->m_camDevice[nid];
                }
                else if (widget == ui->labCh5)    //cam4
                {
                    nid = 4;
                    pf  = m_camCtrl->m_camDevice[nid];
                }
                else if (widget == ui->labCh6)    //cam5
                {
                    nid = 5;
                    pf  = m_camCtrl->m_camDevice[nid];
                }

                m_camCtrl->m_nOperateID = nid;

                if (pf != NULL)
                {
                    objRect = ui->labCh00->rect();
                    DrawImg(pf, m_hdcmain, pf->m_pstCam->pImageBuffer, nid, objRect, pf->m_pstCam->nImageWidth, pf->m_pstCam->nImageHeight);
                    objRect = ui->labCh01->rect();
                    DrawImg(pf, m_hdcsub, pf->m_pstCam->pImageBuffer, nid, objRect, pf->m_pstCam->nImageWidth, pf->m_pstCam->nImageHeight);
                    // MENU RESET
                    m_menuDownValue[0][0] = 0;
                    m_menuDownValue[0][0] = 0;
                    m_menuUpValue[0][0] = 1;
                    m_menuUpValue[1][0] = 1;
                    ui->actionDispClr3->setChecked(m_menuDownValue[0][0] >> 3 & 0X01);
                    ui->actionDisp0->setChecked(m_menuUpValue[0][0] >> 0 & 0X01);
                    ui->actionDispClr3->setChecked(m_menuDownValue[1][0] >> 3 & 0X01);
                    ui->actionDisp0->setChecked(m_menuUpValue[1][0] >> 0 & 0X01);
                }
            }
        }
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        QLabel *lbl = (QLabel *)watched;

        if (mouseEvent->button() == Qt::RightButton)
        {
            m_lblCaller = lbl;
            QString tmpstr = lbl->objectName();

            if (tmpstr.compare("labCh00") == 0)
            {
                m_menuCallerIdx = 0;
            }
            else if (tmpstr.compare("labCh01") == 0)
            {
                m_menuCallerIdx = 1;
            }
            else if (tmpstr.compare("labCh1") == 0)
            {
                m_menuCallerIdx = 2;
            }
            else if (tmpstr.compare("labCh2") == 0)
            {
                m_menuCallerIdx = 3;
            }
            else if (tmpstr.compare("labCh3") == 0)
            {
                m_menuCallerIdx = 4;
            }
            else if (tmpstr.compare("labCh4") == 0)
            {
                m_menuCallerIdx = 5;
            }
            else if (tmpstr.compare("labCh5") == 0)
            {
                m_menuCallerIdx = 6;
            }
            else if (tmpstr.compare("labCh6") == 0)
            {
                m_menuCallerIdx = 7;
            }

            SetSplitUpMenu();
            SetSplitDownMenu();
            m_Qmenu->exec(QCursor::pos());
        }
    }

    return QWidget::eventFilter(watched, event);
}

void MainWindow::initForm()
{
    this->initTableWidget();
    this->initStyle();
    this->initMenu();
    ui->tabWidget->setTabIcon(0, QIcon(QPixmap(":/image/package_yast_system.ico")));
    ui->tabWidget->setTabIcon(1, QIcon(QPixmap(":/image/package_system.ico")));
    ui->tabWidget->setTabIcon(2, QIcon(QPixmap(":/image/Sys.ico")));
    ui->tabWidget->setTabIcon(3, QIcon(QPixmap(":/image/system_monitor.ico")));
    ui->tabWidget->setTabIcon(4, QIcon(QPixmap(":/image/utilities_system_monitor.ico")));
    ui->tabWidget->setTabIcon(5, QIcon(QPixmap(":/image/system_file_manager.ico")));
    ui->tabWidget->setTabIcon(6, QIcon(QPixmap(":/image/preferences_system.ico")));
    ui->lcdMeasType->SetCursor(3, 1);
    ui->lcdMeasType->SetColorPixel(QColor(62, 183, 27));
    ui->lcdMeasType->String(QString("AUTO"));
    ui->camManuBtn->setEnabled(false);
    ui->picManuBtn->setEnabled(false);
    ui->camManuBtn_2->setEnabled(false);
    ui->lblStartNo->setEnabled(false);
    ui->editStartNo->setEnabled(false);
    QRegExp regx("[0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, ui->editStartNo);
    ui->editStartNo->setValidator(validator);
    ui->editStartNo->setText("1");
    ui->camManuBtn_2->setVisible(false);
    ui->cbxLED_ON->setButtonStyle(ImageSwitch::ButtonStyle_2);
    ui->cbxLED_ON_2->setButtonStyle(ImageSwitch::ButtonStyle_2);
    ui->cbxIMGShow_OFF->setButtonStyle(ImageSwitch::ButtonStyle_2);
    ui->cbxIMGShow_OFF->setChecked(false);
    //    displaySplitUpMenu(MENU_DISP_0);
    //    displaySplitDownMenu(MENU_DISPCLR_0);
}

void MainWindow::initTableWidget()
{
    // 履历界面
    //设置列数和列宽
    int width = QGuiApplication::primaryScreen()->availableGeometry().width() - 120;
    ui->dataWidget->setColumnCount(14);
    ui->dataWidget->setColumnWidth(0, width * 0.065);
    ui->dataWidget->setColumnWidth(1, width * 0.16);
    ui->dataWidget->setColumnWidth(2, width * 0.065);
    ui->dataWidget->setColumnWidth(3, width * 0.065);
    ui->dataWidget->setColumnWidth(4, width * 0.065);
    ui->dataWidget->setColumnWidth(5, width * 0.065);
    ui->dataWidget->setColumnWidth(6, width * 0.065);
    ui->dataWidget->setColumnWidth(7, width * 0.065);
    ui->dataWidget->setColumnWidth(8, width * 0.065);
    ui->dataWidget->setColumnWidth(9, width * 0.065);
    ui->dataWidget->setColumnWidth(10, width * 0.065);
    ui->dataWidget->setColumnWidth(11, width * 0.065);
    ui->dataWidget->setColumnWidth(12, width * 0.065);
    ui->dataWidget->setColumnWidth(13, width * 0.065);
    ui->dataWidget->verticalHeader()->setDefaultSectionSize(25);
    QStringList headText;
    headText << "编号" << "时间" << "品种名称" << "检测状态" ;

    for (QString varb : g_sResultName)
    {
        headText << varb;
    }

    //    headText << "-";//凑个数
    ui->dataWidget->setHorizontalHeaderLabels(headText);
    ui->dataWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->dataWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->dataWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->dataWidget->setAlternatingRowColors(true);
    ui->dataWidget->verticalHeader()->setVisible(false);
    ui->dataWidget->horizontalHeader()->setStretchLastSection(true);
    //设置行高
    ui->dataWidget->setRowCount(TABLE_MAX_ROW_COUNT);
    //主界面单个水果显示履历
    // don't set anything, some setting has been changed in QT Designer.
    ui->oneFrttable->setColumnCount(2);
    width = ui->oneFrttable->size().width();
    ui->oneFrttable->setColumnWidth(0, width * 0.6);
    ui->oneFrttable->setColumnWidth(1, width * 0.3);
    headText.clear();
    headText << "名称" << "测量值";
    ui->oneFrttable->setHorizontalHeaderLabels(headText);
    ui->oneFrttable->horizontalHeader()->setStretchLastSection(true);
    QTableWidgetItem *itemDeviceID = new QTableWidgetItem(QString("检测状态"));
    ui->oneFrttable->setItem(0, 0, itemDeviceID);
    ui->oneFrttable->item(0, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    //ui->oneFrttable->item(0, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    for (int i = 0; i < SERIAL_NUM_RESULT_DATA; i++)
    {
        itemDeviceID = new QTableWidgetItem(g_sResultName[i]);
        ui->oneFrttable->setItem(i + 1, 0, itemDeviceID);
        ui->oneFrttable->item(i + 1, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        //ui->oneFrttable->item(i + 1, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    for (int i = SERIAL_EFFICTIVE_NUM_RESULT_DATA; i < SERIAL_NUM_RESULT_DATA; i++)
    {
        ui->oneFrttable->setItem(i + 1, 1, new QTableWidgetItem(QString("-")));
        ui->oneFrttable->item(i + 1, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    // last row
    ui->oneFrttable->setItem(SERIAL_NUM_RESULT_DATA + 1, 0, new QTableWidgetItem("-"));
    ui->oneFrttable->setItem(SERIAL_NUM_RESULT_DATA + 1, 1, new QTableWidgetItem("-"));
    ui->oneFrttable->item(SERIAL_NUM_RESULT_DATA + 1, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->oneFrttable->item(SERIAL_NUM_RESULT_DATA + 1, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->oneFrttable->setItem(SERIAL_NUM_RESULT_DATA + 2, 0, new QTableWidgetItem("-"));
    ui->oneFrttable->setItem(SERIAL_NUM_RESULT_DATA + 2, 1, new QTableWidgetItem("-"));
    ui->oneFrttable->item(SERIAL_NUM_RESULT_DATA + 2, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->oneFrttable->item(SERIAL_NUM_RESULT_DATA + 2, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void MainWindow::InsertTableData(int idx)
{
    int icurcnt = m_dataIndex++;
    QString lblstr;

    if (bResultOK && !bKaraCup)   //
    {
        lblstr = QString("检测OK");
    }
    else if (bKaraCup)
    {
        lblstr = QString("空托盘");
    }
    else
    {
        lblstr = QString("检测NG");
    }

    if (icurcnt >= TABLE_MAX_ROW_COUNT)
    {
        ui->dataWidget->removeRow(0);
        ui->dataWidget->insertRow(TABLE_MAX_ROW_COUNT - 1);
        ui->dataWidget->setRowHeight(TABLE_MAX_ROW_COUNT - 1, 24);
        QTableWidgetItem *itemDeviceID = new QTableWidgetItem(QString::number(m_dataIndex));
        ui->dataWidget->setItem(TABLE_MAX_ROW_COUNT - 1, 0, itemDeviceID);
        QTableWidgetItem *itemDeviceTime = new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
        ui->dataWidget->setItem(TABLE_MAX_ROW_COUNT - 1, 1, itemDeviceTime);
        QTableWidgetItem *itemDeviceName = new QTableWidgetItem(m_sySetting.aHinName);
        ui->dataWidget->setItem(TABLE_MAX_ROW_COUNT - 1, 2, itemDeviceName);
        itemDeviceName = new QTableWidgetItem(lblstr);
        ui->dataWidget->setItem(TABLE_MAX_ROW_COUNT - 1, 3, itemDeviceName);

        for (int j = 0; j < SERIAL_EFFICTIVE_NUM_RESULT_DATA; j++)
        {
            QTableWidgetItem *itemDeviceComm = new QTableWidgetItem(QString::number(g_appleData.dCupData[j], 'f', 2));
            ui->dataWidget->setItem(TABLE_MAX_ROW_COUNT - 1, j + 4, itemDeviceComm);
        }

        for (int j = SERIAL_EFFICTIVE_NUM_RESULT_DATA; j < SERIAL_NUM_RESULT_DATA; j++)
        {
            QTableWidgetItem *itemDeviceComm = new QTableWidgetItem(QString("-"));
            ui->dataWidget->setItem(TABLE_MAX_ROW_COUNT - 1, j + 4, itemDeviceComm);
        }

        ui->dataWidget->setItem(TABLE_MAX_ROW_COUNT - 1, SERIAL_NUM_RESULT_DATA + 4, new QTableWidgetItem("-"));
    }
    else
    {
        ui->dataWidget->setRowHeight(icurcnt, 24);
        QTableWidgetItem *itemDeviceID = new QTableWidgetItem(QString::number(m_dataIndex));
        ui->dataWidget->setItem(icurcnt, 0, itemDeviceID);
        QTableWidgetItem *itemDeviceTime = new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
        ui->dataWidget->setItem(icurcnt, 1, itemDeviceTime);
        QTableWidgetItem *itemDeviceName = new QTableWidgetItem(m_sySetting.aHinName);
        ui->dataWidget->setItem(icurcnt, 2, itemDeviceName);
        itemDeviceName = new QTableWidgetItem(lblstr);
        ui->dataWidget->setItem(icurcnt, 3, itemDeviceName);

        for (int j = 0; j < SERIAL_EFFICTIVE_NUM_RESULT_DATA; j++)
        {
            QTableWidgetItem *itemDeviceComm = new QTableWidgetItem(QString::number(g_appleData.dCupData[j], 'f', 2));
            ui->dataWidget->setItem(icurcnt, j + 4, itemDeviceComm);
        }

        for (int j = SERIAL_EFFICTIVE_NUM_RESULT_DATA; j < SERIAL_NUM_RESULT_DATA; j++)
        {
            QTableWidgetItem *itemDeviceComm = new QTableWidgetItem(QString("-"));
            ui->dataWidget->setItem(icurcnt, j + 4, itemDeviceComm);
        }

        ui->dataWidget->setItem(icurcnt, SERIAL_NUM_RESULT_DATA + 4, new QTableWidgetItem("-"));
    }
}

void MainWindow::ShowScanTypeSlot(int type)
{
    whirlbutton *btn = (whirlbutton *) sender();

    if (btn->objectName().compare("camMeasBtn") == 0)
    {
        btn = (whirlbutton *)ui->camMeasBtn_2;
    }
    else
    {
        btn = (whirlbutton *)ui->camMeasBtn;
    }

    ui->lcdMeasType->Clear();
    ui->lcdMeasType->SetCursor(3, 1);
    killTimer(m_nTimerID500);//kill 500ms timer
    m_nTimerID500 = 0;
    ui->camManuBtn->setEnabled(false);
    ui->camManuBtn_2->setEnabled(false);
    ui->picManuBtn->setEnabled(false);
    ui->lblStartNo->setEnabled(false);
    ui->editStartNo->setEnabled(false);
    m_bmanual = false;
    int flashtimer = 500;

    switch (type)
    {
        case SCAN_TYPE_AUTO:
            ui->lcdMeasType->SetColorPixel(QColor(255, 26, 26));
            ui->lcdMeasType->String(QString("TEST"));
            bFlash[0] = true;
            bFlash[1] = true;
            flashtimer = 1000 / m_sySetting.iFpsTest;
            m_nTimerID500 = startTimer(flashtimer);// start flash
            btn->setDegree(ANGLE_30);
            m_bmanual = true;
            break;

        case SCAN_TYPE_MANUAL:
            ui->lcdMeasType->SetColorPixel(QColor(245, 245, 10));
            ui->lcdMeasType->String(QString("MANUAL"));
            ui->camManuBtn->setEnabled(true);
            ui->camManuBtn_2->setEnabled(true);
            ui->picManuBtn->setEnabled(true);
            ui->lblStartNo->setEnabled(true);
            ui->editStartNo->setEnabled(true);
            btn->setDegree(ANGLE_210);
            m_bmanual = true;
            break;

        case SCAN_TYPE_OFF:
        default:
            ui->lcdMeasType->SetColorPixel(QColor(62, 183, 27));
            ui->lcdMeasType->String(QString("AUTO"));
            btn->setDegree(ANGLE_90);
            break;
    }
}

//void MainWindow::ShowScanTypeSlot2(int type)
//{
//    ui->lcdMeasType->Clear();
//    ui->lcdMeasType->SetCursor(3, 1);
//    ui->camManuBtn_2->setEnabled(false);
//    ui->picManuBtn->setEnabled(false);

//    switch (type) {
//    case SCAN_TYPE_AUTO:
//        ui->lcdMeasType->SetColorPixel(QColor(255, 26, 26));
//        ui->lcdMeasType->String(QString("TEST"));
//        bFlash[1] = true;
//        break;
//    case SCAN_TYPE_MANUAL:
//        ui->lcdMeasType->SetColorPixel(QColor(245, 245, 10));
//        ui->lcdMeasType->String(QString("MANUAL"));

//        ui->camManuBtn_2->setEnabled(true);
//        ui->picManuBtn->setEnabled(true);
//        break;
//    case SCAN_TYPE_OFF:
//    default:
//        ui->lcdMeasType->SetColorPixel(QColor(62, 183, 27));
//        ui->lcdMeasType->String(QString("AUTO"));
//        break;
//    }
//}

void MainWindow::initMenu()
{
    m_Qmenu = new QMenu(this);
    m_Qmenu->addAction(ui->actionDisp0);
    //    m_Qmenu->addAction(ui->actionDisp1);
    //    m_Qmenu->addAction(ui->actionDisp2);
    m_Qmenu->addAction(ui->actionDisp3);
    //    m_Qmenu->addAction(ui->actionDisp4);
    //    m_Qmenu->addAction(ui->actionDisp5);
    //    m_Qmenu->addAction(ui->actionDisp6);
    //    m_Qmenu->addAction(ui->actionDisp7);
    m_Qmenu->addSeparator();
    //    m_Qmenu->addAction(ui->actionDispClr0);
    m_Qmenu->addAction(ui->actionDispClr1);
    m_Qmenu->addAction(ui->actionDispClr2);
    m_Qmenu->addAction(ui->actionDispClr3);
    //    m_Qmenu->addAction(ui->actionDispClr4);
}

void MainWindow::displaySplitDownMenu(int value)
{
    // Get combination value
    if ((m_menuDownValue[m_menuCallerIdx][0] & value) == value)
    {
        return;
    }

    m_menuDownValue[m_menuCallerIdx][0] = (m_menuDownValue[m_menuCallerIdx][0] ^ value) & value;
    SetSplitDownMenu();
    m_menuUpValue[m_menuCallerIdx][0] = 0;
}

void MainWindow::SetSplitDownMenu()
{
    // get each bool from last bit
    ui->actionDispClr0->setChecked(m_menuDownValue[m_menuCallerIdx][0] >> 0 & 0X01);
    ui->actionDispClr1->setChecked(m_menuDownValue[m_menuCallerIdx][0] >> 1 & 0X01);
    ui->actionDispClr2->setChecked(m_menuDownValue[m_menuCallerIdx][0] >> 2 & 0X01);
    ui->actionDispClr3->setChecked(m_menuDownValue[m_menuCallerIdx][0] >> 3 & 0X01);
    ui->actionDispClr4->setChecked(m_menuDownValue[m_menuCallerIdx][0] >> 4 & 0X01);
}

void MainWindow::SetSplitUpMenu()
{
    // get each bool from last bit
    ui->actionDisp0->setChecked(m_menuUpValue[m_menuCallerIdx][0] >> 0 & 0X01);
    ui->actionDisp1->setChecked(m_menuUpValue[m_menuCallerIdx][0] >> 1 & 0X01);
    ui->actionDisp2->setChecked(m_menuUpValue[m_menuCallerIdx][0] >> 2 & 0X01);
    ui->actionDisp3->setChecked(m_menuUpValue[m_menuCallerIdx][0] >> 3 & 0X01);
    ui->actionDisp4->setChecked(m_menuUpValue[m_menuCallerIdx][0] >> 4 & 0X01);
    ui->actionDisp5->setChecked(m_menuUpValue[m_menuCallerIdx][0] >> 5 & 0X01);
    ui->actionDisp6->setChecked(m_menuUpValue[m_menuCallerIdx][0] >> 6 & 0X01);
    ui->actionDisp7->setChecked(m_menuUpValue[m_menuCallerIdx][0] >> 7 & 0X01);
}

void MainWindow::displaySplitUpMenu(int value)
{
    // Get combination value
    if ((m_menuUpValue[m_menuCallerIdx][0] & value) == value)
    {
        return;
    }

    m_menuUpValue[m_menuCallerIdx][0] = (m_menuUpValue[m_menuCallerIdx][0] ^ value) & value;
    SetSplitUpMenu();
    m_menuDownValue[m_menuCallerIdx][0] = 0;
}

void MainWindow::on_actionDisp0_triggered(bool)
{
    //原图
    displaySplitUpMenu(MENU_DISP_0);
    HDC hdc;
    int nid = m_camCtrl->m_nOperateID;

    if (m_menuCallerIdx == 0)
    {
        hdc = m_hdcmain;
    }
    else if (m_menuCallerIdx == 1)
    {
        hdc = m_hdcsub;
    }
    else
    {
        hdc = m_hdc[m_menuCallerIdx - 2];
        nid = m_menuCallerIdx - 2;
    }

    if (nid >= 0 && m_imgProc->m_imgMeaure[nid]->dispImage[_SOURCE_IMAGE])
    {
        QRect objRect = m_lblCaller->rect();

        if (m_camCtrl->m_camDevice[nid] != NULL)
        {
            DrawImg(m_camCtrl->m_camDevice[nid], hdc, (BYTE *)m_imgProc->m_imgMeaure[nid]->dispImage[_SOURCE_IMAGE]->imageData, nid, objRect,
                    m_camCtrl->m_camDevice[nid]->m_pstCam->nImageWidth, m_camCtrl->m_camDevice[nid]->m_pstCam->nImageHeight);
        }
    }
}

void MainWindow::on_actionDisp1_triggered(bool)
{
    displaySplitUpMenu(MENU_DISP_1);
}

void MainWindow::on_actionDisp2_triggered(bool)
{
    displaySplitUpMenu(MENU_DISP_2);
}

void MainWindow::on_actionDisp3_triggered(bool)
{
    // 二值化
    displaySplitUpMenu(MENU_DISP_3);
    HDC hdc;
    int nid = m_camCtrl->m_nOperateID;

    if (m_menuCallerIdx == 0)
    {
        hdc = m_hdcmain;
    }
    else if (m_menuCallerIdx == 1)
    {
        hdc = m_hdcsub;
    }
    else
    {
        hdc = m_hdc[m_menuCallerIdx - 2];
        nid = m_menuCallerIdx - 2;
    }

    if (nid >= 0 && m_imgProc->m_imgMeaure[nid]->dispImage[_BINARY_IMAGE])
    {
        QRect objRect = m_lblCaller->rect();

        if (m_camCtrl->m_camDevice[nid] != NULL)
        {
            DrawImg(m_camCtrl->m_camDevice[nid], hdc, (BYTE *)m_imgProc->m_imgMeaure[nid]->dispImage[_BINARY_IMAGE]->imageData, nid, objRect,
                    m_camCtrl->m_camDevice[nid]->m_pstCam->nImageWidth, m_camCtrl->m_camDevice[nid]->m_pstCam->nImageHeight, false);
        }
    }
}

void MainWindow::on_actionDisp4_triggered(bool)
{
    displaySplitUpMenu(MENU_DISP_4);
}

void MainWindow::on_actionDisp5_triggered(bool)
{
    displaySplitUpMenu(MENU_DISP_5);
}

void MainWindow::on_actionDisp6_triggered(bool)
{
    displaySplitUpMenu(MENU_DISP_6);
}

void MainWindow::on_actionDisp7_triggered(bool)
{
    displaySplitUpMenu(MENU_DISP_7);
}

void MainWindow::on_actionDispClr4_triggered(bool)
{
    displaySplitDownMenu(MENU_DISPCLR_4);
}

void MainWindow::on_actionDispClr3_triggered(bool)
{
    displaySplitDownMenu(MENU_DISPCLR_3);
    // B图
    HDC hdc;
    int nid = m_camCtrl->m_nOperateID;

    if (m_menuCallerIdx == 0)
    {
        hdc = m_hdcmain;
    }
    else if (m_menuCallerIdx == 1)
    {
        hdc = m_hdcsub;
    }
    else
    {
        hdc = m_hdc[m_menuCallerIdx - 2];
        nid = m_menuCallerIdx - 2;
    }

    if (nid >= 0 && m_imgProc->m_imgMeaure[nid]->wDispImage[_WIDE_IMAGE_B])
    {
        QRect objRect = m_lblCaller->rect();

        if (m_camCtrl->m_camDevice[nid] != NULL)
        {
            DrawImg(m_camCtrl->m_camDevice[nid], hdc, (BYTE *)m_imgProc->m_imgMeaure[nid]->wDispImage[_WIDE_IMAGE_B]->imageData, nid, objRect,
                    m_camCtrl->m_camDevice[nid]->m_pstCam->nImageWidth, m_camCtrl->m_camDevice[nid]->m_pstCam->nImageHeight);
        }
    }
}

void MainWindow::on_actionDispClr2_triggered(bool)
{
    displaySplitDownMenu(MENU_DISPCLR_2);
    // G图
    HDC hdc;
    int nid = m_camCtrl->m_nOperateID;

    if (m_menuCallerIdx == 0)
    {
        hdc = m_hdcmain;
    }
    else if (m_menuCallerIdx == 1)
    {
        hdc = m_hdcsub;
    }
    else
    {
        hdc = m_hdc[m_menuCallerIdx - 2];
        nid = m_menuCallerIdx - 2;
    }

    if (nid >= 0 && m_imgProc->m_imgMeaure[nid]->wDispImage[_W_SHIMA_IMAGE_G])
    {
        QRect objRect = m_lblCaller->rect();

        if (m_camCtrl->m_camDevice[nid] != NULL)
        {
            DrawImg(m_camCtrl->m_camDevice[nid], hdc, (BYTE *)m_imgProc->m_imgMeaure[nid]->wDispImage[_W_SHIMA_IMAGE_G]->imageData, nid, objRect,
                    m_camCtrl->m_camDevice[nid]->m_pstCam->nImageWidth, m_camCtrl->m_camDevice[nid]->m_pstCam->nImageHeight);
        }
    }
}

void MainWindow::on_actionDispClr1_triggered(bool)
{
    // R图
    displaySplitDownMenu(MENU_DISPCLR_1);
    HDC hdc;
    int nid = m_camCtrl->m_nOperateID;

    if (m_menuCallerIdx == 0)
    {
        hdc = m_hdcmain;
    }
    else if (m_menuCallerIdx == 1)
    {
        hdc = m_hdcsub;
    }
    else
    {
        hdc = m_hdc[m_menuCallerIdx - 2];
        nid = m_menuCallerIdx - 2;
    }

    if (nid >= 0 && m_imgProc->m_imgMeaure[nid]->wDispImage[_W_BINARY_IMAGE_R])
    {
        QRect objRect = m_lblCaller->rect();

        if (m_camCtrl->m_camDevice[nid] != NULL)
        {
            DrawImg(m_camCtrl->m_camDevice[nid], hdc, (BYTE *)m_imgProc->m_imgMeaure[nid]->wDispImage[_W_BINARY_IMAGE_R]->imageData, nid, objRect,
                    m_camCtrl->m_camDevice[nid]->m_pstCam->nImageWidth, m_camCtrl->m_camDevice[nid]->m_pstCam->nImageHeight);
        }
    }
}

void MainWindow::on_actionDispClr0_triggered(bool)
{
    displaySplitDownMenu(MENU_DISPCLR_0);
}

void MainWindow::on_camManuBtn_clicked()
{
    // capture image
    // 如果当触发回调正在执行的过程中，再次点击触发按键后，此次点击会被屏蔽掉
    if (!m_bIsTrigValid)
    {
        return;
    }
    else
    {
        m_bIsTrigValid = false;
    }

    uint32_t  i = 0;
    //    for (i = 0; i < CAMERANUM - 1; i++) {
    //        bLoadImageFlag[i] = false;
    //        g_stopWatch[i].Start();
    //        if (m_camSettings[i]->bTriggerMode) {
    //            m_camCtrl->ManuCtlByCam(i);
    //        }
    //    }
    ResetEvent(g_KaraCupHnd);

    for (i = 0; i < CAMERANUM; i++)
    {
        bLoadImageFlag[i] = false;
        g_stopWatch.Start();

        if (m_camSettings[i]->bTriggerMode)
        {
            m_camCtrl->ManuCtlByCam(i, g_btrimode);
        }
    }
}

/*
暂时不需要-t，光电未安装
*/
void MainWindow::on_camManuBtn_2_clicked()
{
    // capture image
    // 如果当触发回调正在执行的过程中，再次点击触发按键后，此次点击会被屏蔽掉
    if (!m_bIsTrigValid2)
    {
        return;
    }
    else
    {
        m_bIsTrigValid2 = false;
    }

#ifndef CAM_NUM_ONLY_ONE
    bLoadImageFlag[BOTTOM_CAMERA] = false;
    g_stopWatch.Start();

    if (m_camSettings[BOTTOM_CAMERA]->bTriggerMode)
    {
        m_camCtrl->ManuCtlByCam(BOTTOM_CAMERA);
    }

#endif
}

void MainWindow::ImageLoad()
{
    // LOAD img
    QString file_name;
    bool loadfail = false;

    for (int iCameraIdx = 0; iCameraIdx < CAMERANUM; iCameraIdx++)
    {
        file_name = QString("%1\\%2_%3_%4.bmp").arg(m_strFilePath).arg(IMG_LOAD_NAME).arg(g_iLoadCount).arg(iCameraIdx);

        if (iCameraIdx == CAMERANUM - 1)
        {
            file_name = QString("%1\\%2_%3_%4.bmp").arg(m_strFilePath).arg(IMG_LOAD_NAME).arg(g_iLoadCount/* + m_sySetting.idistance*/).arg(iCameraIdx);
        }

        Mat tMat = imread(file_name.toStdString());

        if (tMat.empty())
        {
            loadfail = true;
            break;
        }

        //        IplImage tmpimg = cvIplImage(tMat);
        //        memcpy(m_imgProc[iCameraIdx]->m_imgMeaure->loadImage->imageData, tMat.data, m_imgProc[iCameraIdx]->m_imgMeaure->loadImage->imageSize);
        bLoadImageFlag[iCameraIdx] = true;
        emit m_camCtrl->SetImageData(iCameraIdx, tMat.data);
        SetEvent(g_camComplHnds[iCameraIdx]);
    }

    if (loadfail)
    {
        QMessageBox::warning(this, "Warning", QString("<font size='10' color='black'>相机图片%1加载失败.</font>").arg(g_iLoadCount));
    }

    g_iLoadCount++;
}

void MainWindow::on_picManuBtn_clicked()
{
    g_stopWatch.Start();
    g_iLoadCount = ui->editStartNo->text().toInt();
    ImageLoad();
    ui->editStartNo->setText(QString::number(g_iLoadCount));
}

void MainWindow::on_updateSepLine(int nid, int val)
{
    m_imgSettings[nid]->iBinarySepLine = val;
}

void MainWindow::on_updateTopThr(int nid, int val)
{
    m_imgSettings[nid]->iBinaryTopThr = val;
}

void MainWindow::on_updateBotThr(int nid, int val)
{
    m_imgSettings[nid]->iBinaryBotThr = val;
}

void MainWindow::on_updateStrpThreShold(int nid, int val)
{
    m_imgSettings[nid]->iStripeThreshold = val;
}

void MainWindow::on_updateStrpAngleWt(int nid, double val)
{
    m_imgSettings[nid]->dStripeAngleWt = val;
}

void MainWindow::on_updateCentreLinepAngleWt(int nid, double val)
{
    m_imgSettings[nid]->dCenterLineAngleWt = val;
}

void MainWindow::on_updateGHosei(int nid, int val)
{
    m_imgSettings[nid]->iGHosei = val;
}

void MainWindow::on_updateHeightHosei(int nid, int val)
{
    m_imgSettings[nid]->iHeightHosei = val;
}

void MainWindow::on_updateShimaBlockSize(int nid, int val)
{
    m_imgSettings[nid]->iShimaBlockSize = val;
}

void MainWindow::on_updateShimaThreshold(int nid, int val)
{
    m_imgSettings[nid]->iShimaThreshold = val;
}

void MainWindow::on_updateShimaMinLen(int nid, int val)
{
    m_imgSettings[nid]->iShimaMinLen = val;
}

void MainWindow::on_updateShimaMinArea(int nid, int val)
{
    m_imgSettings[nid]->iShimaMinArea = val;
}

void MainWindow::on_updateVolumeHoseiA(int nid, double val)
{
    m_imgSettings[nid]->dVolumeHoseiA = val;
}

void MainWindow::on_updateVolumeHoseiB(int nid, double val)
{
    m_imgSettings[nid]->dVolumeHoseiB = val;
}

void MainWindow::on_updateBinaryMaxB(int nid, int val)
{
    m_imgSettings[nid]->iBinaryMaxB = val;
}

void MainWindow::on_updateBinaryMinG(int nid, int val)
{
    m_imgSettings[nid]->iBinaryMinG = val;
}

void MainWindow::on_updateBinaryThreshold(int nid, int val)
{
    m_imgSettings[nid]->iBinaryThreshold = val;
}

void MainWindow::on_updateErodeDilateNum(int nid, int val)
{
    m_imgSettings[nid]->iErodeDilateNum = val;
}

void MainWindow::on_updateBinaryShift(int nid, int val)
{
    m_imgSettings[nid]->iBinaryShift = val;
}

void MainWindow::on_updateSealGHoseiX(int nid, int val)
{
    m_imgSettings[nid]->iSealGHoseiX = val;
}

void MainWindow::on_updateSealGHoseiY(int nid, int val)
{
    m_imgSettings[nid]->iSealGHoseiY = val;
}

void MainWindow::on_updateSealWidth(int nid, int val)
{
    m_imgSettings[nid]->iSealWidth = val;
}

void MainWindow::on_updateSealHeight(int nid, int val)
{
    m_imgSettings[nid]->iSealHeight = val;
}

void MainWindow::on_updateSealMinSize(int nid, int val)
{
    m_imgSettings[nid]->iSealMinSize = val;
}

void MainWindow::on_updateExpoTime(int nid, int val)
{
    m_imgSettings[nid]->iExpoTime = val;

    // update cam hardware set
    if (m_camCtrl->m_camDevice[nid] != NULL && m_camCtrl->m_camDevice[nid]->m_pstCam->bIsOpen)
    {
        GXSetFloat(m_camCtrl->m_camDevice[nid]->m_hDevice, GX_FLOAT_EXPOSURE_TIME, (double)val);
    }
}

void MainWindow::on_updateName(int itab, int nid, const QString &val)
{
    m_imgSettings[itab]->colorSetting[nid].aName = val;
}

void MainWindow::on_updateRed(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iRed = val;
}

void MainWindow::on_updateGreen(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iGreen = val;
}

void MainWindow::on_updateHanni(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iHanni = val;
}

void MainWindow::on_updateWmin(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iWmin = val;
}

void MainWindow::on_updateWmax(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iWmax = val;
}

void MainWindow::updateRG_A1_Slot(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iRG_A1 = val;
}

void MainWindow::updateRG_A2_Slot(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iRG_A2 = val;
}

void MainWindow::updateRG_K_Slot(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iRG_K = val;
}

void MainWindow::updateRG_Rmin_Slot(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iRG_Rmin = val;
}

void MainWindow::on_updateDispR(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iDispR = val;
}

void MainWindow::on_updateDispG(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iDispG = val;
}

void MainWindow::on_updateDispB(int itab, int nid, int val)
{
    m_imgSettings[itab]->colorSetting[nid].iDispB = val;
}

void MainWindow::on_updateUse(int itab, int nid, bool val)
{
    m_imgSettings[itab]->colorSetting[nid].bUse = val;
}

void MainWindow::on_camupdatewhiteBalnChk(int nid, bool val)
{
    m_camSettings[nid]->bAreaWhiteBalance = val;
}

void MainWindow::on_camupdateLeanChk(int nid, bool val)
{
    m_camSettings[nid]->bRotate = val;
}

void MainWindow::on_camupdateWidth(int nid, int val)
{
    m_camSettings[nid]->iWidth = val;
}

void MainWindow::on_camupdateHeight(int nid, int val)
{
    m_camSettings[nid]->iHeight = val;
}

void MainWindow::on_camupdateGain(int nid, int val)
{
    if (m_camSettings[nid]->iGain == val)
    {
        return;
    }

    m_camSettings[nid]->iGain = val;
    m_camSettings[nid]->dR_Gain = 0;
    m_camSettings[nid]->dG_Gain = 0;
    m_camSettings[nid]->dB_Gain = 0;

    if (m_camCtrl->m_camDevice[nid] != NULL && m_camCtrl->m_camDevice[nid]->m_pstCam->bIsOpen)
    {
        GXSetEnum(m_camCtrl->m_camDevice[nid]->m_hDevice, GX_ENUM_GAIN_SELECTOR, GX_GAIN_SELECTOR_ALL);
        GXSetFloat(m_camCtrl->m_camDevice[nid]->m_hDevice, GX_FLOAT_GAIN, val);
    }

    emit SigUpdateGainValue(val, 0, 0, 0, nid);
}

void MainWindow::camupdateR_Gain_Slot(int nid, double val, double maxval)
{
    Q_UNUSED(maxval)

    if (qFuzzyCompare(m_camSettings[nid]->dR_Gain, (double)val))
    {
        return;
    }

    m_camSettings[nid]->dR_Gain = (double)val;
    m_camSettings[nid]->dG_Gain = 0;
    m_camSettings[nid]->iGain = 0;
    m_camSettings[nid]->dB_Gain = 0;

    if (m_camCtrl->m_camDevice[nid] != NULL && m_camCtrl->m_camDevice[nid]->m_pstCam->bIsOpen)
    {
        GXSetEnum(m_camCtrl->m_camDevice[nid]->m_hDevice, GX_ENUM_GAIN_SELECTOR, GX_GAIN_SELECTOR_RED);
        GXSetFloat(m_camCtrl->m_camDevice[nid]->m_hDevice, GX_FLOAT_GAIN, m_camSettings[nid]->dR_Gain);
    }

    emit SigUpdateGainValue(0, val, 0, 0, nid);
}

void MainWindow::camupdateG_Gain_Slot(int nid, double val, double maxval)
{
    Q_UNUSED(maxval)

    if (qFuzzyCompare(m_camSettings[nid]->dG_Gain, (double)val))
    {
        return;
    }

    m_camSettings[nid]->dG_Gain = (double)val;
    m_camSettings[nid]->dR_Gain = 0;
    m_camSettings[nid]->iGain = 0;
    m_camSettings[nid]->dB_Gain = 0;

    if (m_camCtrl->m_camDevice[nid] != NULL && m_camCtrl->m_camDevice[nid]->m_pstCam->bIsOpen)
    {
        GXSetEnum(m_camCtrl->m_camDevice[nid]->m_hDevice, GX_ENUM_GAIN_SELECTOR, GX_GAIN_SELECTOR_GREEN);
        GXSetFloat(m_camCtrl->m_camDevice[nid]->m_hDevice, GX_FLOAT_GAIN, m_camSettings[nid]->dG_Gain);
    }

    emit SigUpdateGainValue(0, 0, val, 0, nid);
}

void MainWindow::camupdateB_Gain_Slot(int nid, double val, double maxval)
{
    Q_UNUSED(maxval)

    if (qFuzzyCompare(m_camSettings[nid]->dB_Gain, (double)val))
    {
        return;
    }

    m_camSettings[nid]->dB_Gain = (double)val;
    m_camSettings[nid]->iGain = 0;
    m_camSettings[nid]->dR_Gain = 0;
    m_camSettings[nid]->dG_Gain = 0;

    if (m_camCtrl->m_camDevice[nid] != NULL && m_camCtrl->m_camDevice[nid]->m_pstCam->bIsOpen)
    {
        GXSetEnum(m_camCtrl->m_camDevice[nid]->m_hDevice, GX_ENUM_GAIN_SELECTOR, GX_GAIN_SELECTOR_BLUE);
        GXSetFloat(m_camCtrl->m_camDevice[nid]->m_hDevice, GX_FLOAT_GAIN, m_camSettings[nid]->dB_Gain);
    }

    emit SigUpdateGainValue(0, 0, 0, val, nid);
}

void MainWindow::on_camupdateTriggerSource(int nid, int val)
{
    g_btrimode = false;
    ui->cbxLED_ON->setChecked(false);
    emit ui->cbxLED_ON->updateChecked(false);//关灯

    switch (val)
    {
        case 0: // Continuous
            m_camSettings[nid]->bTriggerMode = 0;
            m_camSettings[nid]->iTriggerSource = 0;
            ui->cbxLED_ON->setChecked(true);
            emit ui->cbxLED_ON->updateChecked(true);//开灯
            g_btrimode = true;
            break;

        case 1: // 软触发
            m_camSettings[nid]->bTriggerMode = 1;
            m_camSettings[nid]->iTriggerSource = 0;
            break;

        case 2: // 触发源0
            m_camSettings[nid]->bTriggerMode = 1;
            m_camSettings[nid]->iTriggerSource = 1;
            break;

        case 3: // 触发源1
            m_camSettings[nid]->bTriggerMode = 1;
            m_camSettings[nid]->iTriggerSource = 2;
            break;

        case 4: // 触发源2
            m_camSettings[nid]->bTriggerMode = 1;
            m_camSettings[nid]->iTriggerSource = 3;
            break;

        case 5: // 触发源3
            m_camSettings[nid]->bTriggerMode = 1;
            m_camSettings[nid]->iTriggerSource = 4;
            break;
    }

    // update hardware ctrl
    m_camCtrl->TriggerCtrl(nid, m_camSettings[nid]->bTriggerMode, m_camSettings[nid]->iTriggerSource);
}

void MainWindow::on_camupdateLeft(int nid, int val)
{
    m_camSettings[nid]->iLeft = val;
}

void MainWindow::on_camupdateRight(int nid, int val)
{
    m_camSettings[nid]->iRight = val;
}

void MainWindow::on_camupdateTop(int nid, int val)
{
    m_camSettings[nid]->iTop = val;
}

void MainWindow::on_camupdateBottom(int nid, int val)
{
    m_camSettings[nid]->iBottom = val;
}

void MainWindow::on_camupdatePixSize(int nid, double val)
{
    m_camSettings[nid]->dPixSize = val;
}

void MainWindow::on_camupdateMedianFilter(int nid, int val)
{
    m_camSettings[nid]->iMedianFilter = val;
}

void MainWindow::on_camupdateKaraThr(int nid, double val)
{
    m_camSettings[nid]->dKaraThr = val;
}

void MainWindow::on_camupdateMinLabelingSize(int nid, int val)
{
    m_camSettings[nid]->iMinLabelingSize = val;
}

void MainWindow::on_camupdateTanshuku(int nid, double val)
{
    m_camSettings[nid]->dTanshuku = val;
}

void MainWindow::on_camupdateMinValue(int nid, int val)
{
    m_camSettings[nid]->iMinValue = val;
}

void MainWindow::on_camupdateRotateMode(int nid, bool val)
{
    m_camSettings[nid]->iRotateMode = val;
}

void MainWindow::on_camupdateAutowhiteBaln(int nid)
{
    Q_UNUSED(nid);
    //todo
}

void MainWindow::on_camAutowhiteBaln(int nid)
{
    m_camCtrl->CamWhiteBalance(nid);
}

void MainWindow::on_spinBxDistance_valueChanged(int arg1)
{
    m_sySetting.idistance = arg1;
}

void MainWindow::on_CombIOIndexValuedChanged(int index)
{
    m_sySetting.iIOCTLIdx = index;
    m_frmio->InitDevice();// reinit io
    m_frmio->SetTimerRunStop(true);
}

void MainWindow::on_ebxRsPort_valueChanged(int arg1)
{
    if (m_sySetting.iRsPort == arg1)
    {
        return;
    }

    m_sySetting.iRsPort = arg1;

    // close opened port
    if (m_serThread->m_bOpen)
    {
        emit StopSeiral();
        m_serThread->wait();
    }

    delete m_serThread;
    StartSerialThread();
}

void MainWindow::StartSerialThread()
{
    m_serThread = new SerialThread(this);
    connect(this, &MainWindow::StopSeiral, m_serThread, &SerialThread::StopThread);
    connect(m_serThread, &SerialThread::SigMonitorDisp, this,
            &MainWindow::on_SigMonitorDisp, Qt::BlockingQueuedConnection);// ATTENTION HERE : MUST WAIT FOR COUNTING VALUE IN SCREEN
    connect(m_serThread, &SerialThread::SigSysShutDown, this, &MainWindow::on_SigSysShutDown);
    connect(m_serThread, &SerialThread::SigTopCameraMeasure, this, &MainWindow::on_camManuBtn_clicked);
    //    connect(m_serThread, &SerialThread::SigBotCameraMeasure, this, &MainWindow::on_camManuBtn_2_clicked);
    connect(m_serThread, &SerialThread::SigLogFruitDataTbl, this, &MainWindow::on_SigLogFruitDataTbl);
    connect(m_serThread, &SerialThread::SigUpdateHinNo, this, &MainWindow::ebxColorHin_valueSet);

    if (m_serThread->m_bOpen)
    {
        ui->lblRSPort->setStyleSheet("color:green;font:bold");
        m_serThread->start();
    }
    else
    {
        ui->lblRSPort->setStyleSheet("color:red");
    }
}

void MainWindow::ebxColorHin_valueSet(int arg1)
{
    ui->ebxColorHin->setValue(arg1);
}

void MainWindow::on_ebxColorHin_valueChanged(int arg1)
{
    if (arg1 == m_sySetting.iHinNo)
    {
        return;
    }

    m_inicommon.WriteINI(m_camSettings, m_imgSettings, m_sySetting);// save color_xxx.ini
    m_inicommon.ReadINI(m_camSettings, m_imgSettings, &m_sySetting, arg1);// read color_xxx.ini
    m_sySetting.iHinNo = arg1;
    ui->ebxHinsyuName->setText(m_sySetting.aHinName);
    //    ImgInitLoad();
}

void MainWindow::on_ebxCaption_editingFinished()
{
    m_sySetting.aCaption = ui->ebxCaption->text();
    setWindowTitle(m_sySetting.aCaption);
}

void MainWindow::on_ebxHinsyuName_editingFinished()
{
    m_sySetting.aHinName = ui->ebxHinsyuName->text();
}

void MainWindow::on_chbSaveLog_stateChanged(int arg1)
{
    if (arg1 == 0)
    {
        SaveLog::Instance()->m_bSave = false;
    }
    else
    {
        SaveLog::Instance()->m_bSave = true;
    }
}

void MainWindow::ShowLog(QtMsgType type, QString msg)
{
    switch (type)
    {
        case QtInfoMsg:
            ui->txtEvent->append(msg);
            break;

        case QtDebugMsg:
            ui->txtEvent->append(msg);
            break;

        case QtWarningMsg:
            ui->txtEvent->append(msg);
            break;

        case QtCriticalMsg:
            ui->txtError->append(msg);
            break;

        case QtFatalMsg:
            ui->txtError->append(msg);
            break;
    }
}

void MainWindow::SetImageToProcessImg(int nid, BYTE *pbyte)
{
    m_imgProc->SetImageData(pbyte, nid);
}

void MainWindow::ShowSrcImgFromCAMSlot()
{
    HDC hdc;                                   //窗口DC
    QRect       objRect;                       //显示窗口的大小
    bool bcolor = true;
    BYTE *pImageBuffer = NULL;
    int width, height;

    for (int nid = 0; nid < CAMERANUM; nid++)
    {
        CamDevice *pf = m_camCtrl->m_camDevice[nid];
        bcolor = true;

        switch (nid)
        {
            case 0:
                objRect = ui->labCh1->rect();
                break;

            case 1:
                objRect = ui->labCh2->rect();
                break;

            case 2:
                objRect = ui->labCh3->rect();
                break;

            case 3:
                objRect = ui->labCh4->rect();
                break;

            case 4:
                objRect = ui->labCh5->rect();
                break;

            case 5:
                objRect = ui->labCh6->rect();
                break;
        }

        hdc = m_hdc[nid];

        if (m_bShowImgRT)
        {
            pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[nid]->dispImage[_SOURCE_IMAGE]->imageData;
            width = m_imgProc->m_imgMeaure[nid]->dispImage[_SOURCE_IMAGE]->width;
            height = m_imgProc->m_imgMeaure[nid]->dispImage[_SOURCE_IMAGE]->height;

            if (m_menuDownValue[nid + 2][0] >> 1 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[nid]->wDispImage[_W_BINARY_IMAGE_R]->imageData;
                width = m_imgProc->m_imgMeaure[nid]->wDispImage[_W_BINARY_IMAGE_R]->width;
                height = m_imgProc->m_imgMeaure[nid]->wDispImage[_W_BINARY_IMAGE_R]->height;
            }
            else if (m_menuDownValue[nid + 2][0] >> 2 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[nid]->wDispImage[_W_SHIMA_IMAGE_G]->imageData;
                width = m_imgProc->m_imgMeaure[nid]->wDispImage[_W_SHIMA_IMAGE_G]->width;
                height = m_imgProc->m_imgMeaure[nid]->wDispImage[_W_SHIMA_IMAGE_G]->height;
            }
            else if (m_menuDownValue[nid + 2][0] >> 3 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[nid]->wDispImage[_WIDE_IMAGE_B]->imageData;
                width = m_imgProc->m_imgMeaure[nid]->wDispImage[_WIDE_IMAGE_B]->width;
                height = m_imgProc->m_imgMeaure[nid]->wDispImage[_WIDE_IMAGE_B]->height;
            }
            else if (m_menuUpValue[nid + 2][0] >> 3 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[nid]->dispImage[_BINARY_IMAGE]->imageData;
                width = m_imgProc->m_imgMeaure[nid]->dispImage[_BINARY_IMAGE]->width;
                height = m_imgProc->m_imgMeaure[nid]->dispImage[_BINARY_IMAGE]->height;
                bcolor = false;
            }
        }
        else
        {
            if (!m_matvec.empty() && !m_matvec[nid].empty())
            {
                pImageBuffer = m_matvec[nid].data;
            }
            else
            {
                pImageBuffer = NULL;
            }

            width = m_matvec[nid].cols;
            height =  m_matvec[nid].rows;
        }

        if (pImageBuffer)
        {
            DrawImg(pf, hdc, pImageBuffer, nid, objRect, width, height, bcolor);
        }
    }

    if (m_camCtrl->m_nOperateID != -1 || (CAMERANUM == 1 && m_bmanual))
    {
        if (CAMERANUM == 1 && m_bmanual)
        {
            m_camCtrl->m_nOperateID = 0;
        }

        pImageBuffer = NULL;
        //label1
        objRect = ui->labCh00->rect();
        bcolor = true;

        if (m_bShowImgRT)
        {
            pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_SOURCE_IMAGE]->imageData;
            width = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_SOURCE_IMAGE]->width;
            height = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_SOURCE_IMAGE]->height;

            if (m_menuDownValue[0][0] >> 1 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_BINARY_IMAGE_R]->imageData;
                width = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_BINARY_IMAGE_R]->width;
                height = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_BINARY_IMAGE_R]->height;
            }
            else if (m_menuDownValue[0][0] >> 2 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_SHIMA_IMAGE_G]->imageData;
                width = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_SHIMA_IMAGE_G]->width;
                height = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_SHIMA_IMAGE_G]->height;
            }
            else if (m_menuDownValue[0][0] >> 3 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_WIDE_IMAGE_B]->imageData;
                width = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_WIDE_IMAGE_B]->width;
                height = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_WIDE_IMAGE_B]->height;
            }
            else if (m_menuUpValue[0][0] >> 3 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_BINARY_IMAGE]->imageData;
                width = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_BINARY_IMAGE]->width;
                height = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_BINARY_IMAGE]->height;
                bcolor = false;
            }
        }
        else
        {
            if (!m_matvec.empty() && !m_matvec[m_camCtrl->m_nOperateID].empty())
            {
                pImageBuffer = m_matvec[m_camCtrl->m_nOperateID].data;
            }
            else
            {
                pImageBuffer = NULL;
            }

            width = m_matvec[m_camCtrl->m_nOperateID].cols;
            height =  m_matvec[m_camCtrl->m_nOperateID].rows;
        }

        if (pImageBuffer)
        {
            DrawImg(m_camCtrl->m_camDevice[m_camCtrl->m_nOperateID], m_hdcmain, pImageBuffer, m_camCtrl->m_nOperateID, objRect,
                    width, height, bcolor);
        }

        // label2
        objRect = ui->labCh01->rect();
        bcolor = true;
        pImageBuffer = NULL;

        if (m_bShowImgRT)
        {
            pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_SOURCE_IMAGE]->imageData;
            width = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_SOURCE_IMAGE]->width;
            height = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_SOURCE_IMAGE]->height;

            if (m_menuDownValue[1][0] >> 1 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_BINARY_IMAGE_R]->imageData;
                width = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_BINARY_IMAGE_R]->width;
                height = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_BINARY_IMAGE_R]->height;
            }
            else if (m_menuDownValue[1][0] >> 2 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_SHIMA_IMAGE_G]->imageData;
                width = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_SHIMA_IMAGE_G]->width;
                height = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_W_SHIMA_IMAGE_G]->height;
            }
            else if (m_menuDownValue[1][0] >> 3 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_WIDE_IMAGE_B]->imageData;
                width = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_WIDE_IMAGE_B]->width;
                height = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->wDispImage[_WIDE_IMAGE_B]->height;
            }
            else if (m_menuUpValue[1][0] >> 3 & 0X01)
            {
                pImageBuffer = (BYTE *)m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_BINARY_IMAGE]->imageData;
                width = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_BINARY_IMAGE]->width;
                height = m_imgProc->m_imgMeaure[m_camCtrl->m_nOperateID]->dispImage[_BINARY_IMAGE]->height;
                bcolor = false;
            }
        }
        else
        {
            if (!m_matvec.empty() && !m_matvec[m_camCtrl->m_nOperateID].empty())
            {
                pImageBuffer = m_matvec[m_camCtrl->m_nOperateID].data;
            }
            else
            {
                pImageBuffer = NULL;
            }

            width = m_matvec[m_camCtrl->m_nOperateID].cols;
            height =  m_matvec[m_camCtrl->m_nOperateID].rows;
        }

        if (pImageBuffer)
        {
            DrawImg(m_camCtrl->m_camDevice[m_camCtrl->m_nOperateID], m_hdcsub, pImageBuffer, m_camCtrl->m_nOperateID,
                    objRect, width, height, bcolor);
        }
    }
}

void MainWindow::DrawImg(CamDevice *pf, HDC hdc, BYTE *pImageBuffer, int nid, QRect objRect, int width, int height, bool bcolor)
{
    int         nWndWidth     = 0;             //窗口宽
    int         nWndHeight    = 0;             //窗口高
    int         nImgWidth     = 0;             //图像的宽
    int         nImgHeight    = 0;             //图像的高
    HDC         objMemDC;                      //与显示兼容的环境句柄
    HBITMAP     objMemBmp;                     //设备环境相关的设备兼容的位图
    wchar_t     chPutStr[100]  = {'\0'};        //显示帧率字符串数组
    int         ix            = 0;
    int         nrealWidth    = 0;
    //    if (pf)
    //    {
    //        nImgWidth    = (int)(pf->m_pstCam->nImageWidth);
    //        nImgHeight   = (int)(pf->m_pstCam->nImageHeight);
    //    }
    //    else if()
    {
        nImgWidth = width;
        nImgHeight = height;
    }
    nWndWidth  = objRect.width();
    nWndHeight = objRect.height();
    nrealWidth  = nWndHeight * 4 / 3; // 720:540=>4:3
    ix = (nWndWidth - nrealWidth) / 2;
    BITMAPINFO *tmpinfo = NULL;
    char chBmpBuf[2048] = {0};
    //    if (!bcolor || pf == nullptr)       //显示灰色图像
    {
        tmpinfo = (BITMAPINFO *)chBmpBuf;
        tmpinfo->bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);
        tmpinfo->bmiHeader.biWidth          = nImgWidth;
        tmpinfo->bmiHeader.biHeight         = -nImgHeight;
        tmpinfo->bmiHeader.biPlanes         = 1;
        tmpinfo->bmiHeader.biBitCount       = bcolor ? 24 : 8; // 黑白图像为8;
        tmpinfo->bmiHeader.biCompression    = BI_RGB;
        tmpinfo->bmiHeader.biSizeImage      = 0;
        tmpinfo->bmiHeader.biXPelsPerMeter  = 0;
        tmpinfo->bmiHeader.biYPelsPerMeter  = 0;
        tmpinfo->bmiHeader.biClrUsed        = 0;
        tmpinfo->bmiHeader.biClrImportant   = 0;

        // 黑白相机需要进行初始化调色板操作
        if (pf)
        {
            for (int i = 0; i < 256; i++)
            {
                tmpinfo->bmiColors[i].rgbBlue     = i;
                tmpinfo->bmiColors[i].rgbGreen    = i;
                tmpinfo->bmiColors[i].rgbRed      = i;
                tmpinfo->bmiColors[i].rgbReserved = i;
            }
        }
    }
    //    else
    //    {
    //        tmpinfo = pf->m_pstCam->pBmpInfo;
    //    }
    //显示图像，显示帧率
    objMemDC = ::CreateCompatibleDC(hdc);
    objMemBmp = CreateCompatibleBitmap(hdc, nWndWidth, nWndHeight);
    ::SelectObject(objMemDC, objMemBmp);
    // 必须调用该语句，否则图像出现水纹
    ::SetStretchBltMode(objMemDC, COLORONCOLOR);
    StretchDIBits(objMemDC,
                  ix,
                  0,
                  nrealWidth,
                  nWndHeight,
                  0,
                  0,
                  nImgWidth,
                  nImgHeight,
                  pImageBuffer,
                  tmpinfo,
                  DIB_RGB_COLORS,
                  SRCCOPY);

    //显示帧率，序列号
    if (pf)
    {
        swprintf_s(chPutStr, L"相机:%d FPS:%.0f %.2f %.2f %.2f", nid + 1, pf->m_pstCam->fFps, g_dCaptureTime, g_dCalTime, g_dDrawTime);
    }
    else
    {
        swprintf_s(chPutStr, L"相机:%d %.2f %.2f %.2f", nid + 1, g_dCaptureTime, g_dCalTime, g_dDrawTime);
    }

    SetTextColor(objMemDC, RGB(0, 0, 255));
    SetBkMode(objMemDC, TRANSPARENT);
    TextOut(objMemDC, ix, 0, chPutStr, (int)lstrlenW(chPutStr));
    StretchBlt(hdc,
               ix,
               0,
               nrealWidth,
               nWndHeight,
               objMemDC,
               ix,
               0,
               nrealWidth,
               nWndHeight,
               SRCCOPY);
    ::DeleteDC(objMemDC);
    DeleteObject(objMemBmp);
}

int MainWindow::GetCupIndex()
{
    int ret = m_cupIndex - 1;

    if (ret < 0)
    {
        ret = CUPDATA_BUFFER - 1;
    }

    return ret;
}

QString MainWindow::GetSendCamData()
{
    short dt;
    short iSendData[SERIAL_NUM_SEND_DATA] = {0};
    memset(iSendData, 0, sizeof(iSendData));

    if (iCamerasError)   //
    {
        iSendData[0] = 10001;
    }
    else if (iLightError)     //
    {
        iSendData[0] = 10002;
    }
    else
    {
        if (bResultOK && !bKaraCup)
        {
            for (int i = 0; i < SERIAL_NUM_RESULT_DATA; i++)
            {
                iSendData[i] = short(g_appleData.dCupData[i] * 10);
            }
        }
        else
        {
            iSendData[0] = 10000;
        }
    }

    QString SendBuffer;

    for (int i = 0; i < SERIAL_NUM_SEND_DATA; i++)
    {
        char buff[64] = {0};
        dt = iSendData[i] & 0xffff;
        sprintf(buff, "N%02dD%05d\r", i + 1, dt);
        SendBuffer += QString(buff);
    }

    return SendBuffer;
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    Q_UNUSED(index)
#ifndef TRIGGER_WITH_COMM

    if (tab_ioctl->isVisible())
    {
        if (m_frmio)
        {
            m_frmio->SetTimerRunStop(true);
        }
    }
    else
    {
        if (m_frmio)
        {
            m_frmio->SetTimerRunStop(false);
        }
    }

#endif
}

void MainWindow::on_spinBxFps_valueChanged(int arg1)
{
    m_sySetting.iFpsTest = arg1;
}
