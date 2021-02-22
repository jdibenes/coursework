#-------------------------------------------------
#
# Project created by QtCreator 2020-03-12T17:18:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += openglextensions
QT += x11extras

TARGET = opengl_cuda_test
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
INCLUDEPATH += /usr/local/cuda/include

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    openglrender2d.cpp \
    utilities.cpp \
    golgrid.cpp \
    golhelpers.cpp \
    goltiming.cpp

HEADERS += \
    mainwindow.h \
    openglrender2d.h \
    utilities.h \
    cudagolrendergl.h \
    goltypes.h \
    golgrid.h \
    golhelpers.h \
    cudagolgrid.h \
    cudagolgridgl.h \
    cudagoltiming.h \
    goltiming.h

FORMS += \
    mainwindow.ui

LIBS = -L/usr/local/cuda/lib64 -lcuda -lcudart

CUDA_SOURCES = \
    cudagolgrid.cu \
    cudagoltiming.cu \
    cudagolrendergl.cu \
    cudagolgridgl.cu \

cuda.input = CUDA_SOURCES
cuda.output = ${QMAKE_FILE_BASE}.o
cuda.commands = /usr/local/cuda-10.2/bin/nvcc ${QMAKE_FILE_NAME} -I. -c -o ${QMAKE_FILE_OUT} --ptxas-options=-v -arch=sm_61

QMAKE_EXTRA_COMPILERS += cuda
