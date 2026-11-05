<?php
/**
 * @file
 * Forms: Quick Draft and the WordPress-style settings pages, which keep
 * WordPress's field names and write the real Backdrop configuration.
 */

/**
 * A disabled field explaining why a WordPress option has no Backdrop equivalent.
 */
function wp_admin_unavailable($title, $why, $type = 'textfield', $value = NULL) {
  $element = $type === 'select' ? array('#options' => $value !== NULL ? array($value => $value) : array()) : array();
  return $element + array(
    '#type' => $type,
    '#title' => $title,
    '#disabled' => TRUE,
    '#default_value' => $value,
    '#description' => t('<em>Not available in Backdrop:</em> !why', array('!why' => $why)),
    '#prefix' => '<div class="wp-admin-unavailable">',
    '#suffix' => '</div>',
  );
}

/**
 * A "see also" line linking to the Backdrop screen that owns a setting.
 */
function wp_admin_see_also($text, $path) {
  return array('#markup' => '<p class="wp-admin-note">' . $text . ' ' . l(t('Open in Backdrop'), $path) . '.</p>');
}

/**
 * Shared bits for every settings form.
 */
function wp_admin_settings_form_finish($form, $title = NULL) {
  if ($title) {
    backdrop_set_title($title);
  }
  backdrop_add_library('wp_admin', 'pages');
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save Changes'));
  return $form;
}

/* ---------------------------------------------------------------- Quick Draft */

function wp_admin_quick_draft_form($form, &$form_state) {
  $form['title'] = array('#type' => 'textfield', '#title' => t('Title'), '#required' => TRUE, '#maxlength' => 255);
  $form['body'] = array('#type' => 'textarea', '#title' => t('Content'), '#rows' => 4, '#attributes' => array('placeholder' => t('What’s on your mind?')));
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save Draft'));
  return $form;
}

function wp_admin_quick_draft_form_submit($form, &$form_state) {
  global $user;
  $node = new Node(array('type' => 'post', 'title' => $form_state['values']['title'], 'uid' => $user->uid, 'status' => 0, 'langcode' => LANGUAGE_NONE));
  $node->body[LANGUAGE_NONE][0] = array('value' => '<p>' . nl2br(check_plain($form_state['values']['body'])) . '</p>', 'format' => 'filtered_html');
  $node->save();
  backdrop_set_message(t('Draft saved: <a href="@url">@title</a>. Find it under Posts with Status set to Unpublished.', array('@url' => url('node/' . $node->nid . '/edit'), '@title' => $node->title)));
}

/* ------------------------------------------------------------------ General */

