def leer_archivo(ruta="./texto.txt"):
    try:
        with open(ruta, "r", encoding="utf-8") as f:
            contenido = f.read()
            print(contenido)
    except FileNotFoundError:
        print(f"El archivo '{ruta}' no existe.")

if __name__ == "__main__":
    leer_archivo()
