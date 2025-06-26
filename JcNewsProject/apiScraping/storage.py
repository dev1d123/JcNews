import os
from datetime import datetime
from scraper import get_full_article
from utils import sanitize_filename

def save_article(article_data, existing_urls, data_dir):
    url = article_data.get('link') or article_data.get('url')
    if not url:
        return None

    if url in existing_urls:
        print(f"Noticia ya existe: {article_data.get('title', 'Sin título')}")
        return None

    full_content = get_full_article(url)
    if not full_content:
        return None

    txt_filename = sanitize_filename(url)
    txt_path = os.path.join(data_dir, txt_filename)

    txt_content = f"""Título: {article_data.get('title', 'Sin título')}
Fuente: {article_data.get('source_name', 'Desconocida')}
URL: {url}
Fecha: {article_data.get('pubDate', 'Desconocida')}
---
{full_content}
"""

    with open(txt_path, 'w', encoding='utf-8') as f:
        f.write(txt_content)

    return {
        "id": url,
        "titulo": article_data.get('title'),
        "fuente": article_data.get('source_name'),
        "autor": (article_data.get('creator') or [None])[0],
		"fecha": article_data.get('pubDate'),
        "url": url,
        "url_imagen": article_data.get('image_url'),
        "ruta_contenido": txt_path,
        "descripcion": article_data.get('description'),
        "fecha_procesamiento": datetime.now().isoformat()
    }

