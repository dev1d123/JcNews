import requests
import os
import json
from storage import save_article  


DATA_DIR = "../../../../noticiasXDXD"
JSON_FILENAME = "metadatos_noticias.json"
API_KEY = "03bdbb096ace4615a704708025189849"
# cargar las noticias previamente guardadas
def load_existing_data(path):
    try:
        # abrir el json de metadatos
        f = open(path, 'r', encoding='utf-8')
        data = json.load(f)
        f.close()

        # Crear un conjunto con las URLs ya guardadas para evitar duplicados
        urls = set()
        for item in data:
            urls.add(item['url'])

        return data, urls
    except (FileNotFoundError, json.JSONDecodeError):
        return [], set()


# guardar todas las noticias en el json
def save_data(path, data):
    f = open(path, 'w', encoding='utf-8')
    json.dump(data, f, ensure_ascii=False, indent=2)
    f.close()

# procesar las noticias devueltas por la api
def process_articles(api_data, existing_data, existing_urls):
    new_count = 0

    for article in api_data['articles']:
        result = save_article(article, existing_urls)
        if result is not None:
            existing_data.append(result)
            existing_urls.add(result['url'])
            new_count += 1

    return new_count


def run_api_process(api_url):
    response = requests.get(api_url)
    data = response.json()

    if data['status'] != 'ok':
        print("API error:", data.get('message', 'Unknown error'))
        return

    json_path = os.path.join(DATA_DIR, JSON_FILENAME)
    existing_data, existing_urls = load_existing_data(json_path)

    new_articles = process_articles(data, existing_data, existing_urls)

    save_data(json_path, existing_data)

    print("Nuevos articulos agregados:", new_articles)
    print("Total de articulos almacenados:", len(existing_data))



if __name__ == "__main__":
    # solo trae noticia relacionadas al futbol.
    print("EJECUTANDO MAIN")
    API_URL = f"https://newsapi.org/v2/everything?q=football&language=es&apiKey={API_KEY}"
    run_api_process(API_URL)
    sys.exit(0)
