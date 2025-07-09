#ifndef FAKERESULTS_H
#define FAKERESULTS_H

#include <QDialog>
#include <QSet>
#include <QStringList>

namespace Ui {
class FakeResults;
}

class FakeResults : public QDialog
{
    Q_OBJECT

public:
    explicit FakeResults(QWidget *parent = nullptr);
    ~FakeResults();

private:
    Ui::FakeResults *ui;

    // Algoritmos principales
    void algo1();  // Comparación semántica con JSON
    void algo2();  // Análisis de estilo textual

    // Auxiliares
    QStringList tokenizeText(const QString &texto);
    double similitudJaccard(const QSet<QString> &a, const QSet<QString> &b);
};

#endif // FAKERESULTS_H
