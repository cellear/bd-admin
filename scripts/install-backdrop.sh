#!/usr/bin/env bash
# Installs Backdrop CMS (Standard profile) into backdrop/ with the Sample Animal
# Content module and all its extras, then enables the wp_admin module from this
# repo. Uses the project MariaDB (run scripts/start-db.sh first). For ddev use
# scripts/ddev-setup.sh instead.
#   scripts/install-backdrop.sh            fresh install (drops the database tables)
#   scripts/install-backdrop.sh --no-wp-admin   stop before enabling wp_admin
set -euo pipefail
source "$(dirname "$0")/env.sh"
"$ROOT/scripts/download-backdrop.sh" --force

cd "$BACKDROP_DIR"
mariadb -u root -S "$DB_SOCK" -e "DROP DATABASE IF EXISTS backdrop; CREATE DATABASE backdrop CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci; GRANT ALL ON backdrop.* TO '$DB_USER'@'127.0.0.1', '$DB_USER'@'localhost';"
bee install --db-name=backdrop --db-user="$DB_USER" --db-pass="$DB_PASS" --db-host="127.0.0.1:$DB_PORT" \
  --username="$ADMIN_USER" --password="$ADMIN_PASS" --email=admin@example.com \
  --site-name="Backdrop Zoo" --site-mail=admin@example.com --profile=standard --auto

bee enable sample_animal_content -y
bee php-script "$ROOT/scripts/enable-sample-extras.php"
if [ "${1:-}" != "--no-wp-admin" ] && [ -f "$ROOT/modules/wp_admin/wp_admin.info" ]; then
  bee enable wp_admin -y
fi
bee cc all
bee status 2>/dev/null | sed -n 2,12p || true
echo "Backdrop installed. Serve with scripts/serve.sh → http://127.0.0.1:$BACKDROP_PORT (admin/admin)"
