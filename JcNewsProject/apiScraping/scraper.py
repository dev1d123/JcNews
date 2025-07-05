import requests
from bs4 import BeautifulSoup 

"""
    requests -> descarga la pagina
    BeautifulSoup -> extrae el contenido
    
"""
def get_full_article(url):

    try:
        headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
        }

        # Hacer la petición a la URL
        response = requests.get(url, headers=headers, timeout=15)

        # Analizar el HTML con BeautifulSoup
        soup = BeautifulSoup(response.text, 'html.parser')

       

        # Obtener todos los parrafos
        paragraphs = soup.find_all('p')

        # acumular el texto limpio
        clean_paragraphs = []
        for p in paragraphs:
            text = p.get_text(separator=" ", strip=True)
            if text != "":
                clean_paragraphs.append(text)

        # unir todos los párrafos en una sola cadena
        content = " ".join(clean_paragraphs)

        # devolver el titulo y el contenido que está limitado a 20000 caracteres
        return content[:20000]

    except Exception as e:
        #print("Error al obtener", url, ":", str(e))
        return None, None
