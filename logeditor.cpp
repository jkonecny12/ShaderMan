#include "logeditor.h"

LogEditor::LogEditor()
{
    window = NULL;
    maxLines = 500;
}

LogEditor::LogEditor(QTextEdit *logWindow)
{
    window = logWindow;
    maxLines = 500;
    window->document()->setMaximumBlockCount(maxLines);
}

/**
  Add window (QPlainTextEdit) where write log
  */
void LogEditor::setLogWindow(QTextEdit* log)
{
    window = log;
    window->document()->setMaximumBlockCount(maxLines);
}

/**
  Erase old log file
  */
void LogEditor::newCompiling()
{
    QString str = window->toPlainText();
    window->setTextColor(QColor(Qt::gray));
    window->setText(str);
    window->setTextColor(QColor(Qt::black));
}

/**
  Add QString from vertex shader to log
  */
void LogEditor::addVertexLog(QString log)
{
    window->setTextColor(QColor(Qt::blue));
    window->append("VERTEX SHADER");
    window->setTextColor(QColor(Qt::black));
    window->append(log);
    window->moveCursor(QTextCursor::End);
}

/**
  Add QString from fragment shader to log
  */
void LogEditor::addFragmentLog(QString log)
{
    window->setTextColor(QColor(Qt::blue));
    window->append("FRAGMENT SHADER");
    window->setTextColor(QColor(Qt::black));
    window->append(log);
    window->moveCursor(QTextCursor::End);
}

/**
  Add QString log from linking shader programs
  */
void LogEditor::addLinkLog(QString log)
{
    window->setTextColor(QColor(Qt::blue));
    window->append("LINKING SHADER");
    window->setTextColor(QColor(Qt::black));
    window->append(log);
    window->moveCursor(QTextCursor::End);
}

/**
 * @brief LogEditor::addFileError File missing or shader programs not set and etc.. errors
 * @param log Detailed information
 */
void LogEditor::addFileError(QString log)
{
    window->setTextColor(QColor(Qt::black));
    window->append(log);
    window->moveCursor(QTextCursor::End);
}

/**
 * @brief LogEditor::addUniformError Error log if uniform variable gives error
 * @param log Detailed information
 */
void LogEditor::addUniformError(QString log)
{
    window->setTextColor(QColor(Qt::black));
    window->append("UNIFORM VARIABLE ERROR");
    window->append(log);
    window->moveCursor(QTextCursor::End);
}

/**
 * @brief LogEditor::addBufferError Add log about buffer errors and warnings.
 * @param log Message for print
 */
void LogEditor::addBufferError(QString log)
{
    window->setTextColor(QColor(Qt::black));
    window->append("OPENGL BUFFER ERROR");
    window->append(log);
    window->moveCursor(QTextCursor::End);
}

/**
  Add QString to log window
  */
void LogEditor::addToLog(QString str)
{
    window->setPlainText(window->toPlainText() + str);
}

/**
 * @brief LogEditor::addToCompiling Add message to compile errors.
 * @param str String what we want to print in log window.
 */
void LogEditor::addToCompiling(QString str)
{
    window->setPlainText(window->toPlainText() + str);
}
