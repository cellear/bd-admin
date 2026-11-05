<?php
/**
 * @file
 * The WordPress-style admin bar.
 *
 * Variables: $logo_menu (title, href), $site (name, href, links), $comments
 * (count, href, label) or NULL, $new (title, href), $account (name, initial, links).
 */
?>
<div id="wp-admin-bar" class="wp-admin-bar" role="navigation" aria-label="<?php print t('Admin bar'); ?>">
  <button class="wp-admin-bar__burger" type="button" aria-controls="wp-admin-sidebar" aria-expanded="false" aria-label="<?php print t('Menu'); ?>">&#9776;</button>
  <ul class="wp-admin-bar__menu">
    <li class="wp-admin-bar__item wp-admin-bar__item--logo wp-admin-bar__item--has-menu">
      <a class="wp-admin-bar__link" href="<?php print check_plain($logo_menu[0]['href']); ?>" aria-label="<?php print t('About Backdrop'); ?>"><span class="wp-admin-bar__logo" aria-hidden="true">B</span></a>
      <ul class="wp-admin-bar__submenu">
        <?php foreach ($logo_menu as $link): ?><li><a href="<?php print check_plain($link['href']); ?>"><?php print check_plain($link['title']); ?></a></li><?php endforeach; ?>
      </ul>
    </li>
    <li class="wp-admin-bar__item wp-admin-bar__item--site wp-admin-bar__item--has-menu">
      <a class="wp-admin-bar__link" href="<?php print check_plain($site['href']); ?>"><span class="wp-admin-bar__icon" aria-hidden="true">&#8962;</span><?php print check_plain($site['name']); ?></a>
      <ul class="wp-admin-bar__submenu">
        <?php foreach ($site['links'] as $link): ?><li><a href="<?php print check_plain($link['href']); ?>"><?php print check_plain($link['title']); ?></a></li><?php endforeach; ?>
      </ul>
    </li>
    <?php if ($comments): ?>
      <li class="wp-admin-bar__item wp-admin-bar__item--comments">
        <a class="wp-admin-bar__link" href="<?php print check_plain($comments['href']); ?>" title="<?php print check_plain($comments['label']); ?>"><span class="wp-admin-bar__icon" aria-hidden="true">&#128488;</span><span class="wp-admin-bar__count<?php print $comments['count'] ? ' wp-admin-bar__count--pending' : ''; ?>"><?php print $comments['count']; ?></span><span class="element-invisible"><?php print check_plain($comments['label']); ?></span></a>
      </li>
    <?php endif; ?>
    <?php if ($new): ?>
      <li class="wp-admin-bar__item wp-admin-bar__item--new wp-admin-bar__item--has-menu">
        <a class="wp-admin-bar__link" href="<?php print check_plain($new[0]['href']); ?>"><span class="wp-admin-bar__icon" aria-hidden="true">+</span><?php print t('New'); ?></a>
        <ul class="wp-admin-bar__submenu">
          <?php foreach ($new as $link): ?><li><a href="<?php print check_plain($link['href']); ?>"><?php print check_plain($link['title']); ?></a></li><?php endforeach; ?>
        </ul>
      </li>
    <?php endif; ?>
  </ul>
  <ul class="wp-admin-bar__menu wp-admin-bar__menu--secondary">
    <li class="wp-admin-bar__item wp-admin-bar__item--account wp-admin-bar__item--has-menu">
      <a class="wp-admin-bar__link" href="<?php print check_plain($account['links'][0]['href']); ?>"><?php print t('Howdy, @name', array('@name' => $account['name'])); ?><span class="wp-admin-bar__avatar" aria-hidden="true"><?php print check_plain($account['initial']); ?></span></a>
      <ul class="wp-admin-bar__submenu wp-admin-bar__submenu--right">
        <?php foreach ($account['links'] as $link): ?><li><a href="<?php print check_plain($link['href']); ?>"><?php print check_plain($link['title']); ?></a></li><?php endforeach; ?>
      </ul>
    </li>
  </ul>
</div>
