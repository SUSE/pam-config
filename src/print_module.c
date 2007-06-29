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

static void
print_module_krb5_int (const char *type, config_file_t *conf)
{
  if (!conf->use_krb5)
    return;

  printf ("%s:", type);
  if (conf->krb5_debug)
    printf (" debug");
  if (conf->krb5_minuid)
    printf (" minimum_uid=%u", conf->krb5_minuid);
  if (conf->krb5_ignore_unknown_principals)
    printf (" ignore_unknown_principals");
  printf ("\n");
}

void
print_module_krb5 (config_file_t *account, config_file_t *auth,
		    config_file_t *password, config_file_t *session)
{
  print_module_krb5_int ("account", account);
  print_module_krb5_int ("auth", auth);
  print_module_krb5_int ("password", password);
  print_module_krb5_int ("session", session);
}

void
print_module_cracklib (config_file_t *conf)
{
  if (!conf->use_cracklib)
    return;

  printf ("passwd:");
  if (conf->cracklib_debug)
    printf (" debug");
  if (conf->cracklib_dictpath)
    printf (" dictpath=%s", conf->cracklib_dictpath);
  if (conf->cracklib_retry)
    printf (" retry=%d", conf->cracklib_retry);
  printf ("\n");
}

void
print_module_capability (config_file_t *conf)
{
  if (!conf->use_capability)
    return;

  printf ("session:");
  if (conf->capability_debug)
    printf (" debug");
  if (conf->capability_conf)
    printf (" conf=%s", conf->capability_conf);
  printf ("\n");
}
