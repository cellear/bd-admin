<?php
/**
 * @file
 * Page callbacks: dashboard, term pages, explained pages, redirects.
 */

/**
 * Users → Profile: the current user's edit form.
 */
function wp_admin_profile_redirect() {
  global $user;
  backdrop_goto('user/' . $user->uid . '/edit');
}

/**
 * Dashboard → Home.
 */
function wp_admin_dashboard_page() {
  backdrop_set_title(t('Dashboard'));
  backdrop_add_library('wp_admin', 'pages');
  module_load_include('inc', 'wp_admin', 'wp_admin.forms');

  $welcome = wp_admin_dashboard_welcome();
  $columns = array(
    array(wp_admin_dashboard_site_health(), wp_admin_dashboard_at_a_glance(), wp_admin_dashboard_activity()),
    array(wp_admin_dashboard_quick_draft(), wp_admin_dashboard_news()),
  );
  return theme('wp_admin_dashboard', array('welcome' => $welcome, 'columns' => $columns));
}

/**
 * "Welcome to bd-admin!" panel: a collapsible details element.
 */
function wp_admin_dashboard_welcome() {
  $cols = array(
    array(t('Author rich content'), t('Posts, pages, animals and habitats are Backdrop nodes with fields, a CKEditor body and image fields.'), l(t('Add a post'), 'node/add/post')),
    array(t('Customize your entire site'), t('In Backdrop the regions of every page are configured in Layouts; WordPress calls this the Site Editor.'), l(t('Open layouts'), 'admin/structure/layouts')),
    array(t('Switch up your site’s look & feel'), t('Themes change the look of the whole site. Each theme also has its own settings (logo, colors).'), l(t('Choose a theme'), 'admin/appearance')),
  );
  $items = '';
  foreach ($cols as $col) {
    $items .= '<div><h3>' . $col[0] . '</h3><p>' . $col[1] . '</p><p>' . $col[2] . '</p></div>';
  }
  $intro = t('The WordPress admin experience, running on Backdrop CMS @version. <a href="@url">Learn more about what is different in Backdrop.</a>', array('@version' => BACKDROP_VERSION, '@url' => url('wp-admin/tools/backdrop')));
  return '<details class="wp-admin-welcome" open><summary><span class="wp-admin-welcome__dismiss">' . t('Dismiss') . '</span><h2>' . t('Welcome to bd-admin!') . '</h2><p>' . $intro . '</p></summary><div class="wp-admin-welcome__cols">' . $items . '</div></details>';
}

/**
 * "Site Health Status": counts from the status report.
 */
function wp_admin_dashboard_site_health() {
  // The requirements hooks need the install API (REQUIREMENT_* constants).
  include_once BACKDROP_ROOT . '/core/includes/install.inc';
  backdrop_load_updates();
  $errors = $warnings = 0;
  $requirements = module_invoke_all('requirements', 'runtime');
  foreach ($requirements as $requirement) {
    $severity = isset($requirement['severity']) ? $requirement['severity'] : REQUIREMENT_OK;
    if ($severity == REQUIREMENT_ERROR) {
      $errors++;
    }
    elseif ($severity == REQUIREMENT_WARNING) {
      $warnings++;
    }
  }
  $summary = $errors ? t('Should be improved') : ($warnings ? t('Good, with warnings') : t('Good'));
  $content = '<p><strong>' . $summary . '</strong> — ' . format_plural($errors, '1 error', '@count errors') . ', ' . format_plural($warnings, '1 warning', '@count warnings') . '.</p>';
  $content .= '<p>' . t('Backdrop checks your PHP version, database, file permissions, available updates and more on the <a href="@url">Status report</a> (WordPress: Site Health).', array('@url' => url('admin/reports/status'))) . '</p>';
  return array('id' => 'site-health', 'title' => t('Site Health Status'), 'content' => $content);
}

/**
 * "At a Glance": published counts, version, theme.
 */
