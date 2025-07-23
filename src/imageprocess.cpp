#include "imageprocess.h"
#include "mainwindow.h"
#include <windows.h>
#include <QtCore>
#include "dashiziDefectExp.h"

cv::Scalar out_color[] = { cv::Scalar(0, 125, 255), cv::Scalar(255, 125, 0), cv::Scalar(125, 255, 0), cv::Scalar(0, 255, 125), cv::Scalar(255, 0, 125), cv::Scalar(125, 0, 255), cv::Scalar(100, 50, 100), cv::Scalar(50, 50, 100), cv::Scalar(125, 125, 0), cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 125), cv::Scalar(0, 125, 255), cv::Scalar(50, 0, 255), cv::Scalar(50, 255, 0), cv::Scalar(0, 0, 255), cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0) };
std::string type_name[] = { "background", "all", "zhuosedu", "sanhua", "zhuoshang", "shanghen", "lieguo"};//石榴6张

ImageProcess::ImageProcess(ImageSetting **ist, CamSetting **cst, QObject *parent): QThread(parent)
{
    m_camSet = cst;
    m_imgSet = ist;
    m_appleRedRatio = NULL;
    m_appleRedType = NULL;

    for (int i = 0; i < CAMERANUM; i++)
    {
        m_imgMeaure[i] = new ImageMeasure(i, cst[i]->iWidth, cst[i]->iHeight);
        pImageBuffers[i] = NULL;
    }

    QString modelpath = QCoreApplication::applicationDirPath();
    modelpath.append("/").append("dashizi.pt");
    int result = lzsd::init_dashiziDefect(modelpath.toStdString()); //初始化

    if (result == -1 || result == -3)
    {
        qCritical() << "图像检测dll初始化失败";
    }
    else if (result == -2)
    {
        qCritical() << "图像检测dll认证失败";
    }
    else if (result == 1)
    {
        qInfo() << "图像检测dll启动成功";
    }

    m_colorcnt = sizeof(out_color) / sizeof(out_color[0]);
    m_typecnt = sizeof(type_name) / sizeof(type_name[0]);
    m_bStop = false;
    m_parent = parent;

    for (int i = 0; i < IMAGE_BUFFER_CNT; i++)
    {
        m_vecArr[i] = QVector<Mat>(6);
    }
}

ImageProcess::~ImageProcess()
{
    for (int i = 0; i < CAMERANUM; i++)
    {
        delete m_imgMeaure[i];
    }

    lzsd::release_dashiziDefect();//释放pearDefect
}

void ImageProcess::SetImageData(BYTE *pbyte, int iID)
{
    pImageBuffers[iID] = pbyte;
    memcpy(m_imgMeaure[iID]->sourceImage->imageData, pImageBuffers[iID], m_imgMeaure[iID]->sourceImage->imageSize);
    //    memcpy(m_imgMeaure->captureImageBG->imageData, pImageBuffers, m_imgMeaure->captureImageBG->imageSize);
    //    m_imgMeaure->PreProcessing(m_camSet[m_nID]->iRotateMode);
    //    cvCopy(m_imgMeaure->captureImage, m_imgMeaure->sourceImage);
}

