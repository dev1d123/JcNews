#include "mainwindow.h"
#include "newscomponent.h"
#include "configuration.h"
#include "./ui_mainwindow.h"
#include "uploadnews.h"
#include "preferences.h"
#include "trie.h"
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <iostream>
#include <QRegularExpression>
#include <QVector>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrlQuery>
#include <QUrl>
#include <QDirIterator>
#include <QCoreApplication>
#include <QTextStream>
#include <QFileInfo>
#include <QTimer>

QMap<QString, QList<QJsonObject>> indiceInvertido;

QVector<QString> titulos;
Trie* trie = new Trie();

QMap<QString, QStringList> palabrasClavePorTema = {
    {"política", {
                     "gobierno", "presidente", "congreso", "ministro", "ministra", "elecciones", "partido", "política",
                     "democracia", "reforma", "voto", "parlamento", "asamblea", "diputado", "senador", "legislador",
                     "poder ejecutivo", "poder legislativo", "juramentación", "mandato", "campaña electoral", "candidato", "coalición"
                 }},
    {"economía", {
                     "economía", "inflación", "dólar", "euro", "banco", "finanzas", "mercado", "crédito", "interés",
                     "impuestos", "deuda", "negocio", "pib", "aranceles", "inversión", "bolsa", "recesión", "comercio",
                     "exportación", "importación", "ingresos", "egresos", "precio", "costos", "fmi", "tesoro", "subsidio"
                 }},
    {"salud", {
                  "hospital", "covid", "covid-19", "salud", "médico", "médica", "enfermedad", "vacuna", "oms",
                  "clínica", "paciente", "virus", "contagio", "enfermería", "medicina", "quirófano", "sintoma",
                  "tratamiento", "infección", "pandemia", "epidemia", "salubridad", "enfermero", "cura", "salud pública"
              }},
    {"cultura", {
                    "arte", "cultura", "literatura", "cine", "película", "teatro", "museo", "exposición", "pintura",
                    "escultura", "historia", "poesía", "libro", "autor", "actor", "ballet", "obra", "festival",
                    "documental", "fotografía", "cultural", "narrativa", "ensayo", "folklore"
                }},
    {"sociedad", {
                     "sociedad", "protesta", "manifestación", "violencia", "derechos", "comunidad", "familia",
                     "discriminación", "igualdad", "género", "inclusión", "marginación", "movilización",
                     "integración", "pobreza", "conflicto social", "colectivo", "ciudadanía", "juventud", "niñez"
                 }},
    {"entretenimiento", {
                            "famoso", "celebridad", "espectáculo", "música", "actor", "actriz", "película", "serie",
                            "concierto", "premio", "show", "televisión", "streaming", "festival", "reality", "álbum",
                            "cantante", "hollywood", "celebración", "gala", "influencer", "paparazzi", "evento"
                        }},
    {"deportes", {
                     "fútbol", "gol", "liga", "partido", "mundial", "deporte", "jugador", "atleta", "selección",
                     "torneo", "campeonato", "copa", "olimpiadas", "competencia", "marcador", "entrenador",
                     "estadística", "cancha", "estadio", "penal", "balón", "equipo", "baloncesto", "tenis", "natación"
                 }},
    {"tecnología", {
                       "tecnología", "smartphone", "aplicación", "app", "inteligencia artificial", "redes sociales",
                       "internet", "software", "hardware", "google", "microsoft", "apple", "código", "sistema",
                       "nube", "servidor", "datos", "seguridad informática", "criptografía", "algoritmo", "robot",
                       "gadget", "web", "startup", "tecnológico"
                   }},
    {"ciencia", {
                    "ciencia", "investigación", "experimento", "astronomía", "biología", "física", "química",
                    "universo", "planeta", "científico", "teoría", "descubrimiento", "laboratorio", "genética",
                    "evolución", "átomo", "materia", "celular", "espacio", "nasa", "cosmos", "microscopio", "neutrón"
                }},
    {"medio ambiente", {
                           "medio ambiente", "cambio climático", "calentamiento global", "contaminación", "reciclaje",
                           "ecología", "bosque", "energía", "renovable", "sostenibilidad", "naturaleza", "biodiversidad",
                           "verde", "emisiones", "carbono", "deforestación", "eco", "planeta", "agua", "residuos",
                           "incendios forestales", "especies"
                       }},
    {"educación", {
                      "escuela", "educación", "estudiante", "alumno", "profesor", "universidad", "colegio", "beca",
                      "clases", "aprendizaje", "currículo", "docente", "aula", "formación", "maestro", "enseñanza",
                      "evaluación", "tutoría", "nota", "académico", "examen"
                  }},
    {"moda", {
                 "moda", "ropa", "tendencia", "pasarela", "vestido", "diseño", "modelo", "estilo", "colección",
                 "fashion", "desfile", "ropa de lujo", "prenda", "accesorio", "marca", "diseñador"
             }},
    {"crimen", {
                   "crimen", "delito", "asesinato", "robo", "policía", "investigación", "justicia", "homicidio",
                   "fiscalía", "criminal", "violación", "secuestro", "narcotráfico", "violencia", "arma", "detención"
               }},
    {"viajes", {
                   "viaje", "turismo", "hotel", "destino", "aeropuerto", "vacaciones", "avión", "reserva",
                   "turista", "maleta", "crucero", "agencia", "hospedaje", "tour", "itinerario"
               }},
    {"clima", {
                  "clima", "lluvia", "tormenta", "temperatura", "frío", "calor", "viento", "nieve", "huracán",
                  "sequía", "inundación", "granizo", "alerta meteorológica", "pronóstico", "climático"
              }},
    {"negocios", {
                     "empresa", "negocio", "emprendimiento", "start-up", "mercado", "cliente", "producto", "venta",
                     "CEO", "marca", "inversión", "negociación", "oferta", "demanda", "facturación", "beneficio",
                     "estrategia", "franquicia", "comercial", "alianza", "sociedad", "corporativo"
                 }}
};

