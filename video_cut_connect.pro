#-------------------------------------------------
#
# Project created by QtCreator 2020-07-03T11:19:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = video_cut_connect
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += $$PWD/include
LIBS += $$PWD/lib/libslidingbarplugin.so
LIBS += $$PWD/lib/libprogressbarplugin.so
LIBS += $$PWD/lib/libslidingmicrobarplugin.so
LIBS += $$PWD/lib/libvideolistplugin.so



RESOURCES += \
    bar.qrc

unix:!macx { ################################ linux #############################
message('This is linux.')
DEFINES += LINUX

# boost
INCLUDEPATH += /usr/local/include/boost
LIBS +=/usr/local/lib/libboost_thread.a
LIBS +=/usr/local/lib/libboost_system.a
LIBS +=/usr/local/lib/libboost_date_time.a
LIBS +=/usr/local/lib/libboost_filesystem.a
DEFINES +=BOOST_USE_LIB

# opencv
INCLUDEPATH += /usr/local/include \
                /usr/local/include/opencv \
                /usr/local/include/opencv2

LIBS += /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_core.so    \
        /usr/local/lib/libopencv_imgproc.so \
        /usr/local/lib/libopencv_videoio.so \
        /usr/local/lib/libopencv_superres.so \
        /usr/local/lib/libopencv_imgcodecs.so


}
win32 { ################################ windows #############################
message('This is win32.')
DEFINES += WINDOWS
## boost - debug
#INCLUDEPATH += C:\Users\98548\Downloads\boost_1_71_0
#LIBS += C:\Users\98548\Downloads\boost_1_71_0\stage\lib\libboost_filesystem-vc140-mt-gd-x64-1_71.lib
## boost - relase
INCLUDEPATH += D:\.boost\boost_1_68_0
LIBS += D:\.boost\boost_1_68_0\stage\lib\libboost_filesystem-vc141-mt-x64-1_68.lib \
        D:\.boost\boost_1_68_0\stage\lib\libboost_system-vc141-mt-x64-1_68.lib \
        D:\.boost\boost_1_68_0\stage\lib\libboost_thread-vc141-mt-x64-1_68.lib \
        D:\.boost\boost_1_68_0\stage\lib\libboost_date_time-vc141-mt-x64-1_68.lib \
        D:\.boost\boost_1_68_0\stage\lib\libboost_chrono-vc141-mt-x64-1_68.lib
# opencv
INCLUDEPATH += D:\.opencv\opencv-4.0.0\build\install\include \
                D:\.opencv\opencv-4.0.0\build\install\include/opencv \
                D:\.opencv\opencv-4.0.0\build\install\include/opencv2
# (debug版本的库需要在库名称后面加上字母 d )
LIBS += D:\.opencv\opencv-4.0.0\build\install\x64\vc14\lib\opencv_core400.lib \
        D:\.opencv\opencv-4.0.0\build\install\x64\vc14\lib\opencv_highgui400.lib \
        D:\.opencv\opencv-4.0.0\build\install\x64\vc14\lib\opencv_imgcodecs400.lib \
        D:\.opencv\opencv-4.0.0\build\install\x64\vc14\lib\opencv_imgproc400.lib \
        D:\.opencv\opencv-4.0.0\build\install\x64\vc14\lib\opencv_videoio400.lib
}
