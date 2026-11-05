<?php
/**
 * @file
 * An adapted / explained wp-admin page.
 *
 * Variables:
 * - $page: title, intro, wordpress, backdrop, sections[] (heading, text,
 *   links[] with title, href, description, missing_module), note.
 * - $extra: optional rendered markup appended at the end.
 */
?>
<div class="wp-admin-page wp-admin-page--<?php print backdrop_html_class($page['id']); ?>">
  <?php if (!empty($page['intro'])): ?>
    <p class="wp-admin-page__intro"><?php print $page['intro']; ?></p>
  <?php endif; ?>
  <?php if (!empty($page['wordpress'])): ?>
    <div class="wp-admin-section">
      <h2><?php print t('What WordPress does here'); ?></h2>
      <p><?php print nl2br($page['wordpress']); ?></p>
    </div>
  <?php endif; ?>
  <?php if (!empty($page['backdrop'])): ?>
    <div class="wp-admin-section">
      <h2><?php print t('What Backdrop does instead'); ?></h2>
      <p><?php print nl2br($page['backdrop']); ?></p>
    </div>
  <?php endif; ?>
  <?php foreach ($page['sections'] as $section): ?>
    <div class="wp-admin-section">
      <?php if (!empty($section['heading'])): ?><h2><?php print check_plain($section['heading']); ?></h2><?php endif; ?>
      <?php if (!empty($section['text'])): ?><p><?php print nl2br($section['text']); ?></p><?php endif; ?>
      <?php if (!empty($section['links'])): ?>
        <ul class="wp-admin-links">
          <?php foreach ($section['links'] as $link): ?>
            <li>
              <?php if (!empty($link['href'])): ?>
                <a href="<?php print check_plain($link['href']); ?>"<?php print !empty($link['external']) ? ' target="_blank" rel="noopener"' : ''; ?>><?php print check_plain($link['title']); ?></a>
              <?php else: ?>
                <span><?php print check_plain($link['title']); ?></span>
                <?php if (!empty($link['missing_module'])): ?><em class="wp-admin-links__desc"><?php print t('(requires the @module module, which is not installed)', array('@module' => $link['missing_module'])); ?></em><?php endif; ?>
              <?php endif; ?>
              <?php if (!empty($link['description'])): ?><span class="wp-admin-links__desc">— <?php print check_plain($link['description']); ?></span><?php endif; ?>
            </li>
          <?php endforeach; ?>
        </ul>
      <?php endif; ?>
    </div>
  <?php endforeach; ?>
  <?php if (!empty($page['note'])): ?>
    <p class="wp-admin-note"><?php print $page['note']; ?></p>
  <?php endif; ?>
  <?php print $extra; ?>
</div>
