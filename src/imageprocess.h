#pragma once

#include <QThread>
#include "def.h"
#include "imagemeasure.h"
#include <QMutex>
#include <QQueue>
#include <QVector>
#include <QVariant>

class ImageProcess : public QThread
{
    Q_OBJECT
public:
    ImageProcess(ImageSetting **, CamSetting **, QObject *parent = 0);
    ~ImageProcess();
    double dResultData[SERIAL_NUM_RESULT_DATA];
    bool bResultOK; //
    bool bKaraCup; //
    bool m_bStop;
    void SetImageData(BYTE *, int);
    BYTE *pImageBuffers[CAMERANUM];// camera image data
    ImageMeasure *m_imgMeaure[CAMERANUM];

    QVector<Mat> m_vecArr[IMAGE_BUFFER_CNT];

signals:
    void ShowSrcImgFromCAMSig();
    void SigUpdateFruitData();

private:
    ImageSetting **m_imgSet;
    CamSetting **m_camSet;
    Mat *m_appleRedRatio, *m_appleRedType;
    Mat m_resultimg;
    void *m_parent;
    int m_colorcnt, m_typecnt;

    void Process(QVector<Mat> &, Mat &);
    void ProDiameter(Mat &);
    QVariant m_resultDat;

    // QThread interface
protected:
    void run() override;
};

