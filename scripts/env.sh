# Shared settings for the bd-admin scripts. Source this file.
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DB_DIR="$ROOT/.db"
DB_SOCK="$DB_DIR/mysqld.sock"
DB_PORT=3308
DB_USER=dev
DB_PASS=dev
BACKDROP_DIR="$ROOT/backdrop"
WP_DIR="$ROOT/wordpress"
ADMIN_USER=admin
ADMIN_PASS=admin
BACKDROP_PORT=8082
WP_PORT=8083
BACKDROP_VERSION=1.35.2
SAMPLE_CONTENT_VERSION=1.x-1.0.3
export WP_CLI_ALLOW_ROOT=1
export PATH="$ROOT/scripts/bin:$PATH"
