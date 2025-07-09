#include "fakeresults.h"
#include "ui_fakeresults.h"

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

FakeResults::FakeResults(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FakeResults)
{
    ui->setupUi(this);
    setWindowTitle("Resultados Fake");

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

void FakeResults::algo1() {
    QSettings settingsFake("JCNews", "FakeNew");
    QString contenidoActual = settingsFake.value("news/content").toString();
    QStringList tokensActual = tokenizeText(contenidoActual);
    QSet<QString> tokensActualSet = QSet<QString>(tokensActual.begin(), tokensActual.end());

    QSettings settingsConfig("JcNews", "config");
    QString rutaGuardada = settingsConfig.value("dir_news").toString();
    QString jsonPath = QDir(rutaGuardada).filePath("metadatos_noticias.json");

    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->resultado1Text->setHtml("<b style='color:red;'>Error:</b> No se pudo abrir el archivo:<br>" + jsonPath);
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (doc.isNull() || !doc.isArray()) {
        ui->resultado1Text->setHtml("<b style='color:red;'>Error:</b> Error al parsear JSON:<br>" + parseError.errorString());
        return;
    }

    QJsonArray noticias = doc.array();
    if (noticias.isEmpty()) {
        ui->resultado1Text->setHtml("<b>Aviso:</b> El archivo JSON no contiene noticias.");
        return;
    }

    // Tokenizar noticias
    QList<QStringList> tokenizaciones;
    for (const QJsonValue &val : noticias) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();
        QString titulo = obj.value("titulo").toString();
        QString descripcion = obj.value("descripcion").toString();
        QString fuente = obj.value("fuente").toString();
        QString fecha = obj.value("fecha").toString();

        QString combinado = titulo + " " + descripcion + " " + fuente + " " + fecha;
        tokenizaciones.append(tokenizeText(combinado));
    }

    // Índice invertido
    QMap<QString, QSet<int>> indiceInvertido;
    for (int i = 0; i < tokenizaciones.size(); ++i) {
        const QStringList &tokens = tokenizaciones[i];
        for (const QString &token : tokens) {
            indiceInvertido[token].insert(i);
        }
    }

    // Contar intersecciones con documentos candidatos
    QMap<int, int> mapaInterseccion;
    for (const QString &token : tokensActual) {
        if (indiceInvertido.contains(token)) {
            for (int doc_id : indiceInvertido[token]) {
                mapaInterseccion[doc_id]++;
            }
        }
    }

    const double umbral = 0.1; // umbral permisivo

    struct ResultadoSimilitud {
        int id;
        double similitud;
        int tokensDoc;
    };
    QList<ResultadoSimilitud> resultados;

    for (auto it = mapaInterseccion.begin(); it != mapaInterseccion.end(); ++it) {
        int doc_id = it.key();
        int interseccion = it.value();
        int sizeA = tokensActualSet.size();
        int sizeB = tokenizaciones[doc_id].size();
        int unionSize = sizeA + sizeB - interseccion;
        double sim = (unionSize == 0) ? 0.0 : static_cast<double>(interseccion) / unionSize;
        if (sim >= umbral) {
            resultados.append({doc_id, sim, sizeB});
        }
    }

    // Ordenar resultados descendente por similitud
    std::sort(resultados.begin(), resultados.end(), [](const ResultadoSimilitud &a, const ResultadoSimilitud &b) {
        return a.similitud > b.similitud;
    });

    // Construir HTML resultado
    QString html;
    html += "<h2>Análisis de similitud de noticias</h2>";
    html += QString("<p><b>Tokens noticia actual:</b> %1</p>").arg(tokensActualSet.size());
    html += QString("<p><b>Total noticias analizadas:</b> %1</p>").arg(noticias.size());
    html += QString("<p><b>Noticias similares encontradas (umbral &ge; %1):</b> %2</p>")
            .arg(umbral)
            .arg(resultados.size());

    if (resultados.isEmpty()) {
        html += "<p><i>No se encontraron noticias similares.</i></p>";
    } else {
        html += "<ol>";
        for (const auto &res : resultados) {
            QJsonObject obj = noticias[res.id].toObject();
            QString titulo = obj.value("titulo").toString();
            QString descripcion = obj.value("descripcion").toString();
            QString fuente = obj.value("fuente").toString();
            QString fecha = obj.value("fecha").toString();

            html += "<li>";
            html += QString("<h3 style='margin-bottom:0;'>%1</h3>").arg(titulo.toHtmlEscaped());
            html += QString("<p><b>Similitud:</b> %1</p>").arg(QString::number(res.similitud, 'f', 3));
            html += QString("<p><b>Tokens noticia:</b> %1</p>").arg(res.tokensDoc);
            html += QString("<p><b>Descripción:</b> %1</p>").arg(descripcion.toHtmlEscaped());
            html += QString("<p><b>Fuente:</b> %1</p>").arg(fuente.toHtmlEscaped());
            html += QString("<p><b>Fecha:</b> %1</p>").arg(fecha.toHtmlEscaped());
            html += "</li><hr>";
        }
        html += "</ol>";
    }

    // Suma de similitudes
    double sumaSimilitudes = 0.0;
    for (const auto &r : resultados) {
        sumaSimilitudes += r.similitud;
    }
    double probVerdadera = 0.0;

    if (!resultados.isEmpty()) {
        double promedioSim = sumaSimilitudes / resultados.size();
        int nCoincidencias = resultados.size();

        // Probabilidad base si hay al menos una coincidencia
        probVerdadera = 0.7 + promedioSim * 0.3;

        // Pequeño boost por muchas coincidencias
        if (nCoincidencias >= 3)
            probVerdadera += 0.05;
        if (nCoincidencias >= 5)
            probVerdadera += 0.05;

        // Limitar a 100%
        probVerdadera = std::min(1.0, probVerdadera);
    } else {
        // Si no hay ninguna coincidencia: dar chance mínima
        probVerdadera = 0.10;
    }

    probVerdadera *= 100.0;

    html += QString("<h2>Estimación de veracidad</h2>");
    html += QString("<p>Probabilidad estimada de que la noticia sea <b>verdadera</b>: <span style='color:green; font-weight:bold;'>%1%</span></p>")
            .arg(QString::number(probVerdadera, 'f', 2));
    html += QString("<p>Probabilidad estimada de que la noticia sea <b>falsa</b>: <span style='color:red; font-weight:bold;'>%1%</span></p>")
            .arg(QString::number(100.0 - probVerdadera, 'f', 2));

    ui->resultado1Text->setHtml(html);
}

