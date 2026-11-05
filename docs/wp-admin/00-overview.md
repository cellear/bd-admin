# wp-admin: overview

WordPress 7.1.4 admin area, captured 2026-11-05 from a fresh install (Twenty Twenty-Five theme, no extra plugins) holding the Umami demo content.

Each top-level menu has its own file. For every page these files list the menu path, URL, heading, tabs and filters, primary actions, list-table columns, form fields, Screen Options, Help panel tabs, a trimmed dump of the visible text, and a screenshot.

## Admin menu tree

The left-hand `#adminmenu`, in order. The first submenu item of each top-level menu is the same page as the top-level link.

- **Dashboard** → `index.php` ([doc](01-dashboard.md))
  - Home → `index.php`
  - Updates → `update-core.php`
- **Posts** → `edit.php` ([doc](02-posts.md))
  - All Posts → `edit.php`
  - Add Post → `post-new.php`
  - Categories → `edit-tags.php?taxonomy=category`
  - Tags → `edit-tags.php?taxonomy=post_tag`
- **Media** → `upload.php` ([doc](03-media.md))
  - Library → `upload.php`
  - Add Media File → `media-new.php`
- **Pages** → `edit.php?post_type=page` ([doc](04-pages.md))
  - All Pages → `edit.php?post_type=page`
  - Add Page → `post-new.php?post_type=page`
- **Comments** → `edit-comments.php` ([doc](05-comments.md))
- **Appearance** → `themes.php` ([doc](06-appearance.md))
  - Themes → `themes.php`
  - Editor → `site-editor.php`
  - Fonts → `font-library.php`
- **Plugins** → `plugins.php` ([doc](07-plugins.md))
  - Installed Plugins → `plugins.php`
  - Add Plugin → `plugin-install.php`
- **Users** → `users.php` ([doc](08-users.md))
  - All Users → `users.php`
  - Add User → `user-new.php`
  - Profile → `profile.php`
- **Tools** → `tools.php` ([doc](09-tools.md))
  - Available Tools → `tools.php`
  - Import → `import.php`
  - Export → `export.php`
  - Site Health → `site-health.php`
  - Export Personal Data → `export-personal-data.php`
  - Erase Personal Data → `erase-personal-data.php`
  - Theme File Editor → `theme-editor.php`
  - Plugin File Editor → `plugin-editor.php`
- **Settings** → `options-general.php` ([doc](10-settings.md))
  - General → `options-general.php`
  - Connectors → `options-connectors.php`
  - Writing → `options-writing.php`
  - Reading → `options-reading.php`
  - Discussion → `options-discussion.php`
  - Media → `options-media.php`
  - Permalinks → `options-permalink.php`
  - Privacy → `options-privacy.php`

## Toolbar (top bar)

Always visible when logged in: WordPress logo menu (About, Get Involved, Documentation, Learn, Support, Feedback), site name (Visit Site, Dashboard, Themes, Widgets/Menus when applicable), Ctrl+K command palette, Comments count, **+ New** (Post, Media, Page, User), and the "Howdy, {user}" menu (Edit Profile, Log Out).

## Conventions shared by most pages

- **Screen Options** tab (top right): toggle columns and set items-per-page on list tables; toggle dashboard widgets.
- **Help** tab (top right): contextual help with 2–5 tabs per screen plus "For more information" links.
- **List tables** (Posts, Pages, Media, Comments, Users, Plugins): status filter links above the table (All / Published / Drafts / Trash…), bulk actions dropdown, date and category filters, a search box, row hover actions (Edit, Quick Edit, Trash, View), sortable columns, pagination.
- **Add New** screens for Posts and Pages open the block editor, which replaces the whole admin chrome.
- Settings forms end with a **Save Changes** button and show an "Settings saved." notice.

## All captured pages

