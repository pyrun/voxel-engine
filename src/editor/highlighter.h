#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>

class Highlighter : public QSyntaxHighlighter
{
    //Q_OBJECT

public:
    Highlighter(QTextDocument *document);
protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat functionFormat;
};
#endif // HIGHLIGHTER_H
