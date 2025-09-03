/* Copyright (C) 2007, 2008 Thorsten Kukuk
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
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"


static int
parse_config_pwcheck (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("**** parse_config_pwcheck (%s): '%s'\n", type2string(type),
           args?args:"");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
	  ++args;

      if (strcmp (cp, "debug") == 0)
        opt_set->enable (opt_set, "debug", TRUE);
      else if (strcmp (cp, "nullok") == 0)
        opt_set->enable (opt_set, "nullok", TRUE);
      else if (strcmp (cp, "cracklib") == 0)
	opt_set->enable (opt_set, "cracklib", TRUE);
      else if (strncmp (cp, "cracklib=", 9) == 0)
        {
	  opt_set->enable (opt_set, "cracklib", TRUE);
	  opt_set->set_opt (opt_set, "cracklib_path", strdup (&cp[9]));
        }
      else if (strncmp (cp, "maxlen=", 7) == 0)
	opt_set->set_opt (opt_set, "maxlen", strdup(&cp[7]));
      else if (strncmp (cp, "minlen=", 7) == 0)
	opt_set->set_opt (opt_set, "minlen", strdup(&cp[7]));
      else if (strncmp (cp, "tries=", 6) == 0)
	opt_set->set_opt (opt_set, "tries", strdup(&cp[6]));
      else if (strncmp (cp, "remember=", 9) == 0)
	opt_set->set_opt (opt_set, "remember", strdup(&cp[9]));
      else if (strcmp (cp, "use_first_pass") == 0)
        { /* will be ignored */ }
      else if (strcmp (cp, "use_authtok") == 0)
        { /* will be ignored */ }
      else if (strcmp (cp, "no_obscure_checks") == 0)
        opt_set->enable (opt_set, "no_obscure_checks", TRUE);
      else if (strcmp (cp, "enforce_for_root") == 0)
        opt_set->enable (opt_set, "enforce_for_root", TRUE);
      else
        print_unknown_option_error ("pam_pwcheck.so", cp);

    }
  return 1;
}

static int
write_config_pwcheck (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  const char *cp;

  if (debug)
    debug_write_call (this, op);

  /* pam_pwcheck is only used in password part.  */
  if (op != PASSWORD)
    return 0;

  /* pam_pwcheck is not enabled.  */
  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  fprintf (fp, "password\trequisite\tpam_pwcheck.so\t");
  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, "debug ");
  if (opt_set->is_enabled (opt_set, "nullok"))
    fprintf (fp, "nullok ");
  if (opt_set->is_enabled (opt_set, "cracklib"))
    {
      cp = opt_set->get_opt (opt_set, "cracklib_path");
      if (cp)
	fprintf (fp, "cracklib=%s ", cp);
      else
	fprintf (fp, "cracklib ");
    }
  cp = opt_set->get_opt (opt_set, "maxlen");
  if (cp)
    fprintf (fp, "maxlen=%s ", cp);
  cp = opt_set->get_opt (opt_set, "minlen");
  if (cp)
    fprintf (fp, "minlen=%s ", cp);
  cp = opt_set->get_opt (opt_set, "tries");
  if (cp)
    fprintf (fp, "tries=%s ", cp);
  cp = opt_set->get_opt (opt_set, "remember");
  if (cp)
    fprintf (fp, "remember=%s ", cp);
  if (opt_set->is_enabled (opt_set, "no_obscure_checks"))
    fprintf (fp, "no_obscure_checks ");
  if (opt_set->is_enabled (opt_set, "enforce_for_root"))
    fprintf (fp, "enforce_for_root ");

  fprintf (fp, "\n");

  return 0;
}

GETOPT_START_1(PASSWORD)
  else if (strcmp ("cracklib_path", opt) == 0)
    {
      opt_set = this->get_opt_set (this, PASSWORD);
      opt_set->enable (opt_set, "cracklib", g_opt->opt_val);
      opt_set->set_opt (opt_set, "cracklib_path", strdup (optarg));
    }
GETOPT_END_1(PASSWORD)


PRINT_ARGS("pwcheck");
PRINT_XMLHELP("pwcheck");

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_6(is_enabled, debug, nullok, cracklib, no_obscure_checks, enforce_for_root);
DECLARE_STRING_OPTS_5(cracklib_path, maxlen, minlen, tries, remember);
DECLARE_OPT_SETS;

module_helptext_t pwcheck_help[] = {{"", NULL, "Enable/Disable pam_pwcheck.so module in password section."},
				    {"debug", NULL, "Add <option>debug</option> option to all pam_pwcheck.so invocations."},
				    {"nullok", NULL, "Add <option>nullok</option> option to all pam_pwcheck.so invocations."},
				    {"cracklib", NULL, "Add <option>cracklib</option> option to pam_pwcheck.so."},
				    {"no_obscure_checks", NULL, "Add <option>no_obscure_checks</option> option to pam_pwcheck.so."},
				    {"enforce_for_root", NULL,  "Add <option>enforce_for_root</option> option to pam_pwcheck.so."},
				    {"cracklib_path", "path", "Add <option>cracklib_path=</option><replaceable>path</replaceable> to pam_pwcheck.so."},
				    {"maxlen", "N", "Add <option>maxlen=</option><replaceable>N</replaceable> to pam_pwcheck.so."},
				    {"minlen", "N", "Add <option>minlen=</option><replaceable>N</replaceable> to pam_pwcheck.so."},
				    {"tries", "N", "Add <option>tries=</option><replaceable>N</replaceable> to pam_pwcheck.so."},
				    {"remember", "N", "Add <option>remember=</option><replaceable>N</replaceable> to pam_pwcheck.so."},
				    {NULL, NULL, NULL}};


/* at last construct the complete module object */
pam_module_t mod_pam_pwcheck = { "pam_pwcheck.so", opt_sets, pwcheck_help,
				 &parse_config_pwcheck,
				 &def_print_module,
				 &write_config_pwcheck,
				 &get_opt_set,
				 &getopt,
				 &print_args,
				 &print_xmlhelp,
				 -1,
				 -1,
				 PAM_PWCHECK_PASSWORD_PRIORITY,
				 -1
};
