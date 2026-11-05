/**
 * Walks every wp-admin menu item (top-level and submenu) plus a few linked
 * sub-pages, and records for each: menu path, URL, heading, visible text,
 * tabs, form controls, screen options, help text, and a screenshot.
 *
 *   node scripts/capture-wp-admin.mjs
 *
 * Output: docs/wp-admin/capture.json and docs/wp-admin/screenshots/*.jpg
 */
import { createRequire } from 'node:module';
const { chromium } = createRequire(import.meta.url)('playwright');
import { writeFileSync, mkdirSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');
const BASE = process.env.WP_URL || 'http://127.0.0.1:8080';
const USER = process.env.ADMIN_USER || 'admin';
const PASS = process.env.ADMIN_PASS || 'admin';
const OUT = join(ROOT, 'docs', 'wp-admin');
const SHOTS = join(OUT, 'screenshots');
mkdirSync(SHOTS, { recursive: true });

const browser = await chromium.launch();
const ctx = await browser.newContext({ viewport: { width: 1440, height: 900 }, deviceScaleFactor: 2 });
const page = await ctx.newPage();

// Login
await page.goto(`${BASE}/wp-login.php`);
await page.fill('#user_login', USER);
await page.fill('#user_pass', PASS);
await page.click('#wp-submit');
await page.waitForSelector('#adminmenu');

// Read the admin menu tree
const menu = await page.$$eval('#adminmenu > li.menu-top', (items) =>
  items.map((li) => {
    const a = li.querySelector(':scope > a');
    const nameEl = li.querySelector('.wp-menu-name')?.cloneNode(true); nameEl?.querySelectorAll('.awaiting-mod, .update-plugins, .screen-reader-text').forEach((e) => e.remove());
    const name = nameEl?.textContent.trim().replace(/\s+/g, ' ') || '';
    const subs = [...li.querySelectorAll('ul.wp-submenu li a')].map((s) => ({
      label: s.textContent.trim().replace(/\s+\d+$/, '').replace(/\s+/g, ' '),
      href: s.getAttribute('href'),
    }));
    return { label: name.replace(/\s*\d+$/, ''), href: a?.getAttribute('href'), id: li.id, subs };
  }).filter((m) => m.label)
);

// Pages reachable from the menu pages but not in the menu itself.
const extras = [
  { top: 'Dashboard', label: 'Site Health → Info tab', href: 'site-health.php?tab=debug' },
  { top: 'Posts', label: 'Edit Post (block editor)', href: 'post.php?post=36&action=edit' },
  { top: 'Pages', label: 'Edit Page (block editor)', href: 'post.php?post=44&action=edit' },
  { top: 'Media', label: 'Edit Media (attachment details)', href: null, dynamic: 'first-attachment' },
  { top: 'Appearance', label: 'Site Editor → Styles', href: 'site-editor.php?p=%2Fstyles' },
  { top: 'Appearance', label: 'Site Editor → Pages', href: 'site-editor.php?p=%2Fpage' },
  { top: 'Appearance', label: 'Site Editor → Templates', href: 'site-editor.php?p=%2Ftemplate' },
  { top: 'Appearance', label: 'Site Editor → Patterns', href: 'site-editor.php?p=%2Fpattern' },
  { top: 'Appearance', label: 'Site Editor → Navigation', href: 'site-editor.php?p=%2Fnavigation' },
  { top: 'Appearance', label: 'Customize (classic customizer)', href: 'customize.php' },
  { top: 'Plugins', label: 'Add New Plugin (search)', href: 'plugin-install.php?tab=search&s=seo' },
  { top: 'Users', label: 'Edit User', href: 'user-edit.php?user_id=2' },
  { top: 'Tools', label: 'Import → WordPress (installer)', href: 'admin.php?import=wordpress' },
];

const slug = (s) => s.toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-|-$/g, '');
const results = [];
let n = 0;

