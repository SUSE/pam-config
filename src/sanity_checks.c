/* Copyright (C) 2006, 2007, 2008, 2009, 2010, 2016, 2020 Thorsten Kukuk
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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "pam-config.h"
#include "pam-module.h"

static int
check_for_pam_module_path (const char *path, const char *name,
			   int force, int check_only)
{
  char module[strlen(path) + strlen (name) + 2];

  sprintf (module, "%s/%s", path, name);

  if (access (module, F_OK) != 0)
    {
      if (force)
	{
	  if (!check_only)
	    fprintf (stderr, _("WARNING: module %s is not installed.\n"),
		     module);
	  return 0;
	}
      else
	{
	  if (!check_only)
	    fprintf (stderr,
		     _("ERROR: module %s is not installed.\n"),
		     module);
	  return 1;
	}
    }
  return 0;
}

int
check_for_pam_module (const char *name, int force, int check_only)
{
#if defined(__LP64__)
  int i = check_for_pam_module_path ("/lib64/security", name,
				     force, check_only);

  if (i > 0)
    return 1;

  /* Only print warning if 32bit PAM module is missing */
  if (access("/lib/libpam.so.0", F_OK) == 0)
    return check_for_pam_module_path ("/lib/security", name,
				      1, check_only);
#else
  int i = check_for_pam_module_path ("/lib/security", name,
				     force, check_only);

  if (i > 0)
    return 1;
#endif

  return 0;
}

static int
check_for_unix_conflict (pam_module_t **module_list, write_type_t op,
			 int verify)
{
  int with_unix, with_unix2, with_unix_ng;
  int retval = 0;

  with_unix = is_module_enabled (module_list,
				 "pam_unix.so", op );
  with_unix2 = is_module_enabled (module_list,
				  "pam_unix2.so", op );
  with_unix_ng = is_module_enabled (module_list,
                                    "pam_unix_ng.so", op );


  /* exactly one should be enabled */
  if ((with_unix + with_unix2 + with_unix_ng) == 1)
    return retval;

  if (verify)
    {
      /* no one enabled, enable pam_unix */
      if ((with_unix + with_unix2 + with_unix_ng) == 0)
	fprintf (stderr,
		 _("WARNING: no unix module is enabled!\n"));
      else
	fprintf (stderr,
		 _("WARNING: pam_unix.so, pam_unix2.so and/or pam_unix_ng.so are enabled!\n"));
      retval = 1;
    }
  else
    {
      pam_module_t *mod_unix_ng = lookup (module_list, "pam_unix_ng.so");
      pam_module_t *mod_unix = lookup (module_list, "pam_unix.so");
      pam_module_t *mod_unix2 = lookup (module_list, "pam_unix2.so");

      assert(mod_unix_ng);
      assert(mod_unix);
      assert(mod_unix2);

      if ((with_unix + with_unix2 + with_unix_ng) == 0)
	{
	  option_set_t *opt_set_unix = mod_unix->get_opt_set (mod_unix, op);

	  fprintf (stderr,
		   _("INFO: no unix module is enabled for service %s,\nINFO: enabling pam_unix.so.\n"),
		   type2string(op));

	  opt_set_unix->enable (opt_set_unix, "is_enabled", TRUE);
	}
      else if (with_unix_ng)
	{
	  option_set_t *opt_set_unix = mod_unix->get_opt_set (mod_unix, op);
	  option_set_t *opt_set_unix2 = mod_unix2->get_opt_set (mod_unix2, op);

	  fprintf (stderr,
		   _("INFO: pam_unix.so, pam_unix2 and/or pam_unix_ng.so are enabled in service %s,\nINFO: only pam_unix_ng.so will be enabled.\n"),
		   type2string (op));

	  opt_set_unix->enable (opt_set_unix, "is_enabled", FALSE);
	  opt_set_unix2->enable (opt_set_unix2, "is_enabled", FALSE);
	}
      else if (with_unix)
	{
	  option_set_t *opt_set = mod_unix2->get_opt_set (mod_unix2, op);

	  fprintf (stderr,
		   _("INFO: pam_unix.so and pam_unix2 are enabled in service %s,\nINFO: only pam_unix.so will be enabled.\n"),
		   type2string (op));

	  opt_set->enable (opt_set, "is_enabled", FALSE);
	}
      else
	{
	  fprintf(stderr, _("ERROR: something did go wrong with counting unix modules...\n"));
	  retval = 1;
	}
    }

return retval;
}