QString MainWindow::clasificarTemaPorDescripcion(const QString& descripcion, const QMap<QString, QStringList>& palabrasClavePorTema)
{
    QString descripcionLower = descripcion.toLower();
    QMap<QString, int> conteos;

    for (auto it = palabrasClavePorTema.begin(); it != palabrasClavePorTema.end(); ++it) {
        int count = 0;
        for (const QString& palabra : it.value()) {
            if (descripcionLower.contains(palabra)) {
                count++;
            }
        }
        conteos[it.key()] = count;
    }

    QString mejorTema;
    int maxConteo = 0;
    for (auto it = conteos.begin(); it != conteos.end(); ++it) {
        if (it.value() > maxConteo) {
            maxConteo = it.value();
            mejorTema = it.key();
        }
    }
    //umbral para el tema
    return (maxConteo >= 2) ? mejorTema : "";
}

// ----------------- Helpers para integración con API y archivos -----------------

static QString sanitizeFilenameFromUrl(const QString& url) {
    // QUrl to remove scheme and keep host+path+query minimally, then percent-encode
    QUrl u(url);
    QString cleaned;
    if (!u.host().isEmpty()) {
        cleaned = u.host() + u.path();
        if (!u.query().isEmpty()) cleaned += "?" + u.query();
    } else {
        // fallback: remove scheme by regex
        cleaned = url;
        cleaned.remove(QRegularExpression("^https?://"));
    }
    QByteArray encoded = QUrl::toPercentEncoding(cleaned);
    QString name = QString::fromUtf8(encoded);
    if (name.length() > 150) name = name.left(150);
    // remove trailing dots or slashes
    name.replace('/', "%2F");
    name.replace('\\', "%5C");
    return name + ".txt";
}

static QJsonArray loadNoticiasFromJsonFile(const QString& jsonPath) {
    QJsonArray result;
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return result;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing JSON" << parseError.errorString();
        return result;
    }
    if (doc.isArray()) result = doc.array();
    return result;
}

