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

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

static int
write_config_umask (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  if (op != SESSION)
    return 0;

  fprintf (fp, "session\toptional\tpam_umask.so\t");

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_1(SESSION)
GETOPT_END_1(SESSION)

PRINT_ARGS("umask")
PRINT_XMLHELP("umask")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_4 (is_enabled, debug, silent, usergroups);
DECLARE_STRING_OPTS_1 (umask);
DECLARE_OPT_SETS;

module_helptext_t umask_help[] = {{"", NULL, "Add pam_umask.so as optional session module."},
				  {"debug", NULL, "Add <option>debug</option> option to all pam_umask.so invocations in session management."},
				  {"silent", NULL, "Add <option>silent</option> option to all pam_umask.so invocations in session management."},
				  {"usergroups", NULL, "Add <option>usergroups</option> option to all pam_umask.so invocations in session management."},
				  {"umask", "mode", "Add <option>umask=</option><replaceable>mode</replaceable> to pam_umask.so."},
				  {NULL, NULL, NULL}};


/* at last construct the complete module object */
pam_module_t mod_pam_umask = { "pam_umask.so", opt_sets, umask_help,
			       &def_parse_config,
			       &def_print_module,
			       &write_config_umask,
			       &get_opt_set,
                               &getopt,
			       &print_args,
			       &print_xmlhelp,
			       -1,
			       -1,
			       -1,
			       PAM_UMASK_SESSION_PRIORITY
};
