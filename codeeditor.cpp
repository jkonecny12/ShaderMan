#include <QtWidgets>

#include "codeeditor.h"

/**
 * @brief CodeEditor::CodeEditor Create empty code editor.
 * @param parent Get parent of this object.
 * @param type Type shader in editor.
 */
CodeEditor::CodeEditor(QWidget *parent, InfoManager::SHADERTYPE type) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    oldBlockNumber = 0;

    customConnect();

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    pathSet = false;

    // set shader type
    sType = type;

    // set syntax highlighter
    highlight = new Highlighter(this);
    highlight->setDocument(document());

    setDefaultSettings();
    createRegExps();
}

/**
 * @brief CodeEditor::CodeEditor Create empty code editor.
 * @param parent Get parent of this object.
 * @param path Path of opened file.
 * @param type Type of opened shader.
 */
CodeEditor::CodeEditor(QWidget *parent, QString path, InfoManager::SHADERTYPE type) :
    QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    oldBlockNumber = 0;

    customConnect();

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    pathSet = true;
    filePath = path;
    //textModif = false;

    // set shader type
    sType = type;

    // set syntax highlighter
    highlight = new Highlighter(this);
    highlight->setDocument(document());

    // read the file
    readFile();

    setDefaultSettings();
    createRegExps();
}

/**
 * @brief CodeEditor::customConnect Connect signals in this signal.
 */
void CodeEditor::customConnect()
{
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(autoIndendText(int)));
    connect(this, SIGNAL(textChanged()), this, SLOT(autoIndendBack()));
}

/**
 * @brief CodeEditor::setDefaultSettings Set default settings about editor.
 */
void CodeEditor::setDefaultSettings()
{
    int width = 4;

    width *= fontMetrics().width(' ');

    setTabStopWidth(width);
}

/**
 * @brief CodeEditor::createRegExps Create needed regual expressions.
 */
void CodeEditor::createRegExps()
{
    findIf.setPattern(".*\\bif\\s*\\(.*\\)\\s*$");
    findElse.setPattern(".*\\belse\\s*$");
    findWhile.setPattern(".*\\bwhile\\s*\\(.*\\)\\s*$");
    findFor.setPattern(".*\\bfor\\s*\\(.*\\)\\s*$");
    findSwitch.setPattern(".*\\bswitch\\s*\\(.*\\)\\s*$");

    findSwitchBracket.setPattern(".*\\bswitch\\s*\\(.*\\)\\s*\\{?\\s*$");
    findCase.setPattern(".*\\bcase\\s+.*:\\s*$");
    findDefault.setPattern(".*\\bdefault\\s*:\\s*$");
}

/**
 * @brief CodeEditor::setNewIndend Set indendation to current cursor.
 * @param num How many tabs we want to indend.
 */
void CodeEditor::setNewIndend(int num)
{
    QTextCursor newCursor = textCursor();
    QString block = newCursor.block().text();

    if(block.isEmpty())
    {
        for(int i = 0; i < num; ++i)
            newCursor.insertText(QString("\t"));
    }
}

/**
 * @brief CodeEditor::getBlockIndend Get indend of given block.
 * @param block Where we want to get indend.
 * @return Number of tabs.
 */
int CodeEditor::getBlockIndend(QString block)
{
    int count = 0;

    while(block.startsWith('\t'))
    {
        block.remove(0,1);
        count++;
    }

    return count;
}

/**
 * @brief CodeEditor::testBrackets Test brackets indendation and indend when needed.
 * @param block Test this block for indend.
 * @return True if indend, false if not.
 */
bool CodeEditor::testBrackets(QString block)
{
    int lBracket = block.count(QChar('{'));
    int rBracket = block.count(QChar('}'));

    if(lBracket > rBracket) // left brackets
    {
        int indend = getBlockIndend(block);

        setNewIndend(indend+1);

        qDebug() << "number of tabs: " << indend;

        return true;
    }
    else if(rBracket != 0)
    {
        int indend = getBlockIndend(block);

        setNewIndend(indend);

        return true;
    }
    else
        return false;
}

/**
 * @brief CodeEditor::createNewBlock Create if user press enter when typed { .
 */
void CodeEditor::createNewBlock()
{
    QTextCursor cursor = textCursor();

    cursor.movePosition(QTextCursor::Up);
    QString block = cursor.block().text();

    if(block.endsWith("{"))
    {
        if(testBracketsNumber())
            return;

        int indend = getBlockIndend(block);

        QString newBlock = "\n";
        for(int i = 0; i < indend; ++i)
        {
            newBlock.append('\t');
        }

        QTextCursor actual = textCursor();

        actual.insertText(newBlock + "}");
        actual.movePosition(QTextCursor::Up);
        setTextCursor(actual);
    }

}

/**
 * @brief CodeEditor::testBracketsNumber Test left brackets number is lower than right brackets number.
 * @return True if left bracket is less or equal then right brackets.
 */
