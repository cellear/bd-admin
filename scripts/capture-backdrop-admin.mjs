/**
 * Walks the WordPress-style sidebar on the Backdrop site (wp_admin enabled),
 * checks every link returns 200 with a heading, and screenshots each page.
 *   node scripts/capture-backdrop-admin.mjs
 *
 * Output: docs/backdrop-admin/00-overview.md, capture.json, screenshots/*.jpg
 */
import { createRequire } from 'node:module';
import { writeFileSync, mkdirSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
const { chromium } = createRequire(import.meta.url)('playwright');

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');
const BASE = process.env.BACKDROP_URL || 'http://127.0.0.1:8082';
const OUT = join(ROOT, 'docs', 'backdrop-admin');
const SHOTS = join(OUT, 'screenshots');
mkdirSync(SHOTS, { recursive: true });

export async function login(page, base = BASE) {
  await page.goto(`${base}/user/login`);
  await page.fill('#edit-name', process.env.ADMIN_USER || 'admin');
  await page.fill('#edit-pass', process.env.ADMIN_PASS || 'admin');
  await page.click('#edit-submit');
  await page.waitForLoadState('domcontentloaded');
}

const browser = await chromium.launch();
const ctx = await browser.newContext({ viewport: { width: 1440, height: 900 } });
const page = await ctx.newPage();
await login(page);
await page.goto(`${BASE}/wp-admin`);
await page.waitForSelector('#wp-admin-sidebar');

const tree = await page.$$eval('#wp-admin-sidebar > ul > li', (items) =>
  items.map((li) => ({
    label: li.querySelector(':scope > a .wp-admin-menu__label')?.textContent.trim() || '',
    href: li.querySelector(':scope > a')?.getAttribute('href') || null,
    children: [...li.querySelectorAll(':scope > ul a')].map((a) => ({ label: a.textContent.trim(), href: a.getAttribute('href') })),
  })).filter((m) => m.label)
);

const slug = (s) => s.toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-|-$/g, '');
const pages = [];
let n = 0;
async function capture(top, label, href) {
  n++;
  const id = `${String(n).padStart(2, '0')}-${slug(top)}${label && label !== top ? '-' + slug(label) : ''}`;
  const rec = { n, top, label: label || top, href, screenshot: `screenshots/${id}.jpg` };
  const r = await page.goto(href.startsWith('http') ? href : BASE + href, { waitUntil: 'domcontentloaded' });
  await page.waitForTimeout(300);
  rec.status = r.status();
  rec.finalUrl = page.url().replace(BASE, '');
  rec.h1 = await page.$eval('h1', (h) => h.innerText.trim()).catch(() => '');
  rec.hasBackdropAdminBar = !!(await page.$('#admin-bar'));
  await page.screenshot({ path: join(SHOTS, `${id}.jpg`), fullPage: true, type: 'jpeg', quality: 80 });
  pages.push(rec);
  console.log(`${rec.status} ${top}${label && label !== top ? ' → ' + label : ''}  [${rec.h1}]  ${rec.finalUrl}`);
}
const seen = new Set();
for (const m of tree) {
  for (const t of [{ label: '', href: m.href }, ...m.children]) {
    if (!t.href || t.href.includes('/logout')) continue;
    const key = `${m.label}|${t.label}|${t.href}`;
    if (seen.has(key)) continue;
    seen.add(key);
    await capture(m.label, t.label, t.href);
  }
}

const bad = pages.filter((p) => p.status !== 200 || !p.h1 || p.hasBackdropAdminBar);
let md = `# Backdrop wp-admin: every sidebar item\n\nCaptured ${new Date().toISOString().slice(0, 10)} from Backdrop with the wp_admin module enabled on a Standard install with Sample Animal Content. ${pages.length} pages visited, ${bad.length} problems.\n\n| # | Sidebar item | URL | Status | Heading | Screenshot |\n|---|---|---|---|---|---|\n`;
for (const p of pages) md += `| ${p.n} | ${p.top}${p.label !== p.top ? ' → ' + p.label : ''} | \`${p.finalUrl}\` | ${p.status} | ${p.h1} | [view](${p.screenshot}) |\n`;
if (bad.length) md += `\n## Problems\n\n` + bad.map((p) => `- ${p.top} → ${p.label}: HTTP ${p.status}, heading "${p.h1}"${p.hasBackdropAdminBar ? ', Backdrop admin bar visible' : ''}`).join('\n') + '\n';
writeFileSync(join(OUT, '00-overview.md'), md);
writeFileSync(join(OUT, 'capture.json'), JSON.stringify({ base: BASE, tree, pages }, null, 2));
console.log(`\n${pages.length} pages, ${bad.length} problems -> ${OUT}`);
await browser.close();
process.exit(bad.length ? 1 : 0);
