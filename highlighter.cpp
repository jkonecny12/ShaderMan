#include "highlighter.h"

Highlighter::Highlighter(QObject *parent) :
    QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywordPatterns;

    keywordPatterns << "\\battribute\\b" << "\\bconst\\b" << "\\buniform\\b" <<
                       "\\bvarying\\b" << "\\bbuffer\\b" << "\\bshared\\b" <<

                       "\\bcoherent\\b" << "\\bvolatile\\b" << "\\brestrict\\b" << "\\breadonly\\b" <<
                       "\\bwriteonly\\b" <<

                       "\\batomic_uint\\b" <<

                       "\\blayout\\b" <<
                       "\\bcentroid\\b" << "\\bflat\\b" << "\\bsmooth\\b" << "\\bnoperspective\\b" <<

                       "\\bpatch\\b" << "\\bsample\\b" <<

                       "\\bsubroutine\\b" <<

                       "\\bin\\b" << "\\bout\\b" << "\\binout\\b" <<


                       "\\btrue\\b" << "\\bfalse\\b" <<

                       "\\binvariant\\b" <<

                       "\\bdiscard\\b" << "\\breturn\\b" <<

                       "\\bmat2\\b" << "\\bmat3\\b" << "\\bmat4\\b" << "\\bdmat2\\b" <<
                       "\\bdmat3\\b" << "\\bdmat4\\b" <<
                       "\\bmat2x2\\b" << "\\bmat2x3\\b" << "\\bmat2x4\\b" << "\\bdmat2x2\\b" <<
                       "\\bdmat2x3\\b" << "\\bdmat2x4\\b" <<
                       "\\bmat3x2\\b" << "\\bmat3x3\\b" << "\\bmat3x4\\b" << "\\bdmat3x2\\b" <<
                       "\\bdmat3x3\\b" << "\\bdmat3x4\\b" <<
                       "\\bmat4x2\\b" << "\\bmat4x3\\b" << "\\bmat4x4\\b" << "\\bdmat4x2\\b" <<
                       "\\bdmat4x3\\b" << "\\bdmat4x4\\b" <<
                       "\\bvec2\\b" << "\\bvec3\\b" << "\\bvec4\\b" << "\\bivec2\\b" << "\\bivec3\\b" <<
                       "\\bivec4\\b" << "\\bbvec2\\b" << "\\bbvec3\\b" << "\\bbvec4\\b" <<
                       "\\bdvec2\\b" << "\\bdvec3\\b" << "\\bdvec4\\b" <<
                       "\\buvec2\\b" << "\\buvec3\\b" << "\\buvec4\\b" <<

                       "\\blowp\\b" << "\\bmediump\\b" << "\\bhighp\\b" << "\\bprecision\\b" <<

                       "\\bsampler1D\\b" << "\\bsampler2D\\b" << "\\bsampler3D\\b" << "\\bsamplerCube\\b" <<
                       "\\bsampler1DShadow\\b" << "\\bsampler2DShadow\\b" << "\\bsamplerCubeShadow\\b" <<
                       "\\bsampler1DArray\\b" << "\\bsampler2DArray\\b" <<
                       "\\bsampler1DArrayShadow\\b" << "\\bsampler2DArrayShadow\\b" <<
                       "\\bisampler1D\\b" << "\\bisampler2D\\b" << "\\bisampler3D\\b" << "\\bisamplerCube\\b" <<
                       "\\bisampler1DArray\\b" << "\\bisampler2DArray\\b" <<
                       "\\busampler1D\\b" << "\\busampler2D\\b" << "\\busampler3D\\b" << "\\busamplerCube\\b" <<
                       "\\busampler1DArray\\b" << "\\busampler2DArray\\b" <<
                       "\\bsampler2DRect\\b" << "\\bsampler2DRectShadow\\b" <<
                       "\\bisampler2DRect\\b" << "\\busampler2DRect\\b" <<
                       "\\bsamplerBuffer\\b" << "\\bisamplerBuffer\\b" << "\\busamplerBuffer\\b" <<
                       "\\bsampler2DMS\\b" << "\\bisampler2DMS\\b" << "\\busampler2DMS\\b" <<
                       "\\bsampler2DMSArray\\b" << "\\bisampler2DMSArray\\b" << "\\busampler2DMSArray\\b" <<
                       "\\bsamplerCubeArray\\b" << "\\bsamplerCubeArrayShadow\\b" <<
                       "\\bisamplerCubeArray\\b" << "\\busamplerCubeArray\\b" <<

                       "\\bimage1D\\b" << "\\biimage1D\\b" << "\\buimage1D\\b" <<
                       "\\bimage2D\\b" << "\\biimage2D\\b" << "\\buimage2D\\b" <<
                       "\\bimage3D\\b" << "\\biimage3D\\b" << "\\buimage3D\\b" <<
                       "\\bimage2DRect\\b" << "\\biimage2DRect\\b" << "\\buimage2DRect\\b" <<
                       "\\bimageCube\\b" << "\\biimageCube\\b" << "\\buimageCube\\b" <<
                       "\\bimageBuffer\\b" << "\\biimageBuffer\\b" << "\\buimageBuffer\\b" <<
                       "\\bimage1DArray\\b" << "\\biimage1DArray\\b" << "\\buimage1DArray\\b" <<
                       "\\bimage2DArray\\b" << "\\biimage2DArray\\b" << "\\buimage2DArray\\b" <<
                       "\\bimageCubeArray\\b" << "\\biimageCubeArray\\b" << "\\buimageCubeArray\\b" <<
                       "\\bimage2DMS\\b" << "\\biimage2DMS\\b" << "\\buimage2DMS\\b" <<
                       "\\bimage2DMSArray\\b" << "\\biimage2DMSArray\\b" << "\\buimage2DMSArray\\b" <<
                       "\\bstruct\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    QStringList controlKeywords;
    controlKeywordFormat.setForeground(Qt::darkCyan);
    controlKeywordFormat.setFontWeight(QFont::Bold);
    controlKeywords << "\\bbreak\\b" << "\\bcontinue\\b" << "\\bdo\\b" << "\\bfor\\b" << "\\bwhile\\b" <<
                           "\\bswitch\\b" << "\\bcase\\b" << "\\bdefault\\b" <<
                           "\\bif\\b" << "\\belse\\b";

    foreach(const QString pattern, controlKeywords)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = controlKeywordFormat;
        highlightingRules.append(rule);
    }

    QStringList varTypesKeyword;
    varTypesKeywordFormat.setForeground(Qt::darkGreen);
    varTypesKeywordFormat.setFontWeight(QFont::Bold);
    varTypesKeyword << "\\bfloat\\b" << "\\bdouble\\b" << "\\bint\\b" << "\\bvoid\\b" << "\\bbool\\b" <<
                        "\\buint\\b";

    foreach(const QString pattern, varTypesKeyword)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = varTypesKeywordFormat;
        highlightingRules.append(rule);
    }


    // highlight functions
    /*
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);
    */

    // highlight comments (one line)
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    //multiline comment
    multiLineCommentFormat.setForeground(Qt::red);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}


void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    // highlight multicomment line
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
             setCurrentBlockState(1);
             commentLength = text.length() - startIndex;
        } else {
             commentLength = endIndex - startIndex
                + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
