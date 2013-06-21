#Classes for work with uniform variables

HEADERS += \
    uniform/storage/uniformvariable.h \
    uniform/dialogs/comboboxdelegate.h \
    uniform/dialogs/createuniformvariables.h \
    uniform/dialogs/setvariablewidget.h \
    uniform/dialogs/uniformvariableeditor.h \
    uniform/storage/uniformcell.h \
    uniform/storage/specialvariableabstract.h \
    uniform/storage/specialvariableaction.h \
    uniform/storage/specialvariableactionpressed.h \
    uniform/storage/specialvariabletime.h \
    uniform/models/standardmodelunifvalidity.h \
    uniform/dialogs/generateVariableDialogs/selecttypedialog.h \
    uniform/dialogs/generateVariableDialogs/generateprojectionmatrix.h \
    uniform/models/multiplyunifmodel.h \
    uniform/models/combounifnamesdelegate.h \
    uniform/dialogs/generateVariableDialogs/matrixgenerator.h \
    uniform/dialogs/generateVariableDialogs/setnamewithbuttonsdialog.h

SOURCES += \
    uniform/storage/uniformvariable.cpp \
    uniform/dialogs/comboboxdelegate.cpp \
    uniform/dialogs/createuniformvariables.cpp \
    uniform/dialogs/setvariablewidget.cpp \
    uniform/dialogs/uniformvariableeditor.cpp \  #\
    uniform/models/standardmodelunifvalidity.cpp \
    uniform/dialogs/generateVariableDialogs/selecttypedialog.cpp \
    uniform/dialogs/generateVariableDialogs/generateprojectionmatrix.cpp \
    uniform/models/multiplyunifmodel.cpp \
    uniform/models/combounifnamesdelegate.cpp \
    uniform/dialogs/generateVariableDialogs/matrixgenerator.cpp \
    uniform/dialogs/generateVariableDialogs/setnamewithbuttonsdialog.cpp
    #uniform/storage/uniformcell.cpp

FORMS += \
    uniform/dialogs/createuniformvariables.ui \
    uniform/dialogs/uniformvariableeditor.ui \
    uniform/dialogs/generateVariableDialogs/selecttypedialog.ui \
    uniform/dialogs/generateVariableDialogs/generateprojectionmatrix.ui \
    uniform/dialogs/generateVariableDialogs/setnamewithbuttonsdialog.ui
