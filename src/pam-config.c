/* Copyright (C) 2006-2021 Thorsten Kukuk
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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <getopt.h>
#include <errno.h>

#include <pam-config.h>

#include "supported-modules.h"

int debug = 0;
char *confdir = NULL;

static void
print_usage (FILE *stream, const char *program)
{
  fprintf (stream, _("Usage: %s -a|-c|-d [...]\n"),
           program);
}

/* Print the version information.  */
static void
print_version (const char *program, const char *years)
{
  fprintf (stdout, "%s (%s) %s\n", program, PACKAGE, VERSION);
  fprintf (stdout, _("\
Copyright (C) %s Thorsten Kukuk.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
"), years);
  /* fprintf (stdout, _("Written by %s.\n"), "Thorsten Kukuk"); */
}

static void
print_error (const char *program)
{
  fprintf (stderr,
           _("Try `%s --help' or `%s --usage' for more information.\n"),
           program, program);
}

static void
print_help (const char *program)
{
  print_usage (stdout, program);
  fprintf (stdout, _("%s - create PAM config files\n\n"), program);

  fputs (_("  -a, --add         Add options/PAM modules\n"), stdout);
  fputs (_("  -c, --create      Create new configuration\n"), stdout);
  fputs (_("  -d, --delete      Remove options/PAM modules\n"), stdout);
  fputs (_("      --confdir     Use a custom configuration directory\n"),
	 stdout);
  fputs (_("      --initialize  Convert old config and create new one\n"),
	 stdout);
  fputs (_("      --service config  Service to modify config of\n"),
	 stdout);
  fputs (_("      --update      Read current config and write them new\n"),
         stdout);
  fputs (_("      --verify      Read and verify current configuration\n"),
	 stdout);
  fputs (_("  -q, --query       Query for installed modules and options\n"),
	 stdout);
  fputs (_("      --list-modules  List all supported modules\n"),
         stdout);
  fputs (_("      --help        Give this help list\n"), stdout);
  fputs (_("  -u, --usage       Give a short usage message\n"), stdout);
  fputs (_("  -v, --version     Print program version\n"), stdout);

  fputs ("\n", stdout);
  fputs (_("Global Module Options:\n"), stdout);
  fputs (_("      The global modules get inserted into the \n      common-{account,auth,password,session} files which are included by\n      the single service files.\n"),
	 stdout);
  print_module_args (common_module_list);

  fputs ("\n", stdout);
  fputs (_("Single Service Module Options:\n"), stdout);
  fputs (_("      These modules can only be added to single service files.\n"),
	 stdout);
  print_module_args (service_module_list);
}

static void
print_xmlhelp (void)
{
  printf ("        <refsect3>\n");
  printf ("        <title>GLOBAL MODULES</title>\n");
  printf ("        <para>\n");
  printf ("          The global modules get inserted into the\n");
  printf ("          common-{account,auth,password,session} files\n");
  printf ("          which are included by the single service files.\n");
  printf ("        </para>\n");
  printf ("        <variablelist>\n");

  print_module_xmlhelp (common_module_list);

  printf ("        </variablelist>\n");
  printf ("      </refsect3>\n");
  printf ("      <refsect3>\n");
  printf ("        <title>SINGLE SERVICE MODULES</title>\n");
  printf ("        <para>\n");
  printf ("          These modules can only be added to single service files.\n");
  printf ("          See also <xref linkend=\"examples\"/>.\n");
  printf ("        </para>\n");
  printf ("        <variablelist>\n");

  print_module_xmlhelp (service_module_list);

  printf ("        </variablelist>\n");
  printf ("      </refsect3>\n");
}

static void
list_modules (pam_module_t **module_list)
{
  while (*module_list != NULL) {
    fprintf (stdout, "\t%s\n",(*module_list)->name);
    module_list++;
  };
}

static int
check_symlink (const char *sysconfdir, const char *file_pc, const char *file)
{
  char *config;

  if (asprintf (&config, "%s/pam.d/%s", sysconfdir, file) < 0)
    {
      fprintf (stderr, _("Out of memory\n"));
      return 1;
    }

  if (access (config, F_OK) == -1)
    {
	if (symlink (file_pc, config) != 0)
	{
	  fprintf (stderr,
		   _("Error activating %s (%m)\n"), config);
	      fprintf (stderr,
		       _("New config from %s/pam.d/%s is not in use!\n"),
		       sysconfdir, file_pc);
	      return 1;
	}
	return 0;
    }
  else
    {
      char buf[1024];

      memset (&buf, 0, sizeof (buf));
      if (readlink (config, buf, sizeof (buf)) <= 0 ||
          strcmp (file_pc, basename(buf)) != 0)
	{
	  fprintf (stderr,
		   _("File %s is no symlink to %s.\n"), config, file_pc);
	  fprintf (stderr,
		   _("New config from %s is not in use!\n"),
		   file_pc);
	}
      return 0;
    }
}

/* if 'file' exists, make backup from original file. symlink
 * autogenerated one to the original name.
 */
static int
relink (const char *sysconfdir, const char *file, const char *file_pc)
{
  char *config, *config_pc, *config_bak;

  fflush (stdout); /* make sure every message is printed to get consistent
		      log files.  */

  if (asprintf (&config, "%s/pam.d/%s", sysconfdir, file) < 0)
    {
      fprintf (stderr, _("Out of memory\n"));
      return 1;
    }

  if (asprintf (&config_pc, "%s/pam.d/%s", sysconfdir, file_pc) < 0)
    {
      fprintf (stderr, _("Out of memory\n"));
      free (config);
      return 1;
    }

  if (asprintf (&config_bak, "%s.pam-config-backup", config) < 0)
    {
      fprintf (stderr, _("Out of memory\n"));
      free (config);
      free (config_pc);
      return 1;
    }


  if (access (config, F_OK) != 0)
    {
      /* fprintf (stderr,
	       _("WARNING: file '%s' not found. Omitting backup.\n"),
	       file); */
    }
  else if (access (config_bak, F_OK) == 0)
    fprintf (stderr,
	     _("WARNING: Backup file '%s' already exist. Omitting backup.\n"),
	     config_bak);
  else if (link (config, config_bak) != 0)
    fprintf (stderr, _("ERROR: Cannot create backup file '%s' (%m)\n"),
	       config_bak);

  if (unlink (config) != 0 && errno != ENOENT)
    fprintf (stderr, _("ERROR: Cannot remove '%s' (%m)\n"), config);

  if (symlink (file_pc, config) != 0)
    {
      fprintf (stderr,
	       _("Error activating %s (%m)\n"), config);
      fprintf (stderr,
	       _("New config from %s is not in use!\n"), config_pc);
      return 1;
    }
  return 0;
}

static int
replace_obsolete_modules (pam_module_t **module_list)
{
  int with_unix2, with_pwcheck, with_cracklib;

  with_unix2 = is_module_enabled (module_list, "pam_unix2.so", ACCOUNT) |
    is_module_enabled (module_list, "pam_unix2.so", AUTH) |
    is_module_enabled (module_list, "pam_unix2.so", PASSWORD) |
    is_module_enabled (module_list, "pam_unix2.so", SESSION);

  with_pwcheck = is_module_enabled (module_list, "pam_pwcheck.so", ACCOUNT) |
    is_module_enabled (module_list, "pam_pwcheck.so", AUTH) |
    is_module_enabled (module_list, "pam_pwcheck.so", PASSWORD) |
    is_module_enabled (module_list, "pam_pwcheck.so", SESSION);

  with_cracklib = is_module_enabled (module_list, "pam_cracklib.so", PASSWORD);

  if (with_unix2 && check_for_pam_module ("pam_unix2.so", 0, 1) == 1)
    {
      option_set_t *opt_set, *opt_set2;
      pam_module_t *pam_unix2 = lookup (module_list, "pam_unix2.so");
      pam_module_t *pam_unix = lookup (module_list, "pam_unix.so");

      fprintf (stderr, "pam_unix2 used but not installed, replacing with pam_unix.so\n");

      /* Set and check sections.  */
      opt_set = pam_unix->get_opt_set (pam_unix, ACCOUNT);
      opt_set->enable (opt_set, "is_enabled", TRUE);
      opt_set->enable (opt_set, "nis", TRUE);
      opt_set2 = pam_unix2->get_opt_set (pam_unix2, ACCOUNT);
      opt_set2->enable (opt_set2, "is_enabled", FALSE);
      if (opt_set2->is_enabled (opt_set2, "debug"))
	opt_set->enable (opt_set, "debug", TRUE);
      if (opt_set2->is_enabled (opt_set2, "trace"))
	opt_set->enable (opt_set, "audit", TRUE);
      if (opt_set2->is_enabled (opt_set2, "nullok"))
	opt_set->enable (opt_set, "nullok", TRUE);
      if (sanitize_check_account (module_list, 0) != 0)
	return 1;

      opt_set = pam_unix->get_opt_set (pam_unix, AUTH);
      opt_set->enable (opt_set, "is_enabled", TRUE);
      opt_set->enable (opt_set, "nis", TRUE);
      opt_set2 = pam_unix2->get_opt_set (pam_unix2, AUTH);
      opt_set2->enable (opt_set2, "is_enabled", FALSE);
      if (opt_set2->is_enabled (opt_set2, "debug"))
	opt_set->enable (opt_set, "debug", TRUE);
      if (opt_set2->is_enabled (opt_set2, "trace"))
	opt_set->enable (opt_set, "audit", TRUE);
      if (opt_set2->is_enabled (opt_set2, "nullok"))
	opt_set->enable (opt_set, "nullok", TRUE);
      if (sanitize_check_auth (module_list, 0) != 0)
	return 1;

      opt_set = pam_unix->get_opt_set (pam_unix, PASSWORD);
      opt_set->enable (opt_set, "is_enabled", TRUE);
      opt_set->enable (opt_set, "nis", TRUE);
      opt_set->enable (opt_set, "shadow", TRUE);
      opt_set2 = pam_unix2->get_opt_set (pam_unix2, PASSWORD);
      opt_set2->enable (opt_set2, "is_enabled", FALSE);
      if (opt_set2->is_enabled (opt_set2, "debug"))
	opt_set->enable (opt_set, "debug", TRUE);
      if (opt_set2->is_enabled (opt_set2, "trace"))
	opt_set->enable (opt_set, "audit", TRUE);
      if (opt_set2->is_enabled (opt_set2, "nullok"))
	opt_set->enable (opt_set, "nullok", TRUE);
      if (sanitize_check_password (module_list, 0) != 0)
	return 1;

      opt_set = pam_unix->get_opt_set (pam_unix, SESSION);
      opt_set->enable (opt_set, "is_enabled", TRUE);
      opt_set->enable (opt_set, "nis", TRUE);
      opt_set2 = pam_unix2->get_opt_set (pam_unix2, SESSION);
      opt_set2->enable (opt_set2, "is_enabled", FALSE);
      if (opt_set2->is_enabled (opt_set2, "debug"))
	opt_set->enable (opt_set, "debug", TRUE);
      if (opt_set2->is_enabled (opt_set2, "trace"))
	opt_set->enable (opt_set, "audit", TRUE);
      if (opt_set2->is_enabled (opt_set2, "nullok"))
	opt_set->enable (opt_set, "nullok", TRUE);
      if (sanitize_check_session (module_list, 0) != 0)
	return 1;
    }

  /* If pam_cracklib is enabled and pam_pwquality.so installed, replace it
     with pam_pwquality. */
  if (with_cracklib && check_for_pam_module ("pam_pwquality.so", 0, 1) == 0)
    {
      char *cracklib_opts_bool[] = {"debug", "reject_username",
				    "gecoscheck", "enforce_for_root"};
      char *cracklib_opts_str[] = {"authtok_type", "retry", "difok",
				   "minlen", "dcredit",
				   "ucredit", "lcredit", "ocredit",
				   "minclass", "dictpath", "maxrepeat",
				   "maxsequence", "maxclassrepeat"};
      option_set_t *opt_cracklib, *opt_pwquality;
      pam_module_t *pam_cracklib = lookup (module_list, "pam_cracklib.so");
      pam_module_t *pam_pwquality = lookup (module_list, "pam_pwquality.so");

      fprintf (stderr, "pam_cracklib used, replacing with pam_pwquality.\n");

      opt_cracklib = pam_cracklib->get_opt_set (pam_cracklib, PASSWORD);
      opt_pwquality = pam_pwquality->get_opt_set (pam_pwquality, PASSWORD);

      opt_cracklib->enable (opt_cracklib, "is_enabled", FALSE);
      opt_pwquality->enable (opt_pwquality, "is_enabled", TRUE);

      for (size_t i = 0; i < sizeof (cracklib_opts_bool)/sizeof (char *); i++)

	opt_pwquality->enable (opt_pwquality, cracklib_opts_bool[i],
				opt_cracklib->is_enabled (opt_cracklib,
							  cracklib_opts_bool[i]));
      for (size_t i = 0; i < sizeof (cracklib_opts_str)/sizeof (char *); i++)

	opt_pwquality->set_opt (opt_pwquality, cracklib_opts_str[i],
				opt_cracklib->get_opt (opt_cracklib,
						       cracklib_opts_str[i]));

      if (sanitize_check_password (module_list, 0) != 0)
	return 1;
    }
  else if (with_cracklib && check_for_pam_module ("pam_cracklib.so", 0, 1) != 0)
    {
      /* pam_cracklib got removed and no pam_pwquality as replacement, remove it */
      pam_module_t *pam_cracklib = lookup (module_list, "pam_cracklib.so");
      option_set_t *opt_cracklib = pam_cracklib->get_opt_set (pam_cracklib, PASSWORD);

      fprintf (stderr, "pam_cracklib used but not installed, disabling.\n");

      opt_cracklib->enable (opt_cracklib, "is_enabled", FALSE);

      if (sanitize_check_password (module_list, 0) != 0)
	return 1;
    }

  if (with_pwcheck && check_for_pam_module ("pam_pwcheck.so", 0, 1) == 1)
    {
      option_set_t *opt_pwquality, *opt_pwcheck, *opt_pwhistory;
      pam_module_t *pam_pwcheck = lookup (module_list, "pam_pwcheck.so");
      pam_module_t *pam_pwquality = lookup (module_list, "pam_pwquality.so");
      pam_module_t *pam_pwhistory = lookup (module_list, "pam_pwhistory.so");

      fprintf (stderr, "pam_pwcheck used but not installed, replacing with pam_pwquality.\n");

      opt_pwhistory = pam_pwhistory->get_opt_set (pam_pwhistory, PASSWORD);
      opt_pwquality = pam_pwquality->get_opt_set (pam_pwquality, PASSWORD);
      opt_pwquality->enable (opt_pwquality, "is_enabled", TRUE);
      opt_pwcheck = pam_pwcheck->get_opt_set (pam_pwcheck, PASSWORD);
      opt_pwcheck->enable (opt_pwcheck, "is_enabled", FALSE);
      if (opt_pwcheck->is_enabled (opt_pwcheck, "debug"))
	opt_pwquality->enable (opt_pwquality, "debug", TRUE);

      /* check, if we need to enable pam_pwhistory */
      if (opt_pwcheck->get_opt (opt_pwcheck, "remember") != NULL &&
	  !is_module_enabled (module_list, "pam_pwhistory.so", PASSWORD))
	{
	  char *remember = opt_pwcheck->get_opt (opt_pwcheck, "remember");

	  opt_pwhistory->enable (opt_pwhistory, "is_enabled", TRUE);
	  opt_pwhistory->set_opt (opt_pwhistory, "remember", remember);
	  if (opt_pwcheck->is_enabled (opt_pwcheck, "debug"))
	    opt_pwhistory->enable (opt_pwhistory, "debug", TRUE);
	}
      if (sanitize_check_password (module_list, 0) != 0)
	return 1;
    }

  return 0;
}


char *gl_service = NULL;

int
main (int argc, char *argv[])
{
  const char *program = "pam-config";
  global_opt_t opt = {0, 0, 0, 0, 0, 0, 0, 0, 1};
  int retval = 0;
  option_set_t *opt_set;

  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  openlog (program, LOG_ODELAY | LOG_PID, LOG_AUTHPRIV);
  if (argc < 2)
    {
      print_error (program);
      return 1;
    }
  else if (strcmp (argv[1], "--debug") == 0)
    {
      debug = 1;
      argc--;
      argv++;
    }

  if (argc > 1 && strcmp (argv[1], "--confdir") == 0)
  {
	  if (argc < 3)
	  {
	          fprintf (stderr, _("ERROR: too few arguments\n"));
		  print_error (program);
		  return 1;
	  }

	  confdir = argv[2];
	  if(confdir[0] != '/')
	  {
                  fprintf (stderr, _("ERROR: confdir must be an absolute path\n"));
		  print_error(program);
		  return 1;
	  }
      argc--;
      argv++;
      argc--;
      argv++;
  }
  else
  {
	  confdir = strdup(CONFDIR);
  }

  if (argc < 2)
    {
      print_error (program);
      return 1;
    }
  if (strncmp (argv[1], "--service", 9) == 0)
    {
      if (argv[1][9] == '='){
	gl_service = &argv[1][10];
      }
      else
	{
	  if ( argc < 3 || argv[2][0] == '-')
	    {
	      print_error (program);
	      return 1;
	    }
	  gl_service = argv[2];
	  argc--;
	  argv++;
	}
      argc--;
      argv++;
    }
  if (argc < 2)
    {
      print_error (program);
      return 1;
    }

  if (strcmp (argv[1], "-a") == 0 || strcmp (argv[1], "--add") == 0)
    {
      opt.m_add = 1;
      argc--;
      argv++;
    }
  else if (strcmp (argv[1], "-c") == 0 || strcmp (argv[1], "--create") == 0)
    {
      if (gl_service)
	{
	  print_error (program);
	  return 1;
	}
      opt.m_create = 1;
      argc--;
      argv++;
    }
  else if (strcmp (argv[1], "-d") == 0 || strcmp (argv[1], "--delete") == 0)
    {
      opt.m_delete = 1;
      argc--;
      argv++;
      opt.opt_val = 0;
    }
  else if (strcmp (argv[1], "--initialize") == 0)
    {
      opt.m_init = 1;

      argc--;
      argv++;

      if (argc > 1 || gl_service)
	{
	  fprintf (stderr, _("ERROR: Too many arguments or incompatible service specified\n"));
	  print_error (program);
	  return 1;
	}

      /* Load old /etc/security/{pam_unix2,pam_pwcheck}.conf
	 files and delete them afterwards.  */
      if (load_obsolete_conf (common_module_list) != 0)
      {
	fprintf (stderr, _( "WARNING: Couldn't load old config files.\n"));
      }

      if (load_config (confdir, CONF_ACCOUNT, ACCOUNT, common_module_list, 1) != 0)
	{
	load_old_config_error:
	  fprintf (stderr, _("\nCouldn't load config file, aborted!\n"));
	  return 1;
	}
      if (load_config (confdir, CONF_AUTH, AUTH, common_module_list, 1) != 0)
	goto load_old_config_error;
      if (load_config (confdir, CONF_PASSWORD, PASSWORD, common_module_list, 1) != 0)
	goto load_old_config_error;
      if (load_config (confdir, CONF_SESSION, SESSION, common_module_list, 1) != 0)
	goto load_old_config_error;
      if (load_config (confdir, CONF_SESSION_NONLOGIN, SESSION, common_module_list, 1) != 0)
	goto load_old_config_error;
      if (load_config (confdir, CONF_POSTLOGIN_ACCOUNT, ACCOUNT, common_module_list, 1) != 0)
	goto load_old_config_error;
      if (load_config (confdir, CONF_POSTLOGIN_AUTH, AUTH, common_module_list, 1) != 0)
	goto load_old_config_error;
      if (load_config (confdir, CONF_POSTLOGIN_PASSWORD, PASSWORD, common_module_list, 1) != 0)
	goto load_old_config_error;
      if (load_config (confdir, CONF_POSTLOGIN_SESSION, SESSION, common_module_list, 1) != 0)
	goto load_old_config_error;
    }
  else if (strcmp (argv[1], "--update") == 0)
    {
      opt.m_update = 1;
      argc--;
      argv++;
    }
  else if (strcmp (argv[1], "--verify") == 0)
    {
      opt.m_verify = 1;
      argc--;
      argv++;

      if (argc > 1 || gl_service)
	{
	  print_error (program);
	  return 1;
	}
    }
  else if (strcmp (argv[1], "-q") == 0 || strcmp (argv[1], "--query") == 0)
    {
      opt.m_query = 1;
      argc--;
      argv++;
    }

  if (opt.m_add || opt.m_delete || opt.m_update || opt.m_query || opt.m_verify)
    {
      if (argc == 1 && !opt.m_update && !opt.m_query && !opt.m_verify)
	{
	  print_error (program);
	  return 1;
	}

      if (!gl_service)
	{
	  if (load_config (confdir, CONF_ACCOUNT_PC, ACCOUNT, common_module_list, 1) != 0)
	    {
	      if (load_config (confdir, CONF_ACCOUNT, ACCOUNT, common_module_list, 1) != 0)
		{
		  fprintf (stderr, _("\nCouldn't load '%s', aborted!\n"), CONF_ACCOUNT);
		  return 1;
		}
	    }
	  if (load_config (confdir, CONF_AUTH_PC, AUTH, common_module_list, 1) != 0)
	    {
	      if (load_config (confdir, CONF_AUTH, AUTH, common_module_list, 1) != 0)
		{
		  fprintf (stderr, _("\nCouldn't load '%s', aborted!\n"), CONF_AUTH);
		  return 1;
		}
	    }
	  if (load_config (confdir, CONF_PASSWORD_PC, PASSWORD, common_module_list, 1) != 0)
	    {
	      if (load_config (confdir, CONF_PASSWORD, PASSWORD, common_module_list, 1) != 0)
		{
		  fprintf (stderr, _("\nCouldn't load '%s', aborted!\n"), CONF_PASSWORD);
		  return 1;
		}
	    }
	  if (load_config (confdir, CONF_SESSION_PC, SESSION, common_module_list, 1) != 0)
	    {
	      if (load_config (confdir, CONF_SESSION, SESSION, common_module_list, 1) != 0)
		{
		  fprintf (stderr, _("\nCouldn't load '%s', aborted!\n"), CONF_SESSION);
		  return 1;
		}
	    }
	  if (load_config (confdir, CONF_SESSION_NONLOGIN_PC, SESSION, common_module_list, 1) != 0)
	    {
	      if (load_config (confdir, CONF_SESSION_NONLOGIN, SESSION, common_module_list, 1) != 0)
		{
		  fprintf (stderr, _("\nCouldn't load '%s', aborted!\n"), CONF_SESSION_NONLOGIN);
		  return 1;
		}
	    }
	  if (load_config (confdir, CONF_POSTLOGIN_ACCOUNT_PC, ACCOUNT, common_module_list, 1) != 0)
	    {
	      if (load_config (confdir, CONF_POSTLOGIN_ACCOUNT, ACCOUNT, common_module_list, 1) != 0)
		{
		  fprintf (stderr, _("\nCouldn't load '%s', aborted!\n"), CONF_POSTLOGIN_ACCOUNT);
		  return 1;
		}
	    }
	  if (load_config (confdir, CONF_POSTLOGIN_AUTH_PC, AUTH, common_module_list, 1) != 0)
	    {
	      if (load_config (confdir, CONF_POSTLOGIN_AUTH, AUTH, common_module_list, 1) != 0)
		{
		  fprintf (stderr, _("\nCouldn't load '%s', aborted!\n"), CONF_POSTLOGIN_AUTH);
		  return 1;
		}
	    }
	  if (load_config (confdir, CONF_POSTLOGIN_PASSWORD_PC, PASSWORD, common_module_list, 1) != 0)
	    {
	      if (load_config (confdir, CONF_POSTLOGIN_PASSWORD, PASSWORD, common_module_list, 1) != 0)
		{
		  fprintf (stderr, _("\nCouldn't load '%s', aborted!\n"), CONF_POSTLOGIN_PASSWORD);
		  return 1;
		}
	    }
	  if (load_config (confdir, CONF_POSTLOGIN_SESSION_PC, SESSION, common_module_list, 1) != 0)
	    {
	      if (load_config (confdir, CONF_POSTLOGIN_SESSION, SESSION, common_module_list, 1) != 0)
		{
		  fprintf (stderr, _("\nCouldn't load '%s', aborted!\n"), CONF_POSTLOGIN_SESSION);
		  return 1;
		}
	    }
	}
      else
	{
	  /* --service option given */
	  if (load_config (confdir, gl_service, ACCOUNT, service_module_list, 0) != 0)
	    {
	    load_config_error2:
	      fprintf (stderr,
		       _("\nCouldn't load config file '%s', aborted!\n"), gl_service);
	      return 1;
	    }
	  if (load_config (confdir, gl_service, AUTH, service_module_list, 0) != 0)
	    goto load_config_error2;
	  if (load_config (confdir, gl_service, PASSWORD, service_module_list, 0) != 0)
	    goto load_config_error2;
	  if (load_config (confdir, gl_service, SESSION, service_module_list, 0) != 0)
	    goto load_config_error2;
	}
    }
  while (1)
    {
      int c;
      int option_index = 0;
      static struct option common_long_options[] = {
        {"version",                   no_argument,       NULL,  'v' },
        {"usage",                     no_argument,       NULL,  'u' },
        {"force",                     no_argument,       NULL,  'f' },
	{"debug",                     no_argument,       NULL,  254 },
        {"help",                      no_argument,       NULL,  255 },
        {"list-modules",              no_argument,       NULL,  300 },
	{"xmloutput",                 no_argument,       NULL,  301 },
	{"nullok",                    no_argument,       NULL,  900 },
	{"pam-debug",                 no_argument,       NULL,  901 },
	{"ldap",                      no_argument,       NULL, 1900 },
 	{"ldap-account_only",     no_argument,       NULL, 1902 },
	{"nam",                   no_argument,       NULL, 2030 },
        {"winbind",               no_argument,       NULL, 2200 },
        {"winbind-debug",         no_argument,       NULL, 2201 },
	{"sss",                   no_argument,       NULL, 2300 },
        {NULL,                    0,                 NULL,    0 }
      };
      static struct option service_long_options[] = {
        {"version",               no_argument,       NULL,  'v' },
        {"usage",                 no_argument,       NULL,  'u' },
        {"force",                 no_argument,       NULL,  'f' },
	{"debug",                 no_argument,       NULL,  254 },
        {"help",                  no_argument,       NULL,  255 },
	{"nullok",                no_argument,       NULL,  900 },
	{"pam-debug",             no_argument,       NULL,  901 },
	{"cryptpass",		  no_argument,	     NULL, 3200 },
	{"cryptpass-password",	  no_argument,	     NULL, 3201 },
	{NULL,                    0,                 NULL,    0 }
      };

      /* Don't let getopt print error messages, we do it ourself. */
      opterr = 0;

      if (!gl_service)
	c = getopt_long (argc, argv, "fvu",
			 common_long_options, &option_index);
      else
	c = getopt_long (argc, argv, "fvu",
			 service_long_options, &option_index);

      if (c == (-1))
	break;

      switch (c)
	{
	case 'f':
	  opt.force = 1;
	  break;
	case 900: /* --nullok */
	  {
	    pam_module_t **modptr = common_module_list;

	    while (*modptr != NULL)
	      {
		opt_set = (*modptr)->get_opt_set (*modptr, AUTH);
		opt_set->enable (opt_set, "nullok", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, ACCOUNT);
		opt_set->enable (opt_set, "nullok", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, PASSWORD);
		opt_set->enable (opt_set, "nullok", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, SESSION);
		opt_set->enable (opt_set, "nullok", opt.opt_val);
		++modptr;
	      }

	    modptr = service_module_list;

	    while (*modptr != NULL)
	      {
		opt_set = (*modptr)->get_opt_set (*modptr, AUTH);
		opt_set->enable (opt_set, "nullok", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, ACCOUNT);
		opt_set->enable (opt_set, "nullok", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, PASSWORD);
		opt_set->enable (opt_set, "nullok", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, SESSION);
		opt_set->enable (opt_set, "nullok", opt.opt_val);

		++modptr;
	      }
	  }
	  break;
	case 901: /* --pam-debug */
	  {
	    pam_module_t **modptr = common_module_list;

	    while (*modptr != NULL)
	      {
		opt_set = (*modptr)->get_opt_set (*modptr, AUTH);
		opt_set->enable (opt_set, "debug", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, ACCOUNT);
		opt_set->enable (opt_set, "debug", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, PASSWORD);
		opt_set->enable (opt_set, "debug", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, SESSION);
		opt_set->enable (opt_set, "debug", opt.opt_val);

		++modptr;
	      }

	    modptr = service_module_list;

	    while (*modptr != NULL)
	      {
		opt_set = (*modptr)->get_opt_set (*modptr, AUTH);
		opt_set->enable (opt_set, "debug", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, ACCOUNT);
		opt_set->enable (opt_set, "debug", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, PASSWORD);
		opt_set->enable (opt_set, "debug", opt.opt_val);
		opt_set = (*modptr)->get_opt_set (*modptr, SESSION);
		opt_set->enable (opt_set, "debug", opt.opt_val);

		++modptr;
	      }
	  }
	  break;
	case 1902:
	  /* pam_ldap account_only*/

	  if (opt.m_query)
	  {
		  /* if AUTH, PASSWD or SESSION is enabled it cannot be account_only */
		  opt_set = mod_pam_ldap.get_opt_set (&mod_pam_ldap, AUTH);
		  if (opt_set->is_enabled (opt_set, "is_enabled"))
			  break;
	      opt_set = mod_pam_ldap.get_opt_set (&mod_pam_ldap, PASSWORD);
	      if (opt_set->is_enabled (opt_set, "is_enabled"))
			  break;
	      opt_set = mod_pam_ldap.get_opt_set (&mod_pam_ldap, SESSION);
	      if (opt_set->is_enabled (opt_set, "is_enabled"))
			  break;
	      print_module_config (common_module_list, "pam_ldap.so");
	  }
	  else
	    {
	      if (!opt.m_delete && check_for_pam_module ("pam_ldap.so", opt.force, 0) != 0)
		return 1;
	      opt_set = mod_pam_ldap.get_opt_set (&mod_pam_ldap, ACCOUNT);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	      opt_set = mod_pam_ldap.get_opt_set (&mod_pam_ldap, AUTH);
	      opt_set->enable (opt_set, "is_enabled", FALSE);
	      opt_set = mod_pam_ldap.get_opt_set (&mod_pam_ldap, PASSWORD);
	      opt_set->enable (opt_set, "is_enabled", FALSE);
	      opt_set = mod_pam_ldap.get_opt_set (&mod_pam_ldap, SESSION);
	      opt_set->enable (opt_set, "is_enabled", FALSE);
	      opt_set =
		mod_pam_localuser.get_opt_set (&mod_pam_localuser, ACCOUNT);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	    }
	  break;
	case 2030:
	  /* pam_nam.so */
	  if (opt.m_query)
	    print_module_config (common_module_list, "pam_nam.so");
	  else
	    {
	      if (!opt.m_delete && check_for_pam_module ("pam_nam.so", opt.force, 0) != 0)
		return 1;

	      opt_set = mod_pam_nam.get_opt_set (&mod_pam_nam, ACCOUNT);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	      opt_set = mod_pam_nam.get_opt_set (&mod_pam_nam, AUTH);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	      opt_set = mod_pam_nam.get_opt_set (&mod_pam_nam, PASSWORD);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	      opt_set = mod_pam_nam.get_opt_set (&mod_pam_nam, SESSION);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	      opt_set =
		mod_pam_localuser.get_opt_set (&mod_pam_localuser, ACCOUNT);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	    }
	  break;
	case 2200:
	  /* pam_winbind */
	  if (opt.m_query)
            print_module_config (common_module_list, "pam_winbind.so");
	  else
	    {
	      if (!opt.m_delete && check_for_pam_module ("pam_winbind.so", opt.force, 0) != 0)
		return 1;
              opt_set = mod_pam_winbind.get_opt_set (&mod_pam_winbind,
						     ACCOUNT);
              opt_set->enable (opt_set, "is_enabled", opt.opt_val);
              opt_set = mod_pam_winbind.get_opt_set (&mod_pam_winbind,
						     AUTH);
              opt_set->enable (opt_set, "is_enabled", opt.opt_val);
              opt_set = mod_pam_winbind.get_opt_set (&mod_pam_winbind,
						     PASSWORD);
              opt_set->enable (opt_set, "is_enabled", opt.opt_val);
              opt_set = mod_pam_winbind.get_opt_set (&mod_pam_winbind,
						     SESSION);
              opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	      opt_set =
		mod_pam_localuser.get_opt_set (&mod_pam_localuser, ACCOUNT);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	    }
	  break;
	case 2300:
	  /* pam_sss */
	  if (opt.m_query)
	    print_module_config (common_module_list, "pam_sss.so");
	  else
	    {
	      if (!opt.m_delete && check_for_pam_module ("pam_sss.so", opt.force, 0) != 0)
		return 1;
	      opt_set = mod_pam_sss.get_opt_set (&mod_pam_sss, ACCOUNT);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	      opt_set = mod_pam_sss.get_opt_set (&mod_pam_sss, AUTH);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	      opt_set = mod_pam_sss.get_opt_set (&mod_pam_sss, PASSWORD);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	      opt_set = mod_pam_sss.get_opt_set (&mod_pam_sss, SESSION);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	      opt_set =
		mod_pam_localuser.get_opt_set (&mod_pam_localuser, ACCOUNT);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	    }
	  break;
	  /* From here we have single service modules */
	case 3200:
	  /* pam_cryptpass.so */
	  if (opt.m_query)
	    print_module_config (service_module_list, "pam_cryptpass.so");
	  else
	    {
	      if (!opt.m_delete && check_for_pam_module ("pam_cryptpass.so", opt.force, 0) != 0)
		return 1;
	      opt_set = mod_pam_cryptpass.get_opt_set (&mod_pam_cryptpass, SESSION);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	    }
	  break;
	case 3201:
	  /* pam_cryptpass.so */
	  if (opt.m_query)
	    print_module_config (service_module_list, "pam_cryptpass.so");
	  else
	    {
	      if (!opt.m_delete && check_for_pam_module ("pam_cryptpass.so", opt.force, 0) != 0)
		return 1;
	      opt_set = mod_pam_cryptpass.get_opt_set (&mod_pam_cryptpass, PASSWORD);
	      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	    }
	  break;
	case 254:
	  debug = 1;
	  break;
	case 255:
          print_help (program);
          return 0;
	case 300:
	  fprintf (stdout,_("Supported common modules:\n"));
	  list_modules (common_module_list);
	  fprintf (stdout,_("\nSupported service modules:\n"));
	  list_modules (service_module_list);
	  return 0;
	case 301:
          print_xmlhelp ();
          return 0;
        case 'v':
          print_version (program, "2014");
          return 0;
        case 'u':
          print_usage (stdout, program);
	  return 0;
	default:
	  {
	    int notfound;

	    if (gl_service)
	      notfound = module_getopt (service_module_list,
					argv[optind-1], &opt);
	    else
	      notfound = module_getopt (common_module_list,
					argv[optind-1], &opt);

	    if (notfound)
	      {
		if (notfound == 1)
                  {
		     fprintf (stderr, "%s: invalid option -- %s\n",
			      program, argv[optind -1]);
		     print_error (program);
		  }
		return 1;
	      }
	  }
	}
    }

  argc -= optind;
  argv += optind;

  if (argc > 0)
    {
      fprintf (stderr, _("%s: Too many arguments.\n"), program);
      print_error (program);
      return 1;
    }

  if (opt.m_add + opt.m_create + opt.m_delete + opt.m_init + opt.m_update +
      opt.m_query + opt.m_verify != 1)
    {
      print_error (program);
      return 1;
    }


  if (opt.m_query)
    return 0;

  if (opt.m_verify)
    {
      pam_module_t **modptr = common_module_list;
      retval = 0;

      /* Check sections.  */
      if (sanitize_check_account (common_module_list, 1) != 0)
	retval = 1;

      if (sanitize_check_auth (common_module_list, 1) != 0)
	retval = 1;

      if (sanitize_check_password (common_module_list, 1) != 0)
	retval = 1;

      if (sanitize_check_session (common_module_list, 1) != 0)
	retval = 1;


      while (*modptr != NULL)
	{
	  option_set_t *opt_set_auth =
	    (*modptr)->get_opt_set (*modptr, AUTH);
	  option_set_t *opt_set_account =
	    (*modptr)->get_opt_set (*modptr, ACCOUNT);
	  option_set_t *opt_set_password =
	    (*modptr)->get_opt_set (*modptr, PASSWORD);
	  option_set_t *opt_set_session =
	    (*modptr)->get_opt_set (*modptr, SESSION);

	  if (opt_set_auth->is_enabled (opt_set_auth, "is_enabled") ||
	      opt_set_account->is_enabled (opt_set_account, "is_enabled") ||
	      opt_set_password->is_enabled (opt_set_password, "is_enabled") ||
	      opt_set_session->is_enabled (opt_set_session, "is_enabled"))
	    {
	      if (check_for_pam_module ((*modptr)->name, 0, 0))
		retval = 1;
	    }
	  ++modptr;
	}

      return retval;
    }

  if (opt.m_create)
    {
      /* Set and check sections.  */
      opt_set = mod_pam_unix.get_opt_set (&mod_pam_unix, ACCOUNT);
      opt_set->enable (opt_set, "is_enabled", TRUE);
      if (sanitize_check_account (common_module_list, 0) != 0)
	return 1;

      opt_set = mod_pam_unix.get_opt_set (&mod_pam_unix, AUTH);
      opt_set->enable (opt_set, "is_enabled", TRUE);
      if (sanitize_check_auth (common_module_list, 0) != 0)
	return 1;

      if (check_for_pam_module ("pam_pwquality.so", 0, 0) == 0)
	{
	  opt_set = mod_pam_pwquality.get_opt_set (&mod_pam_pwquality, PASSWORD);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	}
      opt_set = mod_pam_unix.get_opt_set (&mod_pam_unix, PASSWORD);
      opt_set->enable (opt_set, "is_enabled", TRUE);
      opt_set->enable (opt_set, "nullok", TRUE);
      opt_set->enable (opt_set, "shadow", TRUE);
      if (sanitize_check_password (common_module_list, 0) != 0)
	return 1;

      opt_set = mod_pam_unix.get_opt_set (&mod_pam_unix, SESSION);
      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
      opt_set = mod_pam_env.get_opt_set (&mod_pam_env, SESSION);
      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
      opt_set = mod_pam_umask.get_opt_set (&mod_pam_umask, SESSION);
      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
      if (check_for_pam_module ("pam_limits.so", 0, 0) == 0)
	{
	  opt_set = mod_pam_limits.get_opt_set (&mod_pam_limits, SESSION);
	  opt_set->enable (opt_set, "is_enabled", opt.opt_val);
	}
      if (check_for_pam_module ("pam_systemd.so", 0, 1) == 0)
	{
	  opt_set = mod_pam_systemd.get_opt_set (&mod_pam_systemd, SESSION);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	}
      if (sanitize_check_session (common_module_list, 0) != 0)
	return 1;

      /* Write sections */
      if (write_config (confdir, CONF_ACCOUNT_PC, ACCOUNT, module_list_account) != 0)
	return 1;

      if (write_config (confdir, CONF_AUTH_PC, AUTH, module_list_auth) != 0)
	return 1;

      if (write_config (confdir, CONF_PASSWORD_PC, PASSWORD, module_list_password) != 0)
	return 1;

      if (write_config (confdir, CONF_SESSION_PC, SESSION, module_list_session) != 0)
	return 1;

      if (write_config (confdir, CONF_SESSION_NONLOGIN_PC, SESSION, module_list_session_nl) != 0)
	return 1;

      if (write_config (confdir, CONF_POSTLOGIN_ACCOUNT_PC, ACCOUNT, module_list_postlogin_account) != 0)
	return 1;

      if (write_config (confdir, CONF_POSTLOGIN_AUTH_PC, AUTH, module_list_postlogin_auth) != 0)
	return 1;

      if (write_config (confdir, CONF_POSTLOGIN_PASSWORD_PC, PASSWORD, module_list_postlogin_password) != 0)
	return 1;

      if (write_config (confdir, CONF_POSTLOGIN_SESSION_PC, SESSION, module_list_postlogin_session) != 0)
	return 1;
    }
  else if (!gl_service)
    {
      /* Replace obsolete modules first */
      replace_obsolete_modules (common_module_list);

      /* Check sections.  */
      if (sanitize_check_account (common_module_list, 0) != 0)
	return 1;

      if (sanitize_check_auth (common_module_list, 0) != 0)
	return 1;

      if (sanitize_check_password (common_module_list, 0) != 0)
	return 1;

      if (sanitize_check_session (common_module_list, 0) != 0)
	return 1;

      /* Write sections.  */
      if (write_config (confdir, CONF_ACCOUNT_PC, ACCOUNT, module_list_account) != 0)
	return 1;

      if (write_config (confdir, CONF_AUTH_PC, AUTH, module_list_auth) != 0)
	return 1;

      if (write_config (confdir, CONF_PASSWORD_PC, PASSWORD, module_list_password) != 0)
	return 1;

      if (write_config (confdir, CONF_SESSION_PC, SESSION, module_list_session) != 0)
	return 1;

      if (write_config (confdir, CONF_SESSION_NONLOGIN_PC, SESSION, module_list_session_nl) != 0)
	return 1;

      if (write_config (confdir, CONF_POSTLOGIN_ACCOUNT_PC, ACCOUNT, module_list_postlogin_account) != 0)
	return 1;

      if (write_config (confdir, CONF_POSTLOGIN_AUTH_PC, AUTH, module_list_postlogin_auth) != 0)
	return 1;

      if (write_config (confdir, CONF_POSTLOGIN_PASSWORD_PC, PASSWORD, module_list_postlogin_password) != 0)
	return 1;

      if (write_config (confdir, CONF_POSTLOGIN_SESSION_PC, SESSION, module_list_postlogin_session) != 0)
	return 1;
    }
  else
    {
      /* Write new single service files */
      pam_module_t **modptr = service_module_list;

      if (debug)
		  printf ("*** write_config (%s/pam.d/%s)\n", confdir, gl_service);

      /*
       * Note that the modules in service_module_list[]
       * do not use the "op" and the "fp" parameters.
       */
      while (*modptr != NULL)
	{
	  retval |= (*modptr)->write_config (*modptr, -1, NULL);
	  ++modptr;
	}
    }

  if (opt.m_init || (opt.m_create && opt.force))
    {
      if (relink (confdir, CONF_ACCOUNT, CONF_ACCOUNT_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_AUTH, CONF_AUTH_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_PASSWORD, CONF_PASSWORD_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_SESSION, CONF_SESSION_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_SESSION_NONLOGIN, CONF_SESSION_NONLOGIN_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_POSTLOGIN_ACCOUNT, CONF_POSTLOGIN_ACCOUNT_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_POSTLOGIN_AUTH, CONF_POSTLOGIN_AUTH_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_POSTLOGIN_PASSWORD, CONF_POSTLOGIN_PASSWORD_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_POSTLOGIN_SESSION, CONF_POSTLOGIN_SESSION_PC) != 0)
	retval = 1;

      if (opt.m_init && retval == 0)
	{
	  rename ("/etc/security/pam_pwcheck.conf",
		  "/etc/security/pam_pwcheck.conf.pam-config-backup");
	  rename ("/etc/security/pam_unix2.conf",
		  "/etc/security/pam_unix2.conf.pam-config-backup");
	}
      return retval;
    }
  else if (opt.force && !gl_service)
    {
      if (relink (confdir, CONF_ACCOUNT, CONF_ACCOUNT_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_AUTH, CONF_AUTH_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_PASSWORD, CONF_PASSWORD_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_SESSION, CONF_SESSION_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_SESSION_NONLOGIN, CONF_SESSION_NONLOGIN_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_POSTLOGIN_ACCOUNT, CONF_POSTLOGIN_ACCOUNT_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_POSTLOGIN_AUTH, CONF_POSTLOGIN_AUTH_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_POSTLOGIN_PASSWORD, CONF_POSTLOGIN_PASSWORD_PC) != 0)
	retval = 1;

      if (relink (confdir, CONF_POSTLOGIN_SESSION, CONF_POSTLOGIN_SESSION_PC) != 0)
	retval = 1;
    }

  if (!gl_service)
    {
      if (check_symlink (confdir, CONF_ACCOUNT_PC, CONF_ACCOUNT) != 0)
	retval = 1;
      if (check_symlink (confdir, CONF_AUTH_PC, CONF_AUTH) != 0)
	retval = 1;
      if (check_symlink (confdir, CONF_PASSWORD_PC, CONF_PASSWORD) != 0)
	retval = 1;
      if (check_symlink (confdir, CONF_SESSION_PC, CONF_SESSION) != 0)
	retval = 1;
      if (check_symlink (confdir, CONF_SESSION_NONLOGIN_PC, CONF_SESSION_NONLOGIN) != 0)
	retval = 1;

      if (check_symlink (confdir, CONF_POSTLOGIN_ACCOUNT_PC, CONF_POSTLOGIN_ACCOUNT) != 0)
	retval = 1;
      if (check_symlink (confdir, CONF_POSTLOGIN_AUTH_PC, CONF_POSTLOGIN_AUTH) != 0)
	retval = 1;
      if (check_symlink (confdir, CONF_POSTLOGIN_PASSWORD_PC, CONF_POSTLOGIN_PASSWORD) != 0)
	retval = 1;
      if (check_symlink (confdir, CONF_POSTLOGIN_SESSION_PC, CONF_POSTLOGIN_SESSION) != 0)
	retval = 1;
    }

  return retval;
}
