/* Copyright (C) 2008 Vincent Untz
   Author: Vincent Untz <vuntz@novell.com>

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
write_config_gnome_keyring (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  switch (op)
    {
    case AUTH:
      fprintf (fp, "auth\toptional\tpam_gnome_keyring.so\t");
      break;
    case ACCOUNT:
      return 0;
      break;
    case PASSWORD:
      /* We use use_authtok since we want to use something that is supposed to
       * be secure.
       * No auto_start option: if daemon is not running, we will start and kill it */
      fprintf (fp, "password\toptional\tpam_gnome_keyring.so\tuse_authtok\n");
      return 0;
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_gnome_keyring.so\t");
      break;
    }

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_ALL
  /* We put the auto_start options in SESSION only */
  else if (strcmp ("auto_start", opt) == 0) {
    opt_set = this->get_opt_set (this, SESSION);
    if (opt_set->enable (opt_set, opt, g_opt->opt_val) == FALSE) {
      return 1;
    }
  }
GETOPT_END_ALL

PRINT_ARGS("gnome_keyring");
PRINT_XMLHELP("gnome_keyring");

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_2(is_enabled, auto_start);
DECLARE_STRING_OPTS_1(only_if);
DECLARE_OPT_SETS;

static module_helptext_t gnome_keyring_helptext[] = {{"", NULL, "Enable/Disable pam_gnome_keyring.so"},
                                                     {NULL, NULL, NULL}};


pam_module_t mod_pam_gnome_keyring = { "pam_gnome_keyring.so", opt_sets,
				       gnome_keyring_helptext,
				       &def_parse_config,
				       &def_print_module,
				       &write_config_gnome_keyring,
				       &get_opt_set,
				       &getopt,
				       &print_args,
				       &print_xmlhelp,
				       NULL,
				       PAM_GNOME_KEYRING_AUTH_PRIORITY,
				       -1,
				       PAM_GNOME_KEYRING_PASSWORD_PRIORITY,
				       PAM_GNOME_KEYRING_SESSION_PRIORITY
};