function wp_admin_settings_general_form($form, &$form_state) {
  $core = config('system.core');
  $date = config('system.date');
  $base = $GLOBALS['base_url'];
  $form['site_name'] = array('#type' => 'textfield', '#title' => t('Site Title'), '#default_value' => $core->get('site_name'), '#required' => TRUE);
  $form['site_slogan'] = array('#type' => 'textfield', '#title' => t('Tagline'), '#default_value' => $core->get('site_slogan'), '#description' => t('In a few words, explain what this site is about. Example: “Just another WordPress site.”'));
  $form['wp_address'] = wp_admin_unavailable(t('WordPress Address (URL)'), t('Backdrop takes its address from the web server and settings.php ($base_url); it is not stored in the database.'), 'textfield', $base);
  $form['site_address'] = wp_admin_unavailable(t('Site Address (URL)'), t('Backdrop is served from the directory that contains index.php; there is no separate home URL.'), 'textfield', $base);
  $form['site_mail'] = array('#type' => 'email', '#title' => t('Administration Email Address'), '#default_value' => $core->get('site_mail'), '#required' => TRUE, '#description' => t('This address is used for admin purposes and as the “From” address of automated emails.'));
  $register = $core->get('user_register');
  $form['register'] = array('#type' => 'checkbox', '#title' => t('Membership: Anyone can register'), '#default_value' => $register !== 'admin_only');
  $form['register_approval'] = array('#type' => 'checkbox', '#title' => t('… but administrator approval is required'), '#default_value' => $register === 'visitors_admin_approval', '#states' => array('visible' => array(':input[name="register"]' => array('checked' => TRUE))), '#description' => t('Backdrop adds this option; WordPress has no approval step.'));
  $form['default_role'] = wp_admin_unavailable(t('New User Default Role'), t('every Backdrop user gets the “Authenticated” role; further roles are assigned per user on the People page or under !roles.', array('!roles' => l(t('Roles'), 'admin/config/people/roles'))), 'select', 'authenticated');
  $form['default_role']['#options'] = array('authenticated' => t('Authenticated user'));
  $form['language'] = wp_admin_unavailable(t('Site Language'), t('Backdrop core is single-language by default; the Language and Locale modules add more languages under Configuration → Regional and language.'), 'select', 'en');
  $form['language']['#options'] = array('en' => t('English'));
  $form['timezone'] = array('#type' => 'select', '#title' => t('Timezone'), '#options' => system_time_zones(), '#default_value' => $date->get('default_timezone'), '#description' => t('Choose either a city in the same timezone as you or a UTC (Coordinated Universal Time) time offset.'));
  $form['formats'] = array('#markup' => '<p>' . t('<strong>Date Format / Time Format:</strong> Backdrop keeps named date formats (short, medium, long and your own) that content types and views pick from. <a href="@url">Edit date and time formats</a>.', array('@url' => url('admin/config/regional/date-time'))) . '</p>');
  $form['first_day'] = array('#type' => 'select', '#title' => t('Week Starts On'), '#options' => array(0 => t('Sunday'), 1 => t('Monday'), 2 => t('Tuesday'), 3 => t('Wednesday'), 4 => t('Thursday'), 5 => t('Friday'), 6 => t('Saturday')), '#default_value' => (int) $date->get('first_day'));
  $form['see'] = wp_admin_see_also(t('Site title, slogan, email and front page also live on Site information.'), 'admin/config/system/site-information');
  return wp_admin_settings_form_finish($form, t('General Settings'));
}

function wp_admin_settings_general_form_submit($form, &$form_state) {
  $v = $form_state['values'];
  $register = !$v['register'] ? 'admin_only' : ($v['register_approval'] ? 'visitors_admin_approval' : 'visitors');
  config('system.core')->set('site_name', $v['site_name'])->set('site_slogan', $v['site_slogan'])->set('site_mail', $v['site_mail'])->set('user_register', $register)->save();
  config('system.date')->set('default_timezone', $v['timezone'])->set('first_day', (int) $v['first_day'])->save();
  backdrop_set_message(t('Settings saved.'));
}

/* ------------------------------------------------------------------ Writing */

