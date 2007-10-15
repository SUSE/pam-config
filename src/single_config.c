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

  if (asprintf (&file, "%s/pam.d/%s", confdir, config_name) < 0)
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

int
write_single_config (const char *service, config_content_t **cfg_content)
{
  if (debug)
      printf("**** write_single_config (%s)**** \n", service);

  FILE *fp;
  fp = create_service_file (service);
  if (fp == NULL) return 1;

  while (cfg_content != NULL && *cfg_content != NULL)
  {
    fprintf (fp, "%s", (*cfg_content)->line);
    *cfg_content = (*cfg_content)->next;
  }
  return close_service_file (fp,gl_service);
}

int
insert_if (config_content_t **cfg, const char *line, int (*predicate)(config_content_t *next), insert_pos_t position)
{
  config_content_t *prev = NULL, *new_element = NULL, **head;
  config_content_t *cfg_content = *cfg;
  int result = FALSE, inserted = FALSE;

  if (debug) {
      char *l = strdup(line);
      printf("**** insert_if (%s)**** \n", strtok(l, "\n"));
      free(l);
  }

  /* save the list head */
  head = cfg;
  while (cfg_content != NULL && !inserted)
  {
    if (predicate (cfg_content))
    {
      new_element = malloc (sizeof (config_content_t));
      if (new_element)
      {
	new_element->line = strdup ( line );
	if (position==BEFORE)
	{
	  if (prev)
	  {
	    new_element->next = cfg_content;
	    prev->next = new_element;
	  }
	  else
	  {
	    /* new_element will be new list head */
	    /* switch data of head and new element and insert
	     * new_element after head -- which effectivly inserts
	     * new_element _before_ head. 
	     */
	    char *tmp = (*head)->line;
	    (*head)->line = new_element->line;
	    new_element->line = tmp;
	    new_element->next = (*head)->next;
	    (*head)->next = new_element;
	  }
	  result = TRUE;
	}
	else if(position==AFTER)
	{
	  prev = cfg_content->next;
	  cfg_content->next = new_element;
	  new_element->next = prev;
	  result = TRUE;
	}
      }
      inserted = TRUE;
    }
    /* skip ourselves, i.e. if a line is encountered that is equal
     * to the one that is to be inserted, we don't need to do
     * anything more.*/
    if (strcmp (cfg_content->line, line) == 0){
      result = TRUE;
      inserted = TRUE;
    }
    prev = cfg_content;
    cfg_content = cfg_content->next;
  }
  return result; 
}

int
remove_module (config_content_t **cfg, const char *module_name)
{
  config_content_t *prev = NULL, **head;
  config_content_t *cfg_content = *cfg;
  int removed = 0;

  if (debug)
      printf("**** remove_module (%s)**** \n", module_name);

  /* save list head */
  head = cfg;
  while (cfg_content != NULL)
  {
    if (strcasestr (cfg_content->line, module_name) != NULL)
    {
      /* found an element cotaining module_name */
      if (prev)
      {
	 if (debug)
            printf("REMOVE: %s", cfg_content->line); 
	prev->next = cfg_content->next;
	free (cfg_content);
	cfg_content = prev;
      }
      else
      {
	/* no prev means we remove list head */
	 if (debug)
            printf("REMOVE from head: %s", cfg_content->line); 
	config_content_t *succ = (*head)->next;
	(*head)->line = succ->line;
	(*head)->next = succ->next;
	free (succ);
	succ = NULL;
      }
      removed += 1;
    }
    prev = cfg_content;
    cfg_content = cfg_content->next;
  }
  if (debug)
    printf("Remove %d items\n", removed); 

  return removed;
}

static char *tmp_file;

FILE *
create_service_file (const char *service)
{
  FILE *fp;
  int fd;
  struct stat f_stat;
  char *conffile;

  if (asprintf (&tmp_file, "%s/pam.d/pam-config.tmpXXXXXX", confdir) < 0)
	  return NULL;

  if (asprintf (&conffile, "%s/pam.d/%s", confdir, service) < 0)
    return NULL;

  if (stat (conffile, &f_stat) != 0)
  {
    fprintf (stderr, _("Cannot stat '%s': %m\n"), conffile);
    free (tmp_file);
    free (conffile);
    return NULL;
  }

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

  if (asprintf (&conffile, "%s/pam.d/%s", confdir, service) < 0)
    return 1;

  if (asprintf (&oldfile, "%s/pam.d/%s.old", confdir, service) < 0)
    return 1;

  fclose (fp);

  rename (conffile, oldfile);
  rename (tmp_file, conffile);

  free (conffile);
  free (oldfile);
  free (tmp_file);
  return 0;
}
