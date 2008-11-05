/* Copyright (C) 2006, 2007, 2008 Thorsten Kukuk
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

#include <pam-config.h>

#include "supported-modules.h"

int debug = 0;
char *confdir;
char *conf_account = NULL;
char *conf_account_pc = NULL;
char *conf_auth = NULL;
char *conf_auth_pc = NULL;
char *conf_password = NULL;
char *conf_password_pc = NULL;
char *conf_session = NULL;
char *conf_session_pc = NULL;

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
check_symlink (const char *old, const char *new)
{
  if (access (new, F_OK) == -1)
    {
      if (symlink (old, new) != 0)
	{
	  fprintf (stderr,
		   _("Error activating %s (%m)\n"), new);
	      fprintf (stderr,
		       _("New config from %s is not in use!\n"),
		       old);
	      return 1;
	}
      return 0;
    }
  else
    {
      char buf[1024];

      memset (&buf, 0, sizeof (buf));
      if (readlink (new, buf, sizeof (buf)) <= 0 ||
          strcmp (old, buf) != 0)
	{
	  fprintf (stderr,
		   _("File %s is no symlink to %s.\n"), new, old);
	  fprintf (stderr,
		   _("New config from %s is is not in use!\n"),
		   old);
	  return 1;
	}
      return 0;
    }
}

/* if 'file' exists, make backup from original file. symlink
 * autogenerated one to the original name.
 */
static int
relink (const char *file, const char *file_pc, const char *file_bak)
{
  fflush (stdout); /* make sure every message is printed to get consistent
		      log files.  */

  if (access (file, F_OK) != 0)
    fprintf (stderr, _("WARNING: file '%s' not found. Omitting backup.\n"),
	     file);
  else if (access (file_bak, F_OK) == 0)
    fprintf (stderr,
	     _("WARNING: Backup file '%s' already exist. Omitting backup.\n"),
	     file_bak);
  else if (link (file, file_bak) != 0)
    fprintf (stderr, _("ERROR: Cannot create backup file '%s' (%m)\n"),
	       file_bak);

  if (unlink (file) != 0)
    fprintf (stderr, _("ERROR: Cannot remove '%s' (%m)\n"), file);

  if (symlink (file_pc, file) != 0)
    {
      fprintf (stderr,
	       _("Error activating %s (%m)\n"), file);
      fprintf (stderr,
	       _("New config from %s is not in use!\n"), file_pc);
      return 1;
    }
  return 0;
}

char *gl_service = NULL;

