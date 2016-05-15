
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTreeView>
#include "editor.h"

Editor::Editor(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *hbox = new QHBoxLayout(this);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);
    m_opengl = new Opengl;

    // font set
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(12);

    // Text box machen
    m_edit = new QTextEdit;
    m_edit->setFont( font);
    m_highlighter = new Highlighter(m_edit->document());

    //l_opengl->setFrameShape(QFrame::StyledPanel);

    // l_editor_splitter
/*    QLabel* l_info = new QLabel;
    l_info->setFrameShape(QFrame::StyledPanel);
    l_info->setText( "test<br>\nTestTEst");*/

    QTreeView* l_info = new QTreeView;
    QStandardItemModel* l_standardModel = new QStandardItemModel ;
    QStandardItem* l_rootNode = l_standardModel->invisibleRootItem();

    // einige werte
    QList<QStandardItem*> americaItem = prepareRow("Image");
    QList<QStandardItem*> mexicoItem = prepareRow("Width", "100");
    QList<QStandardItem*> usaItem = prepareRow("Height", "100");
    americaItem.first()->appendRow(mexicoItem);
    americaItem.first()->appendRow(usaItem);
    l_rootNode->appendRow(americaItem);

    l_info->setModel( l_standardModel);
    l_info->expandAll();
    //l_info->setBaseSize( 100, 50);
    //l_info->setMaximumHeight( 10);
    l_info->setFixedHeight( 150);



    QSplitter *l_editor_splitter = new QSplitter(Qt::Vertical, this);
    l_editor_splitter->addWidget( l_info);
    l_editor_splitter->addWidget( m_edit);

    QSplitter *splitter1 = new QSplitter(Qt::Horizontal, this);

    splitter1->addWidget( l_editor_splitter);
    splitter1->addWidget( m_opengl);

    QFrame *bottom = new QFrame(this);
    bottom->setFrameShape(QFrame::StyledPanel);

    QSplitter *splitter2 = new QSplitter(Qt::Vertical, this);
    splitter2->addWidget(splitter1);
    splitter2->addWidget(bottom);

    QList<int> sizes;
    sizes.append( 10);
    sizes.append( 10);

    splitter2->setSizes(sizes);

    hbox->addWidget(splitter2);
}

QList<QStandardItem *> Editor::prepareRow(const QString &t_first, const QString &t_second)
{
    QList<QStandardItem *> rowItems;
    rowItems << new QStandardItem(t_first);
    rowItems << new QStandardItem(t_second);
    return rowItems;
}