bool CodeEditor::testBracketsNumber()
{
    QString text = toPlainText();

    int lBracket = text.count('{');
    int rBracket = text.count('}');

    if(lBracket > rBracket)
        return false;

    return true;
}

/**
 * @brief CodeEditor::testForIfElseCycles Test for if, else, for, while keywords and indend 1 new line after them.
 * @param block Block where we are testing.
 * @param wasSpace If there are more blocks from first test, we test more only if no character was on line between keyword and this line.
 * @return True if we indended false otherwise.
 */
bool CodeEditor::testForIfElseCycles(QString block, bool* wasSpace)
{
    if(!*wasSpace)
        return false;

    bool found = testKeywordPressence(block);

    if(found)
    {
        int inded = getBlockIndend(block);

        setNewIndend(inded+1);

        return true;
    }
    else
    {
        block = block.simplified();

        if(block.size() == 0)
            *wasSpace = true;
        else if(block.size() == 1)
        {
            if(block.at(0).isSpace())
                *wasSpace = true;
            else
                *wasSpace = false;
        }
        else
            *wasSpace = false;
    }

    return false;
}

/**
 * @brief CodeEditor::testCase Test this block for case presence. If there is a case, indend this block +1.
 * @param block Where we looking.
 * @return True if case was found, false otherwise.
 */
bool CodeEditor::testCase(QString block)
{
    if(findCase.exactMatch(block))
    {
        QTextCursor cursor = textCursor();

        int indend = getBlockIndend(block);

        setNewIndend(indend +1);
        return true;
    }

    return false;
}

/**
 * @brief CodeEditor::testKeywordPressence Test for keyword presence, do not indend.
 * Kaywords are: if, else, while, for, switch.
 * @param block Where we looking.
 * @return True if any of these keywords was found.
 */
bool CodeEditor::testKeywordPressence(QString block)
{
    bool found = findIf.exactMatch(block);

    if(!found)
        found = findElse.exactMatch(block);

    if(!found)
        found = findWhile.exactMatch(block);

    if(!found)
        found = findFor.exactMatch(block);

    if(!found)
        found = findSwitch.exactMatch(block);

    return found;
}

/**
 * @brief CodeEditor::testCaseKeyword Test if any case is on some uplines from position where is given cursor.
 * Cursor is moved up before it start looking. If switch was found before case, then return false.
 * @param cursor From where we start looking.
 * @return True if first found was case, false if nothing found or switch was first.
 */
bool CodeEditor::testCaseKeyword(QTextCursor& cursor)
{
    while(cursor.movePosition(QTextCursor::Up))
    {
        QString block = cursor.block().text();

        if(findCase.exactMatch(block))
        {
            return true;
        }
        else if(findSwitch.exactMatch(block) || findSwitchBracket.exactMatch(block))
        {
            return false;
        }
    }

    return false;
}

/**
 * @brief CodeEditor::testIfFromElse Test if there is a if when we typed else.
 * Start look from cursor position, before start we move cursor 1 line up.
 * This method counting brackets when looking for if.
 * @param cursor From where we start looking.
 * @return True if there is an if, false otherwise.
 */
bool CodeEditor::testIfFromElse(QTextCursor &cursor)
{
    int lBracket = 0;
    int rBracket = 0;

    while(cursor.movePosition(QTextCursor::Up))
    {
        QString block = cursor.block().text();

        lBracket += block.count('{');
        rBracket += block.count('}');

        if(findIf.exactMatch(block))
        {
            if(lBracket == rBracket)
                return true;
        }
    }

    return false;
}

/**
 * @brief CodeEditor::lineNumberAreaWidth Get width of the line numbering area.
 * Width depends on font and on how many numbers will be there.
 * @return Width in pixels.
 */
int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());

    while(max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

/**
 * @brief CodeEditor::updateLineNumberAreaWidth Update area for line numbering, when new block was created.
 */
void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

/**
 * @brief CodeEditor::setFilePath Set new file to this code editor.
 * @param path New path we setting here.
 */
void CodeEditor::setFilePath(QString path)
{
    filePath = path;
    pathSet = true;
}

/**
 * @brief CodeEditor::isPathSet Test if path is set to this code editor.
 * @return True if path is set, false otherwise.
 */
bool CodeEditor::isPathSet()
{
    return pathSet;
}

/**
 * @brief CodeEditor::saveFile Save setted file.
 * @return True if file was saved correctly, else false.
 */
bool CodeEditor::saveFile()
{
    QFile file(filePath);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << this->toPlainText();
        file.close();
        document()->setModified(false);
        emit fileSaved();
        return(true);
    }

    return false;
}

/**
 * @brief CodeEditor::saveFileAs Save this file to somewhere else, do not react on new file.
 * @param path Path where we want to save file.
 * @return True if file was correctly saved, false otherwise.
 */
