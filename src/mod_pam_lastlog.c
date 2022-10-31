/* Copyright (C) 2007, 2008, 2011 Thorsten Kukuk
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
write_config_internal (FILE *fp, option_set_t *opt_set)
{
  fprintf (fp, "session  optional\tpam_lastlog.so\t");

  WRITE_CONFIG_OPTIONS
}

static int
write_config_lastlog (pam_module_t *this,
		       enum write_type op __attribute__((unused)),
		      FILE *unused __attribute__((unused)))
{
  option_set_t *opt_set = this->get_opt_set (this, SESSION);
  int is_written = 0;
  FILE *fp;
  config_content_t *ptr;
  int writeit = opt_set->is_enabled (opt_set, "is_enabled");

  if (debug)
    debug_write_call (this, SESSION);

  load_single_config (gl_service, &ptr);

  fp = create_service_file (gl_service);
  if( fp == NULL ) return 1;

  while (ptr != NULL)
    {
      if (writeit)
	{
          /* don't write old pam_lastlog.so line */
          if (strcasestr (ptr->line, "pam_lastlog.so") == NULL)
            fprintf (fp, "%s", ptr->line);

          if (!is_written)
            {
              if (strcasestr (ptr->line, "common-session") != NULL)
                {
		  write_config_internal (fp, opt_set);
                  is_written = 1;
                }
            }
        }
      else
        {
          if (strcasestr (ptr->line, "pam_lastlog.so") == NULL)
            fprintf (fp, "%s", ptr->line);
          else
            is_written = 1;
        }

      ptr = ptr->next;
    }

  /* make sure we really write it if we have to add it. */
  if (!is_written && writeit)
    write_config_internal (fp, opt_set);

  return close_service_file (fp, gl_service);
}

GETOPT_START_1(SESSION)
GETOPT_END_1(SESSION)

PRINT_ARGS("lastlog")
PRINT_XMLHELP("lastlog")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_10 (is_enabled, debug, silent, never, nodate, nohost, noterm, nowtmp, noupdate, showfailed);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_lastlog = { "pam_lastlog.so", opt_sets, helptext,
				 &def_parse_config,
				 &def_print_module,
				 &write_config_lastlog,
				 &get_opt_set,
				 &getopt,
				 &print_args,
				 &print_xmlhelp};
