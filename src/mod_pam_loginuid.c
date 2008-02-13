/* Copyright (C) 2007, 2008 Thorsten Kukuk
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

static int
write_config_loginuid (pam_module_t *this,
		       enum write_type op __attribute__((unused)),
		       FILE *unused)
{
  option_set_t *opt_set = this->get_opt_set (this, SESSION);
  int is_written = 0;
  FILE *fp;
  config_content_t *ptr;
  int writeit = opt_set->is_enabled (opt_set, "is_enabled");

  if (debug)
    printf ("**** write_config_loginuid (%s) (%s)\n", gl_service,
	    writeit?"enable":"disable");

  assert (unused == NULL);

  load_single_config (gl_service, &ptr);

  fp = create_service_file (gl_service);
  if( fp == NULL ) return 1;

  while (ptr != NULL)
    {
      if (writeit)
	{
	  if (!is_written)
	    {
	      if (strcasestr (ptr->line, "session") != NULL)
		{
		  fprintf (fp, "session  required\tpam_loginuid.so\t");
		  if (opt_set->is_enabled (opt_set, "require_auditd"))
		    fprintf (fp, "require_auditd ");
		  fprintf (fp, "\n");
		  is_written = 1;
		}
	    }

	  /* don't write old pam_loginuid.so line */
	  if (strcasestr (ptr->line, "pam_loginuid.so") == NULL)
	    fprintf (fp, "%s", ptr->line);
	}
      else
	{
	  if (strcasestr (ptr->line, "pam_loginuid.so") == NULL)
	    fprintf (fp, "%s", ptr->line);
	  else
	    is_written = 1;
	}
      ptr = ptr->next;
    }

  return close_service_file (fp, gl_service);
}

GETOPT_START_1(SESSION)
GETOPT_END_1(SESSION)

PRINT_ARGS("loginuid")

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_2 (is_enabled, require_auditd);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_loginuid = { "pam_loginuid.so", opt_sets,
				  &def_parse_config,
				  &def_print_module,
				  &write_config_loginuid,
				  &get_opt_set,
				  &getopt,
				  &print_args};
