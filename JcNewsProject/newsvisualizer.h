#ifndef NEWSVISUALIZER_H
#define NEWSVISUALIZER_H
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <QDialog>

namespace Ui {
class NewsVisualizer;
}

class NewsVisualizer : public QDialog
{
    Q_OBJECT

public:
    explicit NewsVisualizer(QWidget *parent = nullptr);
    ~NewsVisualizer();

    void setIdUrl(const QString& id);
    void setFecha(const QDateTime& fecha);
    void setFuente(const QString& fuente);
    void setTema(const QString& tema);
    void setTitulo(const QString& titulo);
    void setTextPath(const QString& path);
    void setImgUrl(const QString& url);
    void actualizarResumen();

private:
    Ui::NewsVisualizer *ui;
    QString idUrl;
    QString textPath;
    QString imgUrl;
    QNetworkAccessManager* networkManager = nullptr;


private slots:
    void imagenDescargada(QNetworkReply* reply);
};

#endif // NEWSVISUALIZER_H
