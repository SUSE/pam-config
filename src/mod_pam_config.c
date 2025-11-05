/* Copyright (C) 2025 SUSE LLC
   Author: David Mulder <dmulder@suse.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <dirent.h>
#include <libeconf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mod_pam_config.h"
#include "pam-config.h"
#include "pam-module.h"
#include <linux/limits.h>

/* --- generic write_config that uses the per-instance spec ----------- */

static int write_config_configurable(pam_module_t *pm, enum write_type op,
                                     FILE *fp) {
  configurable_module_t *sp = pm->config;
  pam_module_t *ov = sp->fallback;
  option_set_t *opt_set = pm->get_opt_set(pm, op);

  if (debug) {
    debug_write_call(pm, op);
  }

  if (!opt_set->is_enabled(opt_set, "is_enabled")) {
    return 0;
  }

  const char *line = NULL;
  switch (op) {
  case AUTH:
    line = sp->auth_line;
    break;
  case ACCOUNT:
    line = sp->account_line;
    break;
  case PASSWORD:
    line = sp->password_line;
    break;
  case SESSION:
    line = sp->session_line;
    break;
  }
  if (!line) {
    if (ov) {
      return ov->write_config(ov, op, fp);
    }
    return 0; /* nothing local and no fallback */
  }

  /* Each stored line already contains:
   * "<stack>\t<control>\t<soname>\t<args...>" */
  fputs(line, fp);
  fputc('\t', fp);

  WRITE_CONFIG_OPTIONS /* append options, consistent with built-ins */

  return 0;
}

GETOPT_START_ALL
GETOPT_END_ALL

static void print_args(pam_module_t *this) {
  option_set_t *opt_set = this->get_opt_set(this, AUTH);
  bool_option_t **cur_bool_opt = opt_set->bool_opts;
  string_option_t **cur_str_opt = opt_set->string_opts;

  printf("   --%s\n", this->config->modname);

  while (*cur_bool_opt != NULL) {
    if (strcmp((*cur_bool_opt)->key, "is_enabled") != 0) {
      printf("   --%s-%s\n", this->config->modname, (*cur_bool_opt)->key);
    }
    cur_bool_opt++;
  }

  while (*cur_str_opt != NULL) {
    if (strcmp((*cur_str_opt)->key, "empty") != 0) {
      printf("   --%s-%s=<value>\n", this->config->modname,
             (*cur_str_opt)->key);
    }
    cur_str_opt++;
  }
}

/* --- options & plumbing ------------------------------------------- */

static bool_option_t** generate_bool_opts(void) {
  bool_option_t *bool_opt_debug = malloc(sizeof(bool_option_t));
  bool_opt_debug->key = "debug";
  bool_opt_debug->value = 0;
  bool_option_t *bool_opt_is_enabled = malloc(sizeof(bool_option_t));
  bool_opt_is_enabled->key = "is_enabled";
  bool_opt_is_enabled->value = 0;

  bool_option_t **bool_opts = calloc(3, sizeof(bool_option_t*));
  bool_opts[0] = bool_opt_debug;
  bool_opts[1] = bool_opt_is_enabled;
  bool_opts[2] = NULL;

  return bool_opts;
}

