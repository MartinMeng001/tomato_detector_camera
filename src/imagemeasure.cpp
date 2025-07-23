#include "imagemeasure.h"
#include <vector>
using namespace std;

ImageMeasure::ImageMeasure(int nid, int Xsize, int Ysize, QObject *parent) : QObject(parent)
{
    m_nid = nid;
    iXSize = Xsize > 8 ? Xsize : 8;
    iYSize = Ysize > 8 ? Ysize : 8;
    //    bResultOK = false;
    //    bKaraCup = true;
    bLoadMeasure = false;
    CvSize size = cvSize(iXSize, iYSize);
    //    if (nid == TOP_CAMERA) {
    //        captureImageBG = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //        captureImageBGR = cvCreateImage(size, IPL_DEPTH_8U, 3);
    //        captureImageW = cvCreateImage(size, IPL_DEPTH_8U, 3);
    //    } else {
    //        captureImageBG = cvCreateImage(cvSize(iYSize, iXSize), IPL_DEPTH_8U, 1);
    //        captureImageBGR = cvCreateImage(cvSize(iYSize, iXSize), IPL_DEPTH_8U, 3);
    //        captureImageW = cvCreateImage(cvSize(iYSize, iXSize), IPL_DEPTH_8U, 3);
    //    }
    //    captureImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
    loadImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
    sourceImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
    binaryImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    fruitImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
    //    extractImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    hsvImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
    //    hueImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    saturationImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    valueImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    iroExtractImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
    //    labelImage = cvCreateImage(size, IPL_DEPTH_16U, 1);
    anaumeImage = cvCreateImage(size, IPL_DEPTH_8U, 3);// G
    contourImage = cvCreateImage(size, IPL_DEPTH_8U, 3);// B
    //    defectImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    defectRegionImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    maskImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
    //    maskImage8U_1 = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    rotateImage8U_1 = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    rotateImage8U_3 = cvCreateImage(size, IPL_DEPTH_8U, 3);
    //    karaImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    sealImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    yellowImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
    //    wideImage = cvCreateImage(cvSize(_WIDEXMAX, iYSize), IPL_DEPTH_8U, 3);
    wBinaryImage = cvCreateImage(cvSize(_WIDEXMAX, iYSize), IPL_DEPTH_8U, 3);//R
    //    wLabelImage = cvCreateImage(cvSize(_WIDEXMAX, iYSize), IPL_DEPTH_16U, 1);
    //    wShimaImage = cvCreateImage(cvSize(_WIDEXMAX, iYSize), IPL_DEPTH_8U, 1);
    //    colorLUT = cvCreateMat(1, 256, CV_8UC3);
    //    rotationMatrix = cvCreateMat(2, 3, CV_32FC1);
    //    dispImage[_ORIGIN_IMAGE] = rotateImage8U_3;
    //    dispImage[_HSI_IMAGE] = hsvImage;
    //    dispImage[_EXTRACT_IMAGE] = extractImage;
    dispImage[_BINARY_IMAGE] = binaryImage;
    dispImage[_SOURCE_IMAGE] = sourceImage;
    //    dispImage[_FRUIT_IMAGE] = sealImage;
    //    dispImage[_IRO_EXTRACT_IMAGE] = iroExtractImage;
    //    dispImage[_CONTOUR_IMAGE] = contourImage;
    //    wDispImage[_WIDE_IMAGE_B] = wideImage;
    wDispImage[_W_BINARY_IMAGE_R] = wBinaryImage;
    wDispImage[_WIDE_IMAGE_B] = contourImage;
    wDispImage[_W_SHIMA_IMAGE_G] = anaumeImage;
    //    wDispImage[_W_SHIMA_IMAGE_G] = wShimaImage;
}

ImageMeasure::~ImageMeasure()
{
    cvReleaseImage(&sourceImage);
    cvReleaseImage(&binaryImage);
    //    cvReleaseImage(&captureImageBG);
    //    cvReleaseImage(&captureImageBGR);
    //    cvReleaseImage(&captureImageW);
    //    cvReleaseImage(&captureImage);
    //    cvReleaseImage(&extractImage);
    //    cvReleaseImage(&hsvImage);
    //    cvReleaseImage(&hueImage);
    //    cvReleaseImage(&saturationImage);
    //    cvReleaseImage(&valueImage);
    //    cvReleaseImage(&iroExtractImage);
    //    cvReleaseImage(&labelImage);
    //    cvReleaseImage(&defectImage);
    //    cvReleaseImage(&defectRegionImage);
    //    cvReleaseImage(&maskImage);
    //    cvReleaseImage(&maskImage8U_1);
    //    cvReleaseImage(&rotateImage8U_1);
    //    cvReleaseImage(&rotateImage8U_3);
    //    cvReleaseImage(&wideImage);
    cvReleaseImage(&wBinaryImage);
    //    cvReleaseImage(&wLabelImage);
    //    cvReleaseImage(&wShimaImage);

    //    cvReleaseImage(&karaImage);
    //    cvReleaseImage(&sealImage);
    //    cvReleaseImage(&yellowImage);

    if (loadImage != NULL)
    {
        cvReleaseImage(&loadImage);
    }

    //    cvReleaseImage(&fruitImage);
    cvReleaseImage(&anaumeImage);
    cvReleaseImage(&contourImage);

    //    cvReleaseMat(&colorLUT);
    //    cvReleaseMat(&rotationMatrix);
    for (int i = 0; i < _NUM_DISP_IMAGE; i++)
    {
        dispImage[i] = NULL;
    }

    for (int i = 0; i < _NUM_WDISP_IMAGE; i++)
    {
        wDispImage[i] = NULL;
    }
}