static QJsonArray loadNoticiasFromTxtFiles(const QString& dirPath) {
    QJsonArray arr;
    QDir dir(dirPath);
    if (!dir.exists()) return arr;

    QDirIterator it(dirPath, QStringList() << "*.txt", QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        QString filePath = it.next();
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
        QTextStream in(&file);
        QString titulo, fuente, url, fecha;
        QString line;
        QString content;
        bool afterSeparator = false;
        while (!in.atEnd()) {
            line = in.readLine();
            if (!afterSeparator) {
                if (line.startsWith("Título:")) {
                    titulo = line.mid(QString("Título:").length()).trimmed();
                } else if (line.startsWith("Fuente:")) {
                    fuente = line.mid(QString("Fuente:").length()).trimmed();
                } else if (line.startsWith("URL:")) {
                    url = line.mid(QString("URL:").length()).trimmed();
                } else if (line.startsWith("Fecha:")) {
                    fecha = line.mid(QString("Fecha:").length()).trimmed();
                } else if (line.startsWith("---")) {
                    afterSeparator = true;
                }
            } else {
                content += line + "\n";
            }
        }
        file.close();
        QJsonObject obj;
        obj["id"] = url.isEmpty() ? QFileInfo(filePath).fileName() : url;
        obj["titulo"] = titulo;
        obj["fuente"] = fuente;
        obj["autor"] = QJsonValue::Null;
        obj["fecha"] = fecha;
        obj["url"] = url;
        obj["url_imagen"] = QJsonValue::Null;
        obj["ruta_contenido"] = content.trimmed();
        obj["descripcion"] = content.left(300); // small description
        obj["fecha_procesamiento"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        obj["temas"] = QJsonArray(); // vacío
        obj["prioridad"] = "0";
        arr.append(obj);
    }
    return arr;
}

static bool saveApiArticlesToDir(const QJsonArray& articles, const QString& dirPath) {
    QDir dir(dirPath);
    if (!dir.exists()) {
        if (!dir.mkpath(dirPath)) {
            qWarning() << "No se pudo crear carpeta:" << dirPath;
            return false;
        }
    }

    QJsonArray metaArray;
    for (const QJsonValue& v : articles) {
        if (!v.isObject()) continue;
        QJsonObject a = v.toObject();

        QString title = a.value("titulo").toString(a.value("title").toString());
        if (title.isEmpty()) title = a.value("title").toString();
        QString fuente = a.value("fuente").toString(a.value("source_name").toString());
        QString url = a.value("url").toString(a.value("link").toString());
        QString fecha = a.value("fecha").toString(a.value("pubDate").toString());
        QString contenido = a.value("ruta_contenido").toString(); // our API provides texto completo aquí
        if (contenido.isEmpty()) {
            // fallback to description if ruta_contenido missing
            contenido = a.value("descripcion").toString();
        }

        // Build filename and write file
        QString filename = sanitizeFilenameFromUrl(url.isEmpty() ? title : url);
        QString filePath = QDir(dirPath).filePath(filename);

        QFile f(filePath);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "No se pudo escribir archivo:" << filePath;
            continue;
        }
        QTextStream out(&f);
        out.setEncoding(QStringConverter::Utf8);
        out << "Título: " << title << "\n";
        out << "Fuente: " << fuente << "\n";
        out << "URL: " << url << "\n";
        out << "Fecha: " << fecha << "\n";
        out << "---\n";
        out << contenido << "\n";
        f.close();

        // Build metadata object to save in metadatos_noticias.json
        QJsonObject meta;
        meta["id"] = url.isEmpty() ? filename : url;
        meta["titulo"] = title;
        meta["fuente"] = fuente;
        // try to keep autor if present
        if (a.contains("autor")) meta["autor"] = a.value("autor");
        else if (a.contains("creator") && a.value("creator").isArray()) {
            QJsonArray creators = a.value("creator").toArray();
            meta["autor"] = creators.isEmpty() ? QJsonValue::Null : creators.first();
        } else meta["autor"] = QJsonValue::Null;
        meta["fecha"] = fecha;
        meta["url"] = url;
        meta["url_imagen"] = a.value("url_imagen");
        meta["ruta_contenido"] = filePath; // keep path like previous standard
        meta["descripcion"] = a.value("descripcion");
        meta["fecha_procesamiento"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        if (a.contains("temas")) meta["temas"] = a.value("temas");
        else meta["temas"] = QJsonArray();
        meta["prioridad"] = a.value("prioridad").toString("0");
        metaArray.append(meta);
    }

    // Save metadatos_noticias.json
    QString jsonPath = QDir(dirPath).filePath("metadatos_noticias.json");
    QFile jf(jsonPath);
    if (!jf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "No se pudo escribir metadatos:" << jsonPath;
        return false;
    }
    QJsonDocument doc(metaArray);
    jf.write(doc.toJson(QJsonDocument::Indented));
    jf.close();
    return true;
}
static QJsonArray fetchArticlesFromLocalApi(const QString& apiBaseUrl, int count) {
    static QNetworkAccessManager mgr;

    // Construir la URL con los parámetros
    QUrl url(apiBaseUrl);
    QUrlQuery q;
    q.addQueryItem("count", QString::number(count));
    url.setQuery(q);

    // Preparar la solicitud
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Ejecutar la solicitud síncronamente (bloqueante)
    QNetworkReply* reply = mgr.get(req);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();  // Esperar a que termine la solicitud

    // Leer la respuesta
    QByteArray resp = reply->readAll();
    reply->deleteLater();

    // Parsear JSON
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(resp, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return {};
    }

    QJsonObject obj = doc.object();
    return obj.value("articles").toArray();
}



// ----------------- End helpers -----------------

void MainWindow::buscarPorTexto()
{
    QString texto = ui->buscarLine->text().toLower();
    if (texto.isEmpty()) {
        filtrarNoticias();
        return;
    }

    QStringList tokens = texto.split(QRegularExpression("[\\W_]+"), Qt::SkipEmptyParts);
    QSet<QJsonObject> resultados;

    // Acumuladores para debug
    QString debugInfo = "Tokens buscados:\n";

    // Umbral mínimo de tokens que debe coincidir por noticia
    int umbralMinimo = tokens.size() - 6;
    // Mapa para contar cuántos tokens coinciden por cada noticia
    QMap<QString, QPair<QJsonObject, int>> conteoCoincidencias;

    for (const QString& token : tokens) {
        const QList<QJsonObject>& noticias = indiceInvertido.value(token);
        debugInfo += QString("Token: '%1' -> %2 coincidencias\n").arg(token).arg(noticias.size());

        for (const QJsonObject& obj : noticias) {
            QString id = obj["id"].toString();  // Asegúrate de que cada noticia tenga un campo "id" único
            conteoCoincidencias[id].first = obj;
            conteoCoincidencias[id].second += 1;
        }
    }

    // Filtrar por umbral mínimo
    for (const auto& par : std::as_const(conteoCoincidencias)) {
        if (par.second >= umbralMinimo) {
            resultados.insert(par.first);
        }
    }

    debugInfo += QString("\nTotal noticias encontradas (umbral >= %1): %2")
                     .arg(umbralMinimo)
                     .arg(resultados.size());

    QMessageBox::information(this, "Debug Búsqueda", debugInfo);

    // Mostrar resultados
    QWidget* contenedor = ui->noticiasScroll->widget();
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contenedor->layout());
    if (!layout) {
        layout = new QVBoxLayout(contenedor);
        contenedor->setLayout(layout);
    }

    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (QWidget* widget = child->widget()) widget->deleteLater();
        delete child;
    }

    QMap<QString, QString> traduccionesTemas = {
        {"business", "negocios"}, {"sports", "deportes"}, {"technology", "tecnología"},
        {"politics", "política"}, {"health", "salud"}, {"science", "ciencia"},
        {"entertainment", "entretenimiento"}, {"world", "mundo"}, {"economy", "economía"},
        {"education", "educación"}, {"travel", "viajes"}, {"food", "comida"},
        {"environment", "medio ambiente"}, {"top", "principales"}, {"finance", "finanzas"},
        {"law", "ley"}, {"culture", "cultura"}, {"fashion", "moda"},
        {"crime", "crimen"}, {"weather", "clima"}, {"real estate", "bienes raíces"},
        {"society", "sociedad"}
    };

    for (const QJsonObject& obj : resultados) {
        QJsonArray temasJson = obj["temas"].toArray();
        QStringList temasTraducidos;
        for (const QJsonValue& tema : temasJson) {
            QString temaEsp = traduccionesTemas.value(tema.toString(), tema.toString());
            temasTraducidos << temaEsp;
        }

        NewsComponent* comp = new NewsComponent(this);
        comp->setIdUrl(obj["id"].toString());
        comp->setTextPath(obj["ruta_contenido"].toString());
        comp->setImgUrl(obj["url_imagen"].toString());
        comp->setFixedHeight(150);
        comp->setDate(QDateTime::fromString(obj["fecha"].toString(), Qt::ISODate));
        comp->setFuente(obj["fuente"].toString());
        comp->setTitle(obj["titulo"].toString());
        comp->setTema(temasTraducidos.join(", "));

        layout->addWidget(comp);
    }

    layout->addStretch();
}

