#-------------------------------------------------
#
# Project created by QtCreator 2016-03-17T09:20:11
#
#-------------------------------------------------

QT       += core gui network
QT       += printsupport svg
QT       += webkitwidgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(./QxPotrace.pri)
TARGET = iMox-painter
TEMPLATE = app
#CONFIG   += c++11

FORMS += \
    welcom.ui \
    chef/chef.ui \
    chef/editpic.ui \
    chef/yourfood.ui \
    painter/ipainter.ui \
    parentsetting.ui \
    ipinput.ui \
    painter/picture.ui \
    keyboard/frmnum.ui \
    infordialog.ui \
    form.ui \
    picedit.ui \
    potrace-gcode/potracewidget.ui \
    uart/cpserialport.ui

HEADERS += \
    welcom.h \
    chef/chef.h \
    chef/editpic.h \
    chef/yourfood.h \
    painter/ipainter.h \
    painter/paintarea.h \
    picprinter.h \
    parentsetting.h \
    door.h \
    ipinput.h \
    SliceEngine/clipper/clipper.hpp \
    SliceEngine/modelFile/modelFile.h \
    SliceEngine/utils/floatpoint.h \
    SliceEngine/utils/gettime.h \
    SliceEngine/utils/intpoint.h \
    SliceEngine/utils/logoutput.h \
    SliceEngine/utils/polygon.h \
    SliceEngine/utils/polygondebug.h \
    SliceEngine/utils/socket.h \
    SliceEngine/bridge.h \
    SliceEngine/comb.h \
    SliceEngine/fffProcessor.h \
    SliceEngine/gcodeExport.h \
    SliceEngine/includes.h \
    SliceEngine/infill.h \
    SliceEngine/inset.h \
    SliceEngine/layerPart.h \
    SliceEngine/multiVolumes.h \
    SliceEngine/optimizedModel.h \
    SliceEngine/pathOrderOptimizer.h \
    SliceEngine/polygonOptimizer.h \
    SliceEngine/raft.h \
    SliceEngine/settings.h \
    SliceEngine/skin.h \
    SliceEngine/skirt.h \
    SliceEngine/sliceDataStorage.h \
    SliceEngine/slicer.h \
    SliceEngine/support.h \
    SliceEngine/timeEstimate.h \
    painter/picture.h \
    keyboard/frmnum.h \
    infordialog.h \
    form.h \
    picedit.h \
    potrace-gcode/nanosvg.h \
    potrace-gcode/svg2gcode.h \
    potrace-gcode/svgpath.h \
    potrace-gcode/XGetopt.h \
    potrace-gcode/potracewidget.h \
    gmodel.h \
    uart/cpserialport.h \
    gsendmodel.h


SOURCES += \
    main.cpp \
    welcom.cpp \
    chef/chef.cpp \
    chef/editpic.cpp \
    chef/yourfood.cpp \
    painter/ipainter.cpp \
    painter/paintarea.cpp \
    picprinter.cpp \
    parentsetting.cpp \
    door.cpp \
    ipinput.cpp \
    SliceEngine/clipper/clipper.cpp \
    SliceEngine/modelFile/modelFile.cpp \
    SliceEngine/utils/gettime.cpp \
    SliceEngine/utils/logoutput.cpp \
    SliceEngine/utils/socket.cpp \
    SliceEngine/bridge.cpp \
    SliceEngine/comb.cpp \
    SliceEngine/gcodeExport.cpp \
    SliceEngine/infill.cpp \
    SliceEngine/inset.cpp \
    SliceEngine/layerPart.cpp \
    SliceEngine/optimizedModel.cpp \
    SliceEngine/pathOrderOptimizer.cpp \
    SliceEngine/polygonOptimizer.cpp \
    SliceEngine/raft.cpp \
    SliceEngine/settings.cpp \
    SliceEngine/skin.cpp \
    SliceEngine/skirt.cpp \
    SliceEngine/slicer.cpp \
    SliceEngine/support.cpp \
    SliceEngine/timeEstimate.cpp \
    painter/picture.cpp \
    keyboard/frmnum.cpp \
    infordialog.cpp \
    form.cpp \
    picedit.cpp \
    potrace-gcode/svgpath.cpp \
    potrace-gcode/nanosvg.c \
    potrace-gcode/svg2gcode.c \
    potrace-gcode/XGetopt.c \
    potrace-gcode/potracewidget.cpp \
    gmodel.cpp \
    uart/cpserialport.cpp \
    gsendmodel.cpp

RESOURCES += \
    img-res.qrc

RC_FILE = lp.rc




