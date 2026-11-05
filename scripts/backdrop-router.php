<?php
/**
 * Router for PHP's built-in web server: clean URLs for Backdrop.
 *   php -S 127.0.0.1:8082 ../scripts/backdrop-router.php   (from backdrop/)
 */
$path = parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH);
$file = __DIR__ . '/../backdrop' . $path;
// Serve existing files (CSS, JS, images, uploads) directly.
if ($path !== '/' && is_file($file)) {
  return FALSE;
}
// Everything else goes through Backdrop's front controller with ?q=path.
$_GET['q'] = ltrim($path, '/');
$_SERVER['SCRIPT_NAME'] = '/index.php';
$_SERVER['SCRIPT_FILENAME'] = __DIR__ . '/../backdrop/index.php';
chdir(__DIR__ . '/../backdrop');
require 'index.php';
