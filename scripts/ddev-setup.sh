#!/usr/bin/env bash
# Sets up the Backdrop site in ddev: downloads the code, starts the project,
# installs Backdrop (Standard) with Sample Animal Content and all its extras,
# enables wp_admin and opens /wp-admin. Run from the repo root on your machine.
#   scripts/ddev-setup.sh
# Re-running reinstalls the site (the database is dropped by Bee's installer).
set -euo pipefail
cd "$(dirname "$0")/.."

scripts/download-backdrop.sh

ddev start
BEE="php /var/www/html/downloads/bee/bee.php --root=/var/www/html/backdrop"
ddev exec "$BEE install --db-name=db --db-user=db --db-pass=db --db-host=db --username=admin --password=admin --email=admin@example.com --site-name='Backdrop Zoo' --site-mail=admin@example.com --profile=standard --auto"
ddev exec "$BEE enable sample_animal_content -y"
ddev exec "$BEE php-script /var/www/html/scripts/enable-sample-extras.php"
ddev exec "$BEE enable wp_admin -y"
ddev exec "$BEE cc all"
echo "Done. Log in as admin / admin."
ddev launch /wp-admin
