import requests
import os
import json
import sys
from storage import save_article  

JSON_FILENAME = "metadatos_noticias.json"
API_KEY = "03bdbb096ace4615a704708025189849"

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
    for article in api_data['articles']:
        result = save_article(article, existing_urls, data_dir)
        if result is not None:
            existing_data.append(result)
            existing_urls.add(result['url'])
            new_count += 1
    return new_count

def run_api_process(api_url, data_dir):
    os.makedirs(data_dir, exist_ok=True)
    json_path = os.path.join(data_dir, JSON_FILENAME)

    response = requests.get(api_url)
    data = response.json()

    if data['status'] != 'ok':
        print("API error:", data.get('message', 'Unknown error'))
        return

    existing_data, existing_urls = load_existing_data(json_path)
    new_articles = process_articles(data, existing_data, existing_urls, data_dir)
    save_data(json_path, existing_data)

    print("Nuevos artículos agregados:", new_articles)
    print("Total de artículos almacenados:", len(existing_data))


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python main.py <directorio_destino>")
        sys.exit(1)

    data_dir = sys.argv[1]
    print(f"EJECUTANDO MAIN. Carpeta destino: {data_dir}")

    API_URL = f"https://newsapi.org/v2/everything?q=football&language=es&apiKey={API_KEY}"
    run_api_process(API_URL, data_dir)

