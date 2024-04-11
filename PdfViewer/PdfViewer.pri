
INCLUDEPATH += $$PWD/MuPDF/include
LIBS += -L$$PWD/MuPDF/lib -llibmupdf

HEADERS += \
    $$PWD/documentobserver.h \
    $$PWD/navigationtoolbar.h \
    $$PWD/pageview.h \
    $$PWD/viewer.h \

SOURCES += \
    $$PWD/documentobserver.cpp \
    $$PWD/navigationtoolbar.cpp \
    $$PWD/pageview.cpp \
    $$PWD/viewer.cpp
