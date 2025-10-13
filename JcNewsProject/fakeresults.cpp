#include "fakeresults.h"
#include "ui_fakeresults.h"

#include <cmath>
#include <algorithm>
#include <numeric>

#include <QSettings>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>

FakeResults::FakeResults(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FakeResults)
{
    ui->setupUi(this);
    setWindowTitle("Resultados Fake (mejorado)");

    // Mostrar título guardado
    QSettings settings("JCNews", "FakeNew");
    QString titulo = settings.value("news/title").toString();
    ui->titleEdit->setText(titulo);

    algo1();
    algo2();

    connect(ui->atrasButton, &QPushButton::clicked, this, &QDialog::reject);
}

FakeResults::~FakeResults()
{
    delete ui;
}

/*
 * ALGO 1 - Análisis de contenido mejorado:
 * - Tokenización con unigramas + bigramas
 * - Vector TF-IDF y similitud coseno con todo el corpus (metadatos_noticias.json)
 * - Jaccard como evidencia complementaria
 * - Ajustes por recencia y reputación de la fuente
 * - Resultado final combinado y explicado
 */

void FakeResults::algo1() {
    int stepIdx = 0;
    auto step = [&](const QString &msg) {
        QMessageBox::information(this, "Debug algo1", QString("[%1] %2").arg(++stepIdx).arg(msg));
    };

    QSettings settingsFake("JCNews", "FakeNew");
    QString contenidoActual = settingsFake.value("news/content").toString();
    if (contenidoActual.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Debug algo1", "No hay contenido de noticia para analizar (news/content vacío).");
        ui->resultado1Text->setHtml("<b style='color:orange;'>Aviso:</b> No hay contenido de noticia para analizar.");
        return;
    }
    step(QString("Contenido actual cargado. Longitud: %1 caracteres").arg(contenidoActual.size()));

    // 1) Cargar corpus (usar 'JCNews' para ser consistente)
    QSettings settingsConfig("JcNews", "config");
    QString rutaGuardada = settingsConfig.value("dir_news").toString();
    QString jsonPath = QDir(rutaGuardada).filePath("metadatos_noticias.json");
    step(QString("Ruta de corpus: %1").arg(jsonPath));
    step(QString("Ruta de dir_news: %1").arg(rutaGuardada));

    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Debug algo1", "No se pudo abrir el archivo: " + jsonPath);
        ui->resultado1Text->setHtml("<b style='color:red;'>Error:</b> No se pudo abrir el archivo:<br>" + jsonPath);
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    step(QString("Archivo JSON leído. Bytes: %1").arg(jsonData.size()));

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (doc.isNull() || !doc.isArray()) {
        QMessageBox::critical(this, "Debug algo1", "Error al parsear JSON: " + parseError.errorString());
        ui->resultado1Text->setHtml("<b style='color:red;'>Error:</b> Error al parsear JSON:<br>" + parseError.errorString());
        return;
    }

    QJsonArray noticias = doc.array();
    if (noticias.isEmpty()) {
        QMessageBox::warning(this, "Debug algo1", "El archivo JSON no contiene noticias.");
        ui->resultado1Text->setHtml("<b>Aviso:</b> El archivo JSON no contiene noticias.");
        return;
    }
    step(QString("Noticias en corpus: %1").arg(noticias.size()));

    // 2) Tokenizar corpus (unigramas + bigramas)
    QList<QStringList> tokenizaciones;
    QList<QJsonObject> objetos;
    tokenizaciones.reserve(noticias.size());
    objetos.reserve(noticias.size());

    for (const QJsonValue &val : noticias) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();
        objetos.append(obj);

        QString titulo = obj.value("titulo").toString();
        QString descripcion = obj.value("descripcion").toString();
        QString fuente = obj.value("fuente").toString();
        QString fecha = obj.value("fecha").toString();

        QString combinado = titulo + " " + descripcion + " " + fuente + " " + fecha;
        tokenizaciones.append(tokenizeWithBigrams(combinado));
    }
    step(QString("Tokenización del corpus lista. Docs tokenizados: %1").arg(tokenizaciones.size()));

    // Tokenizar noticia actual
    QStringList tokensActual = tokenizeWithBigrams(contenidoActual);
    step(QString("Tokens (unigramas + bigramas) noticia actual: %1").arg(tokensActual.size()));

    // 3) Construir vocabulario
    QMap<QString,int> vocab;
    for (const QStringList &tokens : tokenizaciones) {
        for (const QString &t : tokens) {
            if (!vocab.contains(t)) {
                int idx = vocab.size();
                vocab.insert(t, idx);
            }
        }
    }
    // incluir tokens de la noticia actual
    for (const QString &t : tokensActual) {
        if (!vocab.contains(t)) {
            int idx = vocab.size();
            vocab.insert(t, idx);
        }
    }
    step(QString("Vocabulario construido. Tamaño: %1").arg(vocab.size()));

    // 4) Calcular IDF
    QVector<double> idf = computeIdf(tokenizaciones + QList<QStringList>({tokensActual}), vocab);
    if (idf.size() != vocab.size()) {
        QMessageBox::warning(this, "Debug algo1", QString("Tamaño IDF (%1) != vocab (%2)").arg(idf.size()).arg(vocab.size()));
    }
    step(QString("IDF calculado. Dimensión: %1").arg(idf.size()));

    // 5) Construir vectores TF-IDF y normalizar
    QVector<double> vecActual = buildTfIdfVector(tokensActual, vocab, idf);

    QList<QVector<double>> vectoresDocs;
    vectoresDocs.reserve(tokenizaciones.size());
    for (const QStringList &toks : tokenizaciones) {
        vectoresDocs.append(buildTfIdfVector(toks, vocab, idf));
    }
    step(QString("Vectores TF-IDF generados. Actual dim=%1, Docs=%2")
             .arg(vecActual.size())
             .arg(vectoresDocs.size()));

    // 6) Similitudes (coseno) + Jaccard token-level (unigramas)
    QStringList tokens = tokenizeText(contenidoActual);
    QSet<QString> unigramSetActual;
    for (const QString &token : tokens) {
        unigramSetActual.insert(token);
    }
    struct DocScore {
        int id;
        double coseno;
        double jaccard;
        double combinedContent; // combinación de evidencias de contenido
        double recencyFactor;
        double sourceRep;
    };
    QList<DocScore> scores;

    qint64 nowTs = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();

    for (int i = 0; i < vectoresDocs.size(); ++i) {
        double cos = 0.0;
        if (vectoresDocs[i].size() == vecActual.size()) {
            cos = similitudCoseno(vecActual, vectoresDocs[i]);
        } else {
            // Dimensiones desalineadas (no debería pasar), dejar cos=0 y continuar
        }

        // Jaccard con unigramas: usar una lista temporal única (evitar temporales dobles)
        QString combinedTD = objetos[i].value("titulo").toString() + " " +
                             objetos[i].value("descripcion").toString();
        QStringList docUnigramsList = tokenizeText(combinedTD);
        QSet<QString> unigramsDoc;
        for (const QString &tok : docUnigramsList) unigramsDoc.insert(tok);
        double jac = similitudJaccard(unigramSetActual, unigramsDoc);

        // recency: parse fecha si existe
        double recencyFactor = 1.0;
        qint64 tsDoc = parseDateToTimestamp(objetos[i].value("fecha").toString());
        if (tsDoc > 0) {
            qint64 days = std::max<qint64>(1, (nowTs - tsDoc) / 86400);
            // decaimiento exponencial; noticias muy recientes contribuyen ligeramente más
            recencyFactor = std::exp(- (days / 30.0)); // ~1 for very recent, ->0 for old
            recencyFactor = std::clamp(recencyFactor, 0.05, 1.0);
        }

        // reputación
        double rep = 1;

        // contenido combinado (pondera coseno más que jaccard)
        double combined = cos * 0.75 + jac * 0.25;
        // aplicar recency como multiplicador suave
        combined *= (0.6 + 0.4 * recencyFactor); // recency: [0.6,1.0]

        scores.append({i, cos, jac, combined, recencyFactor, rep});
    }
    step(QString("Similitudes calculadas. Documentos evaluados: %1").arg(scores.size()));

    // 7) Ordenar por combinedContent descendente y tomar top-k
    std::sort(scores.begin(), scores.end(), [](const DocScore &a, const DocScore &b) {
        return a.combinedContent > b.combinedContent;
    });

    int topK = std::min(8, static_cast<int>(scores.size()));
    QList<DocScore> topScores = scores.mid(0, topK);
    step(QString("TopK listo. K=%1").arg(topK));

    // 8) Estimar score de contenido como media ponderada por reputación y recency
    double sumWeights = 0.0;
    double weightedContent = 0.0;
    for (const DocScore &ds : topScores) {
        double w = 1.0 + ds.sourceRep; // aumentar peso si la fuente tiene reputación alta
        weightedContent += ds.combinedContent * w;
        sumWeights += w;
    }
    double contentScore = (sumWeights > 0) ? (weightedContent / sumWeights) : 0.0;
    contentScore = std::clamp(contentScore, 0.0, 1.0);
    step(QString("Puntuación de contenido calculada: %1").arg(QString::number(contentScore, 'f', 3)));

    // 9) Estimar style risk (0..1)
    double styleRisk = computeStyleRisk(contenidoActual); // 0=fuerte estilo neutral, 1=estilo altamente sensational
    step(QString("Riesgo de estilo: %1").arg(QString::number(styleRisk, 'f', 3)));

    // 10) Source reputation directa de nota actual
    QString fuenteActual = objetos.isEmpty() ? QString() : objetos[0].value("fuente").toString();
    double sourceRep = 1;
    step(QString("Reputación de la fuente actual: %1% (%2)")
             .arg(QString::number(sourceRep * 100.0, 'f', 0))
             .arg(fuenteActual));

    // 11) Combinar evidencias:
    //   final_true_score = contentWeight * contentScore + styleWeight * (1 - styleRisk) + sourceWeight * sourceRep

    double finalTrueScore = contentWeight * contentScore + styleWeight * (1.0 - styleRisk) + sourceWeight * sourceRep;
    finalTrueScore = std::clamp(finalTrueScore, 0.0, 1.0);

    // Calibraciones adicionales:
    // - Si contentScore alto (>0.75) AND many top sources are reputable -> small boost
    int goodSources = 0;
    for (const DocScore &ds : topScores) if (ds.sourceRep >= 0.7) goodSources++;
    if (contentScore > 0.75 && goodSources >= 2) finalTrueScore = std::min(1.0, finalTrueScore + 0.05);

    // - If styleRisk very high, penalize further
    if (styleRisk > 0.8) finalTrueScore = std::max(0.0, finalTrueScore - 0.08);

    double probVerdadera = finalTrueScore * 100.0;
    double probFalsa = 100.0 - probVerdadera;
    step(QString("Scores finales: Verdadera=%1% | Falsa=%2%")
             .arg(QString::number(probVerdadera, 'f', 2))
             .arg(QString::number(probFalsa, 'f', 2)));

    // Construir HTML explicativo
    QString html;
    html += "<h2>Análisis avanzado de similitud y veracidad</h2>";
    html += QString("<p><b>Tokens (unigramas + bigramas) noticia actual:</b> %1</p>").arg(tokensActual.size());
    html += QString("<p><b>Total noticias en corpus:</b> %1</p>").arg(noticias.size());
    html += QString("<p><b>Top %1 noticias similares analizadas:</b></p>").arg(topK);

    html += "<ol>";
    for (const DocScore &ds : topScores) {
        QJsonObject obj = noticias[ds.id].toObject();
        QString titulo = obj.value("titulo").toString();
        QString descripcion = obj.value("descripcion").toString();
        QString fuente = obj.value("fuente").toString();
        QString fecha = obj.value("fecha").toString();

        html += "<li>";
        html += QString("<h3 style='margin-bottom:0;'>%1</h3>").arg(titulo.toHtmlEscaped());
        html += QString("<p><b>Similitud (TF-IDF coseno):</b> %1</p>").arg(QString::number(ds.coseno, 'f', 3));
        html += QString("<p><b>Similitud (Jaccard):</b> %1</p>").arg(QString::number(ds.jaccard, 'f', 3));
        html += QString("<p><b>Combinado (contenido):</b> %1</p>").arg(QString::number(ds.combinedContent, 'f', 3));
        html += QString("<p><b>Recencia (factor):</b> %1</p>").arg(QString::number(ds.recencyFactor, 'f', 3));
        html += QString("<p><b>Reputación fuente:</b> %1%</p>").arg(QString::number(ds.sourceRep * 100.0, 'f', 0));
        html += QString("<p><b>Fecha:</b> %1</p>").arg(fecha.toHtmlEscaped());
        html += QString("<p><b>Descripción:</b> %1</p>").arg(descripcion.toHtmlEscaped());
        html += QString("<p><b>Fuente:</b> %1</p>").arg(fuente.toHtmlEscaped());
        html += "</li><hr>";
    }
    html += "</ol>";

    html += "<h2>Estimación final</h2>";
    html += QString("<p><b>Puntuación de contenido (0..1):</b> %1</p>").arg(QString::number(contentScore, 'f', 3));
    html += QString("<p><b>Riesgo de estilo (0..1):</b> %1</p>").arg(QString::number(styleRisk, 'f', 3));
    html += QString("<p><b>Reputación de la fuente:</b> %1%</p>").arg(QString::number(sourceRep * 100.0, 'f', 0));

    html += QString("<h3 style='color:%1;'>Probabilidad de ser noticia <b>verdadera</b>: <span style='font-size:22px;'>%2%</span></h3>")
                .arg(probVerdadera >= 60 ? "#27ae60" : "#f39c12")
                .arg(QString::number(probVerdadera, 'f', 2));

    html += QString("<p>Probabilidad de ser <b>falsa</b>: <b style='color:red;'>%1%</b></p>").arg(QString::number(probFalsa, 'f', 2));
    html += "<p style='font-size:12px; color:#666;'>Nota: Este sistema combina evidencia de contenido (similitud con otras noticias), rasgos de estilo (sensacionalismo), y reputación de fuentes. No sustituye una verificación humana.</p>";

    ui->resultado1Text->setHtml(html);
}