function wp_admin_dashboard_at_a_glance() {
  $count = function ($type) {
    return (int) db_query('SELECT COUNT(*) FROM {node} WHERE type = :type AND status = 1', array(':type' => $type))->fetchField();
  };
  $items = array(
    l(format_plural($count('post'), '1 Published post', '@count Published posts'), 'wp-admin/posts'),
    l(format_plural($count('page'), '1 Published page', '@count Published pages'), 'wp-admin/pages'),
  );
  if (node_type_load('animal')) {
    $items[] = l(format_plural($count('animal'), '1 Published animal', '@count Published animals'), 'wp-admin/animals');
  }
  if (node_type_load('habitat')) {
    $items[] = l(format_plural($count('habitat'), '1 Published habitat', '@count Published habitats'), 'wp-admin/habitats');
  }
  if (module_exists('comment')) {
    $comments = (int) db_query('SELECT COUNT(*) FROM {comment} WHERE status = 1')->fetchField();
    $items[] = l(format_plural($comments, '1 Comment', '@count Comments'), 'admin/content/comment');
  }
  $themes = list_themes();
  $theme = $themes[config_get('system.core', 'theme_default')];
  $content = theme('item_list', array('items' => $items, 'attributes' => array('class' => array('wp-admin-glance'))));
  $content .= '<p>' . t('Backdrop @version running the <a href="@url">@theme</a> theme.', array('@version' => BACKDROP_VERSION, '@theme' => $theme->info['name'], '@url' => url('admin/appearance'))) . '</p>';
  return array('id' => 'at-a-glance', 'title' => t('At a Glance'), 'content' => $content);
}

/**
 * "Activity": recent published content and recent comments.
 */
function wp_admin_dashboard_activity() {
  $rows = array();
  $result = db_query_range('SELECT nid, title, created FROM {node} WHERE status = 1 ORDER BY created DESC', 0, 5);
  foreach ($result as $row) {
    $rows[] = array(format_date($row->created, 'custom', 'M j, g:i a'), l($row->title, 'node/' . $row->nid));
  }
  $content = '<h3>' . t('Recently Published') . '</h3>' . theme('table', array('rows' => $rows, 'empty' => t('No published content yet.')));
  if (module_exists('comment')) {
    $crows = array();
    $result = db_query_range('SELECT c.cid, c.subject, c.name, c.status, n.title, n.nid FROM {comment} c INNER JOIN {node} n ON n.nid = c.nid ORDER BY c.created DESC', 0, 5);
    foreach ($result as $row) {
      $crows[] = array(check_plain($row->name), l($row->title, 'node/' . $row->nid, array('fragment' => 'comment-' . $row->cid)), $row->status ? t('Approved') : t('Pending'));
    }
    $content .= '<h3>' . t('Recent Comments') . '</h3>' . theme('table', array('rows' => $crows, 'empty' => t('No comments yet.')));
  }
  return array('id' => 'activity', 'title' => t('Activity'), 'content' => $content);
}

/**
 * "Quick Draft".
 */
function wp_admin_dashboard_quick_draft() {
  $content = '';
  if (node_access('create', 'post')) {
    $form = backdrop_get_form('wp_admin_quick_draft_form');
    $content .= backdrop_render($form);
  }
  global $user;
  $drafts = array();
  $result = db_query_range('SELECT nid, title FROM {node} WHERE type = :type AND status = 0 AND uid = :uid ORDER BY changed DESC', 0, 5, array(':type' => 'post', ':uid' => $user->uid));
  foreach ($result as $row) {
    $drafts[] = l($row->title, 'node/' . $row->nid . '/edit');
  }
  $content .= '<h3>' . t('Your Recent Drafts') . '</h3>' . ($drafts ? theme('item_list', array('items' => $drafts)) : '<p>' . t('No drafts yet.') . '</p>');
  return array('id' => 'quick-draft', 'title' => t('Quick Draft'), 'content' => $content);
}

/**
 * "WordPress Events and News": explained.
 */
function wp_admin_dashboard_news() {
  $links = array(
    l(t('Backdrop CMS news'), 'https://backdropcms.org/news'),
    l(t('Backdrop LIVE and community events'), 'https://backdropcms.org/events'),
    l(t('Backdrop release notes'), 'https://github.com/backdrop/backdrop/releases'),
    l(t('Backdrop forum'), 'https://forum.backdropcms.org/'),
  );
  $content = '<p>' . t('WordPress shows a live feed of nearby meetups, WordCamps and WordPress news in this panel. Backdrop core has no events feed; its Dashboard module shows a news block from backdropcms.org, and the rest lives on the project site.') . '</p>' . theme('item_list', array('items' => $links));
  return array('id' => 'news', 'title' => t('Backdrop Events and News'), 'content' => $content);
}

/**
 * WordPress-style Categories / Tags page: add form on the left, table on the right.
 */
