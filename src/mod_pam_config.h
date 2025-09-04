#pragma once
#include "pam-module.h"

/* Load configurable modules (NULL-terminated list) from INI files.
 * Search order: /etc/pam-config.d, /usr/local/share/pam-config.d, /usr/share/pam-config.d
 * Returns number of modules (excl. final NULL), or -1 on error. */
int load_configurable_modules(pam_module_t ***out_list);

/* Free the list returned by load_configurable_modules() */
void free_configurable_modules(pam_module_t **list);
