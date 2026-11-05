<?php
/**
 * Recreates the Backdrop sample content in WordPress from content/animals-export.json.
 * Run with: wp eval-file scripts/import-to-wordpress.php   (from the wordpress/ dir)
 *
 * Mapping: Post -> Post; Page -> Page; Animal -> Post in category "Animals" with its
 * Class as a sub-category; Habitat -> Post in category "Habitats" with its Habitat Type
 * as a sub-category; Card -> skipped (a Backdrop layout block); comments -> comments;
 * images -> attachments set as featured image; users -> users with equivalent roles.
 */
require_once ABSPATH . 'wp-admin/includes/image.php';
require_once ABSPATH . 'wp-admin/includes/file.php';
require_once ABSPATH . 'wp-admin/includes/media.php';

$root = dirname(__DIR__);
$data = json_decode(file_get_contents("$root/content/animals-export.json"), true);
$log = fn($m) => WP_CLI::log($m);

update_option('blogname', $data['site']['name']);
update_option('blogdescription', $data['site']['slogan'] ?: 'Sample animal content from Backdrop CMS');
update_option('permalink_structure', '/%postname%/');
update_option('timezone_string', 'UTC');

// --- Users ---
$roleMap = ['administrator' => 'administrator', 'editor' => 'editor', 'author' => 'author'];
$userMap = [];
foreach ($data['users'] as $u) {
  $login = sanitize_user(strtolower(remove_accents(str_replace(' ', '.', $u['name']))), true);
  $existing = get_user_by('email', $u['mail']) ?: get_user_by('login', $login);
  $role = 'subscriber';
  foreach ($u['roles'] as $r) { if (isset($roleMap[$r])) { $role = $roleMap[$r]; break; } }
  if ($existing) { $id = $existing->ID; wp_update_user(['ID' => $id, 'display_name' => $u['name'], 'role' => $role]); }
  else {
    $id = wp_insert_user(['user_login' => $login, 'user_email' => $u['mail'], 'user_pass' => wp_generate_password(), 'display_name' => $u['name'], 'role' => $role]);
    if (is_wp_error($id)) { WP_CLI::warning("user {$u['name']}: " . $id->get_error_message()); continue; }
  }
  $userMap[$u['uid']] = $id;
}
$log(count($userMap) . ' users');

// --- Categories: Animals (classes below), Habitats (habitat types below) ---
$cat = function (string $name, int $parent = 0, string $desc = '') {
  $r = term_exists($name, 'category', $parent) ?: wp_insert_term($name, 'category', ['parent' => $parent, 'description' => $desc]);
  return is_wp_error($r) ? 0 : (int) $r['term_id'];
};
$animalsCat = $cat('Animals', 0, 'Animals from Backdrop’s Sample Animal Content');
$habitatsCat = $cat('Habitats', 0, 'Habitats from Backdrop’s Sample Animal Content');
$termMap = [];
// Hierarchical: parents first.
$terms = $data['terms'];
uasort($terms, fn($a, $b) => $a['parent'] <=> $b['parent']);
foreach ($terms as $t) {
  if ($t['vid'] === 'tags') {
    $r = term_exists($t['name'], 'post_tag') ?: wp_insert_term($t['name'], 'post_tag', ['description' => $t['description']]);
    if (!is_wp_error($r)) $termMap[$t['tid']] = (int) $r['term_id'];
    continue;
  }
  $base = $t['vid'] === 'animal_class' ? $animalsCat : ($t['vid'] === 'habitat_type' ? $habitatsCat : 0);
  if (!$base) continue;
  $parent = $t['parent'] && isset($termMap[$t['parent']]) ? $termMap[$t['parent']] : $base;
  $termMap[$t['tid']] = $cat($t['name'], $parent, $t['description']);
}
$log(count($termMap) . ' terms mapped');

// --- Images ---
$mediaMap = [];
foreach ($data['files'] as $f) {
  if (!$f['path'] || !file_exists($f['path'])) { WP_CLI::warning("missing file for fid {$f['fid']}"); continue; }
  $existing = get_posts(['post_type' => 'attachment', 'meta_key' => '_backdrop_fid', 'meta_value' => $f['fid'], 'fields' => 'ids']);
  if ($existing) { $mediaMap[$f['fid']] = $existing[0]; continue; }
  $tmp = wp_tempnam(basename($f['path']));
  copy($f['path'], $tmp);
  $id = media_handle_sideload(['name' => basename($f['path']), 'tmp_name' => $tmp], 0, $f['title'] ?: pathinfo($f['path'], PATHINFO_FILENAME));
  if (is_wp_error($id)) { WP_CLI::warning("file {$f['path']}: " . $id->get_error_message()); continue; }
  update_post_meta($id, '_wp_attachment_image_alt', $f['alt']);
  update_post_meta($id, '_backdrop_fid', $f['fid']);
  $mediaMap[$f['fid']] = $id;
}
$log(count($mediaMap) . ' images imported');

