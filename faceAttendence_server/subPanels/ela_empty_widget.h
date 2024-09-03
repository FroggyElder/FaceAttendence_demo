#ifndef ELA_EMPTY_WIDGET_H
#define ELA_EMPTY_WIDGET_H

//an empty Widget for testing

#include <QWidget>

class emptyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit emptyWidget(QWidget *parent = nullptr);

signals:
};

#endif // ELA_EMPTY_WIDGET_H
