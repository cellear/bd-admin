#!/usr/bin/env bash
# Downloads the latest WordPress into wordpress/ and installs it against the
# project MariaDB (run scripts/start-db.sh first).
set -euo pipefail
source "$(dirname "$0")/env.sh"

mkdir -p "$ROOT/scripts/bin"
if [ ! -x "$ROOT/scripts/bin/wp" ]; then
  curl -fsSL -o "$ROOT/scripts/bin/wp" https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar
  chmod +x "$ROOT/scripts/bin/wp"
fi

if [ ! -f "$WP_DIR/wp-load.php" ]; then
  mkdir -p "$WP_DIR"
  wp core download --path="$WP_DIR" --version=latest --locale=en_US
fi
cd "$WP_DIR"
if [ ! -f wp-config.php ]; then
  wp config create --dbname=wordpress_bd --dbuser="$DB_USER" --dbpass="$DB_PASS" \
    --dbhost="127.0.0.1:$DB_PORT" --dbcharset=utf8mb4 --skip-check
fi
if ! wp core is-installed 2>/dev/null; then
  wp core install --url="http://127.0.0.1:$WP_PORT" --title="Backdrop Zoo" \
    --admin_user="$ADMIN_USER" --admin_password="$ADMIN_PASS" \
    --admin_email="admin@example.com" --skip-email
fi
wp core version