function wp_admin_settings_writing_form($form, &$form_state) {
  $instance = field_info_instance('node', 'field_category', 'post');
  $options = array('' => t('- None -'));
  if (taxonomy_vocabulary_load('category')) {
    foreach (taxonomy_get_tree('category') as $term) {
      $options[$term->tid] = str_repeat('- ', $term->depth) . $term->name;
    }
  }
  $default = $instance && !empty($instance['default_value'][0]['tid']) ? $instance['default_value'][0]['tid'] : '';
  $form['default_category'] = array('#type' => 'select', '#title' => t('Default Post Category'), '#options' => $options, '#default_value' => $default, '#access' => (bool) $instance, '#description' => t('Pre-selected on new posts (the default value of the Categories field).'));
  $form['default_format'] = wp_admin_unavailable(t('Default Post Format'), t('post formats (aside, gallery, quote…) are a WordPress theme feature. In Backdrop, different kinds of content are different content types, each with its own fields and display.'), 'select', 'standard');
  $form['default_format']['#options'] = array('standard' => t('Standard'));
  $form['email'] = array('#type' => 'fieldset', '#title' => t('Post via email'), '#collapsible' => TRUE, '#collapsed' => TRUE);
  $form['email']['note'] = array('#markup' => '<p>' . t('Not available in Backdrop core. WordPress can poll a secret mailbox and publish each email as a post; Backdrop has no equivalent built in.') . '</p>');
  $form['services'] = wp_admin_unavailable(t('Update Services'), t('WordPress pings services like Ping-O-Matic when you publish. Backdrop does not ping; search engines are informed through the RSS feed and sitemaps (contrib XML Sitemap).'), 'textarea', 'http://rpc.pingomatic.com/');
  $form['backdrop'] = array('#type' => 'fieldset', '#title' => t('Where Backdrop keeps its writing settings'));
  $form['backdrop']['list'] = array('#theme' => 'item_list', '#items' => array(
    t('<a href="@url">Text formats and editors</a> — which HTML is allowed and how CKEditor is configured (WordPress: the block editor).', array('@url' => url('admin/config/content/formats'))),
    t('<a href="@url">Post content type</a> — fields, publishing defaults, comment settings for posts.', array('@url' => url('admin/structure/types/manage/post'))),
    t('<a href="@url">URL alias patterns</a> — how post URLs are built (WordPress: Permalinks).', array('@url' => url('admin/config/urls/path/patterns'))),
  ));
  return wp_admin_settings_form_finish($form, t('Writing Settings'));
}

function wp_admin_settings_writing_form_submit($form, &$form_state) {
  if ($instance = field_info_instance('node', 'field_category', 'post')) {
    $tid = $form_state['values']['default_category'];
    $instance['default_value'] = $tid ? array(array('tid' => $tid)) : array();
    field_update_instance($instance);
  }
  backdrop_set_message(t('Settings saved.'));
}

/* ------------------------------------------------------------------ Reading */

function wp_admin_settings_reading_form($form, &$form_state) {
  $core = config('system.core');
  $front = $core->get('site_frontpage');
  $pages = array();
  $current_page = '';
  $result = db_query("SELECT nid, title FROM {node} WHERE type = 'page' AND status = 1 ORDER BY title");
  foreach ($result as $row) {
    $pages[$row->nid] = $row->title;
    if ($front === 'node/' . $row->nid) {
      $current_page = (string) $row->nid;
    }
  }
  $form['front'] = array('#type' => 'radios', '#title' => t('Your homepage displays'), '#options' => array('home' => t('Your latest posts (the “Home” layout with the Promoted view)'), 'page' => t('A static page')), '#default_value' => $current_page ? 'page' : 'home');
  $form['front_page'] = array('#type' => 'select', '#title' => t('Homepage'), '#options' => $pages, '#empty_option' => t('— Select —'), '#default_value' => $current_page, '#states' => array('visible' => array(':input[name="front"]' => array('value' => 'page'))), '#description' => t('Backdrop stores the front page as a path (system.core: site_frontpage). The default “home” is a layout that shows the Promoted content view.'));
  $form['posts_page'] = wp_admin_unavailable(t('Posts page'), t('Backdrop lists content with Views (for example the Promoted view on the home layout), rather than a designated posts page.'), 'select');
  $view = views_get_view('promoted');
  $pager = $view ? $view->display['default']->display_options['pager'] : array();
  $form['posts_per_page'] = array('#type' => 'number', '#title' => t('Blog pages show at most'), '#field_suffix' => t('posts'), '#min' => 1, '#max' => 100, '#default_value' => isset($pager['options']['items_per_page']) ? $pager['options']['items_per_page'] : 10, '#access' => (bool) $view, '#description' => t('Items per page of the Promoted view.'));
  $feed = $view && isset($view->display['feed']) ? $view->display['feed']->display_options : array();
  $form['feed_items'] = array('#type' => 'number', '#title' => t('Syndication feeds show the most recent'), '#field_suffix' => t('items'), '#min' => 1, '#max' => 100, '#default_value' => isset($feed['pager']['options']['items_per_page']) ? $feed['pager']['options']['items_per_page'] : 10, '#access' => (bool) $feed, '#description' => t('Items in the Promoted view’s RSS feed (/rss.xml).'));
  $form['visibility'] = wp_admin_unavailable(t('Search engine visibility: Discourage search engines from indexing this site'), t('Backdrop core ships a static robots.txt; per-page robots tags come from the contrib Metatag module.'), 'checkbox');
  return wp_admin_settings_form_finish($form, t('Reading Settings'));
}