void ImageProcess::ProDiameter(Mat &image)
{
    bResultOK = false;
    bKaraCup = false;
    memset(dResultData, 0, sizeof(dResultData));
    QT_TRY
    {
        // 将图像转换为灰度图
        Mat img_hsv, img_mask;
        cvtColor(image, img_hsv, COLOR_BGR2HSV);
        Mat blur;
        // 进行形态学操作，使西红柿之间的间隙变细
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        inRange(img_hsv, Scalar(7, 0, 0), Scalar(180, 255, 205), img_mask);
        //    imshow("inRange", img_mask);
        dilate(img_mask, img_mask, kernel, Point(-1, -1), 3);
        //        imshow("erode", img_mask);// 进行轮廓检测，找到每个西红柿的轮廓
        // 对阈值图像进行腐蚀和膨胀操作
        kernel = getStructuringElement(MORPH_ELLIPSE, Size(20, 20));
        morphologyEx(img_mask, img_mask, MORPH_OPEN, kernel, Point(-1, -1), 3);
        //    imshow("img_mask", img_mask);// 进行轮廓检测，找到每个西红柿的轮廓
        vector<vector<Point>> contours;
        findContours(img_mask, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

        // 绘制每个轮廓，并计算其直径
        for (size_t i = 0; i < contours.size(); i++)
        {
            // 找到轮廓的最小外接圆
            //            Point2f center;
            //            float radius;
            //            minEnclosingCircle(contours[i], center, radius);

            //            if (radius >= 600 || radius < 150)
            //            {
            //                continue;
            //            }
            if (contours[i].size() <= 5)
            {
                continue;
            }

            RotatedRect rect = fitEllipse(contours[i]);
            float width = rect.size.width;
            float height = rect.size.height;
            float radius = MAX(width, height);
            float radiusMin = MIN(width, height);

            if (width >= 1200 || height >= 1200 || width <= 300 || height <= 300)
            {
                continue;
            }

            drawContours(image, contours, i, Scalar(255, 255, 0), 2);
            ellipse(image, rect, Scalar(0, 255, 255), 2, 8);
            // 计算圆的直径并绘制
            dResultData[SCRE_APL_DIAMETER] = radius * m_camSet[TOP_CAMERA]->dPixSize;
            dResultData[SCRE_RED_RATIO] = radiusMin / radius * 100;
            //            circle(image, center, (int)radius, Scalar(0, 255, 0), 2);
            /*
            // 在圆心处绘制直径
            Point pt1(center.x, center.y - (diameter / 2));
            Point pt2(center.x, center.y + (diameter / 2));
            line(image, pt1, pt2, Scalar(0, 255, 0), 2);*/
        }

        if (dResultData[SCRE_APL_DIAMETER] == 0)
        {
            bKaraCup = true;// empty tray
        }

    }
    QT_CATCH(...)
    {
        qCritical() << "图像检测失败。";
    }
    bResultOK = true;
}

void ImageProcess::Process(QVector<Mat> &images, Mat &drawMat)
{
    // images process
    bResultOK = false;
    bKaraCup = false;
    memset(dResultData, 0, sizeof(dResultData));
    lzsd::dashiziInfo info;
    QT_TRY
    {
        //        auto start = system_clock::now();
        g_dCalTime = g_stopWatch.StopFlashTimer();
        lzsd::push_frames_dashizi(images[TOP_CAMERA], info, drawMat, true); //检测瑕疵
        //        auto end = system_clock::now();
        //        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        //        gProcessTime = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
        g_dCalTime = g_stopWatch.StopFlashTimer();
    }
    QT_CATCH(...)
    {
        qCritical() << "图像检测dll图片检测失败。";
        return;
    }

    if (info.all_area  <= m_camSet[0]->dKaraThr || info.width * 0.5 * m_camSet[TOP_CAMERA]->dPixSize <= m_camSet[0]->iMedianFilter)
    {
        bKaraCup = true;// empty tray
    }
    else
    {
        dResultData[SCRE_RED_RATIO] = info.jixing_area * 100.0 / info.all_area; //info.height * 100.0 / info.width;//畸形率
        dResultData[SCRE_BAD_RATIO] = info.ganba_area * 1.0 / info.all_area;//干巴
        dResultData[SCRE_APL_DIAMETER] = info.width * 1.0 * m_camSet[TOP_CAMERA]->dPixSize;//直径mm
        dResultData[SCRE_APL_SHAPE] = info.lieguo_area > 0 ? 1 : 0;//裂果
        dResultData[SCRE_BAD_NUM] = info.bingbian_area > 0 ? 1 : 0;//病变
        dResultData[SCRE_APL_REDTYPE] = info.zhuanse_area > 0 ? 1 : 0;//转色
    }

    bResultOK = true;
}

void ImageProcess::run()
{
    MainWindow *lpmain = (MainWindow *)m_parent;

    while (!m_bStop)
    {
        if (lpmain->m_saveImgThread->m_queue.isEmpty())
        {
            usleep(2000);
            continue;
        }

        lpmain->m_saveImgThread->m_mutex.lock();
        QVector<Mat> images = lpmain->m_saveImgThread->m_queue.dequeue();
        lpmain->m_saveImgThread->m_mutex.unlock();
        QT_TRY
        {
            g_stopWatch.StopFlashTimer();
            Process(images, m_resultimg);
            //            ProDiameter(images[0]);
            g_dCalTime =  g_stopWatch.StopFlashTimer();
        }
        QT_CATCH(...)
        {
            qInfo() << "图像检测dll process检测失败。";
        }
        lpmain->m_matvec.clear();
        lpmain->m_matvec.push_back(m_resultimg);
        RESULTDATA qtmp;
        qtmp.bKaraCup = bKaraCup;
        qtmp.bResultOk = bResultOK;
        memcpy(qtmp.dCupData, dResultData, sizeof(dResultData));
        m_resultDat.setValue(qtmp);
        lpmain->on_SigMonitorDisp(m_resultDat);

        if (!lpmain->m_bmanual && !m_bStop)
        {
            // data process ok
            lpmain->m_serThread->SendDataBySerial();
        }

        emit SigUpdateFruitData();
        emit ShowSrcImgFromCAMSig();
        g_dDrawTime = g_stopWatch.StopFlashTimer();
    }
}
