<?php
/**
 * @file
 * Dashboard layout: the full-width welcome panel, then two independent columns.
 *
 * Variables:
 * - $welcome: rendered welcome panel markup.
 * - $columns: array of columns, each a list of panels with id, title, content.
 */
?>
<div class="wp-admin-dashboard">
  <?php if ($welcome): ?>
    <div class="wp-admin-dashboard__welcome"><?php print $welcome; ?></div>
  <?php endif; ?>
  <?php foreach ($columns as $panels): ?>
    <div class="wp-admin-dashboard__col">
      <?php foreach ($panels as $panel): ?>
        <div class="wp-admin-card" id="wp-admin-<?php print $panel['id']; ?>">
          <h2 class="wp-admin-card__title"><?php print check_plain($panel['title']); ?></h2>
          <div class="wp-admin-card__body"><?php print $panel['content']; ?></div>
        </div>
      <?php endforeach; ?>
    </div>
  <?php endforeach; ?>
</div>