function wp_admin_settings_reading_form_submit($form, &$form_state) {
  $v = $form_state['values'];
  $front = $v['front'] === 'page' && $v['front_page'] ? 'node/' . $v['front_page'] : 'home';
  config('system.core')->set('site_frontpage', $front)->save();
  if ($view = views_get_view('promoted')) {
    $view->display['default']->display_options['pager']['options']['items_per_page'] = (int) $v['posts_per_page'];
    if (isset($view->display['feed']) && isset($v['feed_items'])) {
      $view->display['feed']->display_options['pager']['options']['items_per_page'] = (int) $v['feed_items'];
    }
    $view->save();
    views_invalidate_cache();
  }
  backdrop_set_message(t('Settings saved.'));
}

/* --------------------------------------------------------------- Discussion */

function wp_admin_settings_discussion_form($form, &$form_state) {
  $type = node_type_load('post');
  if (!$type) {
    return array('missing' => array('#markup' => t('There is no Post content type on this site.')));
  }
  $s = $type->settings;
  $anon = user_role_load('anonymous');
  $auth = user_role_load('authenticated');
  $form['defaults'] = array('#type' => 'fieldset', '#title' => t('Default post settings'));
  $form['defaults']['allow'] = array('#type' => 'checkbox', '#title' => t('Allow people to submit comments on new posts'), '#default_value' => $s['comment_default'] == COMMENT_NODE_OPEN, '#description' => t('Individual posts may override these settings. Changes here will only be applied to new posts going forward.'));
  $form['defaults']['notify'] = wp_admin_unavailable(t('Attempt to notify any blogs linked to from the post (pingbacks)'), t('Backdrop has no pingback/trackback support.'), 'checkbox');
  $form['other'] = array('#type' => 'fieldset', '#title' => t('Other comment settings'));
  $form['other']['registered'] = array('#type' => 'checkbox', '#title' => t('Users must be registered and logged in to comment'), '#default_value' => !in_array('post comments', $anon->permissions), '#description' => t('Maps to the “Post comments” permission of the Anonymous role.'));
  $form['other']['anonymous_contact'] = array('#type' => 'select', '#title' => t('Anonymous commenters'), '#options' => array(0 => t('May not enter their contact information'), 1 => t('May leave their contact information'), 2 => t('Must leave their contact information')), '#default_value' => $s['comment_anonymous'], '#states' => array('visible' => array(':input[name="registered"]' => array('checked' => FALSE))), '#description' => t('WordPress: “Comment author must fill out name and email”.'));
  $form['other']['threaded'] = array('#type' => 'checkbox', '#title' => t('Enable threaded (nested) comments'), '#default_value' => $s['comment_mode'] == COMMENT_MODE_THREADED);
  $form['other']['per_page'] = array('#type' => 'number', '#title' => t('Break comments into pages with'), '#field_suffix' => t('top level comments per page'), '#min' => 1, '#max' => 1000, '#default_value' => $s['comment_per_page']);
  $form['other']['preview'] = array('#type' => 'select', '#title' => t('Preview comment'), '#options' => array(0 => t('Disabled'), 1 => t('Optional'), 2 => t('Required')), '#default_value' => $s['comment_preview'], '#description' => t('Backdrop adds a preview step; WordPress has none.'));
  $form['before'] = array('#type' => 'fieldset', '#title' => t('Before a comment appears'));
  $form['before']['approve'] = array('#type' => 'checkbox', '#title' => t('Comment must be manually approved'), '#default_value' => !in_array('skip comment approval', $auth->permissions), '#description' => t('Maps to the “Skip comment approval” permission of the Authenticated role. Unapproved comments wait under Comments → Unapproved.'));
  $form['before']['previously'] = wp_admin_unavailable(t('Comment author must have a previously approved comment'), t('approval in Backdrop is a role permission, not per author.'), 'checkbox');
  $form['moderation'] = wp_admin_unavailable(t('Comment Moderation / Disallowed Comment Keys'), t('keyword blocking and link-count limits are not in core; use the contrib Antispam or Honeypot modules.'), 'textarea');
  $form['avatars'] = array('#type' => 'fieldset', '#title' => t('Avatars'));
  $form['avatars']['pictures'] = array('#type' => 'checkbox', '#title' => t('Show Avatars'), '#default_value' => !empty($s['comment_user_picture']), '#description' => t('Backdrop shows the user’s own uploaded picture (no Gravatar). Picture size and defaults are set under <a href="@url">Account settings</a>.', array('@url' => url('admin/config/people/settings'))));
  $form['see'] = wp_admin_see_also(t('All of these live on the Post content type’s comment settings.'), 'admin/structure/types/manage/post');
  return wp_admin_settings_form_finish($form, t('Discussion Settings'));
}

function wp_admin_settings_discussion_form_submit($form, &$form_state) {
  $v = $form_state['values'];
  $type = node_type_load('post');
  $type->settings['comment_default'] = $v['allow'] ? COMMENT_NODE_OPEN : COMMENT_NODE_CLOSED;
  $type->settings['comment_mode'] = $v['threaded'] ? COMMENT_MODE_THREADED : COMMENT_MODE_FLAT;
  $type->settings['comment_per_page'] = (int) $v['per_page'];
  $type->settings['comment_anonymous'] = (int) $v['anonymous_contact'];
  $type->settings['comment_preview'] = (int) $v['preview'];
  $type->settings['comment_user_picture'] = (int) $v['pictures'];
  node_type_save($type);
  $v['registered'] ? user_role_revoke_permissions('anonymous', array('post comments')) : user_role_grant_permissions('anonymous', array('post comments'));
  $v['approve'] ? user_role_revoke_permissions('authenticated', array('skip comment approval')) : user_role_grant_permissions('authenticated', array('skip comment approval'));
  backdrop_set_message(t('Settings saved.'));
}

/* -------------------------------------------------------------------- Media */

function wp_admin_settings_media_form($form, &$form_state) {
  $form['intro'] = array('#markup' => '<p>' . t('The sizes listed below determine the maximum dimensions in pixels to use when adding an image to the Media Library. In Backdrop these are <a href="@url">image styles</a>; each is a chain of effects (scale, crop, convert) applied on demand, and themes pick which style to show where.', array('@url' => url('admin/config/media/image-styles'))) . '</p>');
  foreach (array('thumbnail' => t('Thumbnail size'), 'medium' => t('Medium size'), 'large' => t('Large size')) as $name => $label) {
    $style = image_style_load($name);
    $form[$name] = array('#type' => 'fieldset', '#title' => $label, '#tree' => TRUE);
    $effect = $style ? wp_admin_image_scale_effect($style) : NULL;
    if (!$effect) {
      $form[$name]['missing'] = array('#markup' => t('No “@name” image style with a scale effect exists on this site.', array('@name' => $name)));
      continue;
    }
    $form[$name]['width'] = array('#type' => 'number', '#title' => t('Max Width'), '#min' => 0, '#default_value' => $effect['data']['width'], '#field_suffix' => 'px');
    $form[$name]['height'] = array('#type' => 'number', '#title' => t('Max Height'), '#min' => 0, '#default_value' => $effect['data']['height'], '#field_suffix' => 'px');
    if ($name === 'thumbnail') {
      $form[$name]['crop'] = array('#type' => 'checkbox', '#title' => t('Crop thumbnail to exact dimensions (normally thumbnails are proportional)'), '#default_value' => $effect['name'] === 'image_scale_and_crop');
    }
  }
  $form['uploads'] = array('#type' => 'fieldset', '#title' => t('Uploading Files'));
  $form['uploads']['organize'] = wp_admin_unavailable(t('Organize my uploads into month- and year-based folders'), t('the upload folder is set per image or file field, and the public/private file paths under File system.'), 'checkbox', TRUE);
  $form['uploads']['see'] = wp_admin_see_also(t('Public and private file system paths, temporary directory.'), 'admin/config/media/file-system');
  return wp_admin_settings_form_finish($form, t('Media Settings'));
}

