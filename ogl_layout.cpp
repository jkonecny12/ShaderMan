#include "ogl_layout.h"

OGL_layout::OGL_layout(QWidget *parent) :
    QWidget(parent)
{
}

/**
  Create grid layout and add to it OpenGL window
  Set logger to OpenGL window
  */
void OGL_layout::createOpenGLWindow(QTextEdit *logWindow)
{
    QGridLayout *ogl_gridLayout = new QGridLayout(this);
    ogl_gridLayout->setSpacing(6);
    ogl_gridLayout->setContentsMargins(11, 11, 11, 11);
    ogl_gridLayout->setObjectName(QString::fromUtf8("ogl_gridLayout"));

    // create OpenGL format
    QGLFormat glFormat;
    glFormat.setVersion( 3, 3); // OpenGL core version
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    ogl_window = new OGLwindow(glFormat, logWindow, this);

    ogl_gridLayout->addWidget(ogl_window);
}


/**
  Connect signals from parent to children slots
  */
OGLwindow* OGL_layout::returnOGLwindow()
{
    return ogl_window;
}
