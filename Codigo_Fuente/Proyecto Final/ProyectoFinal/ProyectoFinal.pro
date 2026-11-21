QT += widgets multimedia

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cabra.cpp \
    dumpling.cpp \
    flor.cpp \
    main.cpp \
    mainwindow.cpp \
    nivel1.cpp \
    nivel2.cpp \
    plataformaoscilante.cpp \
    po.cpp

HEADERS += \
    cabra.h \
    dumpling.h \
    flor.h \
    mainwindow.h \
    nivel1.h \
    nivel2.h \
    plataformaoscilante.h \
    po.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    ProyectoFinal_es_CO.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    recursos.qrc