async function capture(top, label, href) {
  n++;
  const url = href.startsWith('http') ? href : `${BASE}/wp-admin/${href}`;
  const id = `${String(n).padStart(2, '0')}-${slug(top)}${label ? '-' + slug(label) : ''}`;
  const rec = { n, top, label: label || top, href, url, screenshot: `screenshots/${id}.jpg` };
  try {
    await page.goto(url, { waitUntil: 'domcontentloaded', timeout: 60000 });
    await page.waitForTimeout(1500);
    // Dismiss block-editor / site-editor welcome guides.
    for (const sel of ['.components-guide .components-modal__header button', '.components-modal__header button[aria-label="Close"]']) {
      const b = await page.$(sel);
      if (b) { await b.click().catch(() => {}); await page.waitForTimeout(400); }
    }
    const isEditor = /post(-new)?\.php|site-editor\.php|customize\.php/.test(url);
    if (isEditor) await page.waitForTimeout(3500);
    rec.title = await page.title();
    rec.finalUrl = page.url();
    rec.h1 = await page.$eval('#wpbody-content h1, h1', (h) => h.innerText.trim()).catch(() => '');
    rec.tabs = await page.$$eval('#wpbody-content .nav-tab-wrapper a, #wpbody-content .subsubsub a', (as) =>
      as.map((a) => a.innerText.trim().replace(/\s*\(\d+\)$/, ''))).catch(() => []);
    rec.buttons = await page.$$eval('#wpbody-content .page-title-action, #wpbody-content input[type=submit], #wpbody-content button.button-primary', (bs) =>
      [...new Set(bs.map((b) => (b.value || b.innerText || '').trim()).filter(Boolean))]).catch(() => []);
    rec.controls = await page.$$eval('#wpbody-content form label', (ls) =>
      [...new Set(ls.map((l) => l.innerText.trim().replace(/\s+/g, ' ')).filter((t) => t && t.length < 80))].slice(0, 80)).catch(() => []);
    rec.tableColumns = await page.$$eval('#wpbody-content table.wp-list-table thead th', (ths) =>
      ths.map((t) => t.innerText.trim().split('\n')[0]).filter(Boolean)).catch(() => []);
    rec.text = await page.$eval('#wpbody-content', (el) => el.innerText.replace(/\n{3,}/g, '\n\n').trim().slice(0, 6000)).catch(async () =>
      (await page.$eval('body', (el) => el.innerText.replace(/\n{3,}/g, '\n\n').trim().slice(0, 6000)).catch(() => '')));
    // Screen options + help panels (classic admin pages only).
    const so = await page.$('#show-settings-link');
    if (so) {
      await so.click().catch(() => {});
      await page.waitForTimeout(300);
      rec.screenOptions = await page.$eval('#screen-options-wrap', (el) => el.innerText.replace(/\s+/g, ' ').trim()).catch(() => '');
      await so.click().catch(() => {});
    }
    const help = await page.$('#contextual-help-link');
    if (help) {
      await help.click().catch(() => {});
      await page.waitForTimeout(300);
      rec.help = await page.$$eval('#contextual-help-columns .help-tab-content', (els) =>
        els.map((e) => e.innerText.replace(/\n{2,}/g, '\n').trim())).catch(() => []);
      rec.helpTabs = await page.$$eval('#contextual-help-columns .contextual-help-tabs a', (as) => as.map((a) => a.innerText.trim())).catch(() => []);
      await help.click().catch(() => {});
      await page.waitForTimeout(300);
    }
    await page.screenshot({ path: join(SHOTS, `${id}.jpg`), fullPage: !isEditor, type: 'jpeg', quality: 80 });
  } catch (e) {
    rec.error = String(e.message || e).slice(0, 300);
    await page.screenshot({ path: join(SHOTS, `${id}.jpg`), type: 'jpeg', quality: 80 }).catch(() => {});
  }
  results.push(rec);
  console.log(`${rec.n}. ${top}${label ? ' → ' + label : ''}  [${rec.h1 || rec.title}]${rec.error ? '  ERROR ' + rec.error : ''}`);
}

for (const m of menu) {
  await capture(m.label, '', m.href);
  for (const s of m.subs) {
    if (s.href === m.href) continue; // first submenu usually repeats the top link
    await capture(m.label, s.label, s.href);
  }
  for (const x of extras.filter((x) => x.top === m.label)) {
    let href = x.href;
    if (x.dynamic === 'first-attachment') {
      await page.goto(`${BASE}/wp-admin/upload.php?mode=list`);
      href = await page.$eval('table.wp-list-table a.row-title', (a) => a.getAttribute('href')).catch(() => null);
      if (!href) continue;
    }
    await capture(m.label, x.label, href);
  }
}

writeFileSync(join(OUT, 'capture.json'), JSON.stringify({ base: BASE, capturedAt: new Date().toISOString(), menu, pages: results }, null, 2));
console.log(`\n${results.length} pages captured -> ${OUT}`);
await browser.close();
