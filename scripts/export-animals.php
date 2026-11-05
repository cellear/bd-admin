<?php
/**
 * Exports the Backdrop site's content (animals, habitats, posts, pages, terms,
 * comments, images, users) to JSON for recreation in WordPress.
 *   bee php-script scripts/export-animals.php   (from backdrop/)
 */

$out = array('site' => array(), 'users' => array(), 'terms' => array(), 'files' => array(), 'nodes' => array(), 'comments' => array());
$core = config('system.core');
$out['site'] = array('name' => $core->get('site_name'), 'slogan' => $core->get('site_slogan'));

foreach (user_load_multiple(FALSE) as $account) {
  if ($account->uid == 0) {
    continue;
  }
  $out['users'][$account->uid] = array('uid' => (int) $account->uid, 'name' => $account->name, 'mail' => $account->mail, 'roles' => array_values($account->roles));
}

foreach (taxonomy_get_vocabularies() as $vocabulary) {
  foreach (taxonomy_get_tree($vocabulary->machine_name, 0, NULL, TRUE) as $term) {
    $out['terms'][$term->tid] = array('tid' => (int) $term->tid, 'vid' => $vocabulary->machine_name, 'vocabulary' => $vocabulary->name,
      'name' => $term->name, 'description' => (string) $term->description, 'parent' => (int) reset($term->parents));
  }
}

$text = function ($node, $field) {
  $items = field_get_items('node', $node, $field);
  return $items ? $items[0]['value'] : '';
};
$refs = function ($node, $field, $key = 'tid') {
  $items = field_get_items('node', $node, $field);
  $ids = array();
  foreach ($items ?: array() as $item) {
    $ids[] = (int) $item[$key];
  }
  return $ids;
};

$nids = db_query("SELECT nid FROM {node} ORDER BY nid")->fetchCol();
foreach (node_load_multiple($nids) as $node) {
  $image = field_get_items('node', $node, 'field_image');
  $file = NULL;
  if ($image) {
    $f = file_load($image[0]['fid']);
    if ($f) {
      $file = array('fid' => (int) $f->fid, 'path' => backdrop_realpath($f->uri), 'alt' => isset($image[0]['alt']) ? $image[0]['alt'] : '', 'title' => isset($image[0]['title']) ? $image[0]['title'] : '');
      $out['files'][$f->fid] = $file;
    }
  }
  $tags = $node->type === 'post' ? $refs($node, 'field_tags') : array();
  $row = array(
    'nid' => (int) $node->nid, 'type' => $node->type, 'title' => $node->title,
    'alias' => backdrop_get_path_alias('node/' . $node->nid),
    'uid' => (int) $node->uid, 'status' => (int) $node->status, 'promote' => (int) $node->promote, 'sticky' => (int) $node->sticky,
    'created' => date('c', $node->created), 'changed' => date('c', $node->changed),
    'body' => $text($node, 'body'),
    'summary' => ($items = field_get_items('node', $node, 'body')) && !empty($items[0]['summary']) ? $items[0]['summary'] : '',
    'image' => $file ? (int) $file['fid'] : NULL,
    'tags' => $tags,
    'comment' => (int) $node->comment,
  );
  if ($node->type === 'animal') {
    $row['class'] = $refs($node, 'field_animal_class');
    $row['habitats'] = $refs($node, 'field_animal_habitat', 'target_id');
    $link = field_get_items('node', $node, 'field_animal_link');
    $row['link'] = $link ? $link[0]['url'] : '';
  }
  if ($node->type === 'habitat') {
    $row['habitat_type'] = $refs($node, 'field_habitat_type');
  }
  $out['nodes'][] = $row;
}

$cids = db_query("SELECT cid FROM {comment} ORDER BY cid")->fetchCol();
foreach (comment_load_multiple($cids) as $comment) {
  $body = field_get_items('comment', $comment, 'comment_body');
  $out['comments'][] = array(
    'cid' => (int) $comment->cid, 'nid' => (int) $comment->nid, 'pid' => (int) $comment->pid,
    'uid' => (int) $comment->uid, 'name' => $comment->name, 'mail' => (string) $comment->mail,
    'subject' => $comment->subject, 'body' => $body ? $body[0]['value'] : '',
    'created' => date('c', $comment->created), 'status' => (int) $comment->status,
  );
}

$path = dirname(BACKDROP_ROOT) . '/content/animals-export.json';
file_put_contents($path, json_encode($out, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE));
print "Wrote $path: " . count($out['nodes']) . " nodes, " . count($out['terms']) . " terms, " . count($out['files']) . " files, " . count($out['comments']) . " comments, " . count($out['users']) . " users\n";
