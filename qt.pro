INCLUDEPATH += include/ /usr/include/vtk-6.2/
CONFIG += qt
LIBS += -lvtkGUISupportQt-6.2 -lvtkRenderingCore-6.2 -lvtkRenderingVolume-6.2\
-lvtkCommonCore-6.2 -lvtkInteractionStyle-6.2 -lvtkRenderingVolumeOpenGL-6.2\
-lvtkCommonExecutionModel-6.2 -lvtkIOImage-6.2 -lvtkIOLegacy-6.2\
-lvtkRenderingOpenGL-6.2 -lvtkCommonDataModel-6.2 -lvtkImagingGeneral-6.2\
-lvtkIOGeometry-6.2
QT += core gui opengl
SOURCES += src/main.cpp
TARGET = error-analyzer
