import requests
import os
import json
import sys
from storage import save_article  

JSON_FILENAME = "metadatos_noticias.json"
API_KEY = "pub_14129718c2424ade985d48e9d12db439"

def load_existing_data(path):
    try:
        with open(path, 'r', encoding='utf-8') as f:
            data = json.load(f)
        urls = {item['url'] for item in data}
        return data, urls
    except (FileNotFoundError, json.JSONDecodeError):
        return [], set()

def save_data(path, data):
    with open(path, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=2)

def process_articles(api_data, existing_data, existing_urls, data_dir):
    new_count = 0
    for article in api_data.get('results', []):  # ✅ CAMBIO AQUÍ
        result = save_article(article, existing_urls, data_dir)
        if result:
            existing_data.append(result)
            existing_urls.add(result['url'])
            new_count += 1
    return new_count

def run_api_process(api_url_base, data_dir, max_articles=10):
    os.makedirs(data_dir, exist_ok=True)
    json_path = os.path.join(data_dir, JSON_FILENAME)

    existing_data, existing_urls = load_existing_data(json_path)

    total_new = 0
    next_page = None

    while total_new < max_articles:
        # Agrega el parámetro de página si existe
        url = api_url_base
        if next_page:
            url += f"&page={next_page}"

        try:
            response = requests.get(url)
            response.raise_for_status()
            data = response.json()
        except requests.RequestException as e:
            #print(f"Error al conectarse a la API: {e}")
            break

        if data.get('status') != 'success':
            #print("API error:", data.get('message', 'Error desconocido'))
            break

        nuevos = process_articles(data, existing_data, existing_urls, data_dir)
        total_new += nuevos

        #print(f"Artículos nuevos procesados en esta página: {nuevos}")

        next_page = data.get('nextPage')
        if not next_page:
            break  # no hay más páginas

    save_data(json_path, existing_data)

    #print("\n✅ Proceso finalizado.")
    #print(f"📰 Total de artículos nuevos agregados: {total_new}")
    #print(f"📁 Total acumulado en JSON: {len(existing_data)}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        #print("Uso: python main.py <directorio_destino>")
        sys.exit(1)

    data_dir = sys.argv[1]
    #print(f"EJECUTANDO MAIN. Carpeta destino: {data_dir}")

    API_URL = f"https://newsdata.io/api/1/latest?apikey={API_KEY}&country=pe"
    run_api_process(API_URL, data_dir)