static option_set_t **generate_opt_sets(void) {
  string_option_t *str_opt_empty = malloc(sizeof(string_option_t));
  str_opt_empty->key = "empty";
  str_opt_empty->value = NULL;

  string_option_t **str_opts = calloc(2, sizeof(string_option_t*));
  str_opts[0] = str_opt_empty;
  str_opts[1] = NULL;

  option_set_t *auth_opts = malloc(sizeof(option_set_t));
  auth_opts->bool_opts = generate_bool_opts();
  auth_opts->string_opts = str_opts;
  auth_opts->is_enabled = is_enabled;
  auth_opts->enable = enable;
  auth_opts->get_opt = get_opt;
  auth_opts->set_opt = set_opt;
  option_set_t *account_opts = malloc(sizeof(option_set_t));
  account_opts->bool_opts = generate_bool_opts();
  account_opts->string_opts = str_opts;
  account_opts->is_enabled = is_enabled;
  account_opts->enable = enable;
  account_opts->get_opt = get_opt;
  account_opts->set_opt = set_opt;
  option_set_t *password_opts = malloc(sizeof(option_set_t));
  password_opts->bool_opts = generate_bool_opts();
  password_opts->string_opts = str_opts;
  password_opts->is_enabled = is_enabled;
  password_opts->enable = enable;
  password_opts->get_opt = get_opt;
  password_opts->set_opt = set_opt;
  option_set_t *session_opts = malloc(sizeof(option_set_t));
  session_opts->bool_opts = generate_bool_opts();
  session_opts->string_opts = str_opts;
  session_opts->is_enabled = is_enabled;
  session_opts->enable = enable;
  session_opts->get_opt = get_opt;
  session_opts->set_opt = set_opt;

  option_set_t **opt_sets = calloc(5, sizeof(option_set_t*));
  opt_sets[0] = auth_opts;
  opt_sets[1] = account_opts;
  opt_sets[2] = password_opts;
  opt_sets[3] = session_opts;
  opt_sets[4] = NULL;

  return opt_sets;
}

static void free_opt_sets(option_set_t **sets)
{
    if (!sets) return;

    // Free the 4 option_set_t structs while collecting the shared str_opts
    string_option_t **shared_str = NULL;

    for (size_t i = 0; i < 4 && sets[i]; i++) {
        option_set_t *os = sets[i];

        // Free per-stack bool opts (entries then the array)
        if (os->bool_opts) {
            for (size_t j = 0; os->bool_opts[j]; j++) {
                // keys are string literals ("debug","is_enabled") -> don't free key
                free(os->bool_opts[j]);
            }
            free(os->bool_opts);
        }

        // Remember the shared string_opts pointer (same for all four)
        if (!shared_str && os->string_opts) {
            shared_str = os->string_opts;
        }

        free(os);
    }

    // Free the shared string opts once
    if (shared_str) {
        for (size_t j = 0; shared_str[j]; j++) {
            // key is a string literal "empty" -> don't free key
            // value is NULL -> nothing to free there
            free(shared_str[j]);
        }
        free(shared_str);
    }

    free(sets);
}

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* --- parser -------------------------------------------------------- */

static int free_pam_module(pam_module_t *m) {
  if (!m) {
    return -1;
  }

  if (m->config) {
    free(m->name);
    configurable_module_t *sp = m->config;
    if (sp) {
      free_pam_module(sp->fallback);
      sp->fallback = NULL;
      configurable_module_free(sp);
    }
    free_opt_sets(m->option_sets);
    free(m);
  }
  return 0;
}

static inline bool
econf_key_is_missing(econf_err rc)
{
    switch (rc) {
    case ECONF_NOKEY:
    case ECONF_EMPTYKEY:
    case ECONF_KEY_HAS_NULL_VALUE:
        return true;
    default:
        return false;
    }
}

/* Treat only [A-Za-z0-9_.-] as valid characters, replace others with '-' */
static inline char sanitize_char(char c)
{
    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        c == '_' || c == '.' || c == '-')
    {
        return c;
    }
    return '-';
}

static char *pam_mod_basename(const char *soname) {
  if (!soname || !*soname)
    return NULL;

  const char *start = soname;
  const char *end = soname + strlen(soname);

  /* strip leading "pam_" if present */
  if (strncmp(start, "pam_", 4) == 0) {
    start += 4;
  }

  /* strip trailing ".so" if present */
  if (end - start > 3 && strcmp(end - 3, ".so") == 0) {
    end -= 3;
  }

  /* compute length and allocate */
  size_t len = (size_t)(end - start);
  if (len == 0) {
    return NULL;
  }

  char *out = malloc(len + 1);
  if (!out) {
    return NULL;
  }

  for (size_t i = 0; i < len; i++) {
    out[i] = sanitize_char(start[i]);
  }
  out[len] = '\0';
  return out;
}

