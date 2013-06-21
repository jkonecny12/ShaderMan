#ifndef OGL_LAYOUT_H
#define OGL_LAYOUT_H

//#define GLEW_STATIC
//#include "glew.h"
#include <QWidget>
#include <QGridLayout>
#include "oglwindow.h"
#include "logeditor.h"


/**
  Layout under OpenGL window
  */
class OGL_layout : public QWidget
{
    Q_OBJECT
public:
    explicit OGL_layout(QWidget *parent = 0);
    void createOpenGLWindow(QTextEdit* logWindow);
    OGLwindow* returnOGLwindow();

protected:
    OGLwindow* ogl_window;
    
signals:
    
public slots:
    
};

#endif // OGL_LAYOUT_H
