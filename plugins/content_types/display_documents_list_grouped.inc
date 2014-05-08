<?php

/**
 * @file
 * Pane rendering the document files referenced on a node grouped by case.
 */

$plugin = array(
  'single' => TRUE,
  'title' => t('Display documents list grouped by case'),
  'description' => t('Pane displaying list of documents rendered to a node grouped by case'),
  'category' => t('Case publishing'),
  'edit form' => 'os2web_case_publishing_display_documents_list_grouped_pane_edit_form',
  'render callback' => 'os2web_case_publishing_display_documents_list_grouped_pane_render',
  'admin info' => 's2web_case_publishing_display_documents_list_grouped_pane_admin_info',
  'defaults' => array(
    'fields' => array(),
    'link_target' => '_self',
  ),
  'all contexts' => TRUE,
);

/**
 * An edit form for the pane's settings.
 */
function os2web_case_publishing_display_documents_list_grouped_pane_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $fields = field_info_field_map();
  $found = array();
  foreach ($fields as $key => $field) {
    if ($field['type'] == 'entityreference') {
      $info = field_info_field($key);
      if ($key != 'field_os2web_case_documents') {
        foreach ($info['settings']['handler_settings']['target_bundles'] as $type => $checked) {
          if (!empty($checked)) {
            if (in_array($type, array('os2web_case_publishing_case', 'os2web_case_publishing_document'))) {
              $found[] = $key;
            }
          }
        }
      }
    }
  }

  $form['fields'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Fields'),
    '#description' => t('The pane will take referenced cases and documents from the checked fields and render a list of files. Notice that if the checked fields does not exists in the node context when rendering the pane, will the fields be skipped!'),
    '#options' => drupal_map_assoc($found),
    '#default_value' => $conf['fields'],
  );
  $form['link_target'] = array(
    '#type' => 'select',
    '#title' => t('Link target'),
    '#description' => t('The target attribute used on the file links.'),
    '#options' => drupal_map_assoc(array('_self', '_blank', '_top', '_parent')),
    '#default_value' => $conf['link_target'],
  );

  return $form;
}

/**
 * Submit function, note anything in the formstate[conf] automatically gets saved
 * Notice, the magic that automatically does that for you.
 */
function os2web_case_publishing_display_documents_list_grouped_pane_edit_form_submit(&$form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

/**
 * 'admin info' callback for panel pane.
 */
function s2web_case_publishing_display_documents_list_grouped_pane_admin_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    $block->content = t('Fields: %fields', array('%fields' => implode(', ', $conf['fields'])));
    return $block;
  }
}

/**
 * Run-time rendering of the body of the block (content type)
 * See ctools_plugin_examples for more advanced info
 */
function os2web_case_publishing_display_documents_list_grouped_pane_render($subtype, $conf, $args, $contexts) {
  if (!empty($contexts)) {
    $context = current($contexts);
    if (in_array('node', $context->type)) {
      $fields = $conf['fields'];

      // Finding nid of all referenced cases and documents
      $nids = array();
      foreach ($fields as $field_key => $field_value) {
        if (!empty($field_value)) {
          $node = (array) $context->data;
          if (isset($node[$field_key]) && is_array(current($node[$field_key]))) {
            foreach (current($node[$field_key]) as $nid) {
              $nids[$nid['target_id']] = (int) $nid['target_id'];
            }
          }
        }
      }

      // Finding all files on the found nid's
      $files = array();
      $nodes = node_load_multiple($nids);
      foreach ($nodes as $node) {
        if (node_access('view', $node)) {

          // If the node is a case will we need to find all the documents
          // under the case.
          if ($node->type == 'os2web_case_publishing_case') {
            $documents = current($node->field_os2web_case_documents);
            if (is_array($documents)) {
              $dnids = array();
              foreach ($documents as $document) {
                $dnids[$document['target_id']] = (int) $document['target_id'];
              }
              $dnodes = node_load_multiple($dnids);

              // Extracting the files
              foreach ($dnodes as $dnode) {
                if (node_access('view', $dnode)) {
                  if (isset($dnode->field_os2web_case_document_file)) {
                    $file = current($dnode->field_os2web_case_document_file);
                    if (is_array($file)) {
                      $file = current($file);
                      $tmp = current($dnode->field_os2web_case_document_name);
                      $file['description'] = $tmp[0]['safe_value'];
                      $case_title = current($node->field_os2web_case_case_title);
                      $case_title = $case_title[0]['value'];
                      $files[$case_title][$file['fid']] = $file;
                    }
                  }
                }
              }
            }
          }
          else {
            if (isset($node->field_os2web_case_document_file)) {
              $file = current($node->field_os2web_case_document_file);
              if (is_array($file)) {
                $file = current($file);
                $tmp = current($node->field_os2web_case_document_name);
                $file['description'] = $tmp[0]['safe_value'];
                //Get the case title from database
                $query = db_select('node', 'n');
                $query->leftJoin('field_data_field_os2web_case_documents', 'd', 'n.nid=d.entity_id');
                $query->leftJoin('field_data_field_os2web_case_case_title', 't', 'n.nid=t.entity_id');
                $case = $query->condition('d.field_os2web_case_documents_target_id', $node->nid)
                  ->condition('n.type', 'os2web_case_publishing_case')
                  ->fields('t', array('field_os2web_case_case_title_value'))
                  ->execute()->fetchField();

                $files[$case][$file['fid']] = $file;
              }
            }
          }
        }
      }

      // The returned content block
      $block = new stdClass();
      $block->title = t('Documents');
      $variables = array(
        'files' => $files,
        'options' => array(
          'target' => $conf['link_target'],
        ),
      );
      $block->content = theme('os2web_case_publishing_documents_list_grouped', $variables);
      return $block;
    }
  }

  return FALSE;
}