<?php
/**
 * @file
 * The WordPress look: the sidebar (wp-admin menu tree) and the top admin bar.
 */

/**
 * Renders the sidebar and the top bar for the current user and path.
 */
function wp_admin_render_chrome() {
  return theme('wp_admin_sidebar', array('items' => wp_admin_sidebar_items()))
    . theme('wp_admin_top_bar', wp_admin_top_bar_data());
}

/**
 * The current internal path, with its alias, for current-item detection.
 */
function wp_admin_current_paths() {
  $path = current_path();
  $paths = array($path, backdrop_get_path_alias($path));
  if (backdrop_is_front_page()) {
    $paths[] = '<front>';
  }
  return array_unique($paths);
}

/**
 * Builds the sidebar items from the wp-admin menu: label, url, icon, current,
 * children (label, url, current). The current item is the top-level item whose
 * own path or any child path matches the page; WordPress shows its children
 * inline under it.
 */
function wp_admin_sidebar_items() {
  $current = wp_admin_current_paths();
  $items = array();
  $icons = wp_admin_icons();
  foreach (menu_tree_all_data('wp-admin') as $element) {
    $link = $element['link'];
    if ($link['hidden'] || !$link['access']) {
      continue;
    }
    $item = array(
      'label' => $link['title'],
      'href' => url($link['link_path']),
      'path' => $link['link_path'],
      'icon' => '',
      'current' => in_array($link['link_path'], $current),
      'children' => array(),
    );
    $key = backdrop_html_class(strtolower($link['title']));
    if (isset($icons[$key])) {
      $item['icon'] = $icons[$key];
    }
    foreach ($element['below'] as $child_element) {
      $child = $child_element['link'];
      if ($child['hidden'] || !$child['access']) {
        continue;
      }
      $is_current = in_array($child['link_path'], $current);
      $item['children'][] = array('label' => $child['title'], 'href' => url($child['link_path']), 'current' => $is_current);
      if ($is_current) {
        $item['current'] = TRUE;
      }
    }
    // The first child repeats the parent (WordPress convention), so mark it.
    if ($item['current'] && !empty($item['children']) && !array_filter(array_column($item['children'], 'current'))) {
      $item['children'][0]['current'] = TRUE;
    }
    $items[] = $item;
  }
  return $items;
}

/**
 * Inline SVG icons keyed by the lower-cased top-level label.
 */
function wp_admin_icons() {
  $icons = &backdrop_static(__FUNCTION__);
  if (!isset($icons)) {
    $icons = array();
    $dir = backdrop_get_path('module', 'wp_admin') . '/icons';
    foreach (glob($dir . '/*.svg') as $file) {
      $icons[basename($file, '.svg')] = file_get_contents($file);
    }
  }
  return $icons;
}

/**
 * Data for the top admin bar.
 */
function wp_admin_top_bar_data() {
  global $user;
  $site_name = config_get('system.core', 'site_name');

  $logo_menu = array(
    array('title' => t('About Backdrop'), 'href' => url('admin/reports/status')),
    array('title' => t('Backdrop Administration'), 'href' => url('admin')),
    array('title' => t('Documentation'), 'href' => 'https://docs.backdropcms.org/'),
    array('title' => t('Support'), 'href' => 'https://forum.backdropcms.org/'),
    array('title' => t('Backdrop CMS'), 'href' => 'https://backdropcms.org/'),
  );
  $site = array(
    'name' => $site_name,
    'href' => url('<front>'),
    'links' => array(
      array('title' => t('Visit Site'), 'href' => url('<front>')),
      array('title' => t('Dashboard'), 'href' => url('wp-admin')),
      array('title' => t('Themes'), 'href' => url('admin/appearance')),
      array('title' => t('Layouts'), 'href' => url('admin/structure/layouts')),
      array('title' => t('Menus'), 'href' => url('admin/structure/menu')),
    ),
  );

  $comments = NULL;
  if (module_exists('comment') && user_access('administer comments')) {
    $pending = (int) db_query('SELECT COUNT(*) FROM {comment} WHERE status = :status', array(':status' => COMMENT_NOT_PUBLISHED))->fetchField();
    $comments = array(
      'count' => $pending,
      'href' => url($pending ? 'admin/content/comment/approval' : 'admin/content/comment'),
      'label' => format_plural($pending, '1 Comment in moderation', '@count Comments in moderation'),
    );
  }

  $new = array();
  $types = node_type_get_types();
  foreach (array('post' => t('Post'), 'page' => t('Page'), 'animal' => t('Animal'), 'habitat' => t('Habitat')) as $type => $label) {
    if (isset($types[$type]) && node_access('create', $type)) {
      $new[] = array('title' => $label, 'href' => url('node/add/' . $type));
    }
    if ($type === 'post' && user_access('create files')) {
      $new[] = array('title' => t('Media'), 'href' => url('file/add'));
    }
  }
  if (user_access('administer users')) {
    $new[] = array('title' => t('User'), 'href' => url('admin/people/create'));
  }

  $account = array(
    'name' => user_format_name($user),
    'initial' => backdrop_strtoupper(backdrop_substr(user_format_name($user), 0, 1)),
    'links' => array(
      array('title' => t('Edit Profile'), 'href' => url('user/' . $user->uid . '/edit')),
      array('title' => t('View Profile'), 'href' => url('user/' . $user->uid)),
      array('title' => t('Log Out'), 'href' => url('user/logout')),
    ),
  );

  return array('logo_menu' => $logo_menu, 'site' => $site, 'comments' => $comments, 'new' => $new, 'account' => $account);
}
