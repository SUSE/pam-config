/* Copyright (C) 2017 SUSE Linux GmbH
   Author: Josef Möllers <jmoellers@suse.de>

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

#include "pam-config.h"
#include "pam-module.h"

static void write_entry(FILE *fp, option_set_t *opt_set);

static int
write_config_keyinit (pam_module_t *this,
		      enum write_type op __attribute__ ((unused)),
		      FILE *unused __attribute__((unused)))
{
  option_set_t *opt_set = this->get_opt_set (this, SESSION);
  FILE *fp;
  config_content_t *cfg_content;
  int writeit = opt_set->is_enabled (opt_set, "is_enabled");
  int is_written = 0;

  if (debug)
    debug_write_call (this, SESSION);

  load_single_config (gl_service, &cfg_content);

  fp = create_service_file (gl_service);
  if (!fp) return 0;

  while (cfg_content != NULL)
  {
    if (writeit)
    {
      if (!is_written)
      {
	/* write this entry as the first in the session part */
	if (strstr(cfg_content->line, "session") != NULL)
	{
	  write_entry(fp, opt_set);
	  is_written = 1;
	}
      }
      /* skip old entries */
      if (strcasestr (cfg_content->line, "pam_keyinit.so") == NULL )
	fprintf (fp, "%s", cfg_content->line);
    }
    else
    {
      /* skip old entries */
      if (strcasestr (cfg_content->line, "pam_keyinit.so") == NULL)
	fprintf (fp, "%s", cfg_content->line);
      else
	is_written = 1;
    }
    cfg_content = cfg_content->next;
  }

  /*
   * If it has not been written yet, write it now
   * This is highly unlikely as most config files include common-session
   * but then ...
   */
  if (!is_written)
  {
    write_entry(fp, opt_set);
    is_written = 1;
  }

  return close_service_file (fp,gl_service);
}

static void
write_entry(FILE *fp, option_set_t *opt_set)
{
  fprintf (fp, "session  optional\tpam_keyinit.so revoke ");
  if (opt_set->is_enabled (opt_set, "force"))
    fprintf (fp, "force ");
  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, "debug ");

  fprintf (fp, "\n");

  return;
}

GETOPT_START_ALL
  else if (strcmp ("force", opt) == 0)
    {
      opt_set = this->get_opt_set (this, SESSION);
      opt_set->enable (opt_set, "force", g_opt->opt_val);
    }
GETOPT_END_ALL

PRINT_ARGS("keyinit")
PRINT_XMLHELP("keyinit")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_4 (is_enabled, debug, force, revoke);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_keyinit = { "pam_keyinit.so", opt_sets, helptext,
			      &def_parse_config,
			      &def_print_module,
			      &write_config_keyinit,
			      &get_opt_set,
			      &getopt,
			      &print_args,
			      &print_xmlhelp,
			      NULL,
			      -1,
			      -1,
			      -1,
			      -1
};
