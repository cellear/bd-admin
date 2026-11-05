#!/usr/bin/env bash
# Exports the Backdrop content, imports it into WordPress and writes the WXR.
set -euo pipefail
source "$(dirname "$0")/env.sh"
(cd "$BACKDROP_DIR" && bee php-script "$ROOT/scripts/export-animals.php")
cd "$WP_DIR"
# Remove the stock "Hello world!" post and "Sample Page" so only migrated content remains.
for id in $(wp post list --post_type=post,page --meta_key=_backdrop_nid --meta_compare=NOT\ EXISTS --format=ids); do
  wp post delete "$id" --force >/dev/null
done
wp eval-file "$ROOT/scripts/import-to-wordpress.php"
wp export --dir="$ROOT/content" --filename_format=wordpress-animals.xml