function wp_admin_terms_page($machine_name) {
  $vocabulary = taxonomy_vocabulary_load($machine_name);
  if (!$vocabulary) {
    return t('The %vocabulary vocabulary does not exist on this site.', array('%vocabulary' => $machine_name));
  }
  backdrop_add_library('wp_admin', 'pages');
  $singular = array('category' => t('Category'), 'tags' => t('Tag'), 'animal_class' => t('Class'), 'habitat_type' => t('Habitat Type'));
  $singular = isset($singular[$machine_name]) ? $singular[$machine_name] : $vocabulary->name;

  $left = '';
  if (user_access('administer taxonomy') || user_access('create terms in ' . $machine_name)) {
    module_load_include('inc', 'taxonomy', 'taxonomy.admin');
    $term = entity_create('taxonomy_term', array('vocabulary' => $machine_name));
    $form = backdrop_get_form('taxonomy_form_term', $term, $vocabulary);
    $form['actions']['submit']['#value'] = t('Add New @type', array('@type' => $singular));
    $left = '<h2>' . t('Add New @type', array('@type' => $singular)) . '</h2>' . backdrop_render($form);
  }

  $counts = db_query('SELECT ti.tid, COUNT(ti.nid) c FROM {taxonomy_index} ti INNER JOIN {taxonomy_term_data} t ON t.tid = ti.tid WHERE t.vocabulary = :v GROUP BY ti.tid', array(':v' => $machine_name))->fetchAllKeyed();
  $rows = array();
  foreach (taxonomy_get_tree($machine_name, 0, NULL, TRUE) as $term) {
    $path = 'taxonomy/term/' . $term->tid;
    $alias = backdrop_get_path_alias($path);
    $rows[] = array(
      str_repeat('— ', $term->depth) . l($term->name, $path . '/edit'),
      check_plain(truncate_utf8(strip_tags($term->description), 120, TRUE, TRUE)),
      $alias !== $path ? check_plain($alias) : t('(none)'),
      isset($counts[$term->tid]) ? $counts[$term->tid] : 0,
      l(t('Edit'), $path . '/edit') . ' | ' . l(t('Delete'), $path . '/delete') . ' | ' . l(t('View'), $path),
    );
  }
  $right = theme('table', array(
    'header' => array(t('Name'), t('Description'), t('Slug'), t('Count'), t('Operations')),
    'rows' => $rows,
    'empty' => t('No @items found.', array('@items' => backdrop_strtolower($vocabulary->name))),
  ));
  $right .= '<p class="wp-admin-note">' . t('Slug is the term’s URL alias. To reorder terms by drag and drop use the <a href="@url">@vocab overview</a>.', array('@url' => url('admin/structure/taxonomy/' . $machine_name), '@vocab' => $vocabulary->name)) . '</p>';
  // The term form sets its own title; restore the WordPress one.
  $titles = array('category' => t('Categories'), 'tags' => t('Tags'), 'animal_class' => t('Classes'), 'habitat_type' => t('Habitat Types'));
  backdrop_set_title(isset($titles[$machine_name]) ? $titles[$machine_name] : $vocabulary->name);
  return '<div class="wp-admin-terms"><div>' . $left . '</div><div>' . $right . '</div></div>';
}

/**
 * Explained / adapted pages from pages/<section>/<page>.json.
 */
function wp_admin_explained_page($id) {
  $file = backdrop_get_path('module', 'wp_admin') . '/pages/' . str_replace(':', '/', $id) . '.json';
  if (!is_file($file)) {
    return backdrop_not_found();
  }
  $page = backdrop_json_decode(file_get_contents($file));
  $page['id'] = str_replace(':', '-', $id);
  $page += array('sections' => array());
  foreach ($page['sections'] as $i => $section) {
    foreach (isset($section['links']) ? $section['links'] : array() as $j => $link) {
      if (!empty($link['module']) && !module_exists($link['module'])) {
        $page['sections'][$i]['links'][$j]['missing_module'] = $link['module'];
        $page['sections'][$i]['links'][$j]['href'] = '';
        continue;
      }
      $url = $link['url'];
      if (preg_match('#^https?://#', $url)) {
        $page['sections'][$i]['links'][$j]['href'] = $url;
        $page['sections'][$i]['links'][$j]['external'] = TRUE;
      }
      else {
        $page['sections'][$i]['links'][$j]['href'] = url($url);
      }
    }
  }
  backdrop_set_title($page['title']);
  backdrop_add_library('wp_admin', 'pages');
  return theme('wp_admin_page', array('page' => $page, 'extra' => ''));
}
