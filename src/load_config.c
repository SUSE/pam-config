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

int
load_config (const char *file, write_type_t wtype,
             pam_module_t **module_list, int warn_unknown_mod)
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
	  pam_module_t *mod = lookup (module_list, module);

	  if (NULL != mod)
	    {
	      if (!mod->parse_config (mod, arguments, wtype))
		fprintf (stderr,
			 _("%s (%s): Arguments will be ignored\n"),
			 file, module);
	    }
	  else if (warn_unknown_mod || debug)
	    fprintf (stderr, _("%s: Unknown module %s, ignored!\n"),
		     file, module);
	}
    }

  fclose (fp);

  if (buf)
    free (buf);

  return 0;
}