/* ---- config format --------------------------------------------------
 * Minimal, INI "key: value" per line. Empty lines and '#' comments ok.
 * Keys (case-insensitive):
 *   [pam_example.so]
 *   Auth-Priority: <int>                   (optional; default -1)
 *   Account-Priority: <int>                (optional; default -1)
 *   Password-Priority: <int>               (optional; default -1)
 *   Session-Priority: <int>                (optional; default -1)
 *
 * Stack lines (optional; if present, they drive write_config output):
 *   Auth:     <stack>\t<control>\t<soname>\t<args...>
 *   Account:  <stack>\t<control>\t<soname>\t<args...>
 *   Password: <stack>\t<control>\t<soname>\t<args...>
 *   Session:  <stack>\t<control>\t<soname>\t<args...>
 *
 * Example:
 *   [pam_himmelblau.so]
 *   Auth:     auth\tsufficient\tpam_himmelblau.so\tignore_unknown_user
 *   Auth-Priority: 800
 *   Account:  account\trequired\tpam_himmelblau.so\tignore_unknown_user
 *   Account-Priority: 300
 *   Password: password\tsufficient\tpam_himmelblau.so\tignore_unknown_user
 *   Password-Priority: 100
 *   Session:  session\toptional\tpam_himmelblau.so
 *   Session-Priority: 2300
 * ------------------------------------------------------------------- */
static int parse_config_modules(const char *path, pam_module_t ***out_list, size_t *out_len) {
  if (!out_list || !out_len) {
    return -1;
  }
  *out_list = NULL;
  *out_len = 0;

  econf_file *kf = NULL;
  econf_err rc = econf_readFile(&kf, path, ":", "#");
  if (rc != ECONF_SUCCESS) {
    return -1;
  }

  char **modules = NULL;
  size_t modules_len = 0;
  rc = econf_getGroups(kf, &modules_len, &modules);
  if (rc != ECONF_SUCCESS) {
    econf_freeFile(kf);
    return -1;
  }

  pam_module_t **ret = calloc(modules_len+1, sizeof(pam_module_t*));
  if (!ret) {
    econf_freeFile(kf);
    return -1;
  }
  memset(ret, 0, sizeof(pam_module_t*)*(modules_len+1));

  for (size_t i = 0; i < modules_len; ++i) {
    const char *section = modules[i];

    pam_module_t *pm = malloc(sizeof(pam_module_t));
    if (!pm) {
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }
    memset(pm, 0, sizeof(pam_module_t));

    pm->config = malloc(sizeof(configurable_module_t));
    if (!pm->config) {
      free(pm);
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }
    memset(pm->config, 0, sizeof(configurable_module_t));

    pm->name = strdup(section);
    pm->config->modname = pam_mod_basename(section);
    if (!pm->config->modname) {
      free(pm->name);
      free(pm);
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }

    rc = econf_getStringValue(kf, section, "Auth", &pm->config->auth_line);
    if (rc != ECONF_SUCCESS && !econf_key_is_missing(rc)) {
      free_pam_module(pm);
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }
    rc = econf_getIntValue(kf, section, "Auth-Priority", &pm->priority_auth);
    if (econf_key_is_missing(rc)) {
      pm->priority_auth = -1;
    } else if (rc != ECONF_SUCCESS) {
      free_pam_module(pm);
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }

    rc = econf_getStringValue(kf, section, "Account", &pm->config->account_line);
    if (rc != ECONF_SUCCESS && !econf_key_is_missing(rc)) {
      free_pam_module(pm);
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }
    rc = econf_getIntValue(kf, section, "Account-Priority", &pm->priority_account);
    if (econf_key_is_missing(rc)) {
      pm->priority_account = -1;
    } else if (rc != ECONF_SUCCESS) {
      free_pam_module(pm);
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }

    rc = econf_getStringValue(kf, section, "Password", &pm->config->password_line);
    if (rc != ECONF_SUCCESS && !econf_key_is_missing(rc)) {
      free_pam_module(pm);
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }
    rc = econf_getIntValue(kf, section, "Password-Priority", &pm->priority_password);
    if (econf_key_is_missing(rc)) {
      pm->priority_password = -1;
    } else if (rc != ECONF_SUCCESS) {
      free_pam_module(pm);
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }

    rc = econf_getStringValue(kf, section, "Session", &pm->config->session_line);
    if (rc != ECONF_SUCCESS && !econf_key_is_missing(rc)) {
      free_pam_module(pm);
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }
    rc = econf_getIntValue(kf, section, "Session-Priority", &pm->priority_session);
    if (econf_key_is_missing(rc)) {
      pm->priority_session = -1;
    } else if (rc != ECONF_SUCCESS) {
      free_pam_module(pm);
      econf_freeFile(kf);
      free_configurable_modules(ret);
      return -1;
    }
    econf_freeFile(kf);

    /* Fill in the variables and helper functions */
    pm->option_sets = generate_opt_sets();
    pm->helptxt = helptext;
    pm->parse_config = def_parse_config;
    pm->print_module = def_print_module;
    pm->write_config = write_config_configurable;
    pm->get_opt_set = get_opt_set;
    pm->getopt = getopt;
    pm->print_args = print_args;

    ret[i] = pm;
  }

  *out_list = ret;
  *out_len = modules_len;
  econf_freeArray(modules);
  return 0;
}

