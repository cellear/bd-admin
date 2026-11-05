#!/usr/bin/env bash
# Installs Backdrop CMS (Standard profile) into backdrop/ with the Sample Animal
# Content module and all its extras, then links in and enables the wp_admin
# module from this repo. Uses the project MariaDB (run scripts/start-db.sh first).
#   scripts/install-backdrop.sh            fresh install (drops the database tables)
#   scripts/install-backdrop.sh --no-wp-admin   stop before enabling wp_admin
set -euo pipefail
source "$(dirname "$0")/env.sh"
DL="$ROOT/downloads"
mkdir -p "$DL" "$ROOT/scripts/bin"

# Bee CLI.
if [ ! -d "$DL/bee" ]; then
  git clone -q --depth 1 --branch 1.x-1.x https://github.com/backdrop-contrib/bee "$DL/bee"
fi
ln -sfn "$DL/bee/bee.php" "$ROOT/scripts/bin/bee"
chmod +x "$DL/bee/bee.php"

# Backdrop core.
if [ ! -f "$DL/backdrop.zip" ]; then
  curl -fsSL -o "$DL/backdrop.zip" "https://github.com/backdrop/backdrop/releases/download/$BACKDROP_VERSION/backdrop.zip"
fi
rm -rf "$BACKDROP_DIR" "$DL/extracted"
unzip -q "$DL/backdrop.zip" -d "$DL/extracted"
mv "$DL/extracted/backdrop" "$BACKDROP_DIR"
chmod -R u+w "$BACKDROP_DIR"
mkdir -p "$BACKDROP_DIR/files" && chmod 777 "$BACKDROP_DIR/files"

# Sample Animal Content.
if [ ! -f "$DL/sample_animal_content.zip" ]; then
  curl -fsSL -o "$DL/sample_animal_content.zip" "https://github.com/backdrop-contrib/sample_animal_content/releases/download/$SAMPLE_CONTENT_VERSION/sample_animal_content.zip"
fi
unzip -q -o "$DL/sample_animal_content.zip" -d "$BACKDROP_DIR/modules"

# Our module.
ln -sfn "$ROOT/modules/wp_admin" "$BACKDROP_DIR/modules/wp_admin"

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