/**
 * Finds the first scale-type effect of an image style (index and effect).
 */
function wp_admin_image_scale_effect($style) {
  foreach ($style['effects'] as $i => $effect) {
    if (in_array($effect['name'], array('image_scale', 'image_scale_and_crop', 'image_resize'))) {
      return $effect + array('index' => $i);
    }
  }
  return NULL;
}

function wp_admin_settings_media_form_submit($form, &$form_state) {
  foreach (array('thumbnail', 'medium', 'large') as $name) {
    $style = image_style_load($name);
    if (!$style || !($effect = wp_admin_image_scale_effect($style)) || empty($form_state['values'][$name])) {
      continue;
    }
    $v = $form_state['values'][$name];
    $i = $effect['index'];
    $style['effects'][$i]['data']['width'] = $v['width'] !== '' ? (int) $v['width'] : '';
    $style['effects'][$i]['data']['height'] = $v['height'] !== '' ? (int) $v['height'] : '';
    if ($name === 'thumbnail') {
      $style['effects'][$i]['name'] = !empty($v['crop']) ? 'image_scale_and_crop' : 'image_scale';
    }
    image_style_save($style);
    image_style_flush($style);
  }
  backdrop_set_message(t('Settings saved.'));
}

/* --------------------------------------------------------------- Permalinks */

function wp_admin_settings_permalinks_form($form, &$form_state) {
  $settings = config('path.settings');
  $form['intro'] = array('#markup' => '<p>' . t('WordPress builds post URLs from a pattern such as /%year%/%monthnum%/%postname%/. Backdrop does the same with URL alias patterns made of tokens, in core. These are the patterns on this site; new content gets its alias from them automatically.') . '</p>');
  $form['common'] = array('#type' => 'radios', '#title' => t('Common Settings'), '#options' => array(
    'plain' => t('Plain — <code>node/[node:nid]</code>'),
    'day' => t('Day and name — <code>[node:created:custom:Y/m/d]/[node:title]</code>'),
    'month' => t('Month and name — <code>[node:created:custom:Y/m]/[node:title]</code>'),
    'postname' => t('Post name — <code>[node:title]</code>'),
    'custom' => t('Custom Structure (edit the patterns below)'),
  ), '#default_value' => 'custom', '#description' => t('Choosing a preset fills the Post pattern below; Save Changes applies it.'));
  $patterns = array(
    'node_post_pattern' => t('Post pattern'),
    'node_page_pattern' => t('Page pattern'),
    'node_animal_pattern' => t('Animal pattern'),
    'node_habitat_pattern' => t('Habitat pattern'),
    'node_pattern' => t('Default content pattern'),
    'taxonomy_term_pattern' => t('Category / tag base (taxonomy term pattern)'),
    'user_pattern' => t('Author base (user pattern)'),
  );
  $form['patterns'] = array('#type' => 'fieldset', '#title' => t('Patterns'), '#tree' => TRUE);
  foreach ($patterns as $key => $label) {
    $value = $settings->get($key);
    if ($value === NULL && !in_array($key, array('node_post_pattern', 'node_page_pattern'))) {
      continue;
    }
    $form['patterns'][$key] = array('#type' => 'textfield', '#title' => $label, '#default_value' => (string) $value, '#description' => t('Tokens such as [node:title], [node:created:custom:Y], [term:name].'));
  }
  $form['see'] = wp_admin_see_also(t('All patterns, bulk generation and the alias list.'), 'admin/config/urls/path/patterns');
  return wp_admin_settings_form_finish($form, t('Permalink Settings'));
}

