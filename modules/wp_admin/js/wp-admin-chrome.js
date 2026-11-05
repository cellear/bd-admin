/**
 * @file
 * Mobile toggle for the WordPress-style sidebar. Nothing else needs script.
 */
(function () {
  'use strict';
  document.addEventListener('click', function (event) {
    var burger = event.target.closest('.wp-admin-bar__burger');
    if (!burger) {
      return;
    }
    var open = document.documentElement.classList.toggle('wp-admin-sidebar-open');
    burger.setAttribute('aria-expanded', open ? 'true' : 'false');
  });
})();
