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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <getopt.h>

#include <pam-config.h>

#define CONF_ACCOUNT CONFDIR"/pam.d/common-account"
#define CONF_ACCOUNT_PC CONF_ACCOUNT"-pc"
#define CONF_AUTH CONFDIR"/pam.d/common-auth"
#define CONF_AUTH_PC CONF_AUTH"-pc"
#define CONF_PASSWORD CONFDIR"/pam.d/common-password"
#define CONF_PASSWORD_PC CONF_PASSWORD"-pc"
#define CONF_SESSION CONFDIR"/pam.d/common-session"
#define CONF_SESSION_PC CONF_SESSION"-pc"

int debug = 0;

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

  fputs (_("  -a, --add      Add options/PAM modules\n"), stdout);
  fputs (_("  -c, --create   Create new configuration\n"), stdout);
  fputs (_("  -d, --delete   Remove options/PAM modules\n"), stdout);
  fputs (_("      --help     Give this help list\n"), stdout);
  fputs (_("  -u, --usage    Give a short usage message\n"), stdout);
  fputs (_("  -v, --version  Print program version\n"), stdout);
}

static int
check_symlink (const char *old, const char *new)
{
  if (access (new, F_OK) == -1)
    {
      printf ("Hello (%m)\n");
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
      readlink (new, buf, sizeof (buf));
      if (strcmp (old, buf) != 0)
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

int
main (int argc, char *argv[])
{
  const char *program = "pam-config";
  int m_add = 0, m_create = 0, m_delete = 0, m_init = 0;
  int opt_val = 1;
  int retval = 0;
  config_file_t config_account, config_auth,
    config_password, config_session;

  memset (&config_account, 0, sizeof (config_account));
  memset (&config_auth, 0, sizeof (config_auth));
  memset (&config_password, 0, sizeof (config_password));
  memset (&config_session, 0, sizeof (config_session));

  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  openlog ("login", LOG_ODELAY | LOG_PID, LOG_AUTHPRIV);

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

  if (strcmp (argv[1], "-a") == 0 || strcmp (argv[1], "--add") == 0)
    {
      m_add = 1;
      argc--;
      argv++;
    }
  else if (strcmp (argv[1], "-c") == 0 || strcmp (argv[1], "--create") == 0)
    {
      m_create = 1;
      argc--;
      argv++;
    }
  else if (strcmp (argv[1], "-d") == 0 || strcmp (argv[1], "--delete") == 0)
    {
      m_delete = 1;
      argc--;
      argv++;
      opt_val = 0;
    }
  else if (strcmp (argv[1], "--initialize") == 0)
    {
      m_init = 1;

      argc--;
      argv++;

      if (argc > 1)
	{
	  print_error (program);
	  return 1;
	}

      /* Load old /etc/security/{pam_unix2,pam_pwcheck}.conf
	 files and delete them afterwards.  */
      load_obsolete_conf (&config_account, &config_auth,
			  &config_password, &config_session);

      if (load_config (CONF_ACCOUNT, "account", &config_account) != 0)
	{
	load_old_config_error:
	  fprintf (stderr, _("\nCouldn't load config file, aborted!\n"));
	  return 1;
	}
      if (load_config (CONF_AUTH, "auth", &config_auth) != 0)
	goto load_old_config_error;
      if (load_config (CONF_PASSWORD, "password", &config_password) != 0)
	goto load_old_config_error;
      if (load_config (CONF_SESSION, "session", &config_session) != 0)
	goto load_old_config_error;
    }

  if (m_add || m_delete)
    {
      if (argc == 1)
	{
	  print_error (program);
	  return 1;
	}

      if (load_config (CONF_ACCOUNT_PC, "account", &config_account) != 0)
	{
	load_config_error:
	  fprintf (stderr, _("\nCouldn't load config file, aborted!\n"));
	  return 1;
	}
      if (load_config (CONF_AUTH_PC, "auth", &config_auth) != 0)
	goto load_config_error;
      if (load_config (CONF_PASSWORD_PC, "password", &config_password) != 0)
	goto load_config_error;
      if (load_config (CONF_SESSION_PC, "session", &config_session) != 0)
	goto load_config_error;
    }

  while (1)
    {
      int c;
      int option_index = 0;
      static struct option long_options[] = {
        {"version",   no_argument, NULL, 'v' },
        {"usage",     no_argument, NULL, 'u' },
	{"nullok",    no_argument, NULL, 900 },
	{"pam-debug", no_argument, NULL, 901 },
	/* pam_pwcheck */
	{"pwcheck",          no_argument,       NULL, 1000 },
	{"pwcheck-debug",    no_argument,       NULL, 1001 },
	{"pwcheck-nullok",   no_argument,       NULL, 1002 },
	{"pwcheck-cracklib", no_argument,       NULL, 1003 },
	{"pwcheck-cracklib-path", required_argument, NULL, 1004 },
	{"pwcheck-maxlen",   required_argument, NULL, 1005 },
	{"pwcheck-minlen",   required_argument, NULL, 1006 },
	{"pwcheck-tries",    required_argument, NULL, 1007 },
	{"pwcheck-remember", required_argument, NULL, 1008 },
	{"pwcheck-nisdir",   required_argument, NULL, 1009 },
	{"mkhomedir",        no_argument,       NULL, 1100 },
	{"limits",           no_argument,       NULL, 1200 },
        {"env",              no_argument,       NULL, 1300 },
        {"xauth",            no_argument,       NULL, 1400 },
        {"make",             no_argument,       NULL, 1500 },
        {"make-dir",         no_argument,       NULL, 1501 },
        {"unix2",            no_argument,       NULL, 1600 },
        {"unix2-debug",      no_argument,       NULL, 1601 },
        {"unix2-nullok",     no_argument,       NULL, 1602 },
	{"bioapi",           no_argument,       NULL, 1700 },
	{"bioapi-options",   required_argument, NULL, 1701 },
	{"debug",   no_argument, NULL, '\254' },
        {"help",    no_argument, NULL, '\255' },
        {NULL,      0,           NULL, '\0'}
      };

      c = getopt_long (argc, argv, "vu",
                       long_options, &option_index);

      if (c == (-1))
        break;
      switch (c)
	{
	case 900: /* --nullok */
	  config_auth.unix2_nullok = opt_val;
	  config_password.pwcheck_nullok = opt_val;
	  config_password.unix2_nullok = opt_val;
	  break;
	case 901: /* --pam-debug */
	  config_account.unix2_debug = opt_val;
	  config_auth.unix2_debug = opt_val;
	  config_password.pwcheck_debug = opt_val;
	  config_password.unix2_debug = opt_val;
	  config_session.unix2_debug = opt_val;
	  break;
	/* pam_pwcheck */
	case 1000:
	  config_password.use_pwcheck = opt_val;
	  break;
	case 1001:
	  config_password.pwcheck_debug = opt_val;
	  break;
	case 1002:
	  config_password.pwcheck_nullok = opt_val;
	  break;
	case 1003:
	  config_password.pwcheck_cracklib = opt_val;
	  break;
	case 1004:
	  config_password.pwcheck_cracklib = opt_val;
	  config_password.pwcheck_cracklib_path = optarg;
	  break;
	case 1005:
	  config_password.pwcheck_maxlen = atoi (optarg);
	  break;
	case 1006:
	  config_password.pwcheck_minlen = atoi (optarg);
	  break;
	case 1007:
	  config_password.pwcheck_tries = atoi (optarg);
	  break;
	case 1008:
	  config_password.pwcheck_remember = atoi (optarg);
	  break;
	case 1009:
	  config_password.pwcheck_nisdir = optarg;
	  break;
	case 1100:
	  config_session.use_mkhomedir = opt_val;
	  break;
	case 1200:
	  config_session.use_limits = opt_val;
	  break;
	case 1300:
	  /* Remove in every case from auth, else we will have it twice.  */
	  config_auth.use_env = 0;
	  config_session.use_env = opt_val;
	  break;
	case 1400:
	  config_session.use_xauth = opt_val;
	  break;
	case 1500:
	  config_session.use_make = opt_val;
	  break;
	case 1501:
	  config_session.make_options = optarg;
	  break;
	case 1600:
	  /* use_unix2 */
	  break;
	case 1601:
	  config_account.unix2_debug = opt_val;
	  config_auth.unix2_debug = opt_val;
	  config_password.unix2_debug = opt_val;
	  config_session.unix2_debug = opt_val;
	  break;
	case 1602:
	  config_account.unix2_nullok = opt_val;
	  config_auth.unix2_nullok = opt_val;
	  config_password.unix2_nullok = opt_val;
	  config_session.unix2_nullok = opt_val;
	  break;
	case 1700:
	  config_auth.use_bioapi = opt_val;
	  break;
	case 1701:
	  config_auth.bioapi_options = optarg;
	  break;
	case '\254':
	  debug = 1;
	  break;
	case '\255':
          print_help (program);
          return 0;
        case 'v':
          print_version (program, "2006");
          return 0;
        case 'u':
          print_usage (stdout, program);
	  return 0;
	default:
	  print_error (program);
	  return 1;
	}
    }

  argc -= optind;
  argv += optind;

  if (argc > 1)
    {
      fprintf (stderr, _("%s: Too many arguments.\n"), program);
      print_error (program);
      return 1;
    }

  if (m_create)
    {
      /* Write account section.  */
      if (write_config_account (CONF_ACCOUNT_PC, &config_account) != 0)
	{
	  fprintf (stderr, _("Error writing %s: %m\n"), CONF_ACCOUNT_PC);
	  return 1;
	}

      /* Write auth section.  */
      if (write_config_auth (CONF_AUTH_PC, &config_auth) != 0)
	{
	  fprintf (stderr, _("Error writing %s: %m\n"), CONF_AUTH_PC);
	  return 1;
	}

      /* Write password section.  */
      config_password.use_pwcheck = 1;
      config_password.pwcheck_nullok = 1;
      config_password.unix2_nullok = 1;
      if (write_config_password (CONF_PASSWORD_PC, &config_password) != 0)
	{
	  fprintf (stderr, _("Error writing %s: %m\n"), CONF_PASSWORD_PC);
	  return 1;
	}

      /* Write session section.  */
      config_session.use_limits = 1;
      config_session.use_env = 1;
      if (write_config_session (CONF_SESSION_PC, &config_session) != 0)
	{
	  fprintf (stderr, _("Error writing %s: %m\n"), CONF_SESSION_PC);
	  return 1;
	}
    }
  else
    {
      /* Write account section.  */
      if (write_config_account (CONF_ACCOUNT_PC, &config_account) != 0)
	{
	  fprintf (stderr, _("Error writing %s: %m\n"), CONF_ACCOUNT_PC);
	  return 1;
	}

      /* Write auth section.  */
      if (write_config_auth (CONF_AUTH_PC, &config_auth) != 0)
	{
	  fprintf (stderr, _("Error writing %s: %m\n"), CONF_AUTH_PC);
	  return 1;
	}

      /* Write password section.  */
      if (write_config_password (CONF_PASSWORD_PC, &config_password) != 0)
	{
	  fprintf (stderr, _("Error writing %s: %m\n"), CONF_PASSWORD_PC);
	  return 1;
	}


      /* Write session section.  */
      if (write_config_session (CONF_SESSION_PC, &config_session) != 0)
	{
	  fprintf (stderr, _("Error writing %s: %m\n"), CONF_SESSION_PC);
	  return 1;
	}

      if (m_init)
	{
	  if (link (CONF_ACCOUNT, CONF_ACCOUNT".pam-config-backup") != 0 ||
	      unlink (CONF_ACCOUNT) != 0 ||
	      symlink (CONF_ACCOUNT_PC, CONF_ACCOUNT) != 0)
	    {
	      fprintf (stderr,
		       _("Error activating %s (%m)\n"), CONF_ACCOUNT);
	      fprintf (stderr,
		       _("New config from %s is not in use!\n"), CONF_ACCOUNT_PC);
	      retval = 1;
	    }

	  if (link (CONF_AUTH, CONF_AUTH".pam-config-backup") != 0 ||
	      unlink (CONF_AUTH) != 0 ||
	      symlink (CONF_AUTH_PC, CONF_AUTH) != 0)
	    {
	      fprintf (stderr,
		       _("Error activating %s (%m)\n"), CONF_AUTH);
	      fprintf (stderr,
		       _("New config from %s is not in use!\n"), CONF_AUTH_PC);
	      retval = 1;
	    }

	  if (link (CONF_PASSWORD, CONF_PASSWORD".pam-config-backup") != 0 ||
	      unlink (CONF_PASSWORD) != 0 ||
	      symlink (CONF_PASSWORD_PC, CONF_PASSWORD) != 0)
	    {
	      fprintf (stderr,
		       _("Error activating %s (%m)\n"), CONF_PASSWORD);
	      fprintf (stderr,
		       _("New config from %s is not in use!\n"),
		       CONF_PASSWORD_PC);
	      retval = 1;
	    }

	  if (link (CONF_SESSION, CONF_SESSION".pam-config-backup") != 0 ||
	      unlink (CONF_SESSION) != 0 ||
	      symlink (CONF_SESSION_PC, CONF_SESSION) != 0)
	    {
	      fprintf (stderr,
		       _("Error activating %s (%m)\n"), CONF_SESSION);
	      fprintf (stderr,
		       _("New config from %s is not in use!\n"),
		       CONF_SESSION_PC);
	      retval = 1;
	    }

	  if (retval == 0)
	    {
	      rename ("/etc/security/pam_pwcheck.conf",
		      "/etc/security/pam_pwcheck.conf.pam-config-backup");
	      rename ("/etc/security/pam_unix2.conf",
		      "/etc/security/pam_unix2.conf.pam-config-backup");
	    }
	  return retval;
	}
    }

  if (check_symlink (CONF_ACCOUNT_PC, CONF_ACCOUNT) != 0)
    retval = 1;
  if (check_symlink (CONF_AUTH_PC, CONF_AUTH) != 0)
    retval = 1;
  if (check_symlink (CONF_PASSWORD_PC, CONF_PASSWORD) != 0)
    retval = 1;
  if (check_symlink (CONF_SESSION_PC, CONF_SESSION) != 0)
    retval = 1;

  return 0;
}
