#-------------------------------------------------
#
# Project created by QtCreator 2017-06-14T21:28:47
#
#-------------------------------------------------

QT       += core gui serialport opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MD1Tools
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    core/printer/printer.cpp \
    core/printer/connection.cpp \
    common.cpp \
    gui/dialogs/setupdlg.cpp \
    gui/widgets/serial_setup.cpp \
    config.cpp \
    gui/log_view.cpp \
    gui/basewnd.cpp \
    test/escher3d/escher3d_wnd.cpp \
    test/terminal.cpp \
    gui/widgets/pluginsbar.cpp \
    core/printer/eeprom.cpp \
    test/eeprom_conf.cpp \
    test/escher3d/escher3d.cpp \
    test/opendact/opendact_wnd.cpp \
    test/opendact/heightmap_widget.cpp \
    test/opendact/opendact/opendact.cpp \
    test/opendact/opendact/opendact_user_var.cpp \
    test/opendact/opendact/opendact_eeprom.cpp \
    test/Delta-Bed-Calibration-Tool/dbct.cpp \
    test/Delta-Bed-Calibration-Tool/dbct_wnd.cpp \
    test/opendact/2dchart.cpp \
    test/escher3d/newpoints.cpp \
    core/printer/extruder.cpp \
    core/printer/printer_object.cpp \
    core/printer/commands.cpp \
    test/control/control_wnd.cpp \
    gui/widgets/termwidget.cpp \
    gui/widgets/up_down_widget.cpp \
    gui/widgets/beds/bed_basic_widget.cpp \
    test/height_map/height_map.cpp \
    test/height_map/plane_widget.cpp \
    gui/widgets/beds/black_bed_widget.cpp

HEADERS  += mainwindow.h \
    core/printer/printer.h \
    core/printer/connection.h \
    core/core.h \
    common.h \
    gui/dialogs/setupdlg.h \
    gui/widgets/serial_setup.h \
    config.h \
    gui/log_view.h \
    gui/basewnd.h \
    test/escher3d/escher3d_wnd.h \
    test/terminal.h \
    core/types.h \
    gui/widgets/pluginsbar.h \
    core/printer/eeprom.h \
    test/eeprom_conf.h \
    test/escher3d/escher3d.h \
    test/opendact/opendact_wnd.h \
    test/opendact/heightmap_widget.h \
    test/opendact/opendact/opendact.h \
    test/opendact/opendact/opendact_user_var.h \
    test/opendact/opendact/opendact_eeprom.h \
    test/Delta-Bed-Calibration-Tool/dbct.h \
    test/Delta-Bed-Calibration-Tool/dbct_wnd.h \
    test/opendact/2dchart.h \
    test/escher3d/newpoints.h \
    core/printer/extruder.h \
    core/printer/printer_object.h \
    core/printer/commands.h \
    test/control/control_wnd.h \
    gui/widgets/termwidget.h \
    gui/widgets/up_down_widget.h \
    gui/widgets/beds/bed_basic_widget.h \
    test/height_map/height_map.h \
    test/height_map/plane_widget.h \
    gui/widgets/beds/black_bed_widget.h

RESOURCES += \
    gui/images/images.qrc

FORMS += \
    gui/dialogs/setupdlg.ui \
    test/escher3d/escher3d_wnd.ui \
    test/terminal.ui \
    test/eeprom_conf.ui \
    test/opendact/opendact_wnd.ui \
    test/Delta-Bed-Calibration-Tool/dbct_wnd.ui \
    test/control/control_wnd.ui \
    test/height_map/height_map.ui

#---------------  Qwt Plot3D  ----------------------------------
#INCLUDEPATH += /usr/include/qwtplot3d-qt5
#LIBS += -L/usr/lib -lqwtplot3d-qt5

#---------------  Qwt Plot  ------------------------------------
#INCLUDEPATH += /usr/local/qwt-6.1.3-qt-5.3.2/include/
#LIBS += -L/usr/local/qwt-6.1.3-qt-5.3.2/lib -lqwt

DISTFILES +=
