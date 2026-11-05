# WordPress admin → Backdrop mapping

Source: WordPress 7.1.4 wp-admin, captured in [docs/wp-admin](wp-admin/00-overview.md).
Result: every sidebar item on Backdrop with the module enabled, captured in
[docs/backdrop-admin](backdrop-admin/00-overview.md). Wireframes drawn for the Drupal
version, reused here: [docs/wireframes](wireframes/).

Target: Backdrop CMS 1.35 (Standard profile: Post, Page, Card; Tags; comments on Post;
Seven admin theme) plus the contrib **Sample Animal Content** module with all its extras
(Animal and Habitat content types, Class and Habitat Type vocabularies, 18 animals,
6 habitats, 40 comments).

Decisions, the same as for the Drupal version: a real **Categories** vocabulary on Post;
comments stay on; **exact WordPress labels**; Backdrop-only screens tucked under
**Tools → Backdrop Administration**; content types WordPress does not have (Animals,
Habitats) get their own top-level menus the way WordPress shows custom post types; the
**full WordPress look** (dark sidebar, black admin bar) replaces Backdrop's admin bar for
users with the `access wp admin` permission; site building (Site Editor, Gutenberg) is
explained, not recreated.

## How it is built

Backdrop has no recipes, so everything is the `wp_admin` module:
- `hook_install()` creates the `wp-admin` menu and its 53 links (flagged *customized* so
  router rebuilds keep them), the `category` vocabulary and `field_category` on Post
  (checkboxes), and grants the permission to administrators and editors.
- `hook_menu()` provides the `/wp-admin/*` pages; `hook_admin_paths()` puts them in the
  admin theme; a Views config (`views.view.wp_admin_content.json`) provides the four list
  pages; `hook_preprocess_page()` appends the sidebar and top bar to every page (the way
  the core admin bar does) and `hook_init()` suppresses the core admin bar for those users.

## What is more real on Backdrop than on Drupal

| WordPress | Backdrop |
|---|---|
| Plugins → Add Plugin (install from the directory) | **Real**: core's Installer module at `admin/modules/install` downloads from backdropcms.org |
| Settings → Permalinks (URL patterns) | **Real**: core URL alias patterns (`path.settings`), edited by the Permalinks form, presets Plain / Day and name / Month and name / Post name |
| Comments on by default | Already so on Post in the Standard profile |

## Sidebar top level

| WordPress menu | Backdrop target | Kind |
|---|---|---|
| Dashboard | `/wp-admin` dashboard | adapted |
| Posts | `/wp-admin/posts` (View: posts, animals and habitats together, with a Type column) | view |
| Media | `admin/content/files` (Manage files) | direct |
| Pages | `/wp-admin/pages` (View: type page) | view |
| *(custom type)* Animals | `/wp-admin/animals` | view |
| *(custom type)* Habitats | `/wp-admin/habitats` | view |
| Comments | `admin/content/comment` | direct |
| Appearance | `admin/appearance` | direct |
| Plugins | `admin/modules` | direct |
| Users | `admin/people` | direct |
| Tools | `/wp-admin/tools` | adapted |
| Settings | `/wp-admin/settings/general` | adapted |

## Every captured wp-admin page

Kind: **direct** = existing Backdrop route · **view** = the module's View · **adapted** =
module page/form built on Backdrop pieces · **explained** = module page describing the gap.

