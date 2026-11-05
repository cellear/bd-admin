/**
 * Checks the WordPress-style sidebar on Backdrop: geometry, current item with
 * inline children, flyouts on hover, no overlap on node edit pages, and the
 * mobile burger.   node scripts/sidebar-check.mjs
 */
import { createRequire } from 'node:module';
const { chromium } = createRequire(import.meta.url)('playwright');
const BASE = process.env.BACKDROP_URL || 'http://127.0.0.1:8082';
const results = [];
const check = (name, ok, detail = '') => { results.push(ok); console.log(`${ok ? 'PASS' : 'FAIL'} ${name}${detail ? ' — ' + detail : ''}`); };

const b = await chromium.launch();
const p = await (await b.newContext({ viewport: { width: 1440, height: 900 } })).newPage();
await p.goto(`${BASE}/user/login`); await p.fill('#edit-name', 'admin'); await p.fill('#edit-pass', 'admin'); await p.click('#edit-submit'); await p.waitForLoadState('domcontentloaded');
await p.goto(`${BASE}/wp-admin`, { waitUntil: 'domcontentloaded' });
let s = await p.evaluate(() => {
  const sb = document.querySelector('#wp-admin-sidebar'); const bar = document.querySelector('#wp-admin-bar');
  const cur = document.querySelector('.wp-admin-menu__item--current');
  const row = document.querySelector('.wp-admin-menu__item--posts > a');
  return {
    sidebarWidth: Math.round(sb.getBoundingClientRect().width), sidebarTop: Math.round(sb.getBoundingClientRect().top),
    barHeight: Math.round(bar.getBoundingClientRect().height),
    current: cur?.querySelector('.wp-admin-menu__label')?.textContent.trim(),
    inlineChildren: [...(cur?.querySelectorAll('.wp-admin-submenu a') || [])].map((a) => a.textContent.trim()),
    inlineVisible: cur ? getComputedStyle(cur.querySelector('.wp-admin-submenu')).display !== 'none' : false,
    currentBg: cur ? getComputedStyle(cur.querySelector(':scope > a')).backgroundColor : '',
    rowHeight: Math.round(row.getBoundingClientRect().height),
    bodyPadLeft: parseInt(getComputedStyle(document.body).paddingLeft, 10),
    backdropBar: !!document.querySelector('#admin-bar'),
  };
});
check('Sidebar is 160px wide below a 32px bar', s.sidebarWidth === 160 && s.sidebarTop === 32 && s.barHeight === 32, JSON.stringify(s));
check('Dashboard is the current item, blue, with Home and Updates inline', s.current === 'Dashboard' && s.inlineVisible && s.inlineChildren.join(',') === 'Home,Updates' && s.currentBg === 'rgb(34, 113, 177)');
check('Rows are 34px', s.rowHeight === 34, String(s.rowHeight));
check('Page content starts right of the sidebar', s.bodyPadLeft >= 160, String(s.bodyPadLeft));
check('Backdrop admin bar is gone', !s.backdropBar);

// Flyout on hover for a non-current item.
await p.hover('.wp-admin-menu__item--settings > a');
await p.waitForTimeout(200);
const flyout = await p.evaluate(() => { const ul = document.querySelector('.wp-admin-menu__item--settings .wp-admin-submenu'); const r = ul.getBoundingClientRect(); return { display: getComputedStyle(ul).display, left: Math.round(r.left) }; });
check('Settings flyout opens on hover to the right of the sidebar', flyout.display !== 'none' && flyout.left >= 160, JSON.stringify(flyout));

// A node edit page: still has the chrome, content not under the sidebar.
await p.goto(`${BASE}/node/1/edit`, { waitUntil: 'domcontentloaded' });
const edit = await p.evaluate(() => ({ h1: document.querySelector('h1')?.innerText.trim(), formLeft: Math.round(document.querySelector('form.node-form')?.getBoundingClientRect().left || 0), chrome: !!document.querySelector('#wp-admin-sidebar') }));
check('Node edit page keeps the chrome and sits right of the sidebar', edit.chrome && edit.formLeft >= 160, JSON.stringify(edit));

// Front end while logged in: chrome present.
await p.goto(`${BASE}/`, { waitUntil: 'domcontentloaded' });
check('Front page shows the chrome for the admin', !!(await p.$('#wp-admin-sidebar')) && !!(await p.$('#wp-admin-bar')));

// Mobile.
const m = await (await b.newContext({ viewport: { width: 768, height: 1024 } })).newPage();
await m.goto(`${BASE}/user/login`); await m.fill('#edit-name', 'admin'); await m.fill('#edit-pass', 'admin'); await m.click('#edit-submit'); await m.waitForLoadState('domcontentloaded');
await m.goto(`${BASE}/wp-admin`, { waitUntil: 'domcontentloaded' });
const hiddenBefore = await m.evaluate(() => getComputedStyle(document.querySelector('#wp-admin-sidebar')).display === 'none');
await m.click('.wp-admin-bar__burger'); await m.waitForTimeout(200);
const shownAfter = await m.evaluate(() => getComputedStyle(document.querySelector('#wp-admin-sidebar')).display !== 'none');
check('Mobile: sidebar hidden until the burger opens it', hiddenBefore && shownAfter);
await b.close();
const failed = results.filter((r) => !r).length;
console.log(`\n${results.length - failed}/${results.length} checks passed`);
process.exit(failed ? 1 : 0);
