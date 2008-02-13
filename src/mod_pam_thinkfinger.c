/* Copyright (C) 2007, 2008 Sven Schober
   Author: Sven Schober <sschober@suse.de>

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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>

#include "pam-config.h"
#include "pam-module.h"

extern char *confdir;

/**
 * @brief A filter for use with scandir. Filters out dot,
 * common* and files containing a dot.
 */
static int
service_filter (const struct dirent *dentry __attribute__((unused)))
{
  /* skip '.' and '..' */
  if (dentry->d_name[0] == '.') return 0;
  /* skip files starting with 'common' */
  if (strncmp (dentry->d_name, "common",6) == 0) return 0;
  /* skip all files containing dots (catches .old, .tmp, etc.)*/
  if (strstr (dentry->d_name, ".") != NULL) return 0;
  return 1;
}

/**
 * @brief Load the config file and search for occurence of
 * module_name.
 */
static int
check_file_for_module (const char *file_name, const char *module_name)
{
  config_content_t *cfg_content;
  load_single_config (file_name, &cfg_content);
  while (cfg_content != NULL)
  {
    if (strcasestr (cfg_content->line, module_name) != NULL)
      return TRUE;
    cfg_content = cfg_content->next;
  }
  return FALSE;
}

/**
 * @brief Skims through all single service files and searches for
 * module.
 *
 * It first scans through pam.d-dir and applies a filter to filter
 * out all non-single service files. Then it checks each file for
 * any occurrence of the string module.
 *
 * This could be done differently: by using load_config together
 * with the service_module_list.
 *
 * I wanted to avoid cluttering the global list, so i chose the
 * manual way.
 */
static int
check_service_files_for_module (const char *module)
{
  struct dirent **namelist;
  int n, found = FALSE;

  char *conf_dname;
  if (confdir)
  {
    if (asprintf( &conf_dname, "%s/pam.d", confdir) < 0)
    {
      fprintf (stderr, _("ERROR: No memory left to construct path.\n"));
      /* if we couldn't check assume the worst (pam_mount.so is
       * enabled).
       */
      return TRUE;
    }
  }
  else conf_dname = CONFDIR"/pam.d";

  if (debug){
    DEBUG ("**** check_service_files_for_module ('%s') in '%s'\n", module, conf_dname);
  }

  n = scandir (conf_dname, &namelist, &service_filter, 0);
  if (n<0)
    fprintf (stderr, _("WARNING: Found no service files in '%s'.\n"), conf_dname);
  else
  {
    /* Iterate over every service file and check it for module
     */
    while (n--)
    {
      if (check_file_for_module (namelist[n]->d_name, module) == TRUE)
	{
	  fprintf (stderr,
		   _("WARNING: Found module '%s' in file '%s'.\n"), module, namelist[n]->d_name);
	  found = TRUE;
	}
      free (namelist[n]);
    }
    free (namelist);
  }
  return found;
}

static int
write_config_thinkfinger (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_mount;

  if (debug)
    printf ("**** write_config_thinkfinger (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_mount = check_service_files_for_module ("pam_mount.so");

  if (with_mount){
    fprintf (stderr, _("ERROR: pam_mount.so is enabled. In order to use pam_thinkfinger.so you need to disable it first!\n"));
    return 1;
  }

  switch (op)
  {
    case AUTH:
	fprintf (fp, "auth\tsufficient\tpam_thinkfinger.so\t");
    default:
      break;
  }

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_1(AUTH)
GETOPT_END_1(AUTH)

PRINT_ARGS("thinkfinger")

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_2( is_enabled, debug );
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_thinkfinger = { "pam_thinkfinger.so", opt_sets,
				     &def_parse_config,
				     &def_print_module,
				     &write_config_thinkfinger,
				     &get_opt_set,
				     &getopt,
				     &print_args};