| # | WordPress page | Backdrop target | Kind |
|---|---|---|---|
| 1 | Dashboard → Home | `/wp-admin`: Welcome, Site Health Status (requirements), At a Glance, Activity, Quick Draft (unpublished post), Backdrop Events and News | adapted |
| 2 | Dashboard → Updates | `admin/reports/updates` | direct |
| 3 | Site Health → Info | `admin/reports/status` (Status report) | direct |
| 4 | Posts → All Posts | `/wp-admin/posts`: posts, animals and habitats with a Type column (animals and habitats also have their own menus) | view |
| 5 | Posts → Add Post | `node/add/post` | direct |
| 6 | Posts → Categories | `/wp-admin/posts/categories` (new `category` vocabulary; add form beside the term table) | adapted |
| 7 | Posts → Tags | `/wp-admin/posts/tags` (`tags` vocabulary) | adapted |
| 8 | Edit Post | `node/N/edit` | direct |
| 9 | Media → Library | `admin/content/files` | direct |
| 10 | Media → Add Media File | `/wp-admin/media/new`: explains `file/add` and image fields | explained |
| 11 | Pages → All Pages | `/wp-admin/pages` | view |
| 12 | Pages → Add Page | `node/add/page` | direct |
| 13 | Edit Page | `node/N/edit` | direct |
| — | Animals / Habitats → All, Add, Classes / Habitat Types | `/wp-admin/animals`, `node/add/animal`, `/wp-admin/animals/classes`; same for habitats | view / direct / adapted |
| 14 | Comments | `admin/content/comment` (Published / Unapproved tabs) | direct |
| 15 | Appearance → Themes | `admin/appearance` | direct |
| 16 | Appearance → Editor | `/wp-admin/appearance/editor`: Templates → Layouts, Navigation → Menus, Styles → theme settings, Patterns → custom blocks | adapted |
| 17 | Appearance → Fonts | `/wp-admin/appearance/fonts` | explained |
| 18–22 | Site Editor sections | covered by the Editor page | adapted |
| 23 | Customize | theme settings, linked from the Editor page | adapted |
| 24 | Plugins → Installed Plugins | `admin/modules` | direct |
| 25–26 | Plugins → Add Plugin | `admin/modules/install` (core Installer) | **direct** |
| 27 | Users → All Users | `admin/people` | direct |
| 28 | Users → Add User | `admin/people/create` | direct |
| 29 | Users → Profile | `/wp-admin/profile` → `user/N/edit` | direct |
| 30 | Edit User | `user/N/edit` | direct |
| 31 | Tools → Available Tools | `/wp-admin/tools`: Configuration, Reports, log, Cron, Performance, Maintenance | adapted |
| 32 | Tools → Import | `/wp-admin/tools/import`: Drupal 7 upgrade, Feeds/Migrate contrib, config import | adapted |
| 33 | Tools → Export | `/wp-admin/tools/export`: config export; content via Views Data Export / Bee | adapted |
| 34 | Tools → Site Health | `admin/reports/status` | direct |
| 35 | Tools → Export Personal Data | `/wp-admin/tools/export-personal-data` | explained |
| 36 | Tools → Erase Personal Data | `/wp-admin/tools/erase-personal-data` (cancel account) | adapted |
| 37 | Tools → Theme File Editor | `/wp-admin/tools/theme-editor` | explained |
| 38 | Tools → Plugin File Editor | `/wp-admin/tools/plugin-editor` | explained |
| 39 | Import → WordPress | covered by Import | adapted |
| — | Tools → Backdrop Administration | `/wp-admin/tools/backdrop`: content types, taxonomy, views, layouts, menus, roles, permissions, configuration, reports, `/admin` | adapted |
| 40 | Settings → General | `/wp-admin/settings/general` → `system.core` (site_name, site_slogan, site_mail, user_register), `system.date` (timezone, first day) | adapted |
| 41 | Settings → Connectors | `/wp-admin/settings/connectors` | explained |
| 42 | Settings → Writing | `/wp-admin/settings/writing` → default category | adapted |
| 43 | Settings → Reading | `/wp-admin/settings/reading` → `system.core:site_frontpage`, Promoted view pager and feed | adapted |
| 44 | Settings → Discussion | `/wp-admin/settings/discussion` → Post type comment settings, comment permissions | adapted |
| 45 | Settings → Media | `/wp-admin/settings/media` → thumbnail / medium / large image styles | adapted |
| 46 | Settings → Permalinks | `/wp-admin/settings/permalinks` → `path.settings` patterns | **adapted (real form)** |
| 47 | Settings → Privacy | `/wp-admin/settings/privacy` → `wp_admin.settings:privacy_page` | adapted |

## Look and feel

| WordPress | Backdrop, with the module |
|---|---|
| Dark left menu, 160px, blue current item, children inline, separators after Dashboard and Comments; icons only below 960px, hidden below 783px | `#wp-admin-sidebar` from `hook_preprocess_page()` with `css/wp-admin-chrome.css`; current item by internal path or alias; flyouts on hover for other items; WordPress's breakpoints (160px above 960px, 36px icon-only 783–960px, burger below 783px) |
| Black admin bar: logo menu, site name (to the site from the admin, to the dashboard from the site), Comments count, + New, Howdy; login lands on the dashboard | `#wp-admin-bar` (`hook_user_login()` sends wp-admin users to `/wp-admin`): "B" logo menu (About Backdrop, Backdrop Administration, Documentation, Support), site name → front (Visit Site, Dashboard, Themes, Layouts, Menus), Comments in moderation, + New (Post, Media, Page, Animal, Habitat, User), Howdy (Edit Profile, View Profile, Log Out) |
| Backdrop's own admin bar | Suppressed for `access wp admin` users via `admin_bar_suppress()`; other users keep it |
| Dashboard welcome panel with Dismiss | "Welcome to bd-admin!", a `<details>` element |
| Content area | Stays Seven |

## Verification

`scripts/install-backdrop.sh` (fresh), `scripts/capture-backdrop-admin.mjs` (53 pages, 0
problems), `scripts/sidebar-check.mjs` (9/9), `scripts/topbar-check.mjs` (24 links, 0
failures, anonymous sees no chrome), `scripts/roundtrip-check.mjs` (10/10), Simpletest
`WpAdminMenuTestCase`.
