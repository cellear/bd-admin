#!/usr/bin/env bash
# Puts the code in place without installing a site: Backdrop core into
# backdrop/, the Sample Animal Content module and this repo's wp_admin module
# into backdrop/modules/, and the Bee CLI into downloads/bee.
#   scripts/download-backdrop.sh            (keeps an existing backdrop/)
#   scripts/download-backdrop.sh --force    (replaces backdrop/)
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
if [ "${1:-}" = "--force" ] || [ ! -f "$BACKDROP_DIR/index.php" ]; then
  rm -rf "$BACKDROP_DIR" "$DL/extracted"
  unzip -q "$DL/backdrop.zip" -d "$DL/extracted"
  mv "$DL/extracted/backdrop" "$BACKDROP_DIR"
  chmod -R u+w "$BACKDROP_DIR"
  mkdir -p "$BACKDROP_DIR/files" && chmod 777 "$BACKDROP_DIR/files"
fi

# Sample Animal Content.
if [ ! -f "$DL/sample_animal_content.zip" ]; then
  curl -fsSL -o "$DL/sample_animal_content.zip" "https://github.com/backdrop-contrib/sample_animal_content/releases/download/$SAMPLE_CONTENT_VERSION/sample_animal_content.zip"
fi
unzip -q -o "$DL/sample_animal_content.zip" -d "$BACKDROP_DIR/modules"

# This repo's module. A relative symlink so it also resolves inside a container.
ln -sfn ../../modules/wp_admin "$BACKDROP_DIR/modules/wp_admin"
echo "Backdrop $BACKDROP_VERSION, Sample Animal Content $SAMPLE_CONTENT_VERSION and wp_admin are in $BACKDROP_DIR"
