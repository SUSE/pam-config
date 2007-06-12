/* Copyright (C) 2006 Thorsten Kukuk
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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "pam-config.h"

static void
parse_option_unix2 (const char *file, const char *service,
		    const char *option,
		    config_file_t *account, config_file_t *auth,
		    config_file_t *password, config_file_t *session)
{
  if (debug)
    printf ("**** parse_option_unix2 (%s, %s, %s, ...)\n",
	    file, service, option);

  if (strcasecmp (option, "debug") == 0)
    {
      if (strcmp (service, "auth") == 0)
	auth->unix2_debug = 1;
      else if (strcmp (service, "account") == 0)
	account->unix2_debug = 1;
      else if (strcmp (service, "password") == 0)
	password->unix2_debug = 1;
      else if (strcmp (service, "session") == 0)
	password->unix2_debug = 1;
      else
	fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
    }
  else if (strcasecmp (option, "nullok") == 0)
    {
      if (strcmp (service, "auth") == 0)
	auth->unix2_nullok = 1;
      else if (strcmp (service, "account") == 0)
	account->unix2_nullok = 1;
      else if (strcmp (service, "password") == 0)
	password->unix2_nullok = 1;
      else if (strcmp (service, "session") == 0)
	session->unix2_nullok = 1;
      else
	fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
    }
  else if (strcasecmp (option, "not_set_pass") == 0)
    password->unix2_not_set_pass = 1;
  else if (strcasecmp (option, "use_first_pass") == 0)
    { /* ignored */ }
  else if (strcasecmp (option, "use_authtok") == 0)
    { /* ignored */ }
  else if (strcasecmp (option, "none") == 0)
    { /* ignored */ }
  else if (strcasecmp (option, "krb5") == 0 ||
	   strcasecmp (option, "use_krb5") == 0)
    {
      if (strcmp (service, "auth") == 0)
	auth->use_krb5 = 1;
      else if (strcmp (service, "account") == 0)
	account->use_krb5 = 1;
      else if (strcmp (service, "password") == 0)
	password->use_krb5 = 1;
      else if (strcmp (service, "session") == 0)
	session->use_krb5 = 1;
      else
	fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
    }
  else if (strcasecmp (option, "ldap") == 0 ||
	   strcasecmp (option, "use_ldap") == 0)
    {
      if (strcmp (service, "auth") == 0)
	auth->use_ldap = 1;
      else if (strcmp (service, "account") == 0)
	account->use_ldap = 1;
      else if (strcmp (service, "password") == 0)
	password->use_ldap = 1;
      else if (strcmp (service, "session") == 0)
	session->use_ldap = 1;
      else
	fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
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
	      if (strcmp (service, "auth") == 0)
		auth->use_ldap = 1;
	      else if (strcmp (service, "account") == 0)
		account->use_ldap = 1;
	      else if (strcmp (service, "password") == 0)
		password->use_ldap = 1;
	      else if (strcmp (service, "session") == 0)
		session->use_ldap = 1;
	      else
		fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
	    }
	  else if (strcmp (cp, "krb5") == 0)
	    {
	      if (strcmp (service, "auth") == 0)
		auth->use_krb5 = 1;
	      else if (strcmp (service, "account") == 0)
		account->use_krb5 = 1;
	      else if (strcmp (service, "password") == 0)
		password->use_krb5 = 1;
	      else if (strcmp (service, "session") == 0)
		session->use_krb5 = 1;
	      else
		fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
	    }
	  else
	    {
	      /* XXX this can be more then one. */
	      if (strcmp (service, "auth") == 0)
		auth->unix2_call_modules = strdup (cp);
	      else if (strcmp (service, "account") == 0)
		account->unix2_call_modules = strdup (cp);
	      else if (strcmp (service, "password") == 0)
		password->unix2_call_modules = strdup (cp);
	      else if (strcmp (service, "session") == 0)
		session->unix2_call_modules = strdup (cp);
	      else
		fprintf (stderr, _("ERROR: Unknown service: [%s: %s], ignored!\n"), service, option);
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
		      const char *option,
		      config_file_t *account __attribute__ ((unused)),
		      config_file_t *auth __attribute__((unused)),
		      config_file_t *password,
		      config_file_t *session __attribute__((unused)))
{
  if (debug)
    printf ("***** parse_option_pwcheck (%s, %s, %s, ...)\n",
	    file, service, option);

  if (strcmp (service, "password") != 0)
    {
      fprintf (stderr,
	       _("WARNING: Unknown option (%s): [%s: %s], ignored!\n"),
	       file, service, option);
      return;
    }

  if (strcasecmp (option, "debug") == 0)
    password->pwcheck_debug = 1;
  else if (strcasecmp (option, "nullok") == 0)
    password->pwcheck_nullok = 1;
  else if (strcasecmp (option, "cracklib") == 0 ||
	   strcasecmp (option, "use_cracklib") == 0)
    password->pwcheck_cracklib = 1;
  else if (strncasecmp (option, "cracklib=", 9) == 0)
    password->pwcheck_cracklib_path = strdup (&option[9]);
  else if (strncasecmp (option, "maxlen=", 7) == 0)
    password->pwcheck_maxlen = atoi (&option[7]);
  else if (strncasecmp (option, "minlen=", 7) == 0)
    password->pwcheck_minlen = atoi (&option[7]);
  else if (strncasecmp (option, "tries=", 6) == 0)
    password->pwcheck_tries = atoi (&option[6]);
  else if (strncasecmp (option, "remember=", 9) == 0)
    password->pwcheck_remember = atoi (&option[9]);
  else if (strncasecmp (option, "nisdir=", 7) == 0)
    password->pwcheck_nisdir = strdup (&option[7]);
  else if (strcasecmp (option, "not_set_pass") == 0)
    password->pwcheck_not_set_pass = 1;
  else if (strcasecmp (option, "use_first_pass") == 0)
    { /* ignored */ }
  else if (strcasecmp (option, "use_authtok") == 0)
    { /* ignored */ }
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
parse_file (const char *file, config_file_t *account, config_file_t *auth,
	    config_file_t *password, config_file_t *session,
	    void (parse_option)(const char *, const char *, const char *,
				config_file_t *, config_file_t *,
				config_file_t *, config_file_t *))
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
                parse_option (file, service, cp, auth, account, password, session);
              else
                {
                  *c = '\0';
		  parse_option (file, service, cp, auth, account, password, session);
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
load_obsolete_conf (config_file_t *account, config_file_t *auth,
		    config_file_t *password, config_file_t *session)
{
  if (debug)
    printf ("*** load_obsolete_conf (...)\n");

  if (parse_file ("/etc/security/pam_unix2.conf", account, auth,
		  password, session, parse_option_unix2) == -1)
    return -1;

  if (parse_file ("/etc/security/pam_pwcheck.conf", account, auth,
		  password, session, parse_option_pwcheck) == -1)
    return -1;

  return 0;
}
