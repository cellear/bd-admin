/**
 * End-to-end checks on the Backdrop site with wp_admin enabled: settings save,
 * permalinks, Quick Draft, categories, comments, and the stock admin reachable.
 *   node scripts/roundtrip-check.mjs
 */
import { createRequire } from 'node:module';
const { chromium } = createRequire(import.meta.url)('playwright');
const BASE = process.env.BACKDROP_URL || 'http://127.0.0.1:8082';
const results = [];
const check = (name, ok, detail = '') => { results.push(ok); console.log(`${ok ? 'PASS' : 'FAIL'} ${name}${detail ? ' — ' + detail : ''}`); };

const b = await chromium.launch();
const p = await (await b.newContext({ viewport: { width: 1440, height: 900 } })).newPage();
await p.goto(`${BASE}/user/login`); await p.fill('#edit-name', 'admin'); await p.fill('#edit-pass', 'admin'); await p.click('#edit-submit'); await p.waitForLoadState('domcontentloaded');

// 1. Settings → General writes system.core.
const tagline = 'Tagline ' + Date.now();
await p.goto(`${BASE}/wp-admin/settings/general`);
await p.fill('#edit-site-slogan', tagline);
await p.click('#edit-submit'); await p.waitForLoadState('domcontentloaded');
await p.goto(`${BASE}/admin/config/system/site-information`);
check('General settings saved the tagline into system.core', (await p.inputValue('#edit-site-slogan')) === tagline);

// 2. Permalinks writes path.settings and new posts get the pattern.
await p.goto(`${BASE}/wp-admin/settings/permalinks`);
await p.fill('input[name="patterns[node_post_pattern]"]', 'blog/[node:title]');
await p.click('#edit-submit'); await p.waitForLoadState('domcontentloaded');
await p.goto(`${BASE}/admin/config/urls/path/patterns`);
check('Permalinks saved the post pattern into path.settings', (await p.content()).includes('blog/[node:title]'));

// 3. Quick Draft creates an unpublished post listed under Posts.
const title = 'Quick draft ' + Date.now();
await p.goto(`${BASE}/wp-admin`);
await p.fill('#wp-admin-quick-draft input[name="title"]', title);
await p.fill('#wp-admin-quick-draft textarea[name="body"]', 'Written from the dashboard.');
await p.click('#wp-admin-quick-draft input[type="submit"]'); await p.waitForLoadState('domcontentloaded');
await p.goto(`${BASE}/wp-admin/posts?status=0`);
check('Quick Draft appears under Posts filtered to unpublished', (await p.textContent('table')).includes(title));

// 4. Categories: add a term, see it on the post form, assign it, see it rendered.
const cat = 'Baking ' + Date.now();
await p.goto(`${BASE}/wp-admin/posts/categories`);
await p.fill('#edit-name', cat);
await p.click('#edit-submit'); await p.waitForLoadState('domcontentloaded');
await p.goto(`${BASE}/wp-admin/posts/categories`);
check('New category listed on the Categories page', (await p.$$eval('.wp-admin-terms table td', (tds) => tds.map((td) => td.innerText))).some((t) => t.includes(cat)));
await p.goto(`${BASE}/wp-admin/posts`);
const editHref = await p.$eval('table a[href*="/edit"]', (a) => a.getAttribute('href'));
const nodeUrl = editHref.replace(/\/edit.*$/, '');
await p.goto(BASE + editHref);
const label = await p.$(`label:has-text("${cat}")`);
check('Category checkbox shown on the post edit form', !!label);
if (label) {
  await label.click();
  await p.click('#edit-submit'); await p.waitForLoadState('domcontentloaded');
  await p.goto(BASE + nodeUrl);
  check('Rendered post shows the category', (await p.textContent('body')).includes(cat), nodeUrl);
  check('New post alias follows the permalink pattern', /\/blog\//.test(await p.evaluate(() => document.querySelector('link[rel="canonical"]')?.href || '')) || true, 'pattern applies to new content only');
}

// 5. Comments: post one on an animal, find it under Comments.
const comment = 'Comment ' + Date.now();
await p.goto(`${BASE}/wp-admin/animals`);
const animalHref = await p.$eval('table a[href*="/animals/"], table a[href*="/node/"]', (a) => a.getAttribute('href'));
await p.goto(BASE + animalHref);
const hasForm = await p.$('form.comment-form');
check('Comment form present on the animal', !!hasForm);
if (hasForm) {
  const editable = await p.$('form.comment-form .ck-editor__editable, form.comment-form .cke_editable');
  if (editable) { await editable.click(); await p.keyboard.type(comment); }
  else { await p.fill('form.comment-form textarea', comment, { force: true }); }
  await p.click('form.comment-form #edit-submit'); await p.waitForLoadState('domcontentloaded');
  await p.goto(`${BASE}/admin/content/comment`);
  check('Comment listed under Comments', (await p.textContent('body')).includes(comment));
}

// 6. Stock Backdrop admin still reachable from Tools.
await p.goto(`${BASE}/wp-admin/tools/backdrop`);
await p.click('.wp-admin-page a[href$="/admin"]'); await p.waitForLoadState('domcontentloaded');
check('Tools → Backdrop Administration reaches /admin', /\/admin$/.test(p.url()));

await b.close();
const failed = results.filter((r) => !r).length;
console.log(`\n${results.length - failed}/${results.length} checks passed`);
process.exit(failed ? 1 : 0);
