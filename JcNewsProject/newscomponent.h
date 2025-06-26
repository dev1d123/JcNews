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
    void setDate(const QDateTime& date);
    void setFuente(const QString& fuente);
    void setTema(const QString& tema);
    void setTitle(const QString& titulo);

    ~NewsComponent();

private:
    Ui::NewsComponent *ui;
};

#endif // NEWSCOMPONENT_H