int
sanitize_check_account (pam_module_t **module_list, int verify)
{
  int with_ldap_auth, with_ldap_account, with_krb5;
  int with_nam, with_winbind, with_localuser, with_sss;
  option_set_t *opt_set;
  int retval = 0;

  retval = check_for_unix_conflict (module_list, ACCOUNT, verify);
  if (retval && !verify)
    return retval;

  with_ldap_account = is_module_enabled (module_list, "pam_ldap.so", ACCOUNT);
  with_ldap_auth = is_module_enabled (module_list, "pam_ldap.so", AUTH);
  with_krb5 = is_module_enabled (module_list, "pam_krb5.so", AUTH);

  if (with_ldap_account && !with_ldap_auth && !with_krb5)
    {
      fprintf (stderr,
	       _("ERROR: ldap-account_only is only allowed in combination with krb5.\nConfiguration not changed!\n"));
      if (verify)
	retval = 1;
      else
	return 1;
    }

  with_nam = is_module_enabled (module_list, "pam_nam.so", ACCOUNT);
  with_sss = is_module_enabled (module_list, "pam_sss.so", ACCOUNT);
  with_winbind = is_module_enabled (module_list, "pam_winbind.so", ACCOUNT);
  with_localuser = is_module_enabled (module_list, "pam_localuser.so", ACCOUNT);

  /*
   * These modules require pam_localuser. Enable it automaticaly if it is disabled.
   * See also bnc#371558 .
   */
  if( (with_ldap_account || with_nam || with_winbind || with_sss) && !with_localuser)
  {
    if (verify)
      {
	fprintf (stderr,
		 _("WARNING: pam_localuser is not enabled!\n"));
	retval = 1;
      }
    else
      {
	pam_module_t *localuser_mod = lookup (module_list, "pam_localuser.so");
	opt_set = localuser_mod->get_opt_set (localuser_mod, ACCOUNT);
	opt_set->enable (opt_set, "is_enabled", TRUE);
      }
  }

  return retval;
}

int
sanitize_check_auth (pam_module_t **module_list, int verify)
{
  int with_ccreds, with_ldap, with_krb5;
  int retval = 0;

  retval = check_for_unix_conflict (module_list, AUTH, verify);
  if (retval && !verify)
    return retval;

  with_ccreds = is_module_enabled (module_list,
				   "pam_ccreds.so", AUTH);
  with_ldap = is_module_enabled (module_list,
				 "pam_ldap.so", AUTH);
  with_krb5 = is_module_enabled (module_list,
				 "pam_krb5.so", AUTH);

  if (with_ccreds && !with_ldap && !with_krb5)
    {
      fprintf (stderr,
	       _("WARNING: pam_ccreds.so needs LDAP or Kerberos5, ignored\n"));
      if (verify)
	retval = 1;
    }

  return retval;
}

int
sanitize_check_password (pam_module_t **module_list, int verify)
{
  int with_pwcheck, with_cracklib;
  option_set_t *opt_set;
  int retval = 0;

  retval = check_for_unix_conflict (module_list, PASSWORD, verify);
  if (retval && !verify)
    return retval;

  with_pwcheck = is_module_enabled (module_list,
				    "pam_pwcheck.so", PASSWORD);
  with_cracklib = is_module_enabled (module_list,
				     "pam_cracklib.so", PASSWORD);

  if (with_pwcheck && with_cracklib)
    {
      if (verify)
	{
	  fprintf (stderr,
		   _("WARNING: pam_pwcheck and pam_cracklib enabled!\n"));
	  retval = 1;
	}
      else
	{
	  fprintf (stderr, _("INFO: pam_pwcheck.so and pam_cracklib.so enabled,\nINFO: only pam_cracklib.so will be used.\n"));

	  pam_module_t *cracklib_mod = lookup (module_list, "pam_cracklib.so");
	  pam_module_t *pwcheck_mod = lookup (module_list, "pam_pwcheck.so");
	  option_set_t *pwcheck_opt_set =
	    pwcheck_mod->get_opt_set (pwcheck_mod, PASSWORD);
	  char *cpath;

	  /* conf->use_cracklib = 1; */
	  opt_set = cracklib_mod->get_opt_set (cracklib_mod, PASSWORD);
	  opt_set->enable (opt_set, "is_enabled", TRUE);

	  pwcheck_opt_set->enable (pwcheck_opt_set, "is_enabled", FALSE);

	  cpath = opt_set->get_opt (opt_set, "dictpath");

	  if (cpath == NULL)
	    {
	      /* If pam_cracklib has a path set and pam_pwcheck not, copy path
		 to pam_pwcheck data. */
	      cpath = pwcheck_opt_set->get_opt (pwcheck_opt_set, "cracklib_path");
	      opt_set->set_opt (opt_set, "dictpath", cpath);
	    }

	  if (pwcheck_opt_set->is_enabled (pwcheck_opt_set, "debug"))
	    {
	      /* conf->cracklib_debug = 1; */
	      opt_set->enable (opt_set, "debug", TRUE);
	    }
	}
    }

  if (with_cracklib)
  {
    pam_module_t *cracklib_mod = lookup (module_list, "pam_cracklib.so");
    option_set_t *cracklib_opt_set = cracklib_mod->get_opt_set (cracklib_mod, PASSWORD);
    char *minlen = cracklib_opt_set->get_opt (cracklib_opt_set, "minlen");
    if ( minlen && atoi(minlen) <= 5 )
    {
      fprintf (stderr,
               _("WARNING: cracklib itself has a length limit of 6 characters!\n"));
    }
  }
  return retval;
}

int
sanitize_check_session (pam_module_t **module_list, int verify)
{
  int with_selinux, with_apparmor;
  int retval = 0;

  retval = check_for_unix_conflict (module_list, SESSION, verify);
  if (retval && !verify)
    return retval;

  with_selinux = is_module_enabled (module_list, "pam_selinux.so", SESSION);
  with_apparmor = is_module_enabled (module_list, "pam_apparmor.so", SESSION);

  if (with_selinux && with_apparmor)
    {
      fprintf (stderr,
	       _("ERROR: pam_apparmor and pam_selinux together is not allowed.\nConfiguration not changed!\n"));
      if (verify)
	retval = 1;
      else
	return 1;
    }

  return retval;
}
