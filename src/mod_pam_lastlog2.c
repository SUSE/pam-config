/* Copyright (C) 2023 Thorsten Kukuk
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

static int
write_config_lastlog2 (pam_module_t * this, enum write_type op, FILE * fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  switch (op)
    {
    case SESSION:
      fprintf (fp, "session\toptional\tpam_lastlog2.so\t");
      break;
    default:
      return 0;
    }

  WRITE_CONFIG_OPTIONS

  return 0;
}

static int
getopt (pam_module_t *this, char *opt, char *optarg, global_opt_t *g_opt)
{
  option_set_t *opt_set;

  if (debug)
    printf ("**** %s->getopt: '%s'='%s'\n", this->name, opt, optarg);

  if (strcmp ("", opt) == 0)
    {
      if (g_opt->m_query)
	this->print_module (this);
      else
	{
	  if (!g_opt->m_delete &&
	      check_for_pam_module (this->name, g_opt->force, 0) != 0)
	    return 1;
	  /* Remove in every case from auth,
	     else we will have it twice.  */
	  opt_set = this->get_opt_set (this, AUTH);
	  opt_set->enable (opt_set, "is_enabled", FALSE);
	  opt_set = this->get_opt_set (this, SESSION);
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val);
	}
    }
GETOPT_END_1(SESSION)

PRINT_ARGS("lastlog2")
PRINT_XMLHELP("lastlog2")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_3 (is_enabled, debug, silent);
DECLARE_STRING_OPTS_1 (database);
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_lastlog2 = {"pam_lastlog2.so", opt_sets, helptext,
			    &def_parse_config,
			    &def_print_module,
			    &write_config_lastlog2,
			    &get_opt_set,
                            getopt,
			    &print_args,
			    &print_xmlhelp};