/*
 * Tokenización simple para unigramas (sin signos) - utilizada por Jaccard
 */
QStringList FakeResults::tokenizeText(const QString &texto) {
    static const QSet<QString> stopwords_comunes = {
        "el", "la", "los", "las", "un", "una", "unos", "unas",
        "de", "del", "en", "con", "para", "por", "y", "o", "a", "al",
        "que", "se", "su", "lo", "le", "es", "ha", "han", "como", "que", "este", "esta",
        "pero", "más", "mas", "si", "no", "ya", "o"
    };

    QString limpio = texto.toLower();
    // mantener letras latinas y tildes y números para casos como "covid-19"
    limpio.remove(QRegularExpression("[^a-z0-9áéíóúñü\\s]"));
    QStringList tokens = limpio.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    QStringList utiles;
    for (const QString &token : tokens) {
        QString t = token.trimmed();
        if (t.isEmpty()) continue;
        if (!stopwords_comunes.contains(t)) {
            utiles << normalizeToken(t);
        }
    }
    return utiles;
}

/*
 * Tokenización que incluye bigramas (unigrams + concatenated bigrams)
 * Esto ayuda a captar frases cortas que son significativas (e.g., "falso positivo")
 */
QStringList FakeResults::tokenizeWithBigrams(const QString &texto) {
    QStringList unigrams = tokenizeText(texto);
    QStringList out = unigrams;
    for (int i = 0; i + 1 < unigrams.size(); ++i) {
        out << (unigrams[i] + "_" + unigrams[i+1]);
    }
    return out;
}