// --- Nodes ---
$nodeMap = [];
$counts = [];
foreach ($data['nodes'] as $n) {
  if ($n['type'] === 'card') continue;
  $postType = $n['type'] === 'page' ? 'page' : 'post';
  $cats = [];
  if ($n['type'] === 'animal') {
    $cats[] = $animalsCat;
    foreach ($n['class'] as $tid) if (isset($termMap[$tid])) $cats[] = $termMap[$tid];
  }
  elseif ($n['type'] === 'habitat') {
    $cats[] = $habitatsCat;
    foreach ($n['habitat_type'] as $tid) if (isset($termMap[$tid])) $cats[] = $termMap[$tid];
  }
  $body = "<!-- wp:html -->\n" . $n['body'] . "\n<!-- /wp:html -->";
  if (!empty($n['link'])) {
    $body .= "\n\n<!-- wp:paragraph -->\n<p><a href=\"" . esc_url($n['link']) . "\">" . esc_html__('Read more on Wikipedia') . "</a></p>\n<!-- /wp:paragraph -->";
  }
  $args = [
    'post_type' => $postType, 'post_title' => $n['title'], 'post_name' => basename($n['alias'] ?: sanitize_title($n['title'])),
    'post_content' => $body, 'post_excerpt' => $n['summary'],
    'post_status' => $n['status'] ? 'publish' : 'draft',
    'post_author' => $userMap[$n['uid']] ?? 1,
    'post_date_gmt' => gmdate('Y-m-d H:i:s', strtotime($n['created'])), 'post_date' => gmdate('Y-m-d H:i:s', strtotime($n['created'])),
    'post_modified_gmt' => gmdate('Y-m-d H:i:s', strtotime($n['changed'])),
    'post_category' => $cats, 'comment_status' => $n['comment'] == 2 ? 'open' : 'closed',
    'meta_input' => ['_backdrop_nid' => $n['nid'], '_backdrop_type' => $n['type']],
  ];
  $existing = get_posts(['post_type' => $postType, 'meta_key' => '_backdrop_nid', 'meta_value' => $n['nid'], 'fields' => 'ids', 'post_status' => 'any']);
  if ($existing) $args['ID'] = $existing[0];
  $id = wp_insert_post($args, true);
  if (is_wp_error($id)) { WP_CLI::warning("node {$n['title']}: " . $id->get_error_message()); continue; }
  $tags = array_values(array_filter(array_map(fn($tid) => $termMap[$tid] ?? null, $n['tags'])));
  if ($tags) wp_set_post_terms($id, $tags, 'post_tag');
  if ($n['image'] && isset($mediaMap[$n['image']])) set_post_thumbnail($id, $mediaMap[$n['image']]);
  if ($n['sticky']) stick_post($id);
  $nodeMap[$n['nid']] = $id;
  $counts[$n['type']] = ($counts[$n['type']] ?? 0) + 1;
}
$log('nodes: ' . json_encode($counts));

// --- Comments (threaded; parents first) ---
$commentMap = [];
$comments = $data['comments'];
usort($comments, fn($a, $b) => $a['pid'] <=> $b['pid'] ?: $a['cid'] <=> $b['cid']);
$done = 0;
foreach ($comments as $c) {
  if (!isset($nodeMap[$c['nid']])) continue;
  $existing = get_comments(['meta_key' => '_backdrop_cid', 'meta_value' => $c['cid'], 'fields' => 'ids']);
  if ($existing) { $commentMap[$c['cid']] = $existing[0]; continue; }
  $id = wp_insert_comment([
    'comment_post_ID' => $nodeMap[$c['nid']], 'comment_author' => $c['name'] ?: 'Anonymous', 'comment_author_email' => $c['mail'],
    'comment_content' => ($c['subject'] ? '<strong>' . esc_html($c['subject']) . '</strong><br>' : '') . $c['body'],
    'comment_date_gmt' => gmdate('Y-m-d H:i:s', strtotime($c['created'])), 'comment_date' => gmdate('Y-m-d H:i:s', strtotime($c['created'])),
    'comment_approved' => $c['status'] ? 1 : 0, 'comment_parent' => $c['pid'] && isset($commentMap[$c['pid']]) ? $commentMap[$c['pid']] : 0,
    'user_id' => $userMap[$c['uid']] ?? 0, 'comment_meta' => ['_backdrop_cid' => $c['cid']],
  ]);
  if ($id) { $commentMap[$c['cid']] = $id; $done++; }
}
$log("$done comments imported");
$log('Done.');
