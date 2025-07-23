#include "inicommon.h"
#include <QFile>
#include <QTextCodec>

QString COLOR_INI_FILE_NAME = QString("Color_%1.ini");
QString SYS_INI_FILE_NAME = QString("sysConfig.ini");
QString SECTION_NAME_SYS = QString("SysSection");
QString SECTION_NAME_COLOR = QString("HinName");

InICommon::InICommon(QObject *parent) : QObject(parent)
{
}

void InICommon::WriteINI(CamSetting **m_camSettings, ImageSetting **m_imgSettings, SYSetting m_sySetting)
{
    // 1.sysConfig.ini
    m_IniFile = new QSettings(SYS_INI_FILE_NAME, QSettings::IniFormat);
#ifdef _MSC_VER
    m_IniFile->setIniCodec(QTextCodec::codecForName("gbk"));
#else
    m_IniFile->setIniCodec(QTextCodec::codecForName("utf-8"));
#endif
    m_IniFile->beginGroup(SECTION_NAME_SYS);
    m_IniFile->setValue(tr("RsPort"), m_sySetting.iRsPort);
    m_IniFile->setValue(tr("Caption"), m_sySetting.aCaption);
    m_IniFile->setValue(tr("ColorHinNo"), m_sySetting.iHinNo);
    m_IniFile->setValue(tr("Distance"), m_sySetting.idistance);
    m_IniFile->setValue(tr("FPSTest"), m_sySetting.iFpsTest);
    m_IniFile->setValue(tr("IOCTRLNO"), m_sySetting.iIOCTLIdx);
    m_IniFile->endGroup();

    for (int i = 0; i < CAMERANUM ; i++)
    {
        m_IniFile->beginGroup(SECTION_NAME[i]);
        m_IniFile->setValue(tr("RotateMode"), m_camSettings[i]->iRotateMode);
        m_IniFile->setValue(tr("Width"), m_camSettings[i]->iWidth);
        m_IniFile->setValue(tr("Height"), m_camSettings[i]->iHeight);
        m_IniFile->setValue(tr("Gain"), m_camSettings[i]->iGain);
        m_IniFile->setValue(tr("AreaWhiteBalance"), m_camSettings[i]->bAreaWhiteBalance);
        m_IniFile->setValue(tr("R_Gain"), m_camSettings[i]->dR_Gain);
        m_IniFile->setValue(tr("G_Gain"), m_camSettings[i]->dG_Gain);
        m_IniFile->setValue(tr("B_Gain"), m_camSettings[i]->dB_Gain);
        m_IniFile->setValue(tr("R_Ratio"), m_camSettings[i]->dR_Ratio);
        m_IniFile->setValue(tr("G_Ratio"), m_camSettings[i]->dG_Ratio);
        m_IniFile->setValue(tr("B_Ratio"), m_camSettings[i]->dB_Ratio);
        // all trigger is soft
        m_IniFile->setValue(tr("TriggerMode"), true/*m_camSettings[i]->bTriggerMode*/);
        m_IniFile->setValue(tr("TriggerSource"), 0/*m_camSettings[i]->iTriggerSource*/);
        m_IniFile->setValue(tr("Left"), m_camSettings[i]->iLeft);
        m_IniFile->setValue(tr("Right"), m_camSettings[i]->iRight);
        m_IniFile->setValue(tr("Top"), m_camSettings[i]->iTop);
        m_IniFile->setValue(tr("Bottom"), m_camSettings[i]->iBottom);
        m_IniFile->setValue(tr("PixSize"), m_camSettings[i]->dPixSize);
        m_IniFile->setValue(tr("MedianFilter"), m_camSettings[i]->iMedianFilter);
        m_IniFile->setValue(tr("Karikomi"), m_camSettings[i]->iKarikomi);
        m_IniFile->setValue(tr("KaraThr"), m_camSettings[i]->dKaraThr);
        m_IniFile->setValue(tr("MinLabelingSize"), m_camSettings[i]->iMinLabelingSize);
        m_IniFile->setValue(tr("Tanshuku"), m_camSettings[i]->dTanshuku);
        m_IniFile->setValue(tr("MinValue"), m_camSettings[i]->iMinValue);
        m_IniFile->setValue(tr("Rotate"), m_camSettings[i]->bRotate);
        m_IniFile->endGroup();
    }

    delete m_IniFile;
    // 2.color_xxx.ini
    m_IniFile = new QSettings(COLOR_INI_FILE_NAME.arg(m_sySetting.iHinNo, 3, 10, QLatin1Char('0')), QSettings::IniFormat);
#ifdef _MSC_VER
    m_IniFile->setIniCodec(QTextCodec::codecForName("gbk"));
#else
    m_IniFile->setIniCodec(QTextCodec::codecForName("utf-8"));
#endif

    for (int i = 0; i < CAMERANUM ; i++)
    {
        for (int j = 0; j < MAX_COLOR_SETTING; j++)
        {
            QString aSettei;
            aSettei = QString("%1_Color_%2").arg(SECTION_NAME[i]).arg(j);
            m_IniFile->beginGroup(aSettei);
            m_IniFile->setValue(tr("Name"), m_imgSettings[i]->colorSetting[j].aName);
            m_IniFile->setValue(tr("Red"), m_imgSettings[i]->colorSetting[j].iRed);
            m_IniFile->setValue(tr("Green"), m_imgSettings[i]->colorSetting[j].iGreen);
            m_IniFile->setValue(tr("Hanni"), m_imgSettings[i]->colorSetting[j].iHanni);
            m_IniFile->setValue(tr("Wmin"), m_imgSettings[i]->colorSetting[j].iWmin);
            m_IniFile->setValue(tr("Wmax"), m_imgSettings[i]->colorSetting[j].iWmax);
            m_IniFile->setValue(tr("RG_A1"), m_imgSettings[i]->colorSetting[j].iRG_A1);
            m_IniFile->setValue(tr("RG_A2"), m_imgSettings[i]->colorSetting[j].iRG_A2);
            m_IniFile->setValue(tr("RG_K"), m_imgSettings[i]->colorSetting[j].iRG_K);
            m_IniFile->setValue(tr("RG_Rmin"), m_imgSettings[i]->colorSetting[j].iRG_Rmin);
            m_IniFile->setValue(tr("DispR"), m_imgSettings[i]->colorSetting[j].iDispR);
            m_IniFile->setValue(tr("DispG"), m_imgSettings[i]->colorSetting[j].iDispG);
            m_IniFile->setValue(tr("DispB"), m_imgSettings[i]->colorSetting[j].iDispB);
            m_IniFile->setValue(tr("Use"), m_imgSettings[i]->colorSetting[j].bUse);
            m_IniFile->endGroup();
        }

        m_IniFile->beginGroup(SECTION_NAME[i]);
        m_IniFile->setValue(tr("BinaryShift"), m_imgSettings[i]->iBinaryShift);
        m_IniFile->setValue(tr("BinaryThreshold"), m_imgSettings[i]->iBinaryThreshold);
        m_IniFile->setValue(tr("SealGHoseiX"), m_imgSettings[i]->iSealGHoseiX);
        m_IniFile->setValue(tr("SealGHoseiY"), m_imgSettings[i]->iSealGHoseiY);
        m_IniFile->setValue(tr("SealWidth"), m_imgSettings[i]->iSealWidth);
        m_IniFile->setValue(tr("SealHeight"), m_imgSettings[i]->iSealHeight);
        m_IniFile->setValue(tr("SealMinSize"), m_imgSettings[i]->iSealMinSize);
        m_IniFile->setValue(tr("ErodeDilateNum"), m_imgSettings[i]->iErodeDilateNum);
        m_IniFile->setValue(tr("BinarySepLine"), m_imgSettings[i]->iBinarySepLine);
        m_IniFile->setValue(tr("BinaryTopThr"), m_imgSettings[i]->iBinaryTopThr);
        m_IniFile->setValue(tr("BinaryBotThr"), m_imgSettings[i]->iBinaryBotThr);
        m_IniFile->setValue(tr("BinaryBackThr"), m_imgSettings[i]->iBinaryMaxB);
        m_IniFile->setValue(tr("BinaryMinG"), m_imgSettings[i]->iBinaryMinG);
        m_IniFile->setValue(tr("StripeThreshold"), m_imgSettings[i]->iStripeThreshold);
        m_IniFile->setValue(tr("StripeAngleWt"), m_imgSettings[i]->dStripeAngleWt);
        m_IniFile->setValue(tr("CenterLineAngleWt"), m_imgSettings[i]->dCenterLineAngleWt);
        m_IniFile->setValue(tr("GHosei"), m_imgSettings[i]->iGHosei);
        m_IniFile->setValue(tr("HeightHosei"), m_imgSettings[i]->iHeightHosei);
        m_IniFile->setValue(tr("ShutterSpeed"), m_imgSettings[i]->iExpoTime);
        m_IniFile->setValue(tr("ShimaBlockSize"), m_imgSettings[i]->iShimaBlockSize);
        m_IniFile->setValue(tr("ShimaThreshold"), m_imgSettings[i]->iShimaThreshold);
        m_IniFile->setValue(tr("ShimaMinLen"), m_imgSettings[i]->iShimaMinLen);
        m_IniFile->setValue(tr("ShimaMinArea"), m_imgSettings[i]->iShimaMinArea);
        m_IniFile->setValue(tr("VolumeHoseiA"), m_imgSettings[i]->dVolumeHoseiA);
        m_IniFile->setValue(tr("VolumeHoseiB"), m_imgSettings[i]->dVolumeHoseiB);
        m_IniFile->endGroup();
    }

    m_IniFile->beginGroup(SECTION_NAME_COLOR);
    m_IniFile->setValue(tr("Name"), m_sySetting.aHinName);
    //    m_IniFile->setValue(tr("MeasType"), m_sySetting.iMeasType);
    m_IniFile->endGroup();
    delete m_IniFile;
}