/*
 * Construye vector TF-IDF (no demasiado costoso)
 */
QVector<double> FakeResults::buildTfIdfVector(const QStringList &tokens, const QMap<QString,int> &vocab, const QVector<double> &idf) {
    int V = vocab.size();
    QVector<double> vec(V);
    vec.fill(0.0);
    for (const QString &t : tokens) {
        auto it = vocab.find(t);
        if (it != vocab.end()) {
            vec[it.value()] += 1.0;
        }
    }
    // TF normalization (log1p)
    for (int i = 0; i < V; ++i) {
        if (vec[i] > 0.0) {
            // FIX: usar acceso por índice en QVector
            vec[i] = std::log1p(vec[i]) * ((i >= 0 && i < idf.size()) ? idf[i] : 1.0);
        } else {
            vec[i] = 0.0;
        }
    }
    // L2 normalization
    double sumSq = 0.0;
    for (double v : vec) sumSq += v * v;
    double norm = (sumSq > 0.0) ? std::sqrt(sumSq) : 1.0;
    for (double &v : vec) v /= norm;
    return vec;
}

/*
 * Calculo IDF: idf[i] = log((N + 1) / (df_i + 1)) + 1
 * Se retorna un QVector indexado por vocab index.
 */
QVector<double> FakeResults::computeIdf(const QList<QStringList> &docs, const QMap<QString,int> &vocab) {
    int V = vocab.size();
    int N = docs.size();
    QVector<int> df(V);
    df.fill(0);
    for (const QStringList &doc : docs) {
        QSet<int> seen;
        for (const QString &t : doc) {
            auto it = vocab.find(t);
            if (it != vocab.end()) seen.insert(it.value());
        }
        for (int idx : seen) df[idx] += 1;
    }
    QVector<double> idf(V);
    for (int i = 0; i < V; ++i) {
        idf[i] = std::log((N + 1.0) / (df[i] + 1.0)) + 1.0;
    }
    return idf;
}

