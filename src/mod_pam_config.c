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
#include <ini.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mod_pam_config.h"
#include "pam-config.h"
#include "pam-module.h"
#include <linux/limits.h>

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
 * Minimal, INI-like "key: value" per line. Empty lines and '#' comments ok.
 * Keys (case-insensitive):
 *   Name: pam_example.so                   (required)
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
 *   Name: pam_himmelblau.so
 *   Auth:     auth\tsufficient\tpam_himmelblau.so\tignore_unknown_user
 *   Auth-Priority: 800
 *   Account:  account\trequired\tpam_himmelblau.so\tignore_unknown_user
 *   Account-Priority: 300
 *   Password: password\tsufficient\tpam_himmelblau.so\tignore_unknown_user
 *   Password-Priority: 100
 *   Session:  session\toptional\tpam_himmelblau.so
 *   Session-Priority: 2300
 * ------------------------------------------------------------------- */
static int parse_handler(void *in, const char *section, const char *key,
                         const char *value) {
  pam_module_t *m = (pam_module_t *)in;

  if (strcmp(section, "") != 0) {
    return 0; /* we ignore section names */
  }

  if (m->config == NULL) {
    m->config = malloc(sizeof(configurable_module_t));
    memset(m->config, 0, sizeof(configurable_module_t));
  }

  if (strcasecmp(key, "Name") == 0) {
    free(m->name);
    m->name = strdup(value);
    m->config->modname = pam_mod_basename(value);
  } else if (strcasecmp(key, "Auth") == 0) {
    free(m->config->auth_line);
    m->config->auth_line = strdup(value);
  } else if (strcasecmp(key, "Auth-Priority") == 0) {
    m->priority_auth = atoi(value);
  } else if (strcasecmp(key, "Account") == 0) {
    free(m->config->account_line);
    m->config->account_line = strdup(value);
  } else if (strcasecmp(key, "Account-Priority") == 0) {
    m->priority_account = atoi(value);
  } else if (strcasecmp(key, "Password") == 0) {
    free(m->config->password_line);
    m->config->password_line = strdup(value);
  } else if (strcasecmp(key, "Password-Priority") == 0) {
    m->priority_password = atoi(value);
  } else if (strcasecmp(key, "Session") == 0) {
    free(m->config->session_line);
    m->config->session_line = strdup(value);
  } else if (strcasecmp(key, "Session-Priority") == 0) {
    m->priority_session = atoi(value);
  } else {
    return 0;
  }

  return 1;
}

/* --- generic write_config that uses the per-instance spec ----------- */

static int write_config_configurable(pam_module_t *pm, enum write_type op,
                                     FILE *fp) {
  configurable_module_t *sp = pm->config;
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
    return 0; /* Nothing to emit for this stack */
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

DECLARE_BOOL_OPTS_2(is_enabled, debug);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* --- parser -------------------------------------------------------- */

static int parse_config_file(const char *path, pam_module_t **out_pm) {
  if (!out_pm) {
    return -1;
  }
  *out_pm = malloc(sizeof(pam_module_t));
  if (!*out_pm) {
    return -1;
  }
  memset(*out_pm, 0, sizeof(pam_module_t));

  pam_module_t *pm = *out_pm;
  pm->priority_auth = -1;
  pm->priority_account = -1;
  pm->priority_password = -1;
  pm->priority_session = -1;

  if (ini_parse(path, parse_handler, pm) < 0) {
    free(*out_pm);
    *out_pm = NULL;
    return -1;
  }

  /* Validate the result */
  if (!pm->name) {
    fprintf(stderr, "%s: Name (ex. 'pam_example.so') is required", path);
    free(pm);
    *out_pm = NULL;
    return -1;
  }
  if (!pm->config) {
    free(pm->name);
    free(pm);
    *out_pm = NULL;
    return -1;
  }
  if (!pm->config->modname) {
    free(pm->name);
    configurable_module_free(pm->config);
    free(pm);
    *out_pm = NULL;
    return -1;
  }
  if (pm->config->auth_line && pm->priority_auth < 0) {
    fprintf(stderr, "%s: Auth cannot be configured without Auth-Priority", path);
    free(pm->name);
    configurable_module_free(pm->config);
    free(pm);
    *out_pm = NULL;
    return -1;
  }
  if (pm->config->account_line && pm->priority_account < 0) {
    fprintf(stderr, "%s: Account cannot be configured without Account-Priority", path);
    free(pm->name);
    configurable_module_free(pm->config);
    free(pm);
    *out_pm = NULL;
    return -1;
  }
  if (pm->config->password_line && pm->priority_password < 0) {
    fprintf(stderr, "%s: Password cannot be configured without Password-Priority", path);
    free(pm->name);
    configurable_module_free(pm->config);
    free(pm);
    *out_pm = NULL;
    return -1;
  }
  if (pm->config->session_line && pm->priority_session < 0) {
    fprintf(stderr, "%s: Session cannot be configured without Session-Priority", path);
    free(pm->name);
    configurable_module_free(pm->config);
    free(pm);
    *out_pm = NULL;
    return -1;
  }

  /* Fill in the variables and helper functions */
  pm->option_sets = opt_sets;
  pm->helptxt = helptext;
  pm->parse_config = def_parse_config;
  pm->print_module = def_print_module;
  pm->write_config = write_config_configurable;
  pm->get_opt_set = get_opt_set;
  pm->getopt = getopt;
  pm->print_args = print_args;

  return 0;
}

/* --- directory scan & loader -------------------------------------- */

static int is_conf(const char *name) {
  size_t n = strlen(name);
  return n >= 6 && strcmp(name + (n - 5), ".conf") == 0;
}

static int append_module(pam_module_t ***list, size_t *n, pam_module_t *pm) {
  pam_module_t **tmp = realloc(*list, (*n + 2) * sizeof(pam_module_t*));
  if (!tmp) {
    return -1;
  }
  tmp[*n] = pm;
  tmp[*n + 1] = NULL;
  *list = tmp;
  (*n)++;
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

    pam_module_t *pm = NULL;
    if (parse_config_file(path, &pm) != 0) {
      continue; /* skip bad files, keep going */
    }
    if (!pm) {
      free(pm->name);
      configurable_module_free(pm->config);
      free(pm);
      continue;
    }
    if (append_module(out, count, pm) != 0) {
      rc = -1; /* out of memory; bail after closing */
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
    free(m->name);
    configurable_module_t *sp = m->config;
    if (sp) {
      configurable_module_free(sp);
    }
    free(m);
  }
  free(list);
}