bool CodeEditor::saveFileAs(QString path)
{
    QString text = toPlainText();

    QFile f(path);

    if(!QFileInfo(f).isWritable())
        return false;
    else
    {
        QTextStream out(&f);
        out << text;
        f.close();
        emit fileSaved();
    }

    return true;
}

/**
 * @brief CodeEditor::isModified Test if this editor windows was modified.
 * @return True if it was.
 */
bool CodeEditor::isModified()
{
    return document()->isModified();
}

/**
 * @brief CodeEditor::getFilePath Get file path setted to this code editor.
 * @return Path to the file.
 */
QString CodeEditor::getFilePath()
{
    return filePath;
}

/**
 * @brief CodeEditor::updateLineNumberArea Update number area of this editor.
 * @param rect Visible rectangle.
 * @param dy Diferrence between actual area and scrolled area in pixels.
 */
void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if(dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if(rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

/**
 * @brief CodeEditor::autoIndendText Automatic indendation of code.
 * @param newBlockCount Number of blocks in this editor.
 */
void CodeEditor::autoIndendText(int newBlockCount)
{
    if(oldBlockNumber >= newBlockCount)
    {
        oldBlockNumber = newBlockCount;
        return;
    }

    oldBlockNumber = newBlockCount;

    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    bool testIfSpace = true;

    //qDebug() << "Auto indend";
    //qDebug() << "Block size: " << cursor.block().length();
    //qDebug() << "Starting new block: " << cursor.atBlockStart();

    if(cursor.atBlockStart())
    {
        createNewBlock(); // if user typed {\n create }

        while(cursor.movePosition(QTextCursor::Up))
        {
            QString block = cursor.block().text();

            if(testBrackets(block))
                return;

            if(testForIfElseCycles(block, &testIfSpace))
                return;

            if(testCase(block))
                return;
        }
    }
}

/**
 * @brief CodeEditor::autoIndendBack Automatic indendation backward if needed etc if \n { (we need to indend backward '{').
 */
void CodeEditor::autoIndendBack()
{
    //qDebug() << "Auto indend back";

    QTextCursor cursor = textCursor();
    QString block = cursor.block().text();

    if(block.isEmpty())
        return;

    int blockPos = cursor.positionInBlock();

    blockPos--;

    if(block.size() < blockPos || blockPos < 0)
        return;

    QChar c = block.at(blockPos);

    //qDebug() << "Cursor characted " << c;

    if(c == '{') // test for cycles and if else
    {
        QTextCursor findCursor(cursor);

        findCursor.movePosition(QTextCursor::Up);

        QString findBlock = findCursor.block().text();

        bool found = testKeywordPressence(findBlock);

        if(found)
        {
            int indendOld = getBlockIndend(block);
            int indend = getBlockIndend(findBlock);

            if((indend+1) == indendOld)
            {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.deleteChar();
            }

            return;
        }
    }
    else if(c == ':')
    {
        if(findCase.exactMatch(block) || findDefault.exactMatch(block))
        {
            int indendOld = getBlockIndend(block);
            QTextCursor findCursor(cursor);

            if(testCaseKeyword(findCursor))
            {
                int indend = getBlockIndend(findCursor.block().text());

                if((indend+1) == indendOld)
                {
                    cursor.movePosition(QTextCursor::StartOfLine);
                    cursor.deleteChar();
                }
            }
        }
    }
    else if(c == 'e')
    {
        if(findElse.exactMatch(block))
        {
            int indendOld = getBlockIndend(block);
            QTextCursor findCursor(cursor);

            if(testIfFromElse(findCursor))
            {
                int indend = getBlockIndend(findCursor.block().text());

                if((indend+1) == indendOld)
                {
                    cursor.movePosition(QTextCursor::StartOfLine);
                    cursor.deleteChar();
                }
            }
        }
    }

}

/**
 * @brief CodeEditor::isExists Test if setted file exists.
 * @return True if file exists.
 */
bool CodeEditor::isExists()
{
    return QFile(filePath).exists();
}

/**
 * @brief CodeEditor::resizeEvent Overrided resize event, needed becouse line numbering.
 * @param e Resize event.
 */
void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

/**
  Read assigment file to editor
  */
void CodeEditor::readFile()
{
    QFile f(filePath);

    f.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream stream(&f);

    QString temp;
    // read file
    while(!stream.atEnd())
        temp += stream.readLine() + '\n';

    // remove last \n
    if(!temp.isEmpty())
        temp.chop(1);

    setPlainText(temp);

    f.close();
}

/**
 * @brief CodeEditor::highlightCurrentLine Highlight line where is cursor.
 */
void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

/**
 * @brief CodeEditor::lineNumberAreaPaintEvent Overrided event becouse of line numbering area.
 * @param event Paint event.
 */
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }

}