/*
 * Similitud coseno entre vectores (ya normalizados, pero calculamos seguro)
 */
double FakeResults::similitudCoseno(const QVector<double> &a, const QVector<double> &b) {
    if (a.size() != b.size()) return 0.0;
    double dot = 0.0;
    double na = 0.0, nb = 0.0;
    for (int i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        na += a[i] * a[i];
        nb += b[i] * b[i];
    }
    double denom = std::sqrt(na) * std::sqrt(nb);
    if (denom == 0.0) return 0.0;
    return std::clamp(dot / denom, 0.0, 1.0);
}

/*
 * Jaccard en sets de strings
 */
double FakeResults::similitudJaccard(const QSet<QString> &a, const QSet<QString> &b) {
    if (a.isEmpty() && b.isEmpty()) return 0.0;
    QSet<QString> inter = a;
    inter.intersect(b);
    QSet<QString> uni = a;
    uni.unite(b);
    return static_cast<double>(inter.size()) / uni.size();
}


/*
 * Parsear fecha a timestamp unix (segundos UTC). Soporta varios formatos comunes.
 */
qint64 FakeResults::parseDateToTimestamp(const QString &fecha) {
    if (fecha.trimmed().isEmpty()) return 0;
    // Intentar ISO
    QDateTime dt = QDateTime::fromString(fecha, Qt::ISODate);
    if (dt.isValid()) return dt.toSecsSinceEpoch();
    // algunos formatos comunes
    QStringList formatos = {
        "yyyy-MM-dd",
        "dd/MM/yyyy",
        "dd-MM-yyyy",
        "yyyy/MM/dd",
        "dd MMM yyyy",
        "d MMM yyyy",
        "yyyy-MM-ddThh:mm:ssZ",
        "yyyy-MM-dd hh:mm:ss"
    };
    for (const QString &fmt : formatos) {
        dt = QDateTime::fromString(fecha, fmt);
        if (dt.isValid()) return dt.toSecsSinceEpoch();
    }
    // fallback: intentar parseo libre con QDateTime (locale)
    dt = QDateTime::fromString(fecha, Qt::TextDate);
    if (dt.isValid()) return dt.toSecsSinceEpoch();
    return 0;
}