//void ImageMeasure::PreProcessing(int iRotateMode)
//{
//    cvCvtColor(captureImageBG, captureImageBGR, CV_BayerBG2BGR);

//    if (m_nid == TOP_CAMERA) {
//        cvLUT(captureImageBGR, captureImage, colorLUT);
//    } else {
//        cvLUT(captureImageBGR, captureImageW, colorLUT);
//        QuartRotateImage(captureImageW, captureImage, iRotateMode);
//    }
//}

void ImageMeasure::QuartRotateImage(IplImage *srcImage, IplImage *dstImage, int iRotateMode)
{
    int width = srcImage->width;
    int height = srcImage->height;
    unsigned char *pdst = reinterpret_cast<unsigned char *>(dstImage->imageData);
    unsigned char *psrc;

    if (iRotateMode == 0)
    {
        for (int y = width - 1; y >= 0; y--)
        {
            psrc = reinterpret_cast<unsigned char *>(srcImage->imageData + y * 3);

            for (int x = 0; x < height; x++)
            {
                *(pdst++) = *psrc;
                *(pdst++) = *(psrc + 1);
                *(pdst++) = *(psrc + 2);
                psrc += srcImage->widthStep;
            }
        }
    }
    else if (iRotateMode == 1)
    {
        for (int y = 0; y < width; y++)
        {
            psrc = reinterpret_cast<unsigned char *>(srcImage->imageData + (height - 1) * srcImage->widthStep + y * 3);

            for (int x = 0; x < height; x++)
            {
                *(pdst++) = *psrc;
                *(pdst++) = *(psrc + 1);
                *(pdst++) = *(psrc + 2);
                psrc -= srcImage->widthStep;
            }
        }
    }
}

/*
 * apple_side_check：对侧视图判断苹果表面是否有较大黑点
 * @param: img_src：原图，bwr_th：二值化阈值
 * @return: bad_num：黑点个数，bad_ratio：黑点瑕疵度
 */
void ImageMeasure::apple_side_check(const Mat &img_src, int bwr_th)
{
    Mat img_0;
    img_src.copyTo(img_0);
    vector<Mat> img_s(3);
    split(img_0, img_s);
    Mat img_red;
    img_s[2].copyTo(img_red);
    double alpha = 1.0;
    Mat img_r;
    img_red.convertTo(img_r, -1,  alpha, 0);
    GaussianBlur(img_r, img_r, Size(3, 3), 0, 0, BORDER_DEFAULT);
    normalize(img_r, img_r,  0, 255,  NORM_MINMAX);
    //// int bwr_th=100;////////////////////
    Mat img_bw;
    threshold(img_r, img_bw, bwr_th, 255, CV_THRESH_BINARY);
    // copy data to binary image show screen
    morphologyEx(img_bw, img_bw, CV_MOP_CLOSE, Mat());
    vector<vector<Point>> contours;
    findContours(img_bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area = Mat::zeros(Size(1, contours.size()), CV_64FC1);

    for (size_t i = 0; i < contours.size(); i++)
    {
        con_area.at<double>(i, 0) = contourArea(contours[i], false);
    }

    double min_area = 0;
    double max_area = 0;
    Point max_loc;
    minMaxLoc(con_area, &min_area, &max_area, 0, &max_loc, noArray());
    max_area = cvRound(max_area);
    Rect bound_rect = boundingRect(contours[max_loc.y]);
    Mat img_bw_1 = img_bw(bound_rect); ////////////////////
    vector<vector<Point>> contour_1;
    findContours(img_bw_1, contour_1, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area_1 = Mat::zeros(Size(1, contour_1.size()), CV_64FC1);

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        con_area_1.at<double>(i, 0) = contourArea(contour_1[i], false);

        if (con_area_1.at<double>(i, 0) > 0.7 * max_area)
        {
            con_area_1.at<double>(i, 0) = 0;
        }
    }

    double min_area_1 = 0;
    double max_area_1 = 0;
    Point max_loc_1;
    minMaxLoc(con_area_1, &min_area_1, &max_area_1, 0, &max_loc_1, noArray());
    max_area_1 = cvRound(max_area_1);
    ////double ratio_1= max_area_1*100/bound_rect.area();
    Rect bound_rect_1 = boundingRect(contour_1[max_loc_1.y]);
    vector<Rect> rect_group(contour_1.size());

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        rect_group[i] = boundingRect(contour_1[i]);

        if (((rect_group[i]& bound_rect_1).area()) > 5 && (rect_group[i] != bound_rect_1))
        {
            con_area_1.at<double>(i, 0) = 0;
        }
    }

    int p = 0;

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        if (con_area_1.at<double>(i, 0) > 10)
        {
            p++;
        }
    }

    //    int bad_num = p;
    //    dResultData[SCRE_BAD_NUM] = bad_num;
    //    Scalar sum_2 = sum(con_area_1);
    //    //double bad_area = sum_2[0];
    //    dResultData[SCRE_BAD_RATIO] = sum_2[0] / max_area;//double bad_ratio = sum_2[0] / max_area;
}

