#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "newscomponent.h"
#include <QString>
#include <QMainWindow>
#include <QJsonArray>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actualizarButton_clicked();
private:
    Ui::MainWindow *ui;
    QJsonArray noticiasOriginales;
    void filtrarNoticias();
    QString clasificarTemaPorDescripcion(const QString& descripcion, const QMap<QString, QStringList>& palabrasClavePorTema);
    void limpiarOrdenamientosRadioButtons(); // ✅ Limpia todos los QRadioButton de ordenamiento
};
#endif // MAINWINDOW_H
