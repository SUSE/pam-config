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

#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "pam-config.h"

static void
parse_option_unix2 (const char *file, const char *service,
		    const char *option, pam_module_t *mod_pam_unix2)
{
  option_set_t *opt_set;

  if (debug)
    printf ("**** parse_option_unix2 (%s, %s, %s, ...)\n",
	    file, service, option);

  if (option == NULL || *option == '\0')
    return;

  if (strcasecmp (option, "debug") == 0)
    {
      if (strcmp (service, "account") == 0)
	{
	  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2, ACCOUNT);
          opt_set->enable (opt_set, "debug", TRUE);
	}
      else if (strcmp (service, "auth") == 0)
	{
	  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2, AUTH);
          opt_set->enable (opt_set, "debug", TRUE);
	}
      else if (strcmp (service, "password") == 0)
	{
	  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2, PASSWORD);
          opt_set->enable (opt_set, "debug", TRUE);
	}
      else if (strcmp (service, "session") == 0)
	{
	  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2, SESSION);
          opt_set->enable (opt_set, "debug", TRUE);
	}
      else
	fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
    }
  else if (strcasecmp (option, "nullok") == 0)
    {
      if (strcmp (service, "account") == 0)
	{
	  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2, ACCOUNT);
          opt_set->enable (opt_set, "nullok", TRUE);
	}
      else if (strcmp (service, "auth") == 0)
	{
	  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2, AUTH);
          opt_set->enable (opt_set, "nullok", TRUE);
	}
      else if (strcmp (service, "password") == 0)
	{
	  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2, PASSWORD);
          opt_set->enable (opt_set, "nullok", TRUE);
	}
      else if (strcmp (service, "session") == 0)
	{
	  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2, SESSION);
          opt_set->enable (opt_set, "nullok", TRUE);
	}
      else
	fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
    }
  else if (strcasecmp (option, "use_first_pass") == 0)
    { /* ignored */ }
  else if (strcasecmp (option, "use_authtok") == 0)
    { /* ignored */ }
  else if (strcasecmp (option, "none") == 0)
    { /* ignored */ }
  else if (strcasecmp (option, "krb5") == 0 ||
	   strcasecmp (option, "use_krb5") == 0)
    {
      pam_module_t *mod = lookup (common_module_list, "pam_krb5.so");

      if (strcmp (service, "account") == 0)
	{
	  opt_set = mod->get_opt_set (mod, ACCOUNT);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	}
      else if (strcmp (service, "auth") == 0)
	{
	  opt_set = mod->get_opt_set (mod, AUTH);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	}
      else if (strcmp (service, "password") == 0)
	{
	  opt_set = mod->get_opt_set (mod, PASSWORD);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	}
      else if (strcmp (service, "session") == 0)
	{
	  opt_set = mod->get_opt_set (mod, SESSION);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	}
      else
	fprintf (stderr,
		 _("ERROR: Unknown service: [%s: %s], ignored!\n"),
		 service, option);
    }
  else if (strcasecmp (option, "ldap") == 0 ||
	   strcasecmp (option, "use_ldap") == 0)
    {
      pam_module_t *mod = lookup (common_module_list, "pam_ldap.so");

      if (strcmp (service, "account") == 0)
	{
	  opt_set = mod->get_opt_set (mod, ACCOUNT);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	}
      else if (strcmp (service, "auth") == 0)
	{
	  opt_set = mod->get_opt_set (mod, AUTH);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	}
      else if (strcmp (service, "password") == 0)
	{
	  opt_set = mod->get_opt_set (mod, PASSWORD);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	}
      else if (strcmp (service, "session") == 0)
	{
	  opt_set = mod->get_opt_set (mod, SESSION);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	}
      else
	fprintf (stderr,
		 _("ERROR: Unknown service: [%s: %s], ignored!\n"),
		 service, option);
    }
  else if (strncasecmp (option, "call_modules=", 13) == 0)
    {
      char *cp;
      char *tmp = strdup (&option[13]);

      cp = strtok (tmp, ",");
      while (cp)
	{
	  if (strcmp (cp, "ldap") == 0)
	    {
	      pam_module_t *mod = lookup (common_module_list, "pam_ldap.so");

	      if (strcmp (service, "account") == 0)
		{
		  opt_set = mod->get_opt_set (mod, ACCOUNT);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else if (strcmp (service, "auth") == 0)
		{
		  opt_set = mod->get_opt_set (mod, AUTH);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else if (strcmp (service, "password") == 0)
		{
		  opt_set = mod->get_opt_set (mod, PASSWORD);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else if (strcmp (service, "session") == 0)
		{
		  opt_set = mod->get_opt_set (mod, SESSION);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else
		fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
	    }
	  else if (strcmp (cp, "krb5") == 0 || strcmp (cp, "krb5afs") == 0)
	    {
	      pam_module_t *mod = lookup (common_module_list, "pam_krb5.so");

	      if (strcmp (service, "account") == 0)
		{
		  opt_set = mod->get_opt_set (mod, ACCOUNT);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else if (strcmp (service, "auth") == 0)
		{
		  opt_set = mod->get_opt_set (mod, AUTH);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else if (strcmp (service, "password") == 0)
		{
		  opt_set = mod->get_opt_set (mod, PASSWORD);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else if (strcmp (service, "session") == 0)
		{
		  opt_set = mod->get_opt_set (mod, SESSION);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else
		fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
	    }
	  else if (strcmp (cp, "winbind") == 0)
	    {
	      pam_module_t *mod =
		lookup (common_module_list, "pam_winbind.so");

	      if (strcmp (service, "account") == 0)
		{
		  opt_set = mod->get_opt_set (mod, ACCOUNT);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else if (strcmp (service, "auth") == 0)
		{
		  opt_set = mod->get_opt_set (mod, AUTH);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else if (strcmp (service, "password") == 0)
		{
		  opt_set = mod->get_opt_set (mod, PASSWORD);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else if (strcmp (service, "session") == 0)
		{
		  opt_set = mod->get_opt_set (mod, SESSION);
		  opt_set->enable (opt_set, "is_enabled", TRUE);
		}
	      else
		fprintf (stderr,
			 _("ERROR: Unknown service: [%s: %s], ignored!\n"),
			 service, option);
	    }
	  else
	    {
	      if (strcmp (service, "account") == 0)
		{
		  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2,
							ACCOUNT);
		  opt_set->set_opt (opt_set, "call_modules", strdup (cp));
		}
	      else if (strcmp (service, "auth") == 0)
		{
		  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2, AUTH);
		  opt_set->set_opt (opt_set, "call_modules", strdup (cp));
		}
	      else if (strcmp (service, "password") == 0)
		{
		  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2,
							PASSWORD);
		  opt_set->set_opt (opt_set, "call_modules", strdup (cp));
		}
	      else if (strcmp (service, "session") == 0)
		{
		  opt_set = mod_pam_unix2->get_opt_set (mod_pam_unix2
							, SESSION);
		  opt_set->set_opt (opt_set, "call_modules", strdup (cp));
		}
	      else
		fprintf (stderr,
			 _("ERROR: Unknown service: [%s: %s], ignored!\n"),
			 service, option);
	    }
	  cp = strtok (NULL, ",");
	}
      free (tmp);
    }
  else if (strcasecmp (option, "md5") == 0)
    fprintf (stderr, _("WARNING: Use /etc/default/passwd to set 'md5' option\n"));
  else if (strcasecmp (option, "bigcrypt") == 0)
    fprintf (stderr, _("WARNING: Use /etc/default/passwd to set 'bigcrypt' option\n"));
  else if (strcasecmp (option, "bf") == 0 || strcasecmp (option, "blowfish") == 0 ||
	   strncasecmp (option, "rounds=", 7) == 0 ||
	   strncasecmp (option, "crypt_rounds=", 13) == 0)
    fprintf (stderr, _("WARNING: Use /etc/default/passwd to set 'blowfish' option\n"));
  else
    fprintf (stderr, _("ERROR: Unknown option (%s): [%s: %s], ignored!\n"),
	     file, service, option);
}

static void
parse_option_pwcheck (const char *file, const char *service,
		      const char *option, pam_module_t *mod_pam_pwcheck)
{
  option_set_t *opt_set;

  if (debug)
    printf ("***** parse_option_pwcheck (%s, %s, %s, ...)\n",
	    file, service, option);

  if (option == NULL || *option == '\0')
    return;

  if (strcmp (service, "password") != 0)
    {
      fprintf (stderr,
	       _("WARNING: Unknown option (%s): [%s: %s], ignored!\n"),
	       file, service, option);
      return;
    }

  opt_set = mod_pam_pwcheck->get_opt_set (mod_pam_pwcheck, PASSWORD);

  if (strcasecmp (option, "debug") == 0)
    opt_set->enable (opt_set, "debug", TRUE);
  else if (strcasecmp (option, "nullok") == 0)
    opt_set->enable (opt_set, "nullok", TRUE);
  else if (strcasecmp (option, "cracklib") == 0 ||
	   strcasecmp (option, "use_cracklib") == 0)
    opt_set->enable (opt_set, "cracklib", TRUE);
  else if (strncasecmp (option, "cracklib=", 9) == 0)
    {
      opt_set->enable (opt_set, "cracklib", TRUE);
      opt_set->set_opt (opt_set, "cracklib_path", strdup (&option[9]));
    }
  else if (strncasecmp (option, "maxlen=", 7) == 0)
    opt_set->set_opt (opt_set, "maxlen", strdup (&option[7]));
  else if (strncasecmp (option, "minlen=", 7) == 0)
    opt_set->set_opt (opt_set, "minlen", strdup (&option[7]));
  else if (strncasecmp (option, "tries=", 6) == 0)
    opt_set->set_opt (opt_set, "tries", strdup (&option[6]));
  else if (strncasecmp (option, "remember=", 9) == 0)
    opt_set->set_opt (opt_set, "remember", strdup (&option[9]));
  else if (strncasecmp (option, "nisdir=", 7) == 0)
    opt_set->set_opt (opt_set, "nisdir", strdup (&option[7]));
  else if (strcasecmp (option, "use_first_pass") == 0)
    { /* ignored */ }
  else if (strcasecmp (option, "use_authtok") == 0)
    { /* ignored */ }
  else if (strcasecmp (option, "no_obscure_checks") == 0)
    opt_set->enable (opt_set, "no_obscure_checks", TRUE);
  else if (strcasecmp (option, "enforce_for_root") == 0)
    opt_set->enable (opt_set, "enforce_for_root", TRUE);
  else if (strcasecmp (option, "md5") == 0)
    fprintf (stderr, _("Please use /etc/default/passwd to set 'md5' option\n"));
  else if (strcasecmp (option, "bigcrypt") == 0)
    fprintf (stderr, _("Please use /etc/default/passwd to set 'bigcrypt' option\n"));
  else if (strcasecmp (option, "bf") == 0 || strcasecmp (option, "blowfish") == 0 ||
	   strncasecmp (option, "rounds=", 7) == 0 ||
	   strncasecmp (option, "crypt_rounds=", 13) == 0)
    fprintf (stderr, _("Please use /etc/default/passwd to set 'blowfish' option\n"));
  else
    fprintf (stderr, _("WARNING: Unknown option (%s): [%s: %s], ignored!\n"),
	     file, service, option);
}


static int
parse_file (const char *file, pam_module_t *mod,
	    void (parse_arguments)(const char *, const char *, const char *,
				   pam_module_t *))
{
  FILE *fp;
  char *buf = NULL;
  size_t buflen = 0;

  if (debug)
    printf ("**** parse_file (%s, ...)\n", file);

  fp = fopen (file, "r");
  if (fp == NULL)
    {
      if (errno != ENOENT)
	return -1;
    }
  else
    {
      while (!feof (fp))
	{
	  char *cp, *tmp;
	  ssize_t n = getline (&buf, &buflen, fp);
	  char *c, *service;

	  cp = buf;

	  if (n < 1)
	    break;

	  tmp = strchr (cp, '#');  /* remove comments */
	  if (tmp)
	    *tmp = '\0';
	  while (isspace ((int)*cp))    /* remove spaces and tabs */
	    ++cp;
	  if (*cp == '\0')        /* ignore empty lines */
	    continue;

	  if (cp[strlen (cp) - 1] == '\n')
	    cp[strlen (cp) - 1] = '\0';

	  service = cp;
	  c = strchr (cp, ':');
          if (c != NULL)
            *c++ = '\0';

          while (c != NULL && strlen (c) > 0)
            {
              /* remove all spaces */
              while (isspace (*c))
                ++c;

              cp = c;
              while (!isspace (*c) && *c != '\0')
                ++c;

              if (*c == '\0')
                parse_arguments (file, service, cp, mod);
              else
                {
                  *c = '\0';
		  parse_arguments (file, service, cp, mod);
                  ++c;
                  cp = c;
                }
	    }
	}
    }

  fclose (fp);

  if (buf)
    free (buf);

  return 0;
}

int
load_obsolete_conf (pam_module_t **module_list)
{
  if (debug)
    printf ("*** load_obsolete_conf (...)\n");

  if (access ("/etc/security/pam_unix2.conf", R_OK) == 0)
    {
      pam_module_t *mod = lookup (module_list, "pam_unix2.so");

      if (parse_file ("/etc/security/pam_unix2.conf", mod,
		      parse_option_unix2) == -1)
	return -1;
    }
  else if (access ("/etc/security/pam_unix2.conf.rpmsave", R_OK) == 0)
    {
      pam_module_t *mod = lookup (module_list, "pam_unix2.so");

      if (parse_file ("/etc/security/pam_unix2.conf.rpmsave", mod,
		      parse_option_unix2) == -1)
	return -1;
    }

  if (access ("/etc/security/pam_pwcheck.conf", R_OK) == 0)
    {
      pam_module_t *mod = lookup (module_list, "pam_pwcheck.so");

      if (parse_file ("/etc/security/pam_pwcheck.conf", mod,
		      parse_option_pwcheck) == -1)
	return -1;
    }
  else if (access ("/etc/security/pam_pwcheck.conf.rpmsave", R_OK) == 0)
    {
      pam_module_t *mod = lookup (module_list, "pam_pwcheck.so");

      if (parse_file ("/etc/security/pam_pwcheck.conf.rpmsave", mod,
		      parse_option_pwcheck) == -1)
	return -1;
    }

  return 0;
}
