#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "newscomponent.h"
#include <QString>
#include <QMainWindow>
#include <QJsonArray>

#include "trie.h"         // Asegúrate de incluir tu clase Trie
#include <QListWidget>
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
    void actualizarSugerencias(const QString& texto);
    void seleccionarSugerencia(QListWidgetItem* item);

    void on_configuracionButton_clicked();

private:
    Ui::MainWindow *ui;
    QJsonArray noticiasOriginales;
    QMap<QString, QList<QJsonObject>> indiceInvertido;
    void filtrarNoticias();
    void buscarPorTexto();
    QString clasificarTemaPorDescripcion(const QString& descripcion, const QMap<QString, QStringList>& palabrasClavePorTema);
    void limpiarOrdenamientosRadioButtons(); // ✅ Limpia todos los QRadioButton de ordenamiento
    Trie* trie;
    QListWidget* sugerenciasList;
    void inicializarAutocompletado(); // Método para organizar la inicialización

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};
#endif // MAINWINDOW_H
