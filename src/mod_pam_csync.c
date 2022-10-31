/* Copyright (C) 2008 Thorsten Kukuk
   Author: Thorsten Kukuk <kukuk@thkukuk.de>

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
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "pam-config.h"
#include "pam-module.h"

static void
write_config_internal (FILE *fp, option_set_t *opt_set, const char *service)
{
  fprintf (fp, "%s  optional\tpam_csync.so\t", service);

  WRITE_CONFIG_OPTIONS
}

static int
write_config_csync (pam_module_t *this,
		    enum write_type op __attribute__((unused)),
		    FILE *unused __attribute__((unused)))
{
  option_set_t *opt_set = this->get_opt_set (this, SESSION);
  int is_written_session = 0, is_written_auth = 0;
  FILE *fp;
  config_content_t *ptr;
  int writeit = opt_set->is_enabled (opt_set, "is_enabled");

  if (debug)
    {
      debug_write_call (this, AUTH);
      debug_write_call (this, SESSION);
    }

  load_single_config (gl_service, &ptr);

  fp = create_service_file (gl_service);
  if( fp == NULL ) return 1;

  while (ptr != NULL)
    {
      if (writeit)
	{
          /* don't write old pam_csync.so line */
          if (strcasestr (ptr->line, "pam_csync.so") == NULL)
            fprintf (fp, "%s", ptr->line);

          if (!is_written_session)
            {
              if (strcasestr (ptr->line, "common-session") != NULL)
                {
		  write_config_internal (fp, opt_set, "session");
                  is_written_session = 1;
                }
            }
          if (!is_written_auth)
            {
              if (strcasestr (ptr->line, "common-auth") != NULL)
                {
		  write_config_internal (fp, opt_set, "auth");
                  is_written_auth = 1;
                }
            }
        }
      else
        {
          if (strcasestr (ptr->line, "pam_csync.so") == NULL)
            fprintf (fp, "%s", ptr->line);
          else
	    {
	      is_written_auth = 1;
	      is_written_session = 1;
	    }
        }

      ptr = ptr->next;
    }

  /* make sure we really write it if we have to add it. */
  if ((!is_written_auth || !is_written_session) && writeit)
    {
      write_config_internal (fp, opt_set, "auth");
      write_config_internal (fp, opt_set, "session");
    }

  return close_service_file (fp, gl_service);
}

GETOPT_START_ALL
GETOPT_END_ALL

PRINT_ARGS("csync")
PRINT_XMLHELP("csync")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_7 (is_enabled, use_first_pass, try_first_pass, soft_try_pass,
		     nullok, debug, silent);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_csync = { "pam_csync.so", opt_sets, helptext,
			       &def_parse_config,
			       &def_print_module,
			       &write_config_csync,
			       &get_opt_set,
			       &getopt,
			       &print_args,
			       &print_xmlhelp};
