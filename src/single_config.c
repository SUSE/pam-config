/* Copyright (C) 2007 Thorsten Kukuk
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
#include <sys/stat.h>
#include <sys/types.h>

#include "pam-config.h"


int
load_single_config (const char *config_name, config_content_t **ptr)
{
  char *file;
  FILE *fp;
  char *buf = NULL;
  size_t buflen = 0;
  config_content_t *cptr = NULL;

  *ptr = NULL;

  if (asprintf (&file, CONFDIR"/pam.d/%s", config_name) < 0)
    return -1;

  if (debug)
    printf ("*** load_single_config (%s)\n", file);

  fp = fopen(file, "r");
  if (fp == NULL)
    {
      int err = errno;

      free (file);

      if (err == ENOENT)
	return 0;
      else
	return -1;
    }

  free (file);

  while (!feof (fp))
    {
      ssize_t n = getline (&buf, &buflen, fp);

      if (n < 1)
        break;

      if (debug)
	printf ("READ: %s", buf);

      if (cptr == NULL)
	{
	  cptr = malloc (sizeof (config_content_t));
	  *ptr = cptr;
	}
      else
	{
	  cptr->next = malloc (sizeof (config_content_t));
	  cptr = cptr->next;
	}
      cptr->line = strdup (buf);
      cptr->next = NULL;
    }

  fclose (fp);

  if (buf)
    free (buf);

  return 0;
}

static char *tmp_file;

FILE *
create_service_file (const char *service)
{
  FILE *fp;
  int fd;
  struct stat f_stat;
  char *conffile;

  tmp_file = strdup (CONFDIR"/pam.d/pam-config.tmpXXXXXX");

  if (asprintf (&conffile, CONFDIR"/pam.d/%s", service) < 0)
    return NULL;

  stat (conffile, &f_stat);
  free (conffile);
  fd = mkstemp (tmp_file);
  if (fchmod (fd, f_stat.st_mode) < 0)
    {
      fprintf (stderr, _("Cannot set permissions for '%s': %m\n"),
               tmp_file);
      unlink (tmp_file);
      free (tmp_file);
      return NULL;
    }
  if (fchown (fd, f_stat.st_uid, f_stat.st_gid) < 0)
    {
      fprintf (stderr,
               _("Cannot change owner/group for `%s': %m\n"),
               tmp_file);
      unlink (tmp_file);
      free (tmp_file);
      return NULL;
    }

  fp = fdopen (fd, "w");
  if (fp == NULL)
    {
      fprintf (stderr, _("Cannot create file handle: %m\n"));
      unlink (tmp_file);
      free (tmp_file);
      return NULL;
    }

  return fp;
}

int
close_service_file (FILE *fp, const char *service)
{
  char *conffile, *oldfile;

  if (asprintf (&conffile, CONFDIR"/pam.d/%s", service) < 0)
    return 1;

  if (asprintf (&oldfile, CONFDIR"/pam.d/%s.old", service) < 0)
    return 1;

  fclose (fp);

  rename (conffile, oldfile);
  rename (tmp_file, conffile);

  free (conffile);
  free (oldfile);
  free (tmp_file);
  return 0;
}
