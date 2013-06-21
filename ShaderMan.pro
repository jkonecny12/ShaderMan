#-------------------------------------------------
#
# Project created by QtCreator 2012-08-12T13:17:18
#
#-------------------------------------------------

QT       += core gui widgets opengl

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ShaderMan
TEMPLATE = app

CONFIG += qt qwt
#CONFIG += warn_off release
#DEFINES += QT_NO_DEBUG_OUTPUT

# includes
unix {
    include(/usr/local/qwt-6.1.0/features/qwt.prf )
}

windows {
    include(C:/qwt-6.1.0/features/qwt.prf )
}

include(model_work/model.pri)
include(profiling/profile.pri)
include(texture/texture.pri)
include(uniform/uniform.pri)
include(project_settings/projectSettings.pri)

SOURCES += main.cpp\
    mainwindow.cpp \
    codeeditor.cpp \
    oglwindow.cpp \
    ogl_layout.cpp \
    tabeditor.cpp \
    infomanager.cpp \
    dialogs/newfile/newfiledialog.cpp \
    dialogs/new_project/newprojectdialog.cpp \
    dialogs/filetype/filetypedialog.cpp \
    logeditor.cpp \
    highlighter.cpp \
    treeview.cpp \
    meta_data/metashaderprog.cpp \
    meta_data/metaproject.cpp \
    meta_data/metashader.cpp \
    dialogs/shader_programs/attachshaderprogram.cpp \
    tools/lineeditdelegate.cpp \
    dialogs/program_manager/shprogvariablewidget.cpp \
    dialogs/program_manager/programmanagerdialog.cpp \
    dialogs/program_manager/shprogwidgetabstract.cpp \
    tools/shaderprogrammodel.cpp \
    dialogs/program_manager/setprognamedialog.cpp \
    dialogs/program_manager/shprogtexturewidget.cpp \
    storage/gltexture.cpp \
    tools/datatimer.cpp \
    storage/projectmanagertreemodel.cpp \
    storage/projecttreeitem.cpp

HEADERS  += mainwindow.h \
    codeeditor.h \
    oglwindow.h \
    ogl_layout.h \
    tabeditor.h \
    infomanager.h \
    treeview.h \
    dialogs/newfile/newfiledialog.h \
    dialogs/new_project/newprojectdialog.h \
    dialogs/filetype/filetypedialog.h \
    logeditor.h \
    highlighter.h \
    meta_data/metashaderprog.h \
    meta_data/metaproject.h \
    meta_data/metashader.h \
    dialogs/shader_programs/attachshaderprogram.h \
    tools/lineeditdelegate.h \
    dialogs/program_manager/shprogvariablewidget.h \
    dialogs/program_manager/programmanagerdialog.h \
    dialogs/program_manager/shprogwidgetabstract.h \
    tools/shaderprogrammodel.h \
    tools/utility.h \
    dialogs/program_manager/setprognamedialog.h \
    dialogs/program_manager/shprogtexturewidget.h \
    storage/gltexture.h \
    tools/datatimer.h \
    storage/projectmanagertreemodel.h \
    storage/projecttreeitem.h

FORMS    += mainwindow.ui \
    dialogs/newfile/newfiledialog.ui \
    dialogs/new_project/newprojectdialog.ui \
    dialogs/filetype/filetypedialog.ui \
    dialogs/shader_programs/attachshaderprogram.ui \
    dialogs/program_manager/programmanagerdialog.ui \
    dialogs/program_manager/setprognamedialog.ui

RESOURCES += \
    resource.qrc

win32: LIBS += -lglew32s -lassimp


unix:!macx: LIBS += -lGLEW -lassimp
