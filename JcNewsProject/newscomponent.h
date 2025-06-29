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
    void setIdUrl(const QString& id);
    void setTextPath(const QString& path);
    void setImgUrl(const QString& url);

    ~NewsComponent();

private:
    Ui::NewsComponent *ui;
    QString idUrl;
    QString textPath;
    QString imgUrl;
};

#endif // NEWSCOMPONENT_H
