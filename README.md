OS2Web Case Publishing
==============================

Description
-----------
This feature creates the frontend foundation for publishing cases and
documents in Drupal from an ESDH system. 

The feature creates two content types. One for cases and one for documents - 
with an entity reference between them.

It also contains a custom pane rendering the files from cases and documents 
referenced to the node in context. The field should use entity reference
to make it work. It's possible in the pane settings to control which entity 
reference fields it should render the files from

Dependencies
------------
- ctools
- date
- entityreference
- features
- file
- number
- strongarm
- text

Installation
------------
This module should reside in the modules directory of the installation,
most commonly profiles/os2web/modules/, but alternativly in sites/all/modules
(This could be for development purposes).

See https://github.com/OS2web/os2web/wiki for further instructions.

This module can also be installed with drush make in your install profile.

Additional Info
---------------
This repository should be governed using Git Flow. for more information see
http://nvie.com/posts/a-successful-git-branching-model/
