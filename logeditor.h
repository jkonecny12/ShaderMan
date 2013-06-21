#ifndef LOGEDITOR_H
#define LOGEDITOR_H

#include <QTextEdit>
#include <QColor>
#include "infomanager.h"

class QTextEdit;
class QColor;

class LogEditor
{
public:
    explicit LogEditor();
    explicit LogEditor(QTextEdit* logWindow);
    void setLogWindow(QTextEdit *log);

    // logging part
    void newCompiling();
    void addVertexLog(QString log);
    void addFragmentLog(QString log);
    void addLinkLog(QString log);
    void addFileError(QString log);
    void addUniformError(QString log);
    void addBufferError(QString log);
    void addToLog(QString str);
    void addToCompiling(QString str);

private:
    unsigned int maxLines;
    QTextEdit* window;
    QString oldLog;
    
signals:
    
public slots:
    
};

#endif // LOGEDITOR_H