| # | Menu path | URL | Heading |
|---|---|---|---|
| 1 | Dashboard | `index.php` | Dashboard |
| 2 | Dashboard → Updates | `update-core.php` | WordPress Updates |
| 3 | Dashboard → Site Health → Info tab | `site-health.php?tab=debug` | Site Health |
| 4 | Posts | `edit.php` | Posts |
| 5 | Posts → Add Post | `post-new.php` | Add Post |
| 6 | Posts → Categories | `edit-tags.php?taxonomy=category` | Categories |
| 7 | Posts → Tags | `edit-tags.php?taxonomy=post_tag` | Tags |
| 8 | Posts → Edit Post (block editor) | `post.php?post=36&action=edit` | Edit Post |
| 9 | Media | `upload.php` | Media Library |
| 10 | Media → Add Media File | `media-new.php` | Upload Media |
| 11 | Pages | `edit.php?post_type=page` | Pages |
| 12 | Pages → Add Page | `post-new.php?post_type=page` | Add Page |
| 13 | Pages → Edit Page (block editor) | `post.php?post=44&action=edit` | Edit Page |
| 14 | Comments | `edit-comments.php` | Comments |
| 15 | Appearance | `themes.php` | Themes 3 |
| 16 | Appearance → Editor | `site-editor.php` | Design |
| 17 | Appearance → Fonts | `font-library.php` | Fonts |
| 18 | Appearance → Site Editor → Styles | `site-editor.php?p=%2Fstyles` | Design |
| 19 | Appearance → Site Editor → Pages | `site-editor.php?p=%2Fpage` | Pages |
| 20 | Appearance → Site Editor → Templates | `site-editor.php?p=%2Ftemplate` | Templates |
| 21 | Appearance → Site Editor → Patterns | `site-editor.php?p=%2Fpattern` | Patterns |
| 22 | Appearance → Site Editor → Navigation | `site-editor.php?p=%2Fnavigation` | Navigation |
| 23 | Appearance → Customize (classic customizer) | `customize.php` | Customize: Umami Food Magazine – Umami demo content from Drupal |
| 24 | Plugins | `plugins.php` | Plugins |
| 25 | Plugins → Add Plugin | `plugin-install.php` | Add Plugins |
| 26 | Plugins → Add New Plugin (search) | `plugin-install.php?tab=search&s=seo` | Add Plugins |
| 27 | Users | `users.php` | Users |
| 28 | Users → Add User | `user-new.php` | Add User |
| 29 | Users → Profile | `profile.php` | Profile |
| 30 | Users → Edit User | `user-edit.php?user_id=2` | Edit User Gregorio Sánchez |
| 31 | Tools | `tools.php` | Tools |
| 32 | Tools → Import | `import.php` | Import |
| 33 | Tools → Export | `export.php` | Export |
| 34 | Tools → Site Health | `site-health.php` | Site Health |
| 35 | Tools → Export Personal Data | `export-personal-data.php` | Export Personal Data |
| 36 | Tools → Erase Personal Data | `erase-personal-data.php` | Erase Personal Data |
| 37 | Tools → Theme File Editor | `theme-editor.php` | Edit Themes |
| 38 | Tools → Plugin File Editor | `plugin-editor.php` | Edit Plugins |
| 39 | Tools → Import → WordPress (installer) | `admin.php?import=wordpress` | Import |
| 40 | Settings | `options-general.php` | General Settings |
| 41 | Settings → Connectors | `options-connectors.php` | Connectors |
| 42 | Settings → Writing | `options-writing.php` | Writing Settings |
| 43 | Settings → Reading | `options-reading.php` | Reading Settings |
| 44 | Settings → Discussion | `options-discussion.php` | Discussion Settings |
| 45 | Settings → Media | `options-media.php` | Media Settings |
| 46 | Settings → Permalinks | `options-permalink.php` | Permalink Settings |
| 47 | Settings → Privacy | `options-privacy.php` | Privacy |

47 pages captured. Pages marked "Site Editor → …" and "(block editor)" are React single-page apps; their screenshot is the viewport only.