void InICommon::ReadINI(CamSetting **m_camSettings, ImageSetting **m_imgSettings, SYSetting *m_sySetting, int ihinno)
{
    bool bexist = true;
    // 1.sysConfig.ini
    QFile qfile(SYS_INI_FILE_NAME);

    if (!qfile.exists())
    {
        bexist = false;
    }
    else
    {
        m_IniFile = new QSettings(SYS_INI_FILE_NAME, QSettings::IniFormat);
#ifdef _MSC_VER
        m_IniFile->setIniCodec(QTextCodec::codecForName("gbk"));
#else
        m_IniFile->setIniCodec(QTextCodec::codecForName("utf-8"));
#endif

        for (int iSetting = 0; iSetting < CAMERANUM; iSetting++)
        {
            m_camSettings[iSetting]->iRotateMode = m_IniFile->value(SECTION_NAME[iSetting]  + "/RotateMode", 0).toInt();
            m_camSettings[iSetting]->iWidth = m_IniFile->value(SECTION_NAME[iSetting]  + "/Width", 720).toInt();
            m_camSettings[iSetting]->iHeight = m_IniFile->value(SECTION_NAME[iSetting]  + "/Height", 540).toInt();
            m_camSettings[iSetting]->iGain = m_IniFile->value(SECTION_NAME[iSetting]  + "/Gain", 0).toInt();
            m_camSettings[iSetting]->bAreaWhiteBalance = m_IniFile->value(SECTION_NAME[iSetting]  + "/AreaWhiteBalance", 0).toBool();
            m_camSettings[iSetting]->dR_Gain = m_IniFile->value(SECTION_NAME[iSetting]  + "/R_Gain", 1).toDouble();
            m_camSettings[iSetting]->dG_Gain = m_IniFile->value(SECTION_NAME[iSetting]  + "/G_Gain", 1).toDouble();
            m_camSettings[iSetting]->dB_Gain = m_IniFile->value(SECTION_NAME[iSetting]  + "/B_Gain", 1).toDouble();
            m_camSettings[iSetting]->dR_Ratio = m_IniFile->value(SECTION_NAME[iSetting]  + "/R_Ratio", 1).toDouble();
            m_camSettings[iSetting]->dG_Ratio = m_IniFile->value(SECTION_NAME[iSetting]  + "/G_Ratio", 1).toDouble();
            m_camSettings[iSetting]->dB_Ratio = m_IniFile->value(SECTION_NAME[iSetting]  + "/B_Ratio", 1).toDouble();
            //all trigger is soft
            m_camSettings[iSetting]->bTriggerMode = true;//m_IniFile->value(SECTION_NAME[iSetting]  + "/TriggerMode", 1).toBool();
            m_camSettings[iSetting]->iTriggerSource = 0;//m_IniFile->value(SECTION_NAME[iSetting]  + "/TriggerSource", 0).toInt();
            m_camSettings[iSetting]->iLeft = m_IniFile->value(SECTION_NAME[iSetting]  + "/Left", 0).toInt();
            m_camSettings[iSetting]->iRight = m_IniFile->value(SECTION_NAME[iSetting]  + "/Right", 320).toInt();
            m_camSettings[iSetting]->iTop = m_IniFile->value(SECTION_NAME[iSetting]  + "/Top", 0).toInt();
            m_camSettings[iSetting]->iBottom = m_IniFile->value(SECTION_NAME[iSetting]  + "/Bottom", 240).toInt();
            m_camSettings[iSetting]->dPixSize = m_IniFile->value(SECTION_NAME[iSetting]  + "/PixSize", 0.2).toDouble();
            m_camSettings[iSetting]->iMedianFilter = m_IniFile->value(SECTION_NAME[iSetting]  + "/MedianFilter", 3).toInt();
            m_camSettings[iSetting]->iKarikomi = m_IniFile->value(SECTION_NAME[iSetting]  + "/Karikomi", 3).toInt();
            m_camSettings[iSetting]->dKaraThr = m_IniFile->value(SECTION_NAME[iSetting]  + "/KaraThr", 30).toDouble();
            m_camSettings[iSetting]->iMinLabelingSize = m_IniFile->value(SECTION_NAME[iSetting]  + "/MinLabelingSize", 500).toInt();
            m_camSettings[iSetting]->dTanshuku = m_IniFile->value(SECTION_NAME[iSetting]  + "/Tanshuku", 80).toDouble();
            m_camSettings[iSetting]->iMinValue = m_IniFile->value(SECTION_NAME[iSetting]  + "/MinValue", 15).toInt();
            m_camSettings[iSetting]->bRotate = m_IniFile->value(SECTION_NAME[iSetting]  + "/Rotate", 1).toBool();
        }

        m_sySetting->iRsPort = m_IniFile->value(SECTION_NAME_SYS  + "/RsPort", 1).toInt();
        m_sySetting->iMinFreeDisk = m_IniFile->value(SECTION_NAME_SYS  + "/MinFreeDisk", 100).toInt();
        m_sySetting->aCaption = m_IniFile->value(SECTION_NAME_SYS  + "/Caption", "外观检测软件").toString();
        m_sySetting->iHinNo = m_IniFile->value(SECTION_NAME_SYS  + "/ColorHinNo", 0).toInt();
        m_sySetting->idistance = m_IniFile->value(SECTION_NAME_SYS  + "/Distance", 0).toInt();
        m_sySetting->iFpsTest = m_IniFile->value(SECTION_NAME_SYS  + "/FPSTest", 0).toInt();
        m_sySetting->bFixMonitor = m_IniFile->value(SECTION_NAME_SYS  + "/FixMonitor", 0).toBool();
        m_sySetting->iMonitorWidth = m_IniFile->value(SECTION_NAME_SYS  + "/MonitorWidth", 1280).toInt();
        m_sySetting->iMonitorHeight = m_IniFile->value(SECTION_NAME_SYS  + "/MonitorHeight", 1024).toInt();
        m_sySetting->iIOCTLIdx = m_IniFile->value(SECTION_NAME_SYS  + "/IOCTRLNO", -1).toInt();
        delete m_IniFile;
    }

    // 2.color_xxx.ini
    int hinnotmp = ihinno == 0 ? m_sySetting->iHinNo : ihinno;
    QString ininame = COLOR_INI_FILE_NAME.arg(hinnotmp, 3, 10, QLatin1Char('0'));
    QFile qfile2(ininame);

    if (!qfile2.exists())
    {
        bexist = false;
    }
    else
    {
        m_IniFile = new QSettings(ininame, QSettings::IniFormat);
#ifdef _MSC_VER
        m_IniFile->setIniCodec(QTextCodec::codecForName("gbk"));
#else
        m_IniFile->setIniCodec(QTextCodec::codecForName("utf-8"));
#endif

        for (int i = 0; i < CAMERANUM; i++)
        {
            for (int j = 0; j < MAX_COLOR_SETTING; j++)
            {
                QString aSettei;
                aSettei = QString("%1_Color_%2").arg(SECTION_NAME[i]).arg(j);
                m_imgSettings[i]->colorSetting[j].aName = m_IniFile->value(aSettei + "/Name", tr("")).toString();
                m_imgSettings[i]->colorSetting[j].iRed = m_IniFile->value(aSettei + "/Red", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iGreen = m_IniFile->value(aSettei + "/Green", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iHanni = m_IniFile->value(aSettei + "/Hanni", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iWmin = m_IniFile->value(aSettei + "/Wmin", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iWmax = m_IniFile->value(aSettei + "/Wmax", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iRG_A1 = m_IniFile->value(aSettei + "/RG_A1", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iRG_A2 = m_IniFile->value(aSettei + "/RG_A2", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iRG_K = m_IniFile->value(aSettei + "/RG_K", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iRG_Rmin = m_IniFile->value(aSettei + "/RG_Rmin", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iDispR = m_IniFile->value(aSettei + "/DispR", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iDispG = m_IniFile->value(aSettei + "/DispG", 0).toInt();
                m_imgSettings[i]->colorSetting[j].iDispB = m_IniFile->value(aSettei + "/DispB", 0).toInt();
                m_imgSettings[i]->colorSetting[j].bUse = m_IniFile->value(aSettei + "/Use", 0).toBool();
            }

            m_imgSettings[i]->iBinaryMaxB = m_IniFile->value(SECTION_NAME[i] + "/BinaryBackThr", 0).toInt();
            m_imgSettings[i]->iBinaryMinG = m_IniFile->value(SECTION_NAME[i] + "/BinaryMinG", 120).toInt();
            m_imgSettings[i]->iBinaryThreshold = m_IniFile->value(SECTION_NAME[i] + "/BinaryThreshold", 0).toInt();
            m_imgSettings[i]->iErodeDilateNum = m_IniFile->value(SECTION_NAME[i] + "/ErodeDilateNum", 0).toInt();
            m_imgSettings[i]->iBinaryShift = m_IniFile->value(SECTION_NAME[i] + "/BinaryShift", 0).toInt();
            m_imgSettings[i]->iSealGHoseiX = m_IniFile->value(SECTION_NAME[i] + "/SealGHoseiX", 10).toInt();
            m_imgSettings[i]->iSealGHoseiY = m_IniFile->value(SECTION_NAME[i] + "/SealGHoseiY", 10).toInt();
            m_imgSettings[i]->iSealWidth = m_IniFile->value(SECTION_NAME[i] + "/SealWidth", 10).toInt();
            m_imgSettings[i]->iSealHeight = m_IniFile->value(SECTION_NAME[i] + "/SealHeight", 10).toInt();
            m_imgSettings[i]->iSealMinSize = m_IniFile->value(SECTION_NAME[i] + "/SealMinSize", 10).toInt();
            m_imgSettings[i]->iBinarySepLine = m_IniFile->value(SECTION_NAME[i] + "/BinarySepLine", 0).toInt();
            m_imgSettings[i]->iBinaryTopThr = m_IniFile->value(SECTION_NAME[i] + "/BinaryTopThr", 0).toInt();
            m_imgSettings[i]->iBinaryBotThr = m_IniFile->value(SECTION_NAME[i] + "/BinaryBotThr", 0).toInt();
            m_imgSettings[i]->iStripeThreshold = m_IniFile->value(SECTION_NAME[i] + "/StripeThreshold", 0).toInt();
            m_imgSettings[i]->dStripeAngleWt = m_IniFile->value(SECTION_NAME[i] + "/StripeAngleWt", 0).toDouble();
            m_imgSettings[i]->dCenterLineAngleWt = m_IniFile->value(SECTION_NAME[i] + "/CenterLineAngleWt", 0).toDouble();
            m_imgSettings[i]->iGHosei = m_IniFile->value(SECTION_NAME[i] + "/GHosei", 0).toInt();
            m_imgSettings[i]->iHeightHosei = m_IniFile->value(SECTION_NAME[i] + "/HeightHosei", 0).toInt();
            m_imgSettings[i]->iExpoTime = m_IniFile->value(SECTION_NAME[i] + "/ShutterSpeed", 20).toInt();
            m_imgSettings[i]->iShimaBlockSize = m_IniFile->value(SECTION_NAME[i] + "/ShimaBlockSize", 0).toInt();
            m_imgSettings[i]->iShimaThreshold = m_IniFile->value(SECTION_NAME[i] + "/ShimaThreshold", 0).toInt();
            m_imgSettings[i]->iShimaMinLen = m_IniFile->value(SECTION_NAME[i] + "/ShimaMinLen", 0).toInt();
            m_imgSettings[i]->iShimaMinArea = m_IniFile->value(SECTION_NAME[i] + "/ShimaMinArea", 0).toInt();
            m_imgSettings[i]->dVolumeHoseiA = m_IniFile->value(SECTION_NAME[i] + "/VolumeHoseiA", 0).toDouble();
            m_imgSettings[i]->dVolumeHoseiB = m_IniFile->value(SECTION_NAME[i] + "/VolumeHoseiB", 0).toDouble();
        }

        m_sySetting->aHinName = m_IniFile->value(SECTION_NAME_COLOR + "/Name", tr("")).toString();
        // m_sySetting->iMeasType = m_IniFile->value(SECTION_NAME_COLOR + "/MeasType", 0).toInt();
        delete m_IniFile;
    }

    // not find ini file,create it
    if (!bexist)
    {
        WriteINI(m_camSettings, m_imgSettings, *m_sySetting);
    }
}
