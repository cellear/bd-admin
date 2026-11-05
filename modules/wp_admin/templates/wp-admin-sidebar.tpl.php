<?php
/**
 * @file
 * The WordPress-style sidebar: the wp-admin menu tree.
 *
 * Variables:
 * - $items: top-level items with label, href, icon (inline SVG), current,
 *   children (label, href, current).
 */
?>
<nav id="wp-admin-sidebar" class="wp-admin-sidebar" aria-label="<?php print t('Main menu'); ?>">
  <ul class="wp-admin-menu">
    <?php foreach ($items as $i => $item): ?>
      <?php $key = backdrop_html_class(strtolower($item['label'])); ?>
      <li class="wp-admin-menu__item wp-admin-menu__item--<?php print $key; ?><?php print $item['current'] ? ' wp-admin-menu__item--current' : ''; ?><?php print $item['children'] ? ' wp-admin-menu__item--has-children' : ''; ?>">
        <a class="wp-admin-menu__link" href="<?php print check_plain($item['href']); ?>">
          <span class="wp-admin-menu__icon" aria-hidden="true"><?php print $item['icon']; ?></span>
          <span class="wp-admin-menu__label"><?php print check_plain($item['label']); ?></span>
        </a>
        <?php if ($item['children']): ?>
          <ul class="wp-admin-submenu">
            <?php foreach ($item['children'] as $child): ?>
              <li class="wp-admin-submenu__item<?php print $child['current'] ? ' wp-admin-submenu__item--current' : ''; ?>">
                <a href="<?php print check_plain($child['href']); ?>"><?php print check_plain($child['label']); ?></a>
              </li>
            <?php endforeach; ?>
          </ul>
        <?php endif; ?>
      </li>
    <?php endforeach; ?>
  </ul>
</nav>
