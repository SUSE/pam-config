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

extern pam_module_t mod_pam_localuser;

static int
write_config_ldap (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_winbind, with_ccreds;

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_winbind = is_module_enabled (common_module_list,
				    "pam_winbind.so", op);
  with_ccreds = is_module_enabled (common_module_list,
				   "pam_ccreds.so", op);

  switch (op)
    {
    case ACCOUNT:
      if (with_winbind)
	fprintf (fp, "account\tsufficient\tpam_ldap.so\tuse_first_pass");
      else
	fprintf (fp, "account\trequired\tpam_ldap.so\tuse_first_pass");
      break;
    case AUTH:
      if (with_ccreds)
	fprintf (fp,
		 "auth\t[authinfo_unavail=ignore success=1 default=2]\tpam_ldap.so\tuse_first_pass");
      else if (with_winbind)
	fprintf (fp, "auth\tsufficient\tpam_ldap.so\tuse_first_pass");
      else
	fprintf (fp, "auth\trequired\tpam_ldap.so\tuse_first_pass");
      break;
    case PASSWORD:
      fprintf (fp,
	       "password\trequired\tpam_ldap.so\ttry_first_pass use_authtok ");
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_ldap.so\t");
      break;
    }

  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, " debug");
  fprintf (fp, "\n");

  if (op == AUTH && with_ccreds)
    {
      fprintf (fp,
	       "auth\t[default=done]\tpam_ccreds.so\taction=validate use_first_pass\n");
      fprintf (fp, "auth\t[default=done]\tpam_ccreds.so\taction=store\n");
      fprintf (fp, "auth\t[default=bad]\tpam_ccreds.so\taction=update\n");
    }

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
	  {
		  /* if one of AUTH, ACCOUNT, PASSWD or SESSION is disabled it cannot be account */
		  opt_set = this->get_opt_set (this, AUTH);
		  if (!opt_set->is_enabled (opt_set, "is_enabled"))
			  return 0;
		  opt_set = this->get_opt_set (this, ACCOUNT);
		  if (!opt_set->is_enabled (opt_set, "is_enabled"))
			  return 0;
	      opt_set = this->get_opt_set (this, PASSWORD);
	      if (!opt_set->is_enabled (opt_set, "is_enabled"))
			  return 0;
	      opt_set = this->get_opt_set (this, SESSION);
	      if (!opt_set->is_enabled (opt_set, "is_enabled"))
			  return 0; 
		  this->print_module (this);
	  }
      else
	{
	  if (!g_opt->m_delete &&
	      check_for_pam_module (this->name, g_opt->force) != 0)
	    return 1;
	  opt_set = this->get_opt_set (this, ACCOUNT);
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val);
	  opt_set = this->get_opt_set (this, AUTH);
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val);
	  opt_set = this->get_opt_set (this, PASSWORD);
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val);
	  opt_set = this->get_opt_set (this, SESSION);
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val);
	  opt_set =
	    mod_pam_localuser.get_opt_set (&mod_pam_localuser, ACCOUNT);
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val);
	}
    }
GETOPT_END_ALL

PRINT_ARGS("ldap")

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_2 (is_enabled, debug);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_ldap = { "pam_ldap.so", opt_sets,
			      &def_parse_config,
			      &def_print_module,
			      &write_config_ldap,
			      &get_opt_set,
			      &getopt,
			      &print_args};
