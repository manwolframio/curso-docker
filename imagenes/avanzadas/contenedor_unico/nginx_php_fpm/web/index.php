<?php
$response = [
  'message' => '¡Bienvenido a tu servidor Apache + PHP + HTTPS con Bootstrap!',
  'datetime' => date('Y-m-d H:i:s'),
  'php_version' => phpversion(),
  'server_name' => $_SERVER['SERVER_NAME'] ?? 'N/A',
];
?>
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Demo Bootstrap</title>
  <link rel="stylesheet" href="assets/bootstrap/css/bootstrap.min.css">
  <link rel="stylesheet" href="assets/css/estilos.css">
</head>
<body>
  <nav class="navbar navbar-expand-lg navbar-dark bg-dark">
    <div class="container">
      <a class="navbar-brand" href="#">Mi Web</a>
      <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#nav"
        aria-controls="nav" aria-expanded="false" aria-label="Toggle navigation">
        <span class="navbar-toggler-icon"></span>
      </button>
      <div id="nav" class="collapse navbar-collapse">
        <ul class="navbar-nav ms-auto">
          <li class="nav-item"><a class="nav-link active" href="index.php">Inicio</a></li>
          <li class="nav-item"><a class="nav-link" href="info.php">Información</a></li>
        </ul>
      </div>
    </div>
  </nav>

  <main class="container py-5">
    <div class="row justify-content-center">
      <div class="col-lg-8">
        <div class="card shadow-sm">
          <div class="card-body">
            <h1 class="card-title text-center mb-4"><?= htmlspecialchars($response['message']) ?></h1>
            <p class="text-center text-muted">Esta página usa Bootstrap <?= htmlspecialchars($response['php_version']) ?><!-- solo para ejemplo --></p>
            <table class="table table-striped">
              <tbody>
                <tr><th scope="row">Fecha y hora</th><td><?= htmlspecialchars($response['datetime']) ?></td></tr>
                <tr><th scope="row">Versión de PHP</th><td><?= htmlspecialchars($response['php_version']) ?></td></tr>
                <tr><th scope="row">Servidor</th><td><?= htmlspecialchars($response['server_name']) ?></td></tr>
              </tbody>
            </table>
            <div class="text-center">
              <a class="btn btn-primary" href="info.php">Ver información del servidor</a>
            </div>
          </div>
        </div>
        <p class="text-center mt-4 text-muted">Edita esta página en <code>web/miweb/index.php</code>.</p>
      </div>
    </div>
  </main>

  <script src="assets/bootstrap/js/bootstrap.bundle.min.js"></script>
</body>
</html>
