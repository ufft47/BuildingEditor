#-------------------------------------------------
#
# Project created by QtCreator 2014-03-19T12:53:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CataBuildingEditor_v2
TEMPLATE = app

CONFIG += c++11 debug_and_release

SOURCES += main.cpp\
        buildingeditor.cpp \
    buildingeditorview.cpp \
    drawableitem.cpp \
    tile.cpp \
    jsonparser.cpp \
    options.cpp \
    buildingmodel.cpp \
    overmapterrain.cpp \
    tripoint.cpp \
    features.cpp \
    jsonwriter.cpp \
    colors.cpp \
    monstergroup.cpp \
    itemgroup.cpp \
    generalwizardpage.cpp \
    newbuildingwizard.cpp \
    currentfeature.cpp \
    vehicle.cpp \
    omtdialog.cpp \
    jsonloader.cpp \
    gaspump.cpp \
    rubble.cpp \
    field.cpp \
    omtdata.cpp \
    searchmodel.cpp \
    common.cpp \
    overmapspeciallayout.cpp \
    specialwizardpage.cpp \
    overmapspecialdata.cpp \
    omtwidget.cpp \
    omtwizardpage.cpp

HEADERS  += buildingeditor.h \
    buildingeditorview.h \
    drawableitem.h \
    tile.h \
    jsonparser.h \
    options.h \
    buildingmodel.h \
    overmapterrain.h \
    tripoint.h \
    features.h \
    colors.h \
    jsonwriter.h \
    monstergroup.h \
    itemgroup.h \
    generalwizardpage.h \
    newbuildingwizard.h \
    currentfeature.h \
    vehicle.h \
    omtdialog.h \
    jsonloader.h \
    gaspump.h \
    rubble.h \
    field.h \
    omtdata.h \
    searchmodel.h \
    common.h \
    overmapspeciallayout.h \
    specialwizardpage.h \
    overmapspecialdata.h \
    omtwidget.h \
    omtwizardpage.h

FORMS    += buildingeditor.ui \
    generalwizardpage.ui \
    newbuildingwizard.ui \
    omtdialog.ui \
    overmapspeciallayout.ui \
    specialwizardpage.ui \
    omtwidget.ui \
    omtwizardpage.ui
