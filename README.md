# bd-admin

The WordPress admin experience (`/wp-admin`) on Backdrop CMS: a module that gives a
Backdrop site the wp-admin menu tree, dashboard, list pages, settings forms and the
WordPress look (dark sidebar, black admin bar), with explanation pages wherever Backdrop
has no equivalent.

Sibling project: [dwordpress-admin](https://github.com/cellear/dwordpress-admin) does the
same for Drupal 11 as a recipe. This repo is self-contained: the WordPress 7.1 research
(`docs/wp-admin/`, `docs/wireframes/`) is copied here.

## Using the module on your own Backdrop site

Copy or symlink `modules/wp_admin` into your site's `modules/` directory and enable it
(`bee enable wp_admin` or the Modules page). Enabling it creates the `wp-admin` menu, adds a
Categories vocabulary and field to the Post type, and gives administrators and editors the
`access wp admin` permission. Those users then get the WordPress look on every page and
Backdrop's own admin bar is suppressed for them; everyone else is unaffected. Open
`/wp-admin`. The stock Backdrop administration stays at `/admin` and under
Tools → Backdrop Administration. The Animals and Habitats menus appear only when the
Sample Animal Content module provides those content types.

Uninstalling removes the menu, the field and the module's settings; the Categories
vocabulary is kept because it may hold terms.

## What is in the repo

- `modules/wp_admin/` — the Backdrop module: the `wp-admin` menu tree with WordPress's
  labels and icons, the `/wp-admin` dashboard, Posts/Pages/Animals/Habitats list pages
  (a View), Categories/Tags/Classes/Habitat Types pages, seven settings forms that write
  real Backdrop config (Permalinks edits core's URL alias patterns), twelve explanation
  pages (`pages/`), the dark sidebar and black admin bar (`css/`, `templates/`), and a
  Simpletest (`tests/`).
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
