import requests
from bs4 import BeautifulSoup
import json
import os
from urllib.parse import quote_plus
import re
from datetime import datetime

# Configuración
DATA_DIR = "../../../../noticiasXDXD"
os.makedirs(DATA_DIR, exist_ok=True)

def sanitize_filename(url):
    """Crea un nombre de archivo seguro a partir de la URL."""
    # Extraer la parte relevante de la URL (después del dominio)
    cleaned = re.sub(r'https?://[^/]+/', '', url)
    # Reemplazar caracteres problemáticos
    cleaned = quote_plus(cleaned)[:150]  # Limitar longitud
    return cleaned + '.txt'

def get_full_article(url):
    """Obtiene el contenido completo de un artículo."""
    try:
        headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
        }
        response = requests.get(url, headers=headers, timeout=15)
        soup = BeautifulSoup(response.text, 'html.parser')
        
        title_tag = soup.find('title')
        page_title = title_tag.get_text(strip=True) if title_tag else "Sin título"
        
        paragraphs = soup.find_all('p')
        content = " ".join([p.get_text(strip=True) for p in paragraphs if p.get_text(strip=True)])
        
        return page_title, content[:20000]  # Limitar a 20k caracteres
    
    except Exception as e:
        print(f"Error al obtener {url}: {str(e)}")
        return None, None

def guardar_noticia(article_data, existing_urls):
    """Guarda el contenido en TXT si la URL no existe."""
    url = article_data['url']
    
    # Verificar si ya existe
    if url in existing_urls:
        print(f"Noticia ya existe: {article_data['title']}")
        return None
    
    # Obtener contenido completo
    page_title, full_content = get_full_article(url)
    
    if not full_content:
        return None
    
    # Crear nombre de archivo seguro
    txt_filename = sanitize_filename(url)
    txt_path = os.path.join(DATA_DIR, txt_filename)
    
    # Crear estructura del TXT
    txt_content = f"""Título: {page_title}
Fuente: {article_data['source']['name']}
URL: {url}
Fecha: {article_data['publishedAt']}
---
{full_content}
"""
    # Guardar TXT
    with open(txt_path, 'w', encoding='utf-8') as f:
        f.write(txt_content)
    
    return {
        "id": url,  # Usamos directamente la URL como ID
        "titulo": article_data['title'],
        "fuente": article_data['source']['name'],
        "autor": article_data.get('author'),
        "fecha": article_data['publishedAt'],
        "url": url,
        "url_imagen": article_data.get('urlToImage'),
        "ruta_contenido": txt_path,
        "descripcion": article_data['description'],
        "fecha_procesamiento": datetime.now().isoformat()  # Añadimos marca temporal
    }

def procesar_api(news_api_url):
    """Procesa los datos de la API evitando duplicados."""
    response = requests.get(news_api_url)
    data = response.json()
    
    if data['status'] != 'ok':
        print("Error en la API:", data.get('message', 'Desconocido'))
        return
    
    # Cargar datos existentes
    json_path = os.path.join(DATA_DIR, "metadatos_noticias.json")
    
    try:
        with open(json_path, 'r', encoding='utf-8') as f:
            existing_data = json.load(f)
            existing_urls = {item['url'] for item in existing_data}  # Conjunto de URLs existentes
    except (FileNotFoundError, json.JSONDecodeError):
        existing_data = []
        existing_urls = set()
    
    nuevos = 0
    
    for article in data['articles']:
        processed = guardar_noticia(article, existing_urls)
        if processed:
            existing_data.append(processed)
            existing_urls.add(processed['url'])
            nuevos += 1
    
    # Guardar metadatos actualizados
    with open(json_path, 'w', encoding='utf-8') as f:
        json.dump(existing_data, f, ensure_ascii=False, indent=2)
    
    print(f"Noticias nuevas agregadas: {nuevos}. Total en archivo: {len(existing_data)}")

if __name__ == "__main__":
    API_URL = "https://newsapi.org/v2/everything?q=fútbol&language=es&apiKey=03bdbb096ace4615a704708025189849"
    procesar_api(API_URL)
