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

void
print_module_pwcheck (config_file_t *conf)
{
  if (!conf->use_pwcheck)
    return;

  printf ("password:");
  if (conf->pwcheck_debug)
    printf (" debug");
  if (conf->pwcheck_nullok)
    printf (" nullok");
  if (conf->pwcheck_cracklib_path)
    printf (" cracklib=%s", conf->pwcheck_cracklib_path);
  else if (conf->pwcheck_cracklib)
    printf (" cracklib");
  if (conf->pwcheck_maxlen)
    printf (" maxlen=%d", conf->pwcheck_maxlen);
  if (conf->pwcheck_minlen)
    printf (" minlen=%d", conf->pwcheck_minlen);
  if (conf->pwcheck_tries)
    printf (" tries=%d", conf->pwcheck_tries);
  if (conf->pwcheck_remember)
    printf (" remember=%d", conf->pwcheck_remember);
  if (conf->pwcheck_nisdir)
    printf (" nisdir=%s", conf->pwcheck_nisdir);
  if (conf->pwcheck_no_obscure_checks)
    printf( " no_obscure_checks");
  printf ("\n");
}

static void
print_module_unix2_int (const char *type, config_file_t *conf)
{
  if (!conf->use_unix2)
    return;

  printf ("%s:", type);
  if (conf->unix2_debug)
    printf (" debug");
  if (conf->unix2_nullok)
    printf (" nullok");
  if (conf->unix2_trace)
    printf (" trace");
  printf ("\n");
}

void
print_module_unix2 (config_file_t *account, config_file_t *auth,
		    config_file_t *password, config_file_t *session)
{
  print_module_unix2_int ("account", account);
  print_module_unix2_int ("auth", auth);
  print_module_unix2_int ("password", password);
  print_module_unix2_int ("session", session);
}

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

static void
print_module_ldap_int (const char *type, config_file_t *conf)
{
  if (!conf->use_ldap)
    return;

  printf ("%s:", type);
  if (conf->ldap_debug)
    printf (" debug");
  printf ("\n");
}

void
print_module_ldap (config_file_t *account, config_file_t *auth,
		    config_file_t *password, config_file_t *session)
{
  print_module_ldap_int ("account", account);
  print_module_ldap_int ("auth", auth);
  print_module_ldap_int ("password", password);
  print_module_ldap_int ("session", session);
}

static void
print_module_winbind_int (const char *type, config_file_t *conf)
{
  if (!conf->use_winbind)
    return;

  printf ("%s:", type);
  if (conf->winbind_debug)
    printf (" debug");
  printf ("\n");
}

void
print_module_winbind (config_file_t *account, config_file_t *auth,
		      config_file_t *password, config_file_t *session)
{
  print_module_winbind_int ("account", account);
  print_module_winbind_int ("auth", auth);
  print_module_winbind_int ("password", password);
  print_module_winbind_int ("session", session);
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
print_module_umask (config_file_t *conf)
{
  if (!conf->use_umask)
    return;

  printf ("session:");
  if (conf->umask_debug)
    printf (" debug");
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
