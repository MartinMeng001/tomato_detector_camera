#pragma once

#include <QObject>
#include <QSettings>
#include "def.h"

class InICommon : public QObject
{
    Q_OBJECT
public:
    explicit InICommon(QObject *parent = nullptr);
    void WriteINI(CamSetting **m_camSettings, ImageSetting **m_imgSettings, SYSetting m_sySetting);
    void ReadINI(CamSetting **m_camSettings, ImageSetting **m_imgSettings, SYSetting *, int ihinno = 0);

private:
    QSettings *m_IniFile;

signals:

};

