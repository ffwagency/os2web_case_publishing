<?php
/**
 * @file
 * os2web_case_publishing.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function os2web_case_publishing_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function os2web_case_publishing_node_info() {
  $items = array(
    'os2web_case_publishing_case' => array(
      'name' => t('Case Publishing: Case'),
      'base' => 'node_content',
      'description' => t('Case published manually or imported from an ESDH system.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'os2web_case_publishing_document' => array(
      'name' => t('Case Publishing: Document'),
      'base' => 'node_content',
      'description' => t('Documents connected to a case published manually or imported from an ESDH system.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