function wp_admin_settings_permalinks_form_submit($form, &$form_state) {
  $v = $form_state['values'];
  $presets = array('plain' => 'node/[node:nid]', 'day' => '[node:created:custom:Y/m/d]/[node:title]', 'month' => '[node:created:custom:Y/m]/[node:title]', 'postname' => '[node:title]');
  $config = config('path.settings');
  foreach ($v['patterns'] as $key => $pattern) {
    $config->set($key, $pattern);
  }
  if (isset($presets[$v['common']])) {
    $config->set('node_post_pattern', $presets[$v['common']]);
  }
  $config->save();
  backdrop_set_message(t('Settings saved. New content gets aliases from these patterns; existing aliases can be regenerated under <a href="@url">URL alias patterns</a>.', array('@url' => url('admin/config/urls/path/patterns'))));
}

/* ------------------------------------------------------------------ Privacy */

function wp_admin_settings_privacy_form($form, &$form_state) {
  $options = array();
  $result = db_query("SELECT nid, title, status FROM {node} WHERE type = 'page' ORDER BY title");
  foreach ($result as $row) {
    $options[$row->nid] = $row->title . ($row->status ? '' : ' (' . t('unpublished') . ')');
  }
  $current = (int) config_get('wp_admin.settings', 'privacy_page');
  $form['intro'] = array('#markup' => '<p>' . t('As a website owner, you may need to follow national or international privacy laws. WordPress keeps a designated Privacy Policy page and links it from the login screen; Backdrop core has no such designation, so this setting is stored by the WordPress-style admin module and the page is an ordinary Page. Linking it in the footer is done in <a href="@menu">Menus</a>.', array('@menu' => url('admin/structure/menu'))) . '</p>');
  $form['privacy_page'] = array('#type' => 'select', '#title' => t('Select a Privacy Policy page'), '#options' => $options, '#empty_option' => t('— Select —'), '#default_value' => $current ?: '');
  if ($current && ($node = node_load($current))) {
    $form['current'] = array('#markup' => '<p>' . t('Current page: <a href="@view">@title</a> (<a href="@edit">edit</a>).', array('@view' => url('node/' . $node->nid), '@title' => $node->title, '@edit' => url('node/' . $node->nid . '/edit'))) . '</p>');
  }
  $form['create'] = array('#markup' => '<p>' . t('Or <a href="@url">create a new Privacy Policy page</a>.', array('@url' => url('node/add/page'))) . '</p>');
  $form['guide'] = array('#type' => 'fieldset', '#title' => t('Policy Guide'), '#collapsible' => TRUE, '#collapsed' => TRUE);
  $form['guide']['text'] = array('#markup' => '<p>' . t('WordPress’s Policy Guide lists what core and each plugin collect. The Backdrop equivalents: comments store name, email and IP of the author; user accounts store email, last login and IP; the Database Logging module keeps request logs with IPs.') . '</p>');
  return wp_admin_settings_form_finish($form, t('Privacy'));
}

function wp_admin_settings_privacy_form_submit($form, &$form_state) {
  config('wp_admin.settings')->set('privacy_page', (int) $form_state['values']['privacy_page'])->save();
  backdrop_set_message(t('Settings saved.'));
}
