<?php
$info = [
  'Sistema operativo'     => PHP_OS,
  'Versión PHP'           => phpversion(),
  'Método de servidor'    => $_SERVER['REQUEST_METHOD'] ?? 'N/A',
  'Dirección remota'      => $_SERVER['REMOTE_ADDR'] ?? 'N/A',
  'Software del servidor' => $_SERVER['SERVER_SOFTWARE'] ?? 'N/A',
  'Nombre del servidor'   => $_SERVER['SERVER_NAME'] ?? 'N/A',
];
?>
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Información del servidor</title>
  <link rel="stylesheet" href="assets/bootstrap/css/bootstrap.min.css">
  <link rel="stylesheet" href="assets/css/estilos.css">
</head>
<body>
  <div class="container py-5">
    <div class="row justify-content-center">
      <div class="col-lg-8">
        <div class="card shadow-sm">
          <div class="card-body">
            <h1 class="mb-4 text-center">Información básica del servidor</h1>
            <table class="table table-hover">
              <tbody>
              <?php foreach ($info as $k => $v): ?>
                <tr>
                  <th scope="row" style="width:40%"><?= htmlspecialchars($k) ?></th>
                  <td><?= htmlspecialchars((string)$v) ?></td>
                </tr>
              <?php endforeach; ?>
              </tbody>
            </table>
            <div class="text-center">
              <a class="btn btn-outline-secondary" href="index.php">Volver al inicio</a>
            </div>
          </div>
        </div>
        <p class="text-center mt-4 text-muted">Archivo: <code>web/miweb/info.php</code></p>
      </div>
    </div>
  </div>

  <script src="assets/bootstrap/js/bootstrap.bundle.min.js"></script>
</body>
</html>