int
main (int argc, char *argv[])
{
  const char *program = "pam-config";
  global_opt_t opt = {0, 0, 0, 0, 0, 0, 0, 1};
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
		  print_error (program);
		  return 1;
	  }
	  
	  confdir = argv[2];
	  if(confdir[0] != '/')
	  {
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

  if (asprintf (&conf_account, "%s/pam.d/common-account", confdir) < 0)
  {
	  print_error (program);
	  return 1;
  }
  if (asprintf (&conf_account_pc, "%s/pam.d/common-account-pc", confdir) < 0)
  {
	  print_error (program);
	  return 1;
  }
  if (asprintf (&conf_auth, "%s/pam.d/common-auth", confdir) < 0)
  {
	  print_error (program);
	  return 1;
  }
  if (asprintf (&conf_auth_pc, "%s/pam.d/common-auth-pc", confdir) < 0)
  {
	  print_error (program);
	  return 1;
  }
  if (asprintf (&conf_password, "%s/pam.d/common-password", confdir) < 0)
  {
	  print_error (program);
	  return 1;
  }
  if (asprintf (&conf_password_pc, "%s/pam.d/common-password-pc", confdir) < 0)
  {
	  print_error (program);
	  return 1;
  }
  if (asprintf (&conf_session, "%s/pam.d/common-session", confdir) < 0)
  {
	  print_error (program);
	  return 1;
  }
  if (asprintf (&conf_session_pc, "%s/pam.d/common-session-pc", confdir) < 0)
  {
	  print_error (program);
	  return 1;
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
	  print_error (program);
	  return 1;
	}

      /* Load old /etc/security/{pam_unix2,pam_pwcheck}.conf
	 files and delete them afterwards.  */
      if ( load_obsolete_conf (common_module_list) != 0)
      {
	fprintf (stderr, _( "WARNING: Couldn't load old config files.\n"));
      }

      if (load_config (conf_account, ACCOUNT, common_module_list, 1) != 0)
	{
	load_old_config_error:
	  fprintf (stderr, _("\nCouldn't load config file, aborted!\n"));
	  return 1;
	}
      if (load_config (conf_auth, AUTH, common_module_list, 1) != 0)
	goto load_old_config_error;
      if (load_config (conf_password, PASSWORD, common_module_list, 1) != 0)
	goto load_old_config_error;
      if (load_config (conf_session, SESSION, common_module_list, 1) != 0)
	goto load_old_config_error;
    }
  else if (strcmp (argv[1], "--update") == 0)
    {
      opt.m_update = 1;
      argc--;
      argv++;
    }
  else if (strcmp (argv[1], "-q") == 0 || strcmp (argv[1], "--query") == 0)
    {
      opt.m_query = 1;
      argc--;
      argv++;
    }

  if (opt.m_add || opt.m_delete || opt.m_update || opt.m_query)
    {
      if (argc == 1 && !opt.m_update && !opt.m_query)
	{
	  print_error (program);
	  return 1;
	}

      if (!gl_service)
	{
	  if (load_config (conf_account_pc, ACCOUNT, common_module_list, 1) != 0)
	    {
	    load_config_error:
	      fprintf (stderr, _("\nCouldn't load config file, aborted!\n"));
	      return 1;
	    }
	  if (load_config (conf_auth_pc, AUTH, common_module_list, 1) != 0)
	    goto load_config_error;
	  if (load_config (conf_password_pc, PASSWORD, common_module_list, 1) != 0)
	    goto load_config_error;
	  if (load_config (conf_session_pc, SESSION, common_module_list, 1) != 0)
	    goto load_config_error;
	}
      else
	{
	  /* --service option given */
	  char *fname;

	  if (asprintf (&fname, "%s/pam.d/%s", confdir, gl_service) < 0)
	    return 1;

	  if (load_config (fname, ACCOUNT, service_module_list, 0) != 0)
	    {
	    load_config_error2:
	      fprintf (stderr,
		       _("\nCouldn't load config file '%s', aborted!\n"),
		       fname);
	      return 1;
	    }
	  if (load_config (fname, AUTH, service_module_list, 0) != 0)
	    goto load_config_error2;
	  if (load_config (fname, PASSWORD, service_module_list, 0) != 0)
	    goto load_config_error2;
	  if (load_config (fname, SESSION, service_module_list, 0) != 0)
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
	      if (!opt.m_delete && check_for_pam_module ("pam_ldap.so", opt.force) != 0)
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
	      if (!opt.m_delete && check_for_pam_module ("pam_nam.so", opt.force) != 0)
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
	      if (!opt.m_delete && check_for_pam_module ("pam_winbind.so", opt.force) != 0)
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
	  /* From here we have single service modules */
	case 3200:
	  /* pam_cryptpass.so */
	  if (opt.m_query)
	    print_module_config (service_module_list, "pam_cryptpass.so");
	  else
	    {
	      if (!opt.m_delete && check_for_pam_module ("pam_cryptpass.so", opt.force) != 0)
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
	      if (!opt.m_delete && check_for_pam_module ("pam_cryptpass.so", opt.force) != 0)
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
          print_version (program, "2008");
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

  if (opt.m_add + opt.m_create + opt.m_delete + opt.m_init + opt.m_update + opt.m_query != 1)
    {
      print_error (program);
      return 1;
    }


  if (opt.m_query)
    return 0;

  if (opt.m_create)
    {
      /* Set and check sections.  */
      opt_set = mod_pam_unix2.get_opt_set (&mod_pam_unix2, ACCOUNT);
      opt_set->enable (opt_set, "is_enabled", TRUE);
      if (sanitize_check_account (common_module_list) != 0)
	return 1;

      opt_set = mod_pam_unix2.get_opt_set (&mod_pam_unix2, AUTH);
      opt_set->enable (opt_set, "is_enabled", TRUE);
      if (sanitize_check_auth (common_module_list) != 0)
	return 1;

      opt_set = mod_pam_cracklib.get_opt_set (&mod_pam_cracklib, PASSWORD);
      if (opt_set->is_enabled (opt_set, "is_enabled"))
	{
	  opt_set = mod_pam_pwhistory.get_opt_set (&mod_pam_pwhistory, PASSWORD);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	  opt_set->enable (opt_set, "use_authtok", TRUE);
	}
      else
	{
	  opt_set = mod_pam_pwcheck.get_opt_set (&mod_pam_pwcheck, PASSWORD);
	  opt_set->enable (opt_set, "is_enabled", TRUE);
	  opt_set->enable (opt_set, "nullok", TRUE);
	}
      opt_set = mod_pam_unix2.get_opt_set (&mod_pam_unix2, PASSWORD);
      opt_set->enable (opt_set, "is_enabled", TRUE);
      opt_set->enable (opt_set, "nullok", TRUE);
      if (sanitize_check_password (common_module_list) != 0)
	return 1;

      opt_set = mod_pam_unix2.get_opt_set (&mod_pam_unix2, SESSION);
      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
      opt_set = mod_pam_limits.get_opt_set (&mod_pam_limits, SESSION);
      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
      opt_set = mod_pam_env.get_opt_set (&mod_pam_env, SESSION);
      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
      opt_set = mod_pam_umask.get_opt_set (&mod_pam_umask, SESSION);
      opt_set->enable (opt_set, "is_enabled", opt.opt_val);
      if (sanitize_check_session (common_module_list) != 0)
	return 1;

	  /* Write sections */
	  if (write_config (ACCOUNT, conf_account_pc, module_list_account) != 0)
		  return 1;

	  if (write_config (AUTH, conf_auth_pc, module_list_auth) != 0)
		  return 1;

	  if (write_config (PASSWORD, conf_password_pc, module_list_password) != 0)
		  return 1;

      if (write_config (SESSION, conf_session_pc, module_list_session) != 0)
	return 1;
    }
  else if (!gl_service)
    {
      /* Check sections.  */
      if (sanitize_check_account (common_module_list) != 0)
	return 1;

      if (sanitize_check_auth (common_module_list) != 0)
	return 1;

      if (sanitize_check_password (common_module_list) != 0)
	return 1;

      if (sanitize_check_session (common_module_list) != 0)
	return 1;

	  /* Write sections.  */
	  if (write_config (ACCOUNT, conf_account_pc, module_list_account) != 0)
		  return 1;

	  if (write_config (AUTH, conf_auth_pc, module_list_auth) != 0)
		  return 1;

	  if (write_config (PASSWORD, conf_password_pc, module_list_password) != 0)
		  return 1;

	  if (write_config (SESSION, conf_session_pc, module_list_session) != 0)
	return 1;
    }
  else
    {
      /* Write new single service files */
      pam_module_t **modptr = service_module_list;

      if (debug)
		  printf ("*** write_config (%s/pam.d/%s)\n", confdir, gl_service);

      /* Check if service file exists */
      char *conffile;
      if (asprintf (&conffile, "%s/pam.d/%s", confdir, gl_service) < 0)
	return 1;

      if (access (conffile, R_OK) != 0)
      {
	fprintf (stderr, _("Cannot access '%s': %m\n"), conffile);
	free (conffile);
	return 1;
      }
      free (conffile);

      while (*modptr != NULL)
	{
	  retval |= (*modptr)->write_config (*modptr, -1, NULL);
	  ++modptr;
	}
    }

  if (opt.m_init || (opt.m_create && opt.force))
    {
      char *bak = NULL;

      if (asprintf (&bak, "%s.pam-config-backup", conf_account) >= 0)
	{
	  if (relink (conf_account, conf_account_pc, bak) != 0)
	    retval = 1;

	  free(bak);
	  bak = NULL;
	}
      else
	retval = 1;

      if (asprintf (&bak, "%s.pam-config-backup", conf_auth) >= 0)
	{
	  if (relink (conf_auth, conf_auth_pc, bak) != 0)
	    retval = 1;

	  free(bak);
	  bak = NULL;
	}
      else
	retval = 1;

      if (asprintf (&bak, "%s.pam-config-backup", conf_password) >= 0)
	{
	  if (relink (conf_password, conf_password_pc, bak) != 0)
	    retval = 1;

	  free(bak);
	  bak = NULL;
	}
      else
	retval = 1;

      if (asprintf (&bak, "%s.pam-config-backup", conf_session) >= 0)
	{
	  if (relink (conf_session, conf_session_pc, bak) != 0)
	    retval = 1;

	  free(bak);
	  bak = NULL;

	}
      else
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
      fflush (stdout); /* Make sure output in logs is consistent */

      if (unlink (conf_account) != 0 ||
	  symlink (conf_account_pc, conf_account) != 0)
	{
	  fprintf (stderr,
		   _("Error activating %s (%m)\n"), conf_account);
	  fprintf (stderr,
		   _("New config from %s is not in use!\n"), conf_account_pc);
	  retval = 1;
	}

      if (unlink (conf_auth) != 0 ||
	  symlink (conf_auth_pc, conf_auth) != 0)
	{
	  fprintf (stderr,
		   _("Error activating %s (%m)\n"), conf_auth);
	  fprintf (stderr,
		   _("New config from %s is not in use!\n"), conf_auth_pc);
	  retval = 1;
	}

      if (unlink (conf_password) != 0 ||
	  symlink (conf_password_pc, conf_password) != 0)
	{
	  fprintf (stderr,
		   _("Error activating %s (%m)\n"), conf_password);
	  fprintf (stderr,
		   _("New config from %s is not in use!\n"),
		   conf_password_pc);
	  retval = 1;
	}

      if (unlink (conf_session) != 0 ||
	  symlink (conf_session_pc, conf_session) != 0)
	{
	  fprintf (stderr,
		   _("Error activating %s (%m)\n"), conf_session);
	  fprintf (stderr,
		   _("New config from %s is not in use!\n"),
		   conf_session_pc);
	  retval = 1;
	}
    }

  if (!gl_service)
    {
      if (check_symlink (conf_account_pc, conf_account) != 0)
	retval = 1;
      if (check_symlink (conf_auth_pc, conf_auth) != 0)
	retval = 1;
      if (check_symlink (conf_password_pc, conf_password) != 0)
	retval = 1;
      if (check_symlink (conf_session_pc, conf_session) != 0)
	retval = 1;
    }

  return retval;
}
