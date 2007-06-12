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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "pam-config.h"

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
sanitize_check_auth (config_file_t *conf)
{
  if (conf->use_ccreds && !conf->use_ldap && !conf->use_krb5)
    fprintf (stderr,
	     _("WARNING: pam_ccreds.so needs LDAP or Kerberos5, ignored\n"));

  return 0;
}

int
sanitize_check_password (config_file_t *conf)
{
  if (conf->use_make && (conf->use_ldap ||conf->use_krb5))
    {
      fprintf (stderr,
	       _("ERROR: pam_make.so does not work with LDAP or Kerberos5.\n"));
      return 1;
    }
  return 0;
}
