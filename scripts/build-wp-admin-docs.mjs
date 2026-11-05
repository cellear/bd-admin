/**
 * Turns docs/wp-admin/capture.json into Markdown: 00-overview.md plus one
 * file per top-level menu.   node scripts/build-wp-admin-docs.mjs
 */
import { readFileSync, writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');
const OUT = join(ROOT, 'docs', 'wp-admin');
const cap = JSON.parse(readFileSync(join(OUT, 'capture.json'), 'utf8'));
const slug = (s) => s.toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-|-$/g, '');
const esc = (s) => (s || '').replace(/</g, '&lt;');
const rel = (href) => href.replace(cap.base, '').replace(/^\/?wp-admin\//, '') || '(root)';

const groups = [];
for (const m of cap.menu) {
  groups.push({ top: m.label, pages: cap.pages.filter((p) => p.top === m.label), menu: m });
}

// ---- overview ----
let ov = `# wp-admin: overview\n\nWordPress ${cap.pages[0].text.match(/WordPress ([\d.]+) running/)?.[1] || ''} admin area, captured ${cap.capturedAt.slice(0, 10)} from a fresh install (Twenty Twenty-Five theme, no extra plugins) holding the Umami demo content.\n\nEach top-level menu has its own file. For every page these files list the menu path, URL, heading, tabs and filters, primary actions, list-table columns, form fields, Screen Options, Help panel tabs, a trimmed dump of the visible text, and a screenshot.\n\n`;
ov += `## Admin menu tree\n\nThe left-hand \`#adminmenu\`, in order. The first submenu item of each top-level menu is the same page as the top-level link.\n\n`;
for (const g of groups) {
  ov += `- **${g.top}** → \`${rel(g.menu.href)}\` ([doc](${String(groups.indexOf(g) + 1).padStart(2, '0')}-${slug(g.top)}.md))\n`;
  for (const s of g.menu.subs) ov += `  - ${s.label} → \`${rel(s.href)}\`\n`;
}
ov += `\n## Toolbar (top bar)\n\nAlways visible when logged in: WordPress logo menu (About, Get Involved, Documentation, Learn, Support, Feedback), site name (Visit Site, Dashboard, Themes, Widgets/Menus when applicable), Ctrl+K command palette, Comments count, **+ New** (Post, Media, Page, User), and the "Howdy, {user}" menu (Edit Profile, Log Out).\n\n`;
ov += `## Conventions shared by most pages\n\n- **Screen Options** tab (top right): toggle columns and set items-per-page on list tables; toggle dashboard widgets.\n- **Help** tab (top right): contextual help with 2–5 tabs per screen plus "For more information" links.\n- **List tables** (Posts, Pages, Media, Comments, Users, Plugins): status filter links above the table (All / Published / Drafts / Trash…), bulk actions dropdown, date and category filters, a search box, row hover actions (Edit, Quick Edit, Trash, View), sortable columns, pagination.\n- **Add New** screens for Posts and Pages open the block editor, which replaces the whole admin chrome.\n- Settings forms end with a **Save Changes** button and show an "Settings saved." notice.\n\n## All captured pages\n\n| # | Menu path | URL | Heading |\n|---|---|---|---|\n`;
for (const p of cap.pages) ov += `| ${p.n} | ${p.top}${p.label !== p.top ? ' → ' + p.label : ''} | \`${rel(p.href)}\` | ${esc(p.h1 || p.title)} |\n`;
ov += `\n${cap.pages.length} pages captured. Pages marked "Site Editor → …" and "(block editor)" are React single-page apps; their screenshot is the viewport only.\n`;
writeFileSync(join(OUT, '00-overview.md'), ov);

// ---- per-menu files ----
groups.forEach((g, i) => {
  const file = `${String(i + 1).padStart(2, '0')}-${slug(g.top)}.md`;
  let md = `# ${g.top}\n\n[← Overview](00-overview.md)\n\nTop-level menu **${g.top}** → \`${rel(g.menu.href)}\`\n\nSubmenu:\n`;
  for (const s of g.menu.subs) md += `- ${s.label} → \`${rel(s.href)}\`\n`;
  if (!g.menu.subs.length) md += `- (no submenu)\n`;
  md += `\nPages in this file:\n` + g.pages.map((p) => `- [${p.label}](#${slug(p.label)})`).join('\n') + '\n';
  for (const p of g.pages) {
    md += `\n---\n\n## ${p.label}\n\n`;
    md += `- **Menu path:** ${p.top}${p.label !== p.top ? ' → ' + p.label : ''}\n`;
    md += `- **URL:** \`${rel(p.href)}\`${p.finalUrl && rel(p.finalUrl) !== rel(p.href) ? ` (redirects to \`${rel(p.finalUrl)}\`)` : ''}\n`;
    md += `- **Heading:** ${esc(p.h1 || '')}\n`;
    md += `- **Browser title:** ${esc(p.title)}\n`;
    if (p.tabs?.length) md += `- **Tabs / filters:** ${p.tabs.map(esc).join(' · ')}\n`;
    if (p.buttons?.length) md += `- **Primary actions:** ${p.buttons.map(esc).join(' · ')}\n`;
    if (p.tableColumns?.length) md += `- **List table columns:** ${p.tableColumns.map(esc).join(' · ')}\n`;
    if (p.screenOptions) md += `- **Screen Options:** ${esc(p.screenOptions).slice(0, 600)}\n`;
    if (p.helpTabs?.length) md += `- **Help tabs:** ${p.helpTabs.map(esc).join(' · ')}\n`;
    if (p.error) md += `- **Capture note:** ${esc(p.error)}\n`;
    md += `\n![${esc(p.label)}](${p.screenshot})\n`;
    if (p.controls?.length) md += `\n<details><summary>Form fields (${p.controls.length})</summary>\n\n${p.controls.map((c) => `- ${esc(c)}`).join('\n')}\n\n</details>\n`;
    if (p.text) md += `\n<details><summary>Page text</summary>\n\n\`\`\`text\n${p.text.slice(0, 4000).replace(/```/g, "'''")}\n\`\`\`\n\n</details>\n`;
    if (p.help?.length) md += `\n<details><summary>Help panel</summary>\n\n${p.help.map((h, j) => `**${esc(p.helpTabs?.[j] || 'Help')}**\n\n${esc(h).slice(0, 2500)}`).join('\n\n')}\n\n</details>\n`;
  }
  writeFileSync(join(OUT, file), md);
  console.log(file, g.pages.length, 'pages');
});