/*
 * ALGO 2 - Mejoras en análisis de estilo.
 * Heurísticas:
 * - proporción de palabras en mayúsculas (relativas)
 * - densidad de signos de exclamación/pregunta por longitud
 * - longitud media de frase y su desviación
 * - lectura estimada (Flesch-like aproximado usando sílabas)
 * - conteo de palabras "clickbait"/alerta
 * - detección de URLs y llamadas a la acción
 *
 * Retorna riesgo en [0,1] (1 = muy sensacionalista)
 */
double FakeResults::computeStyleRisk(const QString &texto) {
    if (texto.trimmed().isEmpty()) return 0.0;

    QString t = texto;
    int totalChars = t.size();
    QStringList words = t.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    int totalWords = words.size();
    if (totalWords == 0) return 0.0;

    // Mayúsculas: contar palabras que están en mayúsculas (y contiene letras)
    int uppercaseWords = 0;
    for (const QString &w : words) {
        if (w.length() > 1 && w == w.toUpper() && w.contains(QRegularExpression("[A-ZÁÉÍÓÚÑ]"))) {
            uppercaseWords++;
        }
    }
    double uppercaseRatio = static_cast<double>(uppercaseWords) / totalWords; // 0..1

    // Signos
    int excl = texto.count("!") + texto.count("¡");
    int inter = texto.count("?") + texto.count("¿");
    double punctDensity = static_cast<double>(excl + inter) / std::max(1, totalWords); // signos por palabra

    // Frases y longitud media
    QStringList sentences = t.split(QRegularExpression("[\\.\\!\\?]+"), Qt::SkipEmptyParts);
    double avgWordsPerSentence = 0.0;
    QVector<int> sentenceLens;
    for (const QString &s : sentences) {
        QStringList ws = s.trimmed().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        sentenceLens.append(ws.size());
    }
    if (!sentenceLens.isEmpty()) {
        double sum = 0.0;
        for (int v : sentenceLens) sum += v;
        avgWordsPerSentence = sum / sentenceLens.size();
    } else {
        avgWordsPerSentence = totalWords;
    }

    // Lecturabilidad aproximada (Flesch-like): se necesita contar sílabas
    int totalSyllables = 0;
    for (const QString &w : words) {
        totalSyllables += approxSyllableCount(w);
    }
    double fleschLike = 0.0;
    if (totalWords > 0) {
        // formula adaptada: 206.835 - 1.015*(words/sentences) - 84.6*(syllables/words)
        double sentencesCount = std::max(1.0, static_cast<double>(sentences.size()));
        fleschLike = 206.835 - 1.015 * (totalWords / sentencesCount) - 84.6 * (static_cast<double>(totalSyllables) / totalWords);
        // normalizar inversamente: texto menos legible -> mayor riesgo
        // Typical Flesch: 0..100; high is easy. We convert to [0,1] risk by (100 - score)/100
        fleschLike = std::clamp((100.0 - fleschLike) / 100.0, 0.0, 1.0);
    }

    // palabras de alarma / clickbait
    QStringList alerta = {"compártelo", "difunde", "increíble", "urgente", "alerta", "inexplicable", "impactante",
                          "no vas a creer", "lo peor", "asombroso", "click aquí", "comparte", "viral", "señalado", "escándalo"};
    int alertaCount = 0;
    QString lower = texto.toLower();
    for (const QString &a : alerta) {
        if (lower.contains(a)) alertaCount++;
    }
    double alertaScore = std::min(1.0, alertaCount / 3.0);

    // URLs o llamadas a la acción
    int urls = 0;
    QRegularExpression urlRe("(https?://[^\\s]+)");
    auto it = urlRe.globalMatch(texto);
    while (it.hasNext()) { urls++; it.next(); }
    double urlScore = std::clamp(urls / 2.0, 0.0, 1.0);

    // Componer riesgo: ponderaciones heurísticas
    double risk = 0.0;
    risk += 0.20 * uppercaseRatio;       // mayúsculas
    risk += 0.20 * std::clamp(punctDensity / 0.15, 0.0, 1.0); // densidad signos
    risk += 0.20 * std::clamp((4.0 - avgWordsPerSentence) / 4.0, 0.0, 1.0); // frases muy cortas -> mayor riesgo
    risk += 0.20 * fleschLike;           // baja legibilidad -> mayor riesgo
    risk += 0.10 * alertaScore;
    risk += 0.10 * urlScore;

    // Normalizar a [0,1]
    risk = std::clamp(risk, 0.0, 1.0);

    // Ajustes finos: si tiene muchas mayúsculas y muchas palabras de alerta -> boost
    if (uppercaseRatio > 0.15 && alertaCount >= 1) risk = std::min(1.0, risk + 0.08);
    if (punctDensity > 0.2) risk = std::min(1.0, risk + 0.05);

    return risk;
}

