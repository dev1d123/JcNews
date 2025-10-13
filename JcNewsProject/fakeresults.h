#ifndef FAKERESULTS_H
#define FAKERESULTS_H

#include <QDialog>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QRegularExpression>
#include <QMap>
#include <QSet>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextBlockFormat>
#include <QRandomGenerator>
#include <QStringList>
#include <QVector>

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

    // Algoritmos mejorados
    void algo1(); // análisis basado en contenido (TF-IDF + similitud coseno + verificación de fuente/recencia)
    void algo2(); // análisis de estilo (más heurísticas)
    QStringList tokenizeText(const QString &texto);
    QStringList tokenizeWithBigrams(const QString &texto); // unigramas + bigramas
    double similitudCoseno(const QVector<double> &a, const QVector<double> &b);
    double similitudJaccard(const QSet<QString> &a, const QSet<QString> &b);
    QVector<double> buildTfIdfVector(const QStringList &tokens, const QMap<QString,int> &vocab, const QVector<double> &idf);
    QVector<double> computeIdf(const QList<QStringList> &docs, const QMap<QString,int> &vocab);
    double computeStyleRisk(const QString &texto);
    int estimateSourceReputation(const QString &fuente); // 0..100
    qint64 parseDateToTimestamp(const QString &fecha); // intenta parsear varias formas
    int approxSyllableCount(const QString &word);
    QString normalizeToken(const QString &token);

    // parámetros configurables
    double contentWeight = 0.60;  // antes 0.2 → ahora tiene más peso en la veracidad
    double styleWeight   = 0.15;  // antes 0.25 → penaliza menos por sensacionalismo
    double sourceWeight  = 0.55;  // antes 0.15 → fuentes confiables impulsan más el score

};

#endif // FAKERESULTS_H
