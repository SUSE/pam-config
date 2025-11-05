/* Copyright (C) 2008 Michael Calmer
   Author: Michael Calmer <mc@suse.de>

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
parse_config_succeed_if (pam_module_t *this __attribute__((unused)),
		   char *args, write_type_t type)
{
  if (debug)
    printf ("**** parse_config_succeed_if (%s): '%s'\n", type2string (type),
	    args ? args : "");

  return 1;
}

static int
write_config_succeed_if (pam_module_t *this, enum write_type op,
		   FILE *fp __attribute__ ((unused)))
{

  if (debug)
    debug_write_call (this, op);

  return 0;
}

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_1 (is_enabled);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_succeed_if = {"pam_succeed_if.so",
				   opt_sets, NULL,
				   &parse_config_succeed_if,
				   &def_print_module,
				   &write_config_succeed_if,
				   &get_opt_set,
				   NULL /* Only used internal */,
				   NULL /* Only used internal */,
				   NULL /* Only used internal */,
				   NULL,
				   -1,
				   -1,
				   -1,
				   -1
};