/*
 * Aproximación sencilla al conteo de sílabas: cuenta grupos de vocales dentro de la palabra.
 */
int FakeResults::approxSyllableCount(const QString &word) {
    QString w = word.toLower();
    // eliminar caracteres no alfabéticos
    w.remove(QRegularExpression("[^a-záéíóúüñ]"));
    if (w.isEmpty()) return 1;
    QRegularExpression vowelGroup("[aeiouáéíóúü]+");
    auto it = vowelGroup.globalMatch(w);
    int groups = 0;
    while (it.hasNext()) { it.next(); groups++; }
    // ajustar para palabras muy cortas
    if (groups == 0) return 1;
    return std::max(1, groups);
}

/*
 * Normalizar token: eliminar tildes simplificadas y acentos para mejor matching
 */
QString FakeResults::normalizeToken(const QString &token) {
    QString r = token;
    static const QMap<QString, QString> replacements = {
        {"á","a"}, {"é","e"}, {"í","i"}, {"ó","o"}, {"ú","u"}, {"ü","u"}, {"ñ","n"}
    };
    for (auto it = replacements.begin(); it != replacements.end(); ++it) {
        r.replace(it.key(), it.value());
    }
    return r;
}

/*
 * Si el usuario desea el algoritmo antiguo de estilo, se mantiene pero ahora lo llamamos computeStyleRisk internamente
 */
