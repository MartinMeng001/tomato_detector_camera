#include "saveimagethread.h"
#include "def.h"
#include "mainwindow.h"

SaveImageThread::SaveImageThread(QObject *parent)
    : QThread{parent}
{
    m_exit = false;
    m_parent = parent;
    m_index = 0;
}

void SaveImageThread::run()
{
    MainWindow *lpmain = (MainWindow *)m_parent;
    SYSetting *lpset = (SYSetting *)(&lpmain->m_sySetting);
    ImageProcess *lpprocess = (ImageProcess *)(lpmain->m_imgProc);
    int distance = 0;

    while (1)
    {
        DWORD iStatus = ::WaitForMultipleObjects(CAMERANUM, g_camComplHnds, true, INFINITE);

        if (m_exit)
        {
            break;
        }
        else
        {
            if (iStatus == WAIT_OBJECT_0)
            {
                g_dCaptureTime = g_stopWatch.StopFlashTimer();
                QVector<cv::Mat> images(CAMERANUM);
                distance = lpset->idistance;

                for (int i = 0; i < CAMERANUM; i++)
                {
                    cv::Mat img = cvarrToMat(lpmain->m_imgProc->m_imgMeaure[i]->sourceImage);
                    images[i] = img.clone();

                    if (i == CAMERANUM - 1)
                    {
                        lpprocess->m_vecArr[m_index % IMAGE_BUFFER_CNT][i] = images.at(i);
                    }

                    //        imshow(std::to_string(i), img);
                    //        waitKey(10);
                }

                lpprocess->m_vecArr[(m_index + distance) % IMAGE_BUFFER_CNT].swap(images);  //save ahead
                m_mutex.lock();
                m_queue.enqueue(lpprocess->m_vecArr[m_index % IMAGE_BUFFER_CNT]);
                m_mutex.unlock();
            }
            else if (iStatus == WAIT_FAILED) //表示函数执行失败
            {
                DWORD dw = GetLastError();
                qCritical() << QString("Image save thread,  Wait For Objects failed.last error code:%1").arg(dw);
            }

            lpmain->m_bIsTrigValid = true;
            lpmain->m_bIsTrigValid2 = true;
        }

        m_index++;
    }
}