void MainWindow::actualizarSugerencias(const QString& texto) {
    if (texto.isEmpty()) {
        sugerenciasList->hide();
        return;
    }

    QStringList resultados;
    trie->searchPrefix(texto, resultados);  // buscar títulos con ese prefijo

    if (resultados.isEmpty()) {
        sugerenciasList->hide();
        return;
    }

    // Limpiar y agregar sugerencias
    sugerenciasList->clear();
    sugerenciasList->addItems(resultados);

    // Posicionar lista justo debajo del QLineEdit
    QPoint pos = ui->buscarLine->mapToGlobal(QPoint(0, ui->buscarLine->height()));
    sugerenciasList->move(pos);
    sugerenciasList->resize(ui->buscarLine->width(), 100);  // altura ajustable
    sugerenciasList->show();
    sugerenciasList->raise();
    sugerenciasList->setFocus();

}

void MainWindow::seleccionarSugerencia(QListWidgetItem* item) {
    ui->buscarLine->setText(item->text());
    sugerenciasList->hide();
    ui->buscarLine->setFocus();
}


void MainWindow::limpiarOrdenamientosRadioButtons()
{
    // Función lambda para desactivar cada QRadioButton
    auto desactivarRadio = [](QRadioButton* rb) {
        rb->setAutoExclusive(false);   // Desactiva exclusividad temporalmente
        rb->setChecked(false);         // Desmarca el botón
        rb->setAutoExclusive(true);    // Restaura exclusividad
    };

    // Limpiar radio buttons de fecha
    desactivarRadio(ui->fechaArribaRadioB);
    desactivarRadio(ui->fechaAbajoRadioB);

    // Limpiar radio buttons de fuente
    desactivarRadio(ui->fuenteAZRadioB);
    desactivarRadio(ui->fuenteZARadioB);

    // Limpiar radio buttons de importancia
    desactivarRadio(ui->importanciaArribaRadioB);
    desactivarRadio(ui->importanciaAbajoRadioB);
}
bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress && sugerenciasList->isVisible()) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint globalPos = mouseEvent->globalPos();

        // Verificar si se hizo clic fuera del input o lista
        if (!ui->buscarLine->geometry().contains(ui->buscarLine->mapFromGlobal(globalPos)) &&
            !sugerenciasList->geometry().contains(sugerenciasList->mapFromGlobal(globalPos))) {
            sugerenciasList->hide();
        }
    }

    // Si el QLineEdit pierde foco
    if ((obj == ui->buscarLine || obj == sugerenciasList) && event->type() == QEvent::FocusOut) {
        sugerenciasList->hide();
    }

    return QMainWindow::eventFilter(obj, event);
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->filtrarButton, &QPushButton::clicked, this, [this]() {
        limpiarOrdenamientosRadioButtons();
    });

    connect(ui->fakeButton, &QPushButton::clicked, this, [this]() {
        UploadNews dlg(this);
        dlg.exec();
    });

    // Conexiones para ordenamiento al cambiar los radio buttons
    connect(ui->fechaArribaRadioB, &QRadioButton::toggled, this, &MainWindow::filtrarNoticias);
    connect(ui->fechaAbajoRadioB, &QRadioButton::toggled, this, &MainWindow::filtrarNoticias);

    connect(ui->fuenteAZRadioB, &QRadioButton::toggled, this, &MainWindow::filtrarNoticias);
    connect(ui->fuenteZARadioB, &QRadioButton::toggled, this, &MainWindow::filtrarNoticias);

    connect(ui->importanciaArribaRadioB, &QRadioButton::toggled, this, &MainWindow::filtrarNoticias);
    connect(ui->importanciaAbajoRadioB, &QRadioButton::toggled, this, &MainWindow::filtrarNoticias);

    connect(ui->buscarButton, &QPushButton::clicked, this, &MainWindow::buscarPorTexto);

    trie = new Trie();  // Ya lo cargas en on_actualizarButton_clicked

    sugerenciasList = new QListWidget(this);
    sugerenciasList->hide();
    sugerenciasList->setWindowFlags(Qt::ToolTip);  // 🟡 Mejor que Qt::Popup para permitir foco y clicks
    sugerenciasList->setFocusPolicy(Qt::StrongFocus);  // ✅ Necesita recibir foco
    sugerenciasList->setMouseTracking(true);


    // Conectar eventos
    connect(ui->buscarLine, &QLineEdit::textChanged, this, &MainWindow::actualizarSugerencias);
    connect(sugerenciasList, &QListWidget::itemClicked, this, &MainWindow::seleccionarSugerencia);
    connect(sugerenciasList, &QListWidget::itemActivated, this, &MainWindow::seleccionarSugerencia); // tecla Enter

    // Ocultar sugerencias si pierde el foco
    qApp->installEventFilter(this);                // Detectar clics globales
    ui->buscarLine->installEventFilter(this);      // Detectar pérdida de foco del input
    sugerenciasList->installEventFilter(this);     // Detectar clics dentro de la lista



    QSettings settings("JcNews", "config");

    QString tema = settings.value("tema", "claro").toString();
    QString fuente = settings.value("fuente", "Fuente 1").toString();

    QString rutaTema = (tema == "oscuro")
                           ? ":/styles/oscuro.qss"
                           : ":/styles/claro.qss";

    QString rutaFuente = ":/styles/font1.qss";
    if (fuente == "Fuente 2") rutaFuente = ":/styles/font2.qss";
    else if (fuente == "Fuente 3") rutaFuente = ":/styles/font3.qss";

    QString estiloFinal;

    QFile fileTema(rutaTema);
    if (fileTema.open(QFile::ReadOnly | QFile::Text)) {
        estiloFinal += QString::fromUtf8(fileTema.readAll());
        fileTema.close();
    }

    QFile fileFuente(rutaFuente);
    if (fileFuente.open(QFile::ReadOnly | QFile::Text)) {
        estiloFinal += "\n" + QString::fromUtf8(fileFuente.readAll());
        fileFuente.close();
    }

    qApp->setStyleSheet(estiloFinal);

    connect(ui->configuracionButton, &QPushButton::clicked, this, [this]() {
        configuration* dialog = new configuration(this);
        dialog->exec();
    });

    connect(ui->aplicarFiltros, &QPushButton::clicked, this, &MainWindow::filtrarNoticias);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actualizarButton_clicked()
{
    // Nueva lógica:
    // 1) obtener dir_news desde settings
    // 2) primero leer noticias existentes (metadatos_noticias.json si existe; sino .txt)
    // 3) poblar UI con esas noticias
    // 4) si el usuario confirma ejecutar actualización -> llamar al API local http://127.0.0.1:5000/articles?count=30
    //    -> guardar los resultados en dir_news como archivos .txt siguiendo el estándar mostrado
    //    -> guardar metadatos_noticias.json y recargar UI

    QSettings settings("JcNews", "config");
    QString rutaGuardada = settings.value("dir_news").toString();

    if (rutaGuardada.isEmpty()) {
        QMessageBox::warning(this, "Ruta no encontrada", "No se ha configurado una ruta válida en 'dir_news'.");
        return;
    }

    // -------------- 2) Leer noticias existentes ----------------
    QString jsonPath = QDir(rutaGuardada).filePath("metadatos_noticias.json");
    QJsonArray loadedArray;
    if (QFile::exists(jsonPath)) {
        loadedArray = loadNoticiasFromJsonFile(jsonPath);
    } else {
        // si no hay JSON, intentar cargar .txt individuales
        loadedArray = loadNoticiasFromTxtFiles(rutaGuardada);
    }

    noticiasOriginales = loadedArray;

    // Setear las fuentes para el combobox
    QSet<QString> fuentesUnicas;
    for (const QJsonValue& value : noticiasOriginales) {
        fuentesUnicas << value.toObject()["fuente"].toString();
    }
    ui->fuenteBox->clear();
    ui->fuenteBox->addItem("Todas");
    for (const QString& fuente : fuentesUnicas) {
        ui->fuenteBox->addItem(fuente);
    }

    // Establecer fecha más antigua y más reciente
    if (!noticiasOriginales.isEmpty()) {
        QDateTime minFecha = QDateTime::currentDateTime();
        QDateTime maxFecha = QDateTime::fromSecsSinceEpoch(0); // muy antigua

        for (const QJsonValue& value : noticiasOriginales) {
            QJsonObject obj = value.toObject();
            QDateTime fecha = QDateTime::fromString(obj["fecha"].toString(), Qt::ISODate);
            if (fecha.isValid()) {
                if (fecha < minFecha) minFecha = fecha;
                if (fecha > maxFecha) maxFecha = fecha;
            }
        }

        // Asignar a los QDateTimeEdit
        ui->fechaInicio->setDateTime(minFecha);
        ui->fechaFin->setDateTime(maxFecha);

        //llenar indice invertido
        indiceInvertido.clear();

        int totalNoticias = noticiasOriginales.size();
        QString resumenDebug = QString("Total noticias: %1\n").arg(totalNoticias);

        int i = 0;
        for (const QJsonValue& value : noticiasOriginales) {
            QJsonObject obj = value.toObject();
            titulos.append(obj["titulo"].toString());

            QString id = obj["id"].toString();
            QString descripcion = obj["descripcion"].toString().toLower();
            descripcion.append(" " + obj["titulo"].toString().toLower());
            QStringList palabras = descripcion.split(QRegularExpression("[\\W_]+"), Qt::SkipEmptyParts);
            QSet<QString> palabrasUnicas = QSet<QString>(palabras.begin(), palabras.end());

            for (const QString& palabra : palabrasUnicas) {
                indiceInvertido[palabra].append(obj);
            }

            if (i < 3) {
                resumenDebug += QString("\nNoticia ID: %1\n").arg(id);
                resumenDebug += QString("Palabras únicas (%1): %2\n")
                                    .arg(palabrasUnicas.size())
                                    .arg(QStringList(QList<QString>(palabrasUnicas.begin(), palabrasUnicas.end())).join(", "));
            }

            ++i;
        }

        QMessageBox::information(this, "Debug Índice Invertido", resumenDebug);
        QMessageBox::information(this, "Debug Titulos", titulos.join("\n"));
        delete trie;
        trie = new Trie();
        for (const QString& titulo : titulos) {
            trie->insert(titulo);
        }
    } else {
        QMessageBox::information(this, "Noticias", "No se encontraron noticias en la carpeta especificada.");
    }

    // Siempre mostrar las noticias cargadas inicialmente
    filtrarNoticias();

    // -------------- 4) Preguntar al usuario si desea actualizar (llamar a la API) -------------
    auto resp = QMessageBox::question(this, "Actualizar desde API",
                                      "¿Deseas llamar al servicio local (http://127.0.0.1:5000) para traer hasta 30 noticias y guardarlas en la carpeta configurada?",
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (resp != QMessageBox::Yes) {
        return;
    }

    QString apiBase = "http://127.0.0.1:5000/articles";
    QJsonArray apiArticles = fetchArticlesFromLocalApi(apiBase, 30);
    QString jsonText = QString::fromUtf8(QJsonDocument(apiArticles).toJson(QJsonDocument::Indented));
    QMessageBox::information(this, "Artículos recibidos", jsonText);


    if (apiArticles.isEmpty()) {
        QMessageBox::warning(this, "API", "No se obtuvieron artículos desde la API local o hubo un error.");
        return;
    }

    // Guardar resultados en dir_news como .txt y generar metadatos_noticias.json
    bool ok = saveApiArticlesToDir(apiArticles, rutaGuardada);
    if (!ok) {
        QMessageBox::warning(this, "Guardar archivos", "Hubo un error guardando los artículos en la carpeta.");
        return;
    }

    // Recargar metadatos y UI desde el JSON recién guardado
    QJsonArray reloaded = loadNoticiasFromJsonFile(QDir(rutaGuardada).filePath("metadatos_noticias.json"));
    noticiasOriginales = reloaded;

    // actualizar combobox de fuentes y fechas como antes
    QSet<QString> fuentesUnicas2;
    for (const QJsonValue& value : noticiasOriginales) {
        fuentesUnicas2 << value.toObject()["fuente"].toString();
    }
    ui->fuenteBox->clear();
    ui->fuenteBox->addItem("Todas");
    for (const QString& fuente : fuentesUnicas2) {
        ui->fuenteBox->addItem(fuente);
    }

    if (!noticiasOriginales.isEmpty()) {
        QDateTime minFecha = QDateTime::currentDateTime();
        QDateTime maxFecha = QDateTime::fromSecsSinceEpoch(0); // muy antigua

        for (const QJsonValue& value : noticiasOriginales) {
            QJsonObject obj = value.toObject();
            QDateTime fecha = QDateTime::fromString(obj["fecha"].toString(), Qt::ISODate);
            if (fecha.isValid()) {
                if (fecha < minFecha) minFecha = fecha;
                if (fecha > maxFecha) maxFecha = fecha;
            }
        }

        ui->fechaInicio->setDateTime(minFecha);
        ui->fechaFin->setDateTime(maxFecha);

        // reconstruir indice invertido y trie
        indiceInvertido.clear();
        titulos.clear();
        int i = 0;
        QString resumenDebug = QString("Total noticias (post-API): %1\n").arg(noticiasOriginales.size());
        for (const QJsonValue& value : noticiasOriginales) {
            QJsonObject obj = value.toObject();
            titulos.append(obj["titulo"].toString());
            QString descripcion = obj["descripcion"].toString().toLower();
            descripcion.append(" " + obj["titulo"].toString().toLower());
            QStringList palabras = descripcion.split(QRegularExpression("[\\W_]+"), Qt::SkipEmptyParts);
            QSet<QString> palabrasUnicas = QSet<QString>(palabras.begin(), palabras.end());
            for (const QString& palabra : palabrasUnicas) {
                indiceInvertido[palabra].append(obj);
            }
            if (i < 3) {
                resumenDebug += obj["id"].toString() + "\n";
            }
            ++i;
        }

        QMessageBox::information(this, "Debug Índice (post-API)", resumenDebug);
        if (trie) {
            delete trie;
            trie = nullptr;
        }
        trie = new Trie();


        for (const QString& t : titulos) trie->insert(t);
    }

    // refrescar la vista

    filtrarNoticias();
}

void MainWindow::filtrarNoticias()
{
    // 1. Rango de fechas y fuente seleccionada
    QDateTime fechaInicio = ui->fechaInicio->dateTime();
    QDateTime fechaFin = ui->fechaFin->dateTime();
    QString fuenteSeleccionada = ui->fuenteBox->currentText();

    // 2. Temas seleccionados
    QSet<QString> temasSeleccionados;
    if (ui->politicaTema->isChecked()) temasSeleccionados << "política";
    if (ui->culturaTema->isChecked()) temasSeleccionados << "cultura";
    if (ui->sociedadTema->isChecked()) temasSeleccionados << "sociedad";
    if (ui->economiaTema->isChecked()) temasSeleccionados << "economía";
    if (ui->deportesTema->isChecked()) temasSeleccionados << "deportes";
    if (ui->entretenimientoTema->isChecked()) temasSeleccionados << "entretenimiento";

    bool incluirOtros = ui->otrosTema->isChecked();

    // 3. Traducciones
    QMap<QString, QString> traduccionesTemas = {
        {"business", "negocios"}, {"sports", "deportes"}, {"technology", "tecnología"},
        {"politics", "política"}, {"health", "salud"}, {"science", "ciencia"},
        {"entertainment", "entretenimiento"}, {"world", "mundo"}, {"economy", "economía"},
        {"education", "educación"}, {"travel", "viajes"}, {"food", "comida"},
        {"environment", "medio ambiente"}, {"top", "principales"}, {"finance", "finanzas"},
        {"law", "ley"}, {"culture", "cultura"}, {"fashion", "moda"},
        {"crime", "crimen"}, {"weather", "clima"}, {"real estate", "bienes raíces"},
        {"society", "sociedad"}
    };

    // 4. Limpiar layout actual
    QWidget* contenedor = ui->noticiasScroll->widget();
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contenedor->layout());
    if (!layout) {
        layout = new QVBoxLayout(contenedor);
        contenedor->setLayout(layout);
    }

    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (QWidget* widget = child->widget()) widget->deleteLater();
        delete child;
    }

    // 5. Filtrar noticias
    QList<QJsonObject> noticiasFiltradas;
    for (const QJsonValue& value : noticiasOriginales) {
        QJsonObject obj = value.toObject();
        QDateTime fechaNoticia = QDateTime::fromString(obj["fecha"].toString(), Qt::ISODate);

        if (fechaNoticia < fechaInicio || fechaNoticia > fechaFin) continue;
        if (fuenteSeleccionada != "Todas" && obj["fuente"].toString() != fuenteSeleccionada) continue;

        QJsonArray temasJson = obj["temas"].toArray();
        QString descripcion = obj["descripcion"].toString();

        // Paso 3: corregir tema si es genérico
        bool tieneTemaGenerico = false;
        for (const QJsonValue& tema : temasJson) {
            QString t = tema.toString().toLower();
            if (t == "top" || t == "principales") {
                tieneTemaGenerico = true;
                break;
            }
        }

        if (tieneTemaGenerico) {
            QString temaDetectado = clasificarTemaPorDescripcion(descripcion, palabrasClavePorTema);
            if (!temaDetectado.isEmpty()) {
                temasJson = QJsonArray();
                temasJson.append(temaDetectado);
                obj["temas"] = temasJson;
            }
        }

        // Traducción para evaluación
        QStringList temasTraducidos;
        for (const QJsonValue& tema : temasJson) {
            QString temaEsp = traduccionesTemas.value(tema.toString(), tema.toString());
            temasTraducidos << temaEsp;
        }

        // Filtrado por tema
        bool coincide = false;
        for (const QString& tema : temasTraducidos) {
            if (temasSeleccionados.contains(tema)) {
                coincide = true;
                break;
            }
        }

        // Si "Otros" está marcado → mostrar todas las noticias
        if (incluirOtros) {
            noticiasFiltradas.append(obj);
            continue;
        }

        // Si no hay temas seleccionados → no filtrar por tema
        if (temasSeleccionados.isEmpty()) {
            noticiasFiltradas.append(obj);
            continue;
        }

        // Si hay temas seleccionados, mantener solo las que coinciden
        if (coincide) {
            noticiasFiltradas.append(obj);
        }

    }

    // 6. Ordenamiento
    std::sort(noticiasFiltradas.begin(), noticiasFiltradas.end(), [this](const QJsonObject& a, const QJsonObject& b) {
        // Orden por fecha
        if (ui->fechaArribaRadioB->isChecked() || ui->fechaAbajoRadioB->isChecked()) {
            QDateTime fa = QDateTime::fromString(a["fecha"].toString(), Qt::ISODate);
            QDateTime fb = QDateTime::fromString(b["fecha"].toString(), Qt::ISODate);
            return ui->fechaArribaRadioB->isChecked() ? fa < fb : fa > fb;
        }

        // Orden por fuente
        if (ui->fuenteAZRadioB->isChecked() || ui->fuenteZARadioB->isChecked()) {
            QString sa = a["fuente"].toString().toLower();
            QString sb = b["fuente"].toString().toLower();
            return ui->fuenteAZRadioB->isChecked() ? sa < sb : sa > sb;
        }

        // Orden por prioridad (número entero)
        if (ui->importanciaArribaRadioB->isChecked() || ui->importanciaAbajoRadioB->isChecked()) {
            int ta = a["prioridad"].toInt();
            int tb = b["prioridad"].toInt();
            return ui->importanciaArribaRadioB->isChecked() ? ta < tb : ta > tb;
        }

        return false;
    });


    // 7. Mostrar noticias ordenadas
    for (const QJsonObject& obj : noticiasFiltradas) {
        QJsonArray temasJson = obj["temas"].toArray();
        QStringList temasTraducidos;
        for (const QJsonValue& tema : temasJson) {
            QString temaEsp = traduccionesTemas.value(tema.toString(), tema.toString());
            temasTraducidos << temaEsp;
        }

        NewsComponent* comp = new NewsComponent(this);
        //agregar el id de la noticia!
        comp->setIdUrl(obj["id"].toString());
        comp->setTextPath(obj["ruta_contenido"].toString());
        comp->setImgUrl(obj["url_imagen"].toString());


        comp->setFixedHeight(150);
        comp->setDate(QDateTime::fromString(obj["fecha"].toString(), Qt::ISODate));
        comp->setFuente(obj["fuente"].toString());
        comp->setTitle(obj["titulo"].toString());
        comp->setTema(temasTraducidos.join(", "));

        layout->addWidget(comp);
    }

    layout->addStretch();
}

void MainWindow::on_configuracionButton_clicked()
{
    // Prueba esto en tu código para verificar
    QPixmap pix(":/new/img/funcion.png");
    if(pix.isNull()) {
        qDebug() << "Error: No se encontró la imagen";
    } else {
        ui->label->setPixmap(pix);
        ui->label->setScaledContents(true); // Ajusta tamaño
    }
}