/*
 * 着色度计算函数
 * @param: img_src：原图，img_contrast：比较图
 * @return: red_ratio：着色度比值
 */
void ImageMeasure::pmt_degree_detect(const Mat &img_src, Mat &img_contrast)
{
    Mat img_0;
    img_contrast.copyTo(img_0);
    vector<Mat> img_s(3);
    split(img_0, img_s);
    Mat img_red;
    img_s[2].copyTo(img_red);
    Mat img_r;
    img_red.convertTo(img_r, -1, 1, 0);
    GaussianBlur(img_r, img_r, Size(3, 3), 0, 0, BORDER_DEFAULT);
    ////normalize(img_r,img_r,  0,255,  NORM_MINMAX);
    Mat red_m;
    Mat red_std;
    meanStdDev(img_r, red_m, red_std, noArray());
    double red_mean = cvRound(red_m.at<double>(0, 0));
    Mat img_1;
    img_src.copyTo(img_1);
    vector<Mat> img_s_1(3);
    split(img_1, img_s_1);
    Mat img_red_1;
    img_s_1[2].copyTo(img_red_1);////////////////////
    Mat img_r_1;
    img_red_1.convertTo(img_r_1, -1, 1, 0);
    GaussianBlur(img_r_1, img_r_1, Size(3, 3), 0, 0, BORDER_DEFAULT);
    ////normalize(img_r_1,img_r_1,  0,255,  NORM_MINMAX);
    int bwr_th = 50; ////////////////////
    Mat img_bw;
    threshold(img_r_1, img_bw, bwr_th, 255, CV_THRESH_BINARY);
    morphologyEx(img_bw, img_bw, CV_MOP_CLOSE, Mat());
    ////dilate( img_bw, img_bw, Mat(),Point(-1,-1));
    vector<vector<Point>> contours;
    findContours(img_bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area = Mat::zeros(Size(1, contours.size()), CV_64FC1);

    for (size_t i = 0; i < contours.size(); i++)
    {
        con_area.at<double>(i, 0) = contourArea(contours[i], false);
    }

    double min_area = 0;
    double max_area = 0;
    Point max_loc;
    minMaxLoc(con_area, &min_area, &max_area, 0, &max_loc, noArray());
    max_area = cvRound(max_area);
    Rect rect_max = boundingRect(contours[max_loc.y]);
    Mat img_one = img_r_1(rect_max);
    Mat img_r_diff;
    img_one.convertTo(img_r_diff, -1, 1, -red_mean);
    int bwr_th_2 = 15; ////////////////////
    Mat img_bw_2;
    threshold(img_r_diff, img_bw_2, bwr_th_2, 255, CV_THRESH_BINARY);
    ////morphologyEx(img_bw_2, img_bw_2, CV_MOP_CLOSE, Mat());
    dilate(img_bw_2, img_bw_2, Mat(), Point(-1, -1));
    vector<vector<Point>> contour_2;
    findContours(img_bw_2, contour_2, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area_2 = Mat::zeros(Size(1, contour_2.size()), CV_64FC1);

    for (size_t i = 0; i < contour_2.size(); i++)
    {
        con_area_2.at<double>(i, 0) = contourArea(contour_2[i], false);
    }

    //    Scalar sum_2 = sum(con_area_2);
    //    dResultData[SCRE_RED_RATIO] = 1 - (sum_2[0] / max_area);//double red_ratio = 1 - (sum_2[0] / max_area);
}

/*
 * 果形判断与直径计算函数
 * @return: diameter:直径计算输出 ;dh_ratio:果形判断输出
 */
void ImageMeasure::fruit_shape_detect(const Mat &img_src, int bwr_th)
{
    Mat img_0;
    img_src.copyTo(img_0);
    vector<Mat> img_s(3);
    split(img_0, img_s);
    Mat img_red;
    img_s[2].copyTo(img_red);
    ////double alpha= 1.0;
    Mat img_r;
    img_red.convertTo(img_r, -1, 1, 0);
    GaussianBlur(img_r, img_r, Size(3, 3), 0, 0, BORDER_DEFAULT);
    normalize(img_r, img_r,  0, 255,  NORM_MINMAX);
    ////int bwr_th= 90;////////////////////
    Mat img_bw;
    threshold(img_r, img_bw, bwr_th, 255, CV_THRESH_BINARY);
    morphologyEx(img_bw, img_bw, CV_MOP_CLOSE, Mat());
    ////dilate( img_bw, img_bw, Mat(),Point(-1,-1));
    vector<vector<Point>> contours;
    findContours(img_bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area = Mat::zeros(Size(1, contours.size()), CV_64FC1);

    for (size_t i = 0; i < contours.size(); i++)
    {
        con_area.at<double>(i, 0) = contourArea(contours[i], false);
    }

    double min_area = 0;
    double max_area = 0;
    Point max_loc;
    minMaxLoc(con_area, &min_area, &max_area, 0, &max_loc, noArray());
    max_area = cvRound(max_area);
    Rect rect_max = boundingRect(contours[max_loc.y]);
    ////////////////////////////////////////////
    /*dResultData[SCRE_APL_DIAMETER] = rect_max.width * 1.0;*///double diameter = rect_max.width * 1.0;
    //    dResultData[SCRE_APL_SHAPE] = rect_max.width * 1.0 / rect_max.height;//double dh_ratio = rect_max.width * 1.0 / rect_max.height;
}

/*
 * 片红判断函数
 * @param: img_src：原图，img_contrast：比较图
 * @return: full_red= 1：表示片红，full_red= 0：表示条红
 */
void ImageMeasure::red_type_detect(const Mat &img_src, Mat &img_contrast)
{
    Mat img_0;
    img_contrast.copyTo(img_0);
    vector<Mat> img_s(3);
    split(img_0, img_s);
    Mat img_red;
    img_s[2].copyTo(img_red);
    ////double alpha= 1.0;
    Mat img_r;
    img_red.convertTo(img_r, -1, 1, 0);
    GaussianBlur(img_r, img_r, Size(3, 3), 0, 0, BORDER_DEFAULT);
    ////normalize(img_r,img_r,  0,255,  NORM_MINMAX);
    //    imshow("img_r", img_r);
    Mat red_m;
    Mat red_std;
    meanStdDev(img_r, red_m, red_std, noArray());
    double red_m_0 = cvRound(red_m.at<double>(0, 0));
    double red_std_0 = red_std.at<double>(0, 0);
    Mat img_1;
    img_src.copyTo(img_1);
    //    imshow("img_1", img_1);
    vector<Mat> img_s_1(3);
    split(img_1, img_s_1);
    Mat img_red_1;
    img_s_1[2].copyTo(img_red_1);////////////////////
    ////double alpha= 1.0;
    Mat img_r_1;
    img_red_1.convertTo(img_r_1, -1, 1, -0);
    GaussianBlur(img_r_1, img_r_1, Size(3, 3), 0, 0, BORDER_DEFAULT);
    int bwr_th = 50; ////////////////////
    Mat img_bw;
    threshold(img_r_1, img_bw, bwr_th, 255, CV_THRESH_BINARY);
    ////morphologyEx(img_bw, img_bw, CV_MOP_CLOSE, Mat());
    dilate(img_bw, img_bw, Mat(), Point(-1, -1));
    vector<vector<Point>> contours;
    findContours(img_bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area = Mat::zeros(Size(1, contours.size()), CV_64FC1);

    for (size_t i = 0; i < contours.size(); i++)
    {
        con_area.at<double>(i, 0) = contourArea(contours[i], false);
    }

    double min_area = 0;
    double max_area = 0;
    Point max_loc;
    minMaxLoc(con_area, &min_area, &max_area, 0, &max_loc, noArray());
    max_area = cvRound(max_area);
    Rect rect_max = boundingRect(contours[max_loc.y]);
    Mat img_one = img_r_1(rect_max);
    Rect rect_5 = Rect(cvRound(0.5 * rect_max.width) - 50, cvRound(0.5 * rect_max.height) - 50, 100, 100);
    Mat img_5 = img_one(rect_5);
    Mat red_m_5;
    Mat red_std_5;
    meanStdDev(img_5, red_m_5, red_std_5, noArray());
    double mean_value = cvRound(red_m_5.at<double>(0, 0));
    cout << "red_mean_5 :" << mean_value << endl;
    double std_value = red_std_5.at<double>(0, 0);
    int full_red = 0;

    if (mean_value / red_m_0 > 0.9 && std_value / red_std_0 < 1.5)
    {
        full_red = 1;
    }
    else
    {
        full_red = 0;
    }

    //    dResultData[SCRE_APL_REDTYPE] = full_red;
}

/*
 * apple_top_check：对俯视图判断苹果表面黑点瑕疵度
 * @param: img_src：原图，bwr_th：二值化阈值
 * @return: bad_num：黑点个数，bad_ratio：黑点瑕疵度
 */
void ImageMeasure::apple_top_check(const Mat &img_src, int bwr_th)
{
    Mat img_0;
    img_src.copyTo(img_0);
    vector<Mat> img_s(3);
    split(img_0, img_s);
    Mat img_red;
    img_s[2].copyTo(img_red);
    //    memcpy(wBinaryImage->imageData, img_red.data, wBinaryImage->imageSize);
    double alpha = 1.0;
    Mat img_r;
    img_red.convertTo(img_r, -1,  alpha, 0);
    GaussianBlur(img_r, img_r, Size(3, 3), 0, 0, BORDER_DEFAULT);
    normalize(img_r, img_r,  0, 255,  NORM_MINMAX);
    ////int bwr_th= 100;////////////////////
    Mat img_bw;
    threshold(img_r, img_bw, bwr_th, 255, CV_THRESH_BINARY);
    // copy data to binary image show screen
    //    memcpy(binaryImage->imageData, img_bw.data, binaryImage->imageSize);
    morphologyEx(img_bw, img_bw, CV_MOP_CLOSE, Mat());
    vector<vector<Point>> contours;
    findContours(img_bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area = Mat::zeros(Size(1, contours.size()), CV_64FC1);

    for (size_t i = 0; i < contours.size(); i++)
    {
        con_area.at<double>(i, 0) = contourArea(contours[i], false);
    }

    double min_area = 0;
    double max_area = 0;
    Point max_loc;
    minMaxLoc(con_area, &min_area, &max_area, 0, &max_loc, noArray());
    max_area = cvRound(max_area);
    Rect rect_max = boundingRect(contours[max_loc.y]);
    Mat img_bw_1 = img_bw(rect_max);
    vector<vector<Point>> contour_1;
    findContours(img_bw_1, contour_1, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area_1 = Mat::zeros(Size(1, contour_1.size()), CV_64FC1);

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        con_area_1.at<double>(i, 0) = contourArea(contour_1[i], false);

        if (con_area_1.at<double>(i, 0) > 0.7 * max_area)
        {
            con_area_1.at<double>(i, 0) = 0;
        }
    }

    double min_area_1 = 0;
    double max_area_1 = 0;
    Point max_loc_1;
    minMaxLoc(con_area_1, &min_area_1, &max_area_1, 0, &max_loc_1, noArray());
    max_area_1 = cvRound(max_area_1);
    ////double ratio_1= max_area_1*100/rect_max.area();
    Rect bound_rect_1 = boundingRect(contour_1[max_loc_1.y]);
    vector<Rect> rect_group(contour_1.size());

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        rect_group[i] = boundingRect(contour_1[i]);

        if ((rect_group[i] & bound_rect_1).area() > 5)
        {
            con_area_1.at<double>(i, 0) = 0;
        }
    }

    Rect rect_cmp = Rect(cvRound(0.5 * rect_max.width) - 5, cvRound(0.5 * rect_max.height) - 5, 10, 10);
    vector<Rect> rect_group_1(contour_1.size());

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        rect_group_1[i] = boundingRect(contour_1[i]);

        if ((rect_group_1[i] & rect_cmp).area() > 5)
        {
            con_area_1.at<double>(i, 0) = 0;
        }
    }

    int p = 0;

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        if (con_area_1.at<double>(i, 0) > 10)
        {
            p++;
        }
    }

    //    int bad_num = p;
    //    dResultData[SCRE_BAD_NUM] = bad_num;
    //    Scalar sum_2 = sum(con_area_1);
    //    //double bad_area = sum_2[0];
    //    dResultData[SCRE_BAD_RATIO] = sum_2[0] / max_area; //double bad_ratio = sum_2[0] / max_area;
}

/*
 * apple_bottom_check：对仰视图判断苹果表面黑点瑕疵度
 * @param: img_src：原图，bwr_th：二值化阈值
 * @return: bad_num：黑点个数，bad_ratio：黑点瑕疵度
 */
void ImageMeasure::apple_bottom_check(const Mat &img_src, int bwr_th)
{
    Mat img_0;
    img_src.copyTo(img_0);
    vector<Mat> img_s(3);
    split(img_0, img_s);
    Mat img_red;
    img_s[2].copyTo(img_red);
    //    memcpy(wBinaryImage->imageData, img_red.data, wBinaryImage->imageSize);
    double alpha = 1.0;
    Mat img_r;
    img_red.convertTo(img_r, -1,  alpha, 0);
    GaussianBlur(img_r, img_r, Size(3, 3), 0, 0, BORDER_DEFAULT);
    normalize(img_r, img_r,  0, 255,  NORM_MINMAX);
    ////int bwr_th= 100;////////////////////
    Mat img_bw;
    threshold(img_r, img_bw, bwr_th, 255, CV_THRESH_BINARY);
    // copy data to binary image show screen
    //    memcpy(binaryImage->imageData, img_bw.data, binaryImage->imageSize);
    morphologyEx(img_bw, img_bw, CV_MOP_CLOSE, Mat());
    vector<vector<Point>> contours;
    findContours(img_bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area = Mat::zeros(Size(1, contours.size()), CV_64FC1);

    for (size_t i = 0; i < contours.size(); i++)
    {
        con_area.at<double>(i, 0) = contourArea(contours[i], false);
    }

    double min_area = 0;
    double max_area = 0;
    Point max_loc;
    minMaxLoc(con_area, &min_area, &max_area, 0, &max_loc, noArray());
    max_area = cvRound(max_area);
    Rect rect_max = boundingRect(contours[max_loc.y]);
    Mat img_bw_1 = img_bw(rect_max);
    vector<vector<Point>> contour_1;
    findContours(img_bw_1, contour_1, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area_1 = Mat::zeros(Size(1, contour_1.size()), CV_64FC1);

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        con_area_1.at<double>(i, 0) = contourArea(contour_1[i], false);

        if (con_area_1.at<double>(i, 0) > 0.7 * max_area)
        {
            con_area_1.at<double>(i, 0) = 0;
        }
    }

    double min_area_1 = 0;
    double max_area_1 = 0;
    Point max_loc_1;
    minMaxLoc(con_area_1, &min_area_1, &max_area_1, 0, &max_loc_1, noArray());
    max_area_1 = cvRound(max_area_1);
    ////double ratio_1= max_area_1*100/rect_max.area();
    Rect bound_rect_1 = boundingRect(contour_1[max_loc_1.y]);
    vector<Rect> rect_group(contour_1.size());

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        rect_group[i] = boundingRect(contour_1[i]);

        if ((rect_group[i] & bound_rect_1).area() > 5)
        {
            con_area_1.at<double>(i, 0) = 0;
        }
    }

    Rect rect_cmp = Rect(cvRound(0.5 * rect_max.width) - 5, cvRound(0.5 * rect_max.height) - 5,  10, 10);
    vector<Rect> rect_group_1(contour_1.size());

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        rect_group_1[i] = boundingRect(contour_1[i]);

        if ((rect_group_1[i] & rect_cmp).area() > 5)
        {
            con_area_1.at<double>(i, 0) = 0;
        }
    }

    int p = 0;

    for (size_t i = 0; i < contour_1.size(); i++)
    {
        if (con_area_1.at<double>(i, 0) > 10)
        {
            p++;
        }
    }

    //    int bad_num = p;
    //    dResultData[SCRE_BAD_NUM] = bad_num;
    //    Scalar sum_2 = sum(con_area_1);
    //    //double bad_area = sum_2[0];
    //    dResultData[SCRE_BAD_RATIO] = sum_2[0] / max_area; //double bad_ratio = sum_2[0] / max_area;
}

/*
 * no_fruit_detect：是否苹果图判断函数，适用于侧视与俯视相机图
 * @param: img_src：原图
 * @return: p：p=0：表示背景空图，p=1：表示苹果图
 */
void ImageMeasure::no_fruit_detect(const Mat &img_src, int bwr_th)
{
    Mat img_0;
    img_src.copyTo(img_0);
    vector<Mat> img_s(3);
    split(img_0, img_s);
    Mat img_red, img_green, img_blue;
    img_s[2].copyTo(img_red);
    img_s[0].copyTo(img_blue);
    img_s[1].copyTo(img_green);
    IplImage tmpimg = cvIplImage(img_red);
    cvMerge(0, 0, &tmpimg, 0, wBinaryImage);
    //    cvCopy(&tmpimg, wBinaryImage);
    tmpimg = cvIplImage(img_green);
    cvMerge(0, &tmpimg, 0,  0, anaumeImage);
    //    cvCopy(&tmpimg, anaumeImage);
    tmpimg = cvIplImage(img_blue);
    cvMerge(&tmpimg, 0,  0,  0, contourImage);
    //    cvCopy(&tmpimg, contourImage);
    Mat img_r;
    img_red.convertTo(img_r, -1, 1, 0);
    GaussianBlur(img_r, img_r, Size(3, 3), 0, 0, BORDER_DEFAULT);
    normalize(img_r, img_r,  0, 255,  NORM_MINMAX);
    Mat img_bw;
    threshold(img_r, img_bw, bwr_th, 255, CV_THRESH_BINARY);
    // copy data to binary image show screen
    tmpimg = cvIplImage(img_bw);
    cvCopy(&tmpimg, binaryImage);
    ////morphologyEx(img_bw, img_bw, CV_MOP_CLOSE, Mat());
    dilate(img_bw, img_bw, Mat(), Point(-1, -1));
    vector<vector<Point>> contours;
    findContours(img_bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area = Mat::zeros(Size(1, contours.size()), CV_64FC1);

    for (size_t i = 0; i < contours.size(); i++)
    {
        con_area.at<double>(i, 0) = contourArea(contours[i], false);
    }

    double min_area = 0;
    double max_area = 0;
    Point max_loc;
    minMaxLoc(con_area, &min_area, &max_area, 0, &max_loc, noArray());
    max_area = cvRound(max_area);
    Rect rect_max = boundingRect(contours[max_loc.y]);
    int src_w = img_bw.size().width;
    int src_h = img_bw.size().height;
    Rect rect_5 = Rect(cvRound(0.5 * src_w) - 30, cvRound(0.5 * src_h) - 40, 60, 80);
    double area_ratio = rect_max.area() * 1.0 / (src_w * src_h);
    double w_ratio = rect_max.width * 1.0 / src_w;
    double h_ratio = rect_max.height * 1.0 / src_h;
    //    int p = 0;
    //    if ((w_ratio > 0.2 && h_ratio > 0.2) && (area_ratio > 0.2) && (rect_max & rect_5).area() > 5)
    //    {
    //        //        p = 1;
    //        bKaraCup = false;
    //        dResultData[SCRE_APL_DIAMETER] = rect_max.width * 1.0;
    //    }
    //    else
    //    {
    //        //        p = 0;
    //        bKaraCup = true;
    //        dResultData[SCRE_APL_DIAMETER] = 0;
    //    }
}

/*
 * bottom_null_detect：底部是否空托盘判断函数
 * @param: img_src：原图
 * @return: p：p=0：表示背景空图，p=1：表示苹果图
 */
bool ImageMeasure::bottom_null_detect(const Mat &img_src, int bwr_th)
{
    Mat img_0;
    img_src.copyTo(img_0);
    vector<Mat> img_s(3);
    split(img_0, img_s);
    Mat img_red, img_green, img_blue;
    img_s[2].copyTo(img_red);
    img_s[0].copyTo(img_blue);
    img_s[1].copyTo(img_green);
    IplImage tmpimg = cvIplImage(img_red);
    cvMerge(0, 0, &tmpimg, 0, wBinaryImage);
    //    cvCopy(&tmpimg, wBinaryImage);
    tmpimg = cvIplImage(img_green);
    cvMerge(0, &tmpimg, 0,  0, anaumeImage);
    //    cvCopy(&tmpimg, anaumeImage);
    tmpimg = cvIplImage(img_blue);
    cvMerge(&tmpimg, 0,  0,  0, contourImage);
    //    cvCopy(&tmpimg, contourImage);
    Mat img_r;
    img_red.convertTo(img_r, -1, 1, 0);
    GaussianBlur(img_r, img_r, Size(3, 3), 0, 0, BORDER_DEFAULT);
    normalize(img_r, img_r,  0, 255,  NORM_MINMAX);
    Mat img_bw;
    threshold(img_r, img_bw, bwr_th, 255, CV_THRESH_BINARY_INV);
    // copy data to binary image show screen
    tmpimg = cvIplImage(img_bw);
    cvCopy(&tmpimg, binaryImage);
    dilate(img_bw, img_bw, Mat(), Point(-1, -1));
    int src_w = img_bw.size().width;
    int src_h = img_bw.size().height;
    Rect rect_5 = Rect(cvRound(0.5 * src_w) - 40, cvRound(0.5 * src_h) - 40, 80, 80);
    vector<vector<Point>> contours;
    findContours(img_bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
    Mat con_area = Mat::zeros(Size(1, contours.size()), CV_64FC1);

    for (size_t i = 0; i < contours.size(); i++)
    {
        con_area.at<double>(i, 0) = contourArea(contours[i], false);
    }

    vector<Rect> rect_group(contours.size());

    for (size_t i = 0; i < contours.size(); i++)
    {
        rect_group[i] = boundingRect(contours[i]);
    }

    for (size_t i = 0; i < contours.size(); i++)
    {
        if ((rect_group[i].width > 0.5 * src_w) || (rect_group[i].height > 0.5 * src_h))
        {
            con_area.at<double>(i, 0) = 0;
        }
    }

    double min_area = 0;
    double max_area = 0;
    Point max_loc;
    minMaxLoc(con_area, &min_area, &max_area, 0, &max_loc, noArray());
    max_area = cvRound(max_area);
    Rect rect_max = boundingRect(contours[max_loc.y]);
    double area_ratio = rect_max.area() * 1.0 / (src_w * src_h);
    double w_ratio = rect_max.width * 1.0 / src_w;
    double h_ratio = rect_max.height * 1.0 / src_h;
    double rect_wh = rect_max.width * 1.0 / rect_max.height;

    if (max_area < 10)
    {
        area_ratio = 0;
        w_ratio = 0;
        h_ratio = 0;
    }

    //    int p = 1;
    bool ret = false;
    //    if (w_ratio > 0.25 && h_ratio > 0.35 && area_ratio > 0.13 && rect_wh > 0.8 && (rect_max & rect_5).area() > 100)
    //    {
    //        //        p = 0;
    //        ret = true;
    //        bKaraCup = true;
    //    }
    //    else
    //    {
    //        //        p = 1;
    //        ret = false;
    //        bKaraCup = false;
    //    }
    return ret;
}

void ImageMeasure::TopMeasure(int bwr_th)
{
    //    bResultOK = false;
    //    bKaraCup = true;
    //    if (bLoadMeasure)   //
    //    {
    //        bLoadMeasure = false;
    //        if (loadImage == NULL)
    //        {
    //            return;
    //        }
    //        cvCopy(loadImage, sourceImage);
    //    }
    //    Mat img_bw = cvarrToMat(sourceImage);
    //    /*memcpy(img_bw.data, sourceImage->imageData, sourceImage->imageSize);*/
    //    // check empty fruit
    //    no_fruit_detect(img_bw, bwr_th);
    //    g_bTopCamKaraCup = bKaraCup;
    //    SetEvent(g_KaraCupHnd);
    //    if (!bKaraCup)
    //    {
    //        apple_top_check(img_bw, bwr_th);
    //    }
    //    else
    //    {
    //        //        cvSetZero(fruitImage);
    //        //        cvSetZero(iroExtractImage);
    //        //        cvSetZero(anaumeImage);
    //        //        cvSetZero(contourImage);
    //        //        cvSetZero(defectImage);
    //    }
    //    bResultOK = true;
}

void ImageMeasure::BotMeasure(int bwr_th)
{
    //    bResultOK = false;
    //    bKaraCup = true;
    if (bLoadMeasure)   //
    {
        bLoadMeasure = false;

        if (loadImage == NULL)
        {
            return;
        }

        cvCopy(loadImage, sourceImage);
    }

    Mat img_bw = cvarrToMat(sourceImage);

    // check empty fruit
    if (!bottom_null_detect(img_bw, bwr_th))
    {
        //        Mat img_bw = cvarrToMat(sourceImage);
        //        memcpy(img_bw.data, sourceImage->imageData, sourceImage->imageSize);
        apple_bottom_check(img_bw, bwr_th);
    }
    else
    {
        //        cvSetZero(fruitImage);
        //        cvSetZero(iroExtractImage);
        //        cvSetZero(anaumeImage);
        //        cvSetZero(contourImage);
        //        cvSetZero(defectImage);
    }

    //    bResultOK = true;
}

void ImageMeasure::SideMeasure(int bwr_th, Mat &img_contrast_redtype, Mat &img_contrast_redratio)
{
    //    bResultOK = false;
    //    bKaraCup = true;
    if (bLoadMeasure)   //
    {
        bLoadMeasure = false;

        if (loadImage == NULL)
        {
            return;
        }

        cvCopy(loadImage, sourceImage);
    }

    Mat img_bw = cvarrToMat(sourceImage);
    //    memcpy(img_bw.data, sourceImage->imageData, sourceImage->imageSize);
    Mat img_0;
    img_bw.copyTo(img_0);
    vector<Mat> img_s(3);
    split(img_0, img_s);
    Mat img_red, img_green, img_blue;
    img_s[2].copyTo(img_red);
    img_s[0].copyTo(img_blue);
    img_s[1].copyTo(img_green);
    IplImage tmpimg = cvIplImage(img_red);
    cvMerge(0, 0, &tmpimg, 0, wBinaryImage);
    //    cvCopy(&tmpimg, wBinaryImage);
    tmpimg = cvIplImage(img_green);
    cvMerge(0, &tmpimg, 0,  0, anaumeImage);
    //    cvCopy(&tmpimg, anaumeImage);
    tmpimg = cvIplImage(img_blue);
    cvMerge(&tmpimg, 0,  0,  0, contourImage);
    //    cvCopy(&tmpimg, contourImage);
    double alpha = 1.0;
    Mat img_r;
    img_red.convertTo(img_r, -1,  alpha, 0);
    GaussianBlur(img_r, img_r, Size(3, 3), 0, 0, BORDER_DEFAULT);
    normalize(img_r, img_r,  0, 255,  NORM_MINMAX);
    Mat img_bw1;
    threshold(img_r, img_bw1, bwr_th, 255, CV_THRESH_BINARY);
    // copy data to binary image show screen
    tmpimg = cvIplImage(img_bw1);
    cvCopy(&tmpimg, binaryImage);
    WaitForSingleObject(g_KaraCupHnd, 150);
    //    bKaraCup = g_bTopCamKaraCup;
    //    if (!bKaraCup)
    //    {
    //        if (m_nid == CAM_DEFAULT_CHECK_SIDE)
    //        {
    //            fruit_shape_detect(img_bw, bwr_th);
    //            if (img_contrast_redtype.data)
    //            {
    //                red_type_detect(img_bw, img_contrast_redtype);
    //            }
    //        }
    //        if (img_contrast_redratio.data)
    //        {
    //            pmt_degree_detect(img_bw, img_contrast_redratio);
    //        }
    //        apple_side_check(img_bw, bwr_th);
    //    }
    //    else
    //    {
    //        //        cvSetZero(fruitImage);
    //        //        cvSetZero(iroExtractImage);
    //        //        cvSetZero(anaumeImage);
    //        //        cvSetZero(contourImage);
    //        //        cvSetZero(defectImage);
    //    }
    //    bResultOK = true;
}