void FakeResults::algo2() {
    QSettings settings("JCNews", "FakeNew");
    QString texto = settings.value("news/content").toString();

    double riesgo = computeStyleRisk(texto); // 0..1
    int porcentajeFalsa = std::round(riesgo * 100.0);

    // Estadísticas adicionales para mostrar
    int mayusculas = 0;
    QStringList palabras = texto.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    for (const QString &palabra : palabras) {
        if (palabra.length() > 2 && palabra == palabra.toUpper() && palabra.contains(QRegularExpression("[A-ZÁÉÍÓÚÑ]"))) {
            mayusculas++;
        }
    }
    int signosExclamacion = texto.count("!") + texto.count("¡");
    int signosInterrogacion = texto.count("?") + texto.count("¿");

    QStringList frases = texto.split(QRegularExpression("[\\.\\!\\?]+"), Qt::SkipEmptyParts);
    int frasesCortas = 0;
    for (const QString &frase : frases) {
        QStringList palabrasFrase = frase.trimmed().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (!palabrasFrase.isEmpty() && palabrasFrase.size() <= 4) {
            frasesCortas++;
        }
    }

    int palabrasAlerta = 0;
    QStringList alerta = {"compártelo", "difunde", "increíble", "urgente", "alerta", "inexplicable", "impactante","compartelo","viral","click"};
    for (const QString &word : palabras) {
        if (alerta.contains(word.toLower())) {
            palabrasAlerta++;
        }
    }

    // HTML de salida
    QString html;
    html += "<div style='padding:10px;'>";
    html += "<h2>🧪 Estadísticas del estilo (mejorado)</h2>";
    html += "<ul style='font-size:15px; line-height:1.6em;'>";
    html += QString("<li><b>🧠 Palabras en MAYÚSCULA:</b> %1</li>").arg(mayusculas);
    html += QString("<li><b>❗ Signos de exclamación (!/¡):</b> %1</li>").arg(signosExclamacion);
    html += QString("<li><b>❓ Signos de interrogación (?/¿):</b> %1</li>").arg(signosInterrogacion);
    html += QString("<li><b>✂️ Frases muy cortas (≤ 4 palabras):</b> %1</li>").arg(frasesCortas);
    html += QString("<li><b>🚨 Palabras de alerta encontradas:</b> %1</li>").arg(palabrasAlerta);
    html += QString("<li><b>🔬 Puntuación de riesgo de estilo (0..1):</b> %1</li>").arg(QString::number(riesgo, 'f', 3));
    html += "</ul>";

    html += "<hr style='margin:20px 0;'>";

    html += QString("<h3 style='color:%1;'>Probabilidad estimada de ser noticia falsa por estilo: <span style='font-size:24px;'>%2%</span></h3>")
                .arg(porcentajeFalsa >= 60 ? "#e74c3c" : "#27ae60")
                .arg(porcentajeFalsa);

    html += "<p style='font-size:12px; color:#666;'>Este score mide rasgos de estilo sensacionalista y clickbait. Es una heurística que complementa el análisis de contenido.</p>";

    html += "</div>";

    ui->resultado2Text->setHtml(html);
}
