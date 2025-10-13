"""
Punto de entrada WSGI para la aplicación Flask.
"""

from app import app  # Importa la instancia de Flask desde app.py

# Si se ejecuta directamente (modo desarrollo)
if __name__ == "__main__":
    # Puedes cambiar el puerto si lo necesitas
    app.run(host="0.0.0.0", port=5000, debug=False)