/* --- directory scan & loader -------------------------------------- */

static int is_conf(const char *name) {
  size_t n = strlen(name);
  return n >= 6 && strcmp(name + (n - 5), ".conf") == 0;
}

static int replace_or_append_configurable(pam_module_t ***list, size_t *n, pam_module_t **modules, size_t count)
{
  for (size_t i = 0; i < count; ++i) {
    pam_module_t *pm = modules[i];
    if (!pm) {
      continue;
    }

    /* replace existing module with same soname */
    for (size_t j = 0; j < *n; ++j) {
      pam_module_t *old = (*list)[j];
      if (old && old->name && pm->name && strcasecmp(old->name, pm->name) == 0) {
        /* Keep the old module for items not overridden. */
        pm->config->fallback = old;

        /* Restore module priority if necessary. */
        if (pm->priority_account == -1) {
          pm->priority_account = old->priority_account;
        }
        if (pm->priority_auth == -1) {
          pm->priority_auth = old->priority_auth;
        }
        if (pm->priority_password == -1) {
          pm->priority_password = old->priority_password;
        }
        if (pm->priority_session == -1) {
          pm->priority_session = old->priority_session;
        }

        (*list)[j] = pm;
        return 0;
      }
    }

    /* append if not found */
    pam_module_t **tmp = realloc(*list, (*n + 2) * sizeof(pam_module_t*));
    if (!tmp) {
      return -1;
    }
    tmp[*n] = pm;
    tmp[*n + 1] = NULL;
    *list = tmp;
    (*n)++;
  }

  return 0;
}

static int load_dir(const char *dir, pam_module_t ***out, size_t *count) {
  DIR *d = opendir(dir);
  if (!d) {
    return -1;
  }

  struct dirent *de;
  int rc = 0;

  while ((de = readdir(d))) {
    if (de->d_name[0] == '.') {
      continue;
    }
    if (!is_conf(de->d_name)) {
      continue;
    }

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", dir, de->d_name);

    pam_module_t **modules = NULL;
    size_t n = 0;
    if (parse_config_modules(path, &modules, &n) != 0) {
      continue; /* skip bad files, keep going */
    }
    if (replace_or_append_configurable(out, count, modules, n) != 0) {
      free_configurable_modules(modules);
      rc = -1;
      break;
    }
  }

  closedir(d);
  return rc;
}

int load_configurable_modules(pam_module_t ***out_list) {
  if (!out_list) {
    return -1;
  }
  *out_list = NULL;
  size_t n = 0;

  /* lowest precedence first */
  (void)load_dir("/usr/lib/pam-config.d",        out_list, &n);
  (void)load_dir("/usr/local/lib/pam-config.d",  out_list, &n);
  (void)load_dir("/etc/pam-config.d",            out_list, &n);

  return (int)n;
}

void free_configurable_modules(pam_module_t **list) {
  if (!list) {
    return;
  }

  for (size_t i = 0; list[i]; ++i) {
    pam_module_t *m = list[i];
    free_pam_module(m);
  }
  free(list);
}
