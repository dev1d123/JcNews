
import re
from urllib.parse import quote_plus

"""
    re: Manejar expresiones regulares
    quote_plus: convierte carácteres especiales "?/:" en una versión sgura para los archivos
        " "	+
        /	%2F
        ?	%3F
        =	%3D
        &	%26
"""

def sanitize_filename(url):
    """
        Crea un nombre de archivo seguro a partir de la URL, ya que la URL es un valor
        de la noticia. Esto evita guardar noticias duplicadas
    """
    # Extrae la parte relevante de la URL
    cleaned = re.sub(r'https?://[^/]+/', '', url)
    # Reemplazar caracteres problemáticos
    cleaned = quote_plus(cleaned)[:150]  # limitar longitud del nombre
    return cleaned + '.txt'
