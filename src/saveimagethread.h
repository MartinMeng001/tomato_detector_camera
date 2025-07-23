#ifndef SAVEIMAGETHREAD_H
#define SAVEIMAGETHREAD_H

#include <QThread>
#include <QQueue>
#include <QVector>
#include <opencv2/opencv.hpp>
#include <QMutex>

using namespace std;

class SaveImageThread : public QThread
{
    Q_OBJECT
public:
    explicit SaveImageThread(QObject *parent = nullptr);
    QQueue <QVector<cv::Mat>> m_queue;
    QMutex m_mutex;
    bool m_exit;

    // QThread interface
protected:
    void run() override;
private:
    void *m_parent;
    int m_index;
};

#endif // SAVEIMAGETHREAD_H
