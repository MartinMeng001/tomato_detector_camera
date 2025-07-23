QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#***********************************************************************************************************
#***********************************************************************************************************
#***********************************************************************************************************
DEFINES += TRIGGER_WITH_COMM  #compile with serial trigger camera

DEFINES += CAM_NUM_ONLY_ONE  #compile with one camera
#***********************************************************************************************************
#***********************************************************************************************************
#***********************************************************************************************************

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH +=  $$PWD/midform \
                $$PWD/include \
                $$PWD/customWidget \
                $$PWD/src \
                $$PWD/include/inc



SOURCES += \
    main.cpp \
    mainwindow.cpp 

HEADERS += \
    def.h \
    head.h \
    mainwindow.h  \
    version.h

FORMS += \
    mainwindow.ui 

RESOURCES   += resource/qss.qrc
RESOURCES   += resource/main.qrc \
                resource/staticdi.qrc  \
                resource/staticdo.qrc

RC_FILE     += resource/app.rc

include (ui/ui.pri)
include (midform/midform.pri)
include (customWidget/customWidget.pri)
include (src/src.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -lWinmm -lgdi32 -lAdvapi32 -luser32

DEFINES += _AMD64_
#QMAKE_CFLAGS *= /utf-8 #仅仅在一个值不存在于一个变量的值的列表中的时候，把它添加进去
#QMAKE_CXXFLAGS *= /utf-8
win32: LIBS += -L$$PWD/lib_msvc/  -lGxIAPI -lDxImageProc
    MOC_DIR = $$PWD/bin/moc
    RCC_DIR = $$PWD/bin/rcc
    UI_DIR  = $$PWD/bin/ui
    OBJECTS_DIR = $$PWD/bin/obj
    win32:CONFIG(debug, debug|release){
#    message('debug')
        LIBS += -L$$PWD/lib_msvc/ -lGxIAPI -lDxImageProc -lgdi32 -lopencv_world347d -lomisliderplugind -ldashiziDefect -luser32 -lAdvapi32 -llibomilicqt
        DESTDIR=$$PWD/bin/debug
        QMAKE_CXXFLAGS_DEBUG -= -MDd
        QMAKE_CXXFLAGS_DEBUG += -MTd
        QMAKE_CFLAGS_DEBUG -= -MDd
        QMAKE_CFLAGS_DEBUG += -MTd
        QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcrtd.lib
    }else{
#    message('release')
        LIBS += -L$$PWD/lib_msvc/ -lGxIAPI -lDxImageProc -lgdi32 -lopencv_world347 -lomisliderplugin -ldashiziDefect -luser32 -lAdvapi32 -llibomilicqt
        DESTDIR=$$PWD/bin/release
        QMAKE_CXXFLAGS_RELEASE -= -MD
        QMAKE_CXXFLAGS_RELEASE += -MT
        QMAKE_CFLAGS_RELEASE -= -MD
        QMAKE_CFLAGS_RELEASE += -MT
        QMAKE_LFLAGS_RELEASE +=  /NODEFAULTLIB:msvcrt.lib
        QMAKE_CXXFLAGS_EXCEPTIONS_ON = /EHa
        QMAKE_CXXFLAGS_STL_ON = /EHa
    }

QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_CXXFLAGS_RELEASE += /Od #编译器禁用优化
#QMAKE_LFLAGS_RELEASE += /DEBUG
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

contains(CONFIG, static){
    # Static compilation does not require deployment
}else{
    # Windows platform(only for x86 architecture)
    win32 {
        DEPLOY_TOOL = $${dirname(QMAKE_QMAKE)}/windeployqt.exe
        DEPLOY_TOOL = $$replace(DEPLOY_TOOL, /, \\)

        contains(CONFIG, debug, debug|release){
            DEPLOY_TARGET = $${OUT_PWD}/debug/$${TARGET}.exe
        }else{
            DEPLOY_TARGET = $${OUT_PWD}/release/$${TARGET}.exe
        }

        DEPLOY_TARGET=$$replace(DEPLOY_TARGET, /, \\)
        !build_pass:message($$DEPLOY_TARGET)
        msvc {
            QMAKE_POST_LINK+=$${DEPLOY_TOOL} $${DEPLOY_TARGET} --force --no-translations $$escape_expand(\\n)
        }else{
            QMAKE_POST_LINK+='$$escape_expand("\\n\\t") $${DEPLOY_TOOL} $${DEPLOY_TARGET} --force --no-translations $$escape_expand("\\n\\t")'
        }
    }
}
