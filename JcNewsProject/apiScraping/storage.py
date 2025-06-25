import os
import json
from datetime import datetime
from scraper import get_full_article
from utils import sanitize_filename

DATA_DIR = "../../../../noticiasXDXD"
os.makedirs(DATA_DIR, exist_ok=True)


def save_article(article_data, existing_urls):
    url = article_data['url']
    if url in existing_urls:
        print(f"Noticia ya existe: {article_data['title']}")
        return None
    full_content = get_full_article(url)
    
    if not full_content: return None
    
    # generar nombre de archivo seguro
    txt_filename = sanitize_filename(url)
    txt_path = os.path.join(DATA_DIR,txt_filename)
    
    # preparar el contenido del archivo
    txt_content = f"""Título: {article_data['title']}
Fuente: {article_data['source']['name']}
URL: {url}
Fecha: {article_data['publishedAt']}
---
{full_content}
"""
    
    # abrir el txt y llenarlo de informacion
    f = open(txt_path, 'w', encoding='utf-8')
    f.write(txt_content)
    f.close()
    
    return {
        "id": url,
        "titulo": article_data['title'],
        "fuente": article_data['source']['name'],
        "autor": article_data.get('author'),
        "fecha": article_data['publishedAt'],
        "url": url,
        "url_imagen": article_data.get('urlToImage'),
        "ruta_contenido": txt_path,
        "descripcion": article_data['description']
    }
