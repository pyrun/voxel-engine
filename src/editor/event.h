#ifndef EVENT_H
#define EVENT_H

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtGui/QKeyEvent>

class Event : public QObject
{
    protected:
        bool eventFilter(QObject *obj, QEvent *event);
};

#endif // EVENT_H
