<?php
/**
 * Adds all Sample Animal Content extras (10 more animals, threaded comments,
 * the Habitat content type with 6 habitats) without clicking through the
 * module's admin form.   bee php-script scripts/enable-sample-extras.php
 */
module_load_include('inc', 'sample_animal_content', 'sample_animal_content.admin');
$config = config('sample_animal_content.settings');
$extras = $config->get('extras_added') ?: array();
$form = array();
foreach (array('extra_animals', 'comments', 'habitats') as $extra) {
  if (!empty($extras[$extra])) {
    print "$extra: already added\n";
    continue;
  }
  $form_state = array();
  $function = "sample_animal_content_add_{$extra}_submit";
  $function($form, $form_state);
  print "$extra: added\n";
}
foreach (backdrop_get_messages() as $type => $messages) {
  foreach ($messages as $message) {
    print "[$type] " . strip_tags($message) . "\n";
  }
}
