import requests
from bs4 import BeautifulSoup

def get_full_article(url):
    """
    Devuelve el contenido del artículo como texto (máx 20000 chars) o None si falla.
    """
    try:
        headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
                          "AppleWebKit/537.36 (KHTML, like Gecko) "
                          "Chrome/91.0.4472.124 Safari/537.36"
        }

        response = requests.get(url, headers=headers, timeout=15)
        response.raise_for_status()

        soup = BeautifulSoup(response.text, "html.parser")
        paragraphs = soup.find_all("p")

        clean_paragraphs = []
        for p in paragraphs:
            text = p.get_text(separator=" ", strip=True)
            if text:
                clean_paragraphs.append(text)

        content = " ".join(clean_paragraphs)
        return content[:20000] if content else None

    except Exception:
        return None