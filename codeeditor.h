#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include "highlighter.h"
#include "infomanager.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;

/**
  Main editor
  */
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    CodeEditor(QWidget *parent = 0, InfoManager::SHADERTYPE type = InfoManager::VERTEX);
    CodeEditor(QWidget *parent, QString path, InfoManager::SHADERTYPE type = InfoManager::VERTEX);
    
    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth();

    void setFilePath(QString path);
    QString getFilePath();
    bool isPathSet();
    bool saveFile();

    bool saveFileAs(QString path);

    //bool isTextChanged();
    //void setTextModified(bool mod = true);
    bool isModified();

    bool isExists();


protected:
    void resizeEvent(QResizeEvent *e);
    void readFile();

private:
    void customConnect();
    void setDefaultSettings();
    void createRegExps();

    void setNewIndend(int num);
    int getBlockIndend(QString block);
    bool testBrackets(QString block);
    void createNewBlock();

    bool testBracketsNumber();

    bool testForIfElseCycles(QString block, bool *wasSpace);
    bool testCase(QString block);

    bool testKeywordPressence(QString block);

    bool testCaseKeyword(QTextCursor &cursor);
    bool testIfFromElse(QTextCursor &cursor);

public:


private:
    Highlighter* highlight;
    QWidget *lineNumberArea;
    QString filePath;
    bool pathSet;
    //bool textModif;
    InfoManager::SHADERTYPE sType;
    int oldBlockNumber;

    QRegExp findIf;
    QRegExp findElse;
    QRegExp findWhile;
    QRegExp findFor;
    QRegExp findSwitch;
    QRegExp findSwitchBracket;
    QRegExp findCase;
    QRegExp findDefault;

signals:
    void fileSaved();
    
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

    void autoIndendText(int newBlockCount);
    void autoIndendBack();
    
};


/**
  Numbering lines in editor
  */
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_H
