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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "pam-config.h"
#include "pam-module.h"

/* Definitions of:
 *
 * -  parse_pwcheck_options
 * -  parse_unix2_options
 *
 * have gone into separate files: mod_<module-name>.c
 */
#ifdef NOTDEFINIED

static void
parse_krb5_options (config_file_t *conf, char *args)
{
  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	conf->krb5_debug = 1;
      else if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
      else if (strcmp (cp, "use_authtok") == 0)
	{ /* will be ignored */ }
      else if (strncmp (cp, "minimum_uid=", 12) == 0)
	conf->krb5_minuid = atoi (&cp[12]);
      else if (strcmp (cp, "ignore_unknown_principals") == 0)
	conf->krb5_ignore_unknown_principals = 1;
      else
	print_unknown_option_error ("pam_krb5.so", cp);
    }
  return;
}

static void
parse_winbind_options (config_file_t *conf, char *args)
{
  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	conf->winbind_debug = 1;
      else if (strcmp (cp, "try_first_pass") == 0)
	{ /* will be ignored */ }
      else if (strcmp (cp, "use_authtok") == 0)
	{ /* will be ignored */ }
      else if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
      else
	print_unknown_option_error ("pam_winbind.so", cp);
    }
  return;
}

static void
parse_cracklib_options (config_file_t *conf, char *args)
{
  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	conf->cracklib_debug = 1;
      else if (strcmp (cp, "use_authtok") == 0)
	{ /* will be ignored */ }
      else if (strncmp (cp, "dictpath=", 9) == 0)
	conf->cracklib_dictpath = strdup (&cp[9]);
      else if (strncmp (cp, "retry=", 6) == 0)
	conf->cracklib_retry = atoi (&cp[6]);
      else
	print_unknown_option_error ("pam_cracklib.so", cp);
    }
  return;
}

static void
parse_capability_options (config_file_t *conf, char *args)
{
  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	conf->capability_debug = 1;
      else if (strncmp (cp, "conf=", 5) == 0)
	conf->capability_conf = strdup (&cp[5]);
      else
	print_unknown_option_error ("pam_capability.so", cp);
    }
  return;
}
#endif

int
load_config (const char *file, write_type_t wtype,
             pam_module_t **module_list)
{
  FILE *fp;
  char *buf = NULL;
  size_t buflen = 0;
  const char *wanted = type2string (wtype);

  if (debug)
    printf ("*** load_config (%s, %s, ...)\n", file, wanted);

  fp = fopen(file, "r");
  if (fp == NULL)
    {
      if (errno == ENOENT)
	return 0;
      else
	return -1;
    }

  while (!feof (fp))
    {
      char *cp, *tmp, *type, *control, *module, *arguments;
      ssize_t n = getline (&buf, &buflen, fp);

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

      type = strsep (&cp, " \t");
      if (cp == NULL)
	{
	  fprintf (stderr, "%s: broken line: %s\n", file, buf);
	  fclose (fp);
	  free (buf);
	  return -1;
	}
      while (isspace ((int)*cp))
	++cp;

      if (*cp == '[')
	{
	  control = cp;
	  cp = strchr (cp, ']');
	  if (cp)
	    {
	      cp++;
	      *cp++ = '\0';
	    }
	}
      else
	control = strsep (&cp, " \t");
      if (cp == NULL)
	{
	  fprintf (stderr, "%s: broken line: '%s'\n", file, buf);
	  fclose (fp);
	  free (buf);
	  return -1;
	}
      while (isspace ((int)*cp))
	++cp;
      module = strsep (&cp, " \t");
      if (cp)
	{
	  while (isspace ((int)*cp))
	    ++cp;
	  if (strlen (cp) > 0)
	    arguments = cp;
	  else
	    arguments = NULL;
	}
      else
	arguments = NULL;

      if (debug)
	printf ("**** [%s, %s, %s, %s]\n", type, control, module,
		arguments?arguments:"");

      if (strcmp (type, wanted) == 0)
	{
	  handle_module( file, module, arguments, module_list, wtype );
	}

#if 0
      /* XXX remove every module converted */
      if (strcmp (module, "pam_pwcheck.so") == 0)
	{
	  conf->use_pwcheck = 1;
	  if (arguments)
	    parse_pwcheck_options (conf, arguments);
	}
      else if (strcmp (module, "pam_limits.so") == 0)
	{
	  conf->use_limits = 1;
	  if (arguments)
	    fprintf (stderr, _("%s (%s): Arguments will be ignored\n"),
		     file, module);
	}
      else if (strcmp (module, "pam_bioapi.so") == 0)
	{
	  conf->use_bioapi = 1;
	  if (arguments)
	    conf->bioapi_options = strdup (arguments);
	}
      else if (strcmp (module, "pam_env.so") == 0)
	{
	  conf->use_env = 1;
	  if (arguments)
	    fprintf (stderr, _("%s (%s): Arguments will be ignored\n"),
		     file, module);
	}
      else if (strcmp (module, "pam_make.so") == 0)
	{
	  conf->use_make = 1;
	  if (arguments)
	    conf->make_options = strdup (arguments);
	}
      else if (strcmp (module, "pam_krb5.so") == 0)
	{
	  conf->use_krb5 = 1;
	  if (arguments)
	    parse_krb5_options (conf, arguments);
	}
      else if (strcmp (module, "pam_winbind.so") == 0)
	{
	  conf->use_winbind = 1;
	  if (arguments)
	    parse_winbind_options (conf, arguments);
	}
      else if (strcmp (module, "pam_ccreds.so") == 0)
	conf->use_ccreds = 1;
      else if (strcmp (module, "pam_pkcs11.so") == 0)
	conf->use_pkcs11 = 1;
      else if (strcmp (module, "pam_apparmor.so") == 0)
	conf->use_apparmor = 1;
      else if (strcmp (module, "pam_cracklib.so") == 0)
	{
	  conf->use_cracklib = 1;
	  if (arguments)
	    parse_cracklib_options (conf, arguments);
	}
      else if (strcmp (module, "pam_nam.so") == 0)
	conf->use_lum = 1;
      else if (strcmp (module, "pam_capability.so") == 0)
	{
	  conf->use_capability = 1;
	  if (arguments)
	    parse_capability_options (conf, arguments);
	}
      else if (strcmp (module, "pam_localuser.so") == 0)
	{ /* ignore, used for account with pam_ldap.so */ }
      else
	{
	  fprintf (stderr, _("%s: Unknown module %s, ignored!\n"),
		   file, module);
	}
#endif
    }

  fclose (fp);

  if (buf)
    free (buf);

  return 0;
}
