#include "mainwindow.h"
#include <QTextCodec>
#include <QApplication>
#include "GxIAPI.h"
#include <QMessageBox>
#include <savelog.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#ifdef _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#endif
    GX_STATUS status = GX_STATUS_SUCCESS;
    //在起始位置调用GXInitLib()进行初始化，申请资源
    status = GXInitLib();

    if (status != GX_STATUS_SUCCESS)
    {
        QMessageBox::critical(NULL, "Error", "<font size='10' color='black'>GXInitLib init failed.</font>");
        return 0;
    }

    // log save
    //#ifndef QT_DEBUG
    SaveLog::Instance()->start();
    //#endif
    MainWindow w;
    w.setWindowIcon(QIcon(":image/Camera.ico"));
    w.showMaximized();
    int ret = a.exec();
    //#ifndef QT_DEBUG
    SaveLog::Instance()->stop();
    //#endif
    return ret;
}
