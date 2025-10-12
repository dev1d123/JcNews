from flask import Flask, request, jsonify
import requests
import os
from storage import save_article

# 🔑 Clave de la API
API_KEY = "pub_14129718c2424ade985d48e9d12db439"
BASE_URL = f"https://newsdata.io/api/1/latest?apikey={API_KEY}&country=pe"

app = Flask(__name__)


def fetch_articles_from_api(max_articles=30):
    """
    Descarga artículos de NewsData.io usando 'nextPage' para obtener varias páginas.
    Se detiene cuando alcanza max_articles o ya no hay más páginas disponibles.
    """
    collected = []
    existing_urls = set()
    total_new = 0

    next_page = None  # control de paginación

    while total_new < max_articles:
        # Construir la URL con paginación
        url = BASE_URL
        if next_page:
            url += f"&page={next_page}"

        try:
            resp = requests.get(url, timeout=15)
            resp.raise_for_status()
            data = resp.json()
        except requests.RequestException as e:
            print(f"⚠️ Error de conexión con la API: {e}")
            break

        # Si la API devuelve error o estructura inesperada
        if data.get("status") != "success":
            print("⚠️ Error en la respuesta de la API:", data.get("message", "Desconocido"))
            break

        # Procesar artículos recibidos
        for article in data.get("results", []):
            if total_new >= max_articles:
                break

            result = save_article(article, existing_urls)
            if result:
                collected.append(result)
                existing_urls.add(result["url"])
                total_new += 1

        # Avanzar a la siguiente página si existe
        next_page = data.get("nextPage")
        if not next_page:
            break  # No hay más páginas

    return collected


@app.route("/articles", methods=["GET"])
def articles_endpoint():
    """
    GET /articles?count=30
    - count (opcional): número máximo de artículos a devolver (por defecto 30)
    """
    count_param = request.args.get("count", "30")
    try:
        count = int(count_param)
        if count <= 0:
            raise ValueError()
    except ValueError:
        return jsonify({"error": "El parámetro 'count' debe ser un entero positivo."}), 400

    articles = fetch_articles_from_api(max_articles=count)
    return jsonify({
        "requested": count,
        "returned": len(articles),
        "articles": articles
    }), 200


if __name__ == "__main__":
    port = int(os.environ.get("PORT", 5000))
    print(f"🚀 Servidor Flask corriendo en http://127.0.0.1:{port}")
    app.run(host="0.0.0.0", port=port)
