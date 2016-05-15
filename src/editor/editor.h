#ifndef EDITOR_H
#define EDITOR_H

#include <QtWidgets/QWidget>
#include <QtGui/QStandardItemModel>

#include "highlighter.h"
#include "opengl.h"

class Editor : public QWidget
{
    public:
        Editor(QWidget *parent = 0);
    protected:
        QList<QStandardItem *> prepareRow(const QString &t_first, const QString &t_second = "");
    private:
        QTextEdit* m_edit;
        Highlighter* m_highlighter;
        Opengl* m_opengl;

};

#endif // EDITOR_H
