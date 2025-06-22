#ifndef NEWSCOMPONENT_H
#define NEWSCOMPONENT_H

#include <QWidget>

namespace Ui {
class NewsComponent;
}

class NewsComponent : public QWidget
{
    Q_OBJECT

public:
    explicit NewsComponent(QWidget *parent = nullptr);
    ~NewsComponent();

private:
    Ui::NewsComponent *ui;
};

#endif // NEWSCOMPONENT_H
