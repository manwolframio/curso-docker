from flask import Flask, render_template_string, jsonify

app = Flask(__name__)

HTML_INDEX = """
<!doctype html>
<html lang="es">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Web de prueba 1</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
</head>
<body>
    <div class="container py-5">
        <div class="p-5 mb-4 bg-light border rounded-3">
            <div class="container-fluid py-3">
                <h1 class="display-5 fw-bold">Web de prueba 2</h1>
                <p class="col-md-8 fs-4">Aplicación Flask funcionando correctamente.</p>
            </div>
        </div>
    </div>
</body>
</html>
"""

@app.route("/index")
def index():
    return render_template_string(HTML_INDEX)

@app.route("/healthcheck")
def healthcheck():
    return jsonify({"status": "ok"}), 200