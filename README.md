# bd-admin

The WordPress admin experience (`/wp-admin`) on Backdrop CMS: a module that gives a
Backdrop site the wp-admin menu tree, dashboard, list pages, settings forms and the
WordPress look (dark sidebar, black admin bar), with explanation pages wherever Backdrop
has no equivalent.

Sibling project: [dwordpress-admin](https://github.com/cellear/dwordpress-admin) does the
same for Drupal 11 as a recipe. This repo is self-contained: the WordPress 7.1 research
(`docs/wp-admin/`, `docs/wireframes/`) is copied here.

## What is in the repo

- `modules/wp_admin/` — the Backdrop module.
- `docs/mapping.md` — how each wp-admin page maps to Backdrop.
- `docs/wp-admin/` — the WordPress 7.1 admin, every menu item and page, with screenshots.
- `docs/backdrop-admin/` — the result: every sidebar item on Backdrop, with screenshots.
- `content/` — the sample content (Backdrop's Sample Animal Content module with all extras)
  recreated in WordPress; `wordpress-animals.xml` is the WXR export.
- `scripts/` — reproduce everything: local MariaDB, both installs, content migration,
  captures, checks.

## Reproducing the environment

Requirements: PHP 8.3, Node 22, MariaDB server + client packages
(`apt-get install mariadb-server mariadb-client`), `unzip`, `git`.

```bash
scripts/start-db.sh          # project-local MariaDB in .db/ (port 3308, user dev/dev)
scripts/install-backdrop.sh  # Backdrop 1.35.2 + Sample Animal Content (all extras) + wp_admin
scripts/install-wordpress.sh # latest WordPress into wordpress/ (wp-cli in scripts/bin)
scripts/serve.sh             # Backdrop on :8082, WordPress on :8083, admin/admin on both
```

Versions used: Backdrop 1.35.2, Sample Animal Content 1.x-1.0.3, WordPress 7.1.4.
