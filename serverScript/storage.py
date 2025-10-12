from datetime import datetime
from scraper import get_full_article

def save_article(article_data, existing_urls):
    """
    Transforma los datos crudos de la API en el diccionario que antes se guardaba en disco.
    No realiza operaciones de archivo. Si el artículo ya existe (por URL) o no se puede obtener
    el contenido completo mediante el scraper, devuelve None.
    """
    url = article_data.get("link") or article_data.get("url")
    if not url:
        return None

    if url in existing_urls:
        return None

    # Obtener contenido completo con el scraper (antes esto se guardaba en .txt)
    full_content = get_full_article(url)
    if not full_content:
        # Si no se pudo obtener el contenido, descartamos (mismo comportamiento previo)
        return None

    return {
        "id": url,
        "titulo": article_data.get("title"),
        "fuente": article_data.get("source_name"),
        # 'creator' en la API a veces es lista, intentamos extraer el primer autor
        "autor": (article_data.get("creator") or [None])[0],
        "fecha": article_data.get("pubDate"),
        "url": url,
        "url_imagen": article_data.get("image_url"),
        # Antes se guardaba la ruta del archivo con el contenido; ahora incluimos el contenido directamente
        "ruta_contenido": full_content,
        "descripcion": article_data.get("description"),
        "fecha_procesamiento": datetime.now().isoformat(),
        "temas": article_data.get("category") or [],
        "prioridad": article_data.get("source_priority", "0")
    }