#!/usr/bin/env bash
# Serves both sites with PHP's built-in server:
#   Backdrop   http://127.0.0.1:8082
#   WordPress  http://127.0.0.1:8083
set -euo pipefail
source "$(dirname "$0")/env.sh"
"$(dirname "$0")/start-db.sh"
pkill -f "php -S 127.0.0.1:$WP_PORT" 2>/dev/null || true
pkill -f "php -S 127.0.0.1:$BACKDROP_PORT" 2>/dev/null || true
setsid php -S 127.0.0.1:$WP_PORT -t "$WP_DIR" > "$ROOT/wordpress.log" 2>&1 < /dev/null &
(cd "$BACKDROP_DIR" && setsid php -S 127.0.0.1:$BACKDROP_PORT "$ROOT/scripts/backdrop-router.php" > "$ROOT/backdrop.log" 2>&1 < /dev/null &)
sleep 1
echo "Backdrop:  http://127.0.0.1:$BACKDROP_PORT/wp-admin  (admin/admin)"
echo "WordPress: http://127.0.0.1:$WP_PORT/wp-admin  (admin/admin)"