QStringList FakeResults::tokenizeText(const QString &texto) {
    static const QSet<QString> stopwords_comunes = {
        "el", "la", "los", "las", "un", "una", "unos", "unas",
        "de", "del", "en", "con", "para", "por", "y", "o", "a", "al",
        "que", "se", "su", "lo", "le", "es", "ha", "han", "como"
    };

    QString limpio = texto.toLower();
    limpio.remove(QRegularExpression("[^a-záéíóúñü\\s]"));
    QStringList tokens = limpio.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    QStringList utiles;
    for (const QString &token : tokens) {
        if (!stopwords_comunes.contains(token)) {
            utiles << token;
        }
    }
    return utiles;
}

double FakeResults::similitudJaccard(const QSet<QString> &a, const QSet<QString> &b) {
    if (a.isEmpty() && b.isEmpty()) return 0.0;

    QSet<QString> interseccion = a;
    interseccion.intersect(b);

    QSet<QString> unionSet = a;
    unionSet.unite(b);

    return static_cast<double>(interseccion.size()) / unionSet.size();
}
void FakeResults::algo2() {
    QSettings settings("JCNews", "FakeNew");
    QString texto = settings.value("news/content").toString();

    int mayusculas = 0;
    int signosExclamacion = texto.count("!") + texto.count("¡");
    int signosInterrogacion = texto.count("?") + texto.count("¿");

    QStringList palabras = texto.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    for (const QString &palabra : palabras) {
        if (palabra.length() > 2 && palabra == palabra.toUpper() && palabra.contains(QRegularExpression("[A-ZÁÉÍÓÚÑ]"))) {
            mayusculas++;
        }
    }

    int frasesCortas = 0;
    QStringList frases = texto.split(QRegularExpression("[\\.\\!\\?]+"), Qt::SkipEmptyParts);
    for (const QString &frase : frases) {
        QStringList palabrasFrase = frase.trimmed().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (!palabrasFrase.isEmpty() && palabrasFrase.size() <= 4) {
            frasesCortas++;
        }
    }

    QStringList alerta = {"compártelo", "difunde", "increíble", "urgente", "alerta", "inexplicable", "impactante"};
    int palabrasAlerta = 0;
    for (const QString &word : palabras) {
        if (alerta.contains(word.toLower())) {
            palabrasAlerta++;
        }
    }

    // Función para normalizar valores con tope
    auto clamp = [](double x, double minVal, double maxVal) {
        return std::max(minVal, std::min(x, maxVal));
    };

    // Ponderaciones con máximo contribución en porcentaje
    double pesoMayus = clamp(mayusculas / 6.0, 0.0, 1.0) * 25.0;
    double pesoExcl = clamp(signosExclamacion / 6.0, 0.0, 1.0) * 15.0;
    double pesoInter = clamp(signosInterrogacion / 6.0, 0.0, 1.0) * 10.0;
    double pesoFrases = clamp(frasesCortas / 5.0, 0.0, 1.0) * 25.0;
    double pesoAlertas = clamp(palabrasAlerta / 4.0, 0.0, 1.0) * 25.0;

    int porcentajeFalsa = std::round(pesoMayus + pesoExcl + pesoInter + pesoFrases + pesoAlertas);

    // HTML de salida
    QString html;
    html += "<div style='padding:10px;'>";

    html += "<h2>🧪 Estadísticas del estilo de la noticia</h2>";
    html += "<ul style='font-size:15px; line-height:1.6em;'>";
    html += QString("<li><b>🧠 Palabras en MAYÚSCULA:</b> %1</li>").arg(mayusculas);
    html += QString("<li><b>❗ Signos de exclamación (!/¡):</b> %1</li>").arg(signosExclamacion);
    html += QString("<li><b>❓ Signos de interrogación (?/¿):</b> %1</li>").arg(signosInterrogacion);
    html += QString("<li><b>✂️ Frases muy cortas (≤ 4 palabras):</b> %1</li>").arg(frasesCortas);
    html += QString("<li><b>🚨 Palabras de alerta encontradas:</b> %1</li>").arg(palabrasAlerta);
    html += "</ul>";

    html += "<hr style='margin:20px 0;'>";

    html += QString("<h3 style='color:%1;'>Probabilidad de ser noticia falsa: <span style='font-size:24px;'>%2%</span></h3>")
                .arg(porcentajeFalsa >= 60 ? "#e74c3c" : "#27ae60")
                .arg(porcentajeFalsa);

    html += "</div>";

    ui->resultado2Text->setHtml(html);
}

