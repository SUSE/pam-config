/* Copyright (C) 2022 Thorsten Kukuk
   Author: Thorsten Kukuk <kukuk@suse.com>

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

static void
write_config_internal (FILE *fp, option_set_t *opt_set)
{
  fprintf (fp, "auth     required\tpam_u2f.so\t");
  WRITE_CONFIG_OPTIONS
}


static int
write_config_u2f (pam_module_t *this,
		  enum write_type op __attribute__ ((unused)),
		  FILE *unused __attribute__((unused)))
{
  option_set_t *opt_set = this->get_opt_set (this, AUTH);
  int is_written = 0;
  FILE *fp;
  config_content_t *cfg_content;
  int writeit = opt_set->is_enabled (opt_set, "is_enabled");

  if (debug)
    debug_write_call (this, AUTH);

  load_single_config (gl_service, &cfg_content);

  fp = create_service_file (gl_service);
  if (!fp)
    return 0;

  while (cfg_content != NULL)
  {
    if (writeit)
      {
	/* skip old entries */
	if (strcasestr (cfg_content->line, "pam_u2f.so") == NULL )
	  fprintf (fp, "%s", cfg_content->line);

	if (!is_written)
	  {
	    /* make sure pam_u2f.so get's written to the service file
	     * after the include common-auth line.
	     */
	    if (strcasestr (cfg_content->line, "common-auth") != NULL)
	      {
		write_config_internal (fp, opt_set);
		is_written = 1;
	      }
	  }
      }
    else
      {
	/* skip old entries */
	if (strcasestr (cfg_content->line, "pam_u2f.so") == NULL)
	  fprintf (fp, "%s", cfg_content->line);
      }
    cfg_content = cfg_content->next;
  }
  return close_service_file (fp,gl_service);
}

GETOPT_START_1(AUTH)
GETOPT_END_1(AUTH)

PRINT_ARGS("u2f")
PRINT_XMLHELP("u2f")

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_10( is_enabled, debug, nouserok, openasuser, alwaysok,
		      interactive, manual, cue, nodetect, sshformat);
DECLARE_STRING_OPTS_11 (debug_file, origin, appid, authfile, authpending_file,
			max_devices, prompt, cue_prompt, userpresence,
			userverification, pinverification);
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_u2f = { "pam_u2f.so", opt_sets, helptext,
			     &def_parse_config,
			     &def_print_module,
			     &write_config_u2f,
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
