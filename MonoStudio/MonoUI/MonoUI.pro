QT += widgets

TEMPLATE = lib
DEFINES += MONOUI_LIBRARY

CONFIG += c++11 precompile_header

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR= $$PWD/../bin
INCLUDEPATH+= $$PWD/

PRECOMPILED_HEADER = $$PWD/defines.h

SOURCES += \
    common/MonoUI.cpp \
    common/MonoWidget.cpp \
    common/qimageblur.cpp \
    effect/MColorfulShadowEffect.cpp \
    style/MStyleCalendar.cpp \
    style/MStyleNavbar.cpp \
    style/MStyleWindow.cpp \
    style/MTitleBar.cpp

HEADERS += \
    defines.h \
    common/qimageblur.h \
    common/qmemrotate.h \
    common/MonoUI.h \
    common/MonoWidget.h \
    effect/MColorfulShadowEffect.h \
    style/MStyleCalendar.h \
    style/MStyleNavbar.h \
    style/MStyleWindow.h \
    style/MTitleBar.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
