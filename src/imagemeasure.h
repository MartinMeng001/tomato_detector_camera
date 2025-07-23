#pragma once

#define _WIDEXMAX   720

#define _NUM_DISP_IMAGE     8
#define _ORIGIN_IMAGE       0
#define _HSI_IMAGE          1
#define _EXTRACT_IMAGE      2
#define _BINARY_IMAGE       3
#define _SOURCE_IMAGE       4
#define _FRUIT_IMAGE        5
#define _IRO_EXTRACT_IMAGE  6
#define _CONTOUR_IMAGE      7

#define _NUM_WDISP_IMAGE    3
#define _WIDE_IMAGE_B         0
#define _W_BINARY_IMAGE_R     1
#define _W_SHIMA_IMAGE_G      2

#define _NUM_RESULT_DATA 11                       //
#define _SEAL_EXIST     0                         //
#define _CIR_SIZE       1                         //
#define _TOP_HENKEI1    2                         //
#define _TOP_HENKEI2    3                         //
#define _YELLOW_RATIO   4                         //
#define _BROWN_RATIO    5                         //
#define _FRUIT_HEIGHT   6                         //
#define _HENPEI         7                        //
#define _SANKAKU2       8                        //
#define _S_AREA_RATIO   9
#define _S_PITCH        10

#define _MAX_SHIMA 1024

// ---------------------------------------------------------------------------
#define MAX_COLOR_SETTING 3
#define _YELLOW_ID  0
#define _BROWN_ID   1
#define _SEAL_ID    2

#include <QObject>
#include "def.h"
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

class ImageMeasure : public QObject
{
    Q_OBJECT
public:
    explicit ImageMeasure(int nid, int Xsize, int Ysize, QObject *parent = nullptr);
    ~ImageMeasure();
    bool bLightError; // LEDエラー
    int iXSize, iYSize;
    bool bLoadMeasure;

    //    IplImage *captureImageBG; // カメラから取り込み画像
    //    IplImage *captureImageBGR; // ベーア変換後の画像
    //    IplImage *captureImageW; // WhiteBalance調整後画像
    //    IplImage *captureImage; //
    IplImage *sourceImage; //
    //    IplImage *extractImage; //
    IplImage *binaryImage; //
    //    IplImage *hsvImage; //
    //    IplImage *hueImage; //
    //    IplImage *saturationImage; // 彩度画像
    //    IplImage *valueImage; // 明度画像
    //    IplImage *labelImage; // ラベリング画像
    //    IplImage *maskImage; // マスク画像
    //    IplImage *maskImage8U_1; // マスク１Channel画像
    IplImage *loadImage; // 読み込みの画像
    //    IplImage *fruitImage; // 梨抽出画像
    IplImage *anaumeImage; // 穴埋め画像
    IplImage *contourImage; // 輪郭画像
    //    IplImage *defectImage; // 傷画像
    //    IplImage *defectRegionImage; // 傷検出範囲
    //    IplImage *iroExtractImage; // 色抽出画像
    //    IplImage *rotateImage8U_1; // 傾け、回転する画像
    //    IplImage *rotateImage8U_3; // 傾け、回転する画像
    //    IplImage *karaImage; // 空カップ判定画像
    //    IplImage *sealImage;
    //    IplImage *yellowImage;
    IplImage *dispImage[_NUM_DISP_IMAGE]; // 表示用のPoint 独立のメモリなし

    //    IplImage *wideImage; // 4面を引き伸ばし画像    横_WIDEXMAX(360)、縦iYSize
    IplImage *wBinaryImage; // 4面引き伸ばし画像、縞模様の2値化画像
    //    IplImage *wLabelImage; // 4面引き伸ばし画像、ラべリング画像
    //    IplImage *wShimaImage; // 4面引き伸ばし画像、縞模様画像
    IplImage *wDispImage[_NUM_WDISP_IMAGE];

    QT_DEPRECATED_X("not used") void PreProcessing(int iRotateMode);
    QT_DEPRECATED_X("not used") void QuartRotateImage(IplImage *srcImage, IplImage *dstImage, int iRotateMode);
    QT_DEPRECATED_X("not used") void apple_side_check(const Mat &img_src, int bwr_th);
    QT_DEPRECATED_X("not used") void pmt_degree_detect(const Mat &img_src, Mat &img_contrast);
    QT_DEPRECATED_X("not used") void fruit_shape_detect(const Mat &img_src,  int bwr_th);
    QT_DEPRECATED_X("not used") void red_type_detect(const Mat &img_src, Mat &img_contrast);
    QT_DEPRECATED_X("not used") void apple_top_check(const Mat &img_src, int bwr_th);
    QT_DEPRECATED_X("not used") void apple_bottom_check(const Mat &img_src, int bwr_th);
    QT_DEPRECATED_X("not used") void no_fruit_detect(const Mat &img_src, int bwr_th);
    QT_DEPRECATED_X("not used") bool bottom_null_detect(const Mat &img_src, int bwr_th);
    QT_DEPRECATED_X("not used") void TopMeasure(int bwr_th);
    QT_DEPRECATED_X("not used") void BotMeasure(int bwr_th);
    QT_DEPRECATED_X("not used") void SideMeasure(int bwr_th, Mat &img_contrast_redtype, Mat &img_contrast_redratio);

private:
    int m_nid;
    //    CvMat *colorLUT; //
    //    CvMat *rotationMatrix;

signals:

};

