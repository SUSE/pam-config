/* Copyright (C) 2006, 2007, 2008 Thorsten Kukuk
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
sanitize_check_account (pam_module_t **module_list)
{
  int with_ldap_auth, with_ldap_account, with_krb5;

  check_for_unix_conflict (module_list, ACCOUNT);

  with_ldap_account = is_module_enabled (module_list, "pam_ldap.so", ACCOUNT);
  with_ldap_auth = is_module_enabled (module_list, "pam_ldap.so", AUTH);
  with_krb5 = is_module_enabled (module_list, "pam_krb5.so", AUTH);

  if (with_ldap_account && !with_ldap_auth && !with_krb5)
    {
      fprintf (stderr,
	       _("ERROR: ldap-account_only is only allowed in combination with krb5.\nConfiguration not changed!\n"));
      return 1;
    }

  return 0;
}

int
sanitize_check_auth (pam_module_t **module_list)
{
  int with_ccreds, with_ldap, with_krb5;

  check_for_unix_conflict( module_list, AUTH );

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

  check_for_unix_conflict( module_list, PASSWORD );

  with_pwcheck = is_module_enabled (module_list,
				    "pam_pwcheck.so", PASSWORD);
  with_cracklib = is_module_enabled (module_list,
				     "pam_cracklib.so", PASSWORD);

  if (with_pwcheck && with_cracklib)
    {
      fprintf (stderr, _("INFO: pam_pwcheck.so and pam_cracklib.so enabled,\nINFO: only pam_pwcheck.so with cracklib support enabled will be used.\n"));

      pam_module_t *cracklib_mod = lookup (module_list, "pam_cracklib.so");
      pam_module_t *pwcheck_mod = lookup (module_list, "pam_pwcheck.so");
      option_set_t *pwcheck_opt_set =
	pwcheck_mod->get_opt_set (pwcheck_mod, PASSWORD);
      char *cpath;

      /* conf->use_cracklib = 0; */
      opt_set = cracklib_mod->get_opt_set (cracklib_mod, PASSWORD);
      opt_set->enable (opt_set, "is_enabled", FALSE);

      pwcheck_opt_set->enable (pwcheck_opt_set, "cracklib", TRUE);

      cpath = pwcheck_opt_set->get_opt (pwcheck_opt_set, "cracklib_path");

      if (cpath == NULL)
	{
	  /* If pam_cracklib has a path set and pam_pwcheck not, copy path
	     to pam_pwcheck data. */
	  cpath = opt_set->get_opt (opt_set, "dictpath");
	  pwcheck_opt_set->set_opt (pwcheck_opt_set, "cracklib_path", cpath);
	}

      if (opt_set->is_enabled (opt_set, "debug"))
	{
	  /* conf->pwcheck_debug = 1; */
	  opt_set->enable (opt_set, "debug", TRUE);
	}
    }

  return 0;
}

int
check_for_unix_conflict (pam_module_t **module_list, write_type_t op)
{
  int with_unix, with_unix2;

  with_unix = is_module_enabled (module_list,
				  "pam_unix.so", op );
  with_unix2 = is_module_enabled (module_list,
				  "pam_unix2.so", op );

  if (with_unix && with_unix2 )
  {
    //fprintf (stderr, _("WARN: pam_unix.so and pam_unix2.so enabled,\nWARN: both will be written to config file %s.\nWARN: You'll have to manually disable one of them!\n"), type2string( op ) );
    fprintf (stderr, _("INFO: pam_unix.so and pam_unix2.so enabled in service %s,\nINFO: only pam_unix2.so will be enabled.\n"), type2string (op));
    pam_module_t *mod_unix = lookup (module_list, "pam_unix.so");
    if (mod_unix)
    {
      option_set_t *opt_set = mod_unix->get_opt_set (mod_unix, op);
      if (opt_set)
      {
	opt_set->enable (opt_set, "is_enabled", FALSE);
      }
      else
      {
	fprintf(stderr, _("ERROR: Failed to disable pam_unix.so.\n"));
      }
    }
    else
    {
      fprintf(stderr, _("ERROR: Failed to disable pam_unix.so.\n"));
    }
  }
  return 0;
}

int
sanitize_check_session (pam_module_t **module_list)
{
  check_for_unix_conflict( module_list, SESSION );
  return 0;
}
