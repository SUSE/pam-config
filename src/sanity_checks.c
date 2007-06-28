/* Copyright (C) 2006, 2007 Thorsten Kukuk
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
#include <unistd.h>

#include "pam-config.h"
#include "pam-module.h"

int
check_for_pam_module (const char *name, int force)
{
  const char *path[] = { "/lib/security", "/lib64/security" };
  int i, retval = 0;

  for (i = 0; i < 1; i++)
    {
      if (access (path[i], F_OK) == 0)
	{
	  char module[strlen(path[i]) + strlen (name) + 2];

	  sprintf (module, "%s/%s", path[i], name);

	  if (access (module, F_OK) != 0)
	    {
	      if (force)
		fprintf (stderr, _("WARNING: module %s is not installed.\n"),
			 module);
	      else
		{
		  fprintf (stderr,
			   _("ERROR: module %s is not installed.\n"),
			   module);
		  retval=1;
		}
	    }
	}
    }
  return retval;
}

int
sanitize_check_account (pam_module_t **module_list __attribute__((unused)))
{
  return 0;
}

int
sanitize_check_auth (pam_module_t **module_list)
{
  int with_ccreds, with_ldap, with_krb5;

  with_ccreds = is_module_enabled (module_list,
				   "pam_ccreds.so", AUTH);
  with_ldap = is_module_enabled (module_list,
				 "pam_ldap.so", AUTH);
  with_krb5 = is_module_enabled (module_list,
				 "pam_krb5.so", AUTH);

  if (with_ccreds && !with_ldap && !with_krb5)
    fprintf (stderr,
	     _("WARNING: pam_ccreds.so needs LDAP or Kerberos5, ignored\n"));

  return 0;
}

int
sanitize_check_password (pam_module_t **module_list)
{
  int with_pwcheck, with_cracklib;
  option_set_t *opt_set;

  with_pwcheck = is_module_enabled (module_list,
				    "pam_pwcheck.so", PASSWORD);
  with_cracklib = is_module_enabled (module_list,
				     "pam_cracklib.so", PASSWORD);

  if (with_pwcheck && with_cracklib)
    {
      fprintf (stderr, _("INFO: pam_pwcheck.so and pam_cracklib.so enabled,\nINFO: only pam_pwcheck.so with cracklib support enabled will be used.\n"));

      pam_module_t *cracklib_mod = lookup (module_list, "pam_cracklib.so");

      /* conf->use_cracklib = 0; */
      opt_set = cracklib_mod->get_opt_set (cracklib_mod, PASSWORD);
      opt_set->enable (opt_set, "is_enabled", FALSE);

      /* XXX convert */
#if 0
      if (conf->pwcheck_cracklib == 0)
	conf->pwcheck_cracklib = 1;
      if (conf->pwcheck_cracklib_path == NULL)
	conf->pwcheck_cracklib_path = conf->cracklib_dictpath;
#endif
      if (opt_set->is_enabled (opt_set, "debug"))
	{
	  /* conf->pwcheck_debug = 1; */
	  pam_module_t *pwcheck_mod = lookup (module_list, "pam_pwcheck.so");

	  opt_set = pwcheck_mod->get_opt_set (pwcheck_mod, PASSWORD);
	  opt_set->enable (opt_set, "debug", TRUE);
	}
    }

  return 0;
}

int
sanitize_check_session (pam_module_t **module_list __attribute__((unused)))
{
  return 0;
}
