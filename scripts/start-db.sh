#!/usr/bin/env bash
# Starts a local MariaDB server owned by this project (data in .db/) and creates
# the `backdrop` and `wordpress_bd` databases plus the `dev` user.
# Requires: apt-get install mariadb-server mariadb-client
set -euo pipefail
source "$(dirname "$0")/env.sh"

if [ -S "$DB_SOCK" ] && mariadb -u root -S "$DB_SOCK" -e 'select 1' >/dev/null 2>&1; then
  echo "MariaDB already running on $DB_SOCK"
  exit 0
fi

mkdir -p "$DB_DIR/data"
RUN_AS=""; [ "$(id -u)" = 0 ] && RUN_AS="--user=mysql" && chown -R mysql:mysql "$DB_DIR"
if [ ! -d "$DB_DIR/data/mysql" ]; then
  mariadb-install-db $RUN_AS --datadir="$DB_DIR/data" --auth-root-authentication-method=normal >/dev/null
fi

setsid mariadbd $RUN_AS --datadir="$DB_DIR/data" --socket="$DB_SOCK" --port="$DB_PORT" \
  --bind-address=127.0.0.1 --pid-file="$DB_DIR/mysqld.pid" \
  --log-error="$DB_DIR/mariadb.log" --skip-name-resolve >/dev/null 2>&1 < /dev/null &

for i in $(seq 1 30); do
  if mariadb -u root -S "$DB_SOCK" -e 'select 1' >/dev/null 2>&1; then break; fi
  sleep 1
done

mariadb -u root -S "$DB_SOCK" <<SQL
CREATE DATABASE IF NOT EXISTS backdrop CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
CREATE DATABASE IF NOT EXISTS wordpress_bd CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci;
CREATE USER IF NOT EXISTS '$DB_USER'@'localhost' IDENTIFIED BY '$DB_PASS';
CREATE USER IF NOT EXISTS '$DB_USER'@'127.0.0.1' IDENTIFIED BY '$DB_PASS';
GRANT ALL ON backdrop.* TO '$DB_USER'@'localhost', '$DB_USER'@'127.0.0.1';
GRANT ALL ON wordpress_bd.* TO '$DB_USER'@'localhost', '$DB_USER'@'127.0.0.1';
GRANT ALL ON \`backdrop_test%\`.* TO '$DB_USER'@'localhost', '$DB_USER'@'127.0.0.1';
FLUSH PRIVILEGES;
SQL
echo "MariaDB running: socket $DB_SOCK, port $DB_PORT, databases backdrop + wordpress_bd, user $DB_USER"
