#Classes for work with 3D model

HEADERS +=  model_work/dialogs/modelattachmentdialog.h \
    model_work/storage/modelnode.h \
    model_work/storage/model.h \
    model_work/storage/mesh.h \
    model_work/dialogs/modelvaluesviewer.h \
    model_work/dialogs/pickshprogdialog.h \
    model_work/tools/assimpprogresshandler.h \
    model_work/tools/nodeshprogstruct.h

SOURCES +=  model_work/dialogs/modelattachmentdialog.cpp \
    model_work/storage/modelnode.cpp \
    model_work/storage/model.cpp \
    model_work/storage/mesh.cpp \
    model_work/dialogs/modelvaluesviewer.cpp \
    model_work/dialogs/pickshprogdialog.cpp \
    model_work/tools/assimpprogresshandler.cpp \
    model_work/tools/nodeshprogstruct.cpp

FORMS +=    model_work/dialogs/modelattachmentdialog.ui \
    model_work/dialogs/modelvaluesviewer.ui \
    model_work/dialogs/pickshprogdialog.ui
