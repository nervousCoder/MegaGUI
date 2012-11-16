HEADERS += \
    wizard.h \
    megaethernet.h \
    ftdidevice.h \
    ftd2xx.h

SOURCES += \
    wizard.cpp \
    megaethernet.cpp \
    ftdidevice.cpp \
    main.cpp

LIBS += \
    ftd2xx.lib

RESOURCES += \
    res.qrc

QT += \
    network

RC_FILE = megagui.rc

OTHER_FILES += \
    ftd2xx.lib
