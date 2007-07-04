
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"



static int
parse_config_unix (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("**** parse_config_unix (%s): '%s'\n", type2string(type),
           args?args:"");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (opt_set->enable (opt_set, cp, TRUE) == FALSE)
	print_unknown_option_error ("pam_unix.so", cp);
#ifdef NOT_DEFINED
      if (strcmp (cp, "debug") == 0)
	opt_set->enable( opt_set, "debug", TRUE );
      else if (strcmp (cp, "audit") == 0)
	opt_set->enable( opt_set, "audit", TRUE );
      else if (strcmp (cp, "nullok") == 0)
	opt_set->enable( opt_set, "nullok", TRUE );
      else if (strcmp (cp, "shadow") == 0)
	opt_set->enable( opt_set, "shadow", TRUE );
      else if (strcmp (cp, "nullok") == 0)
	opt_set->enable( opt_set, "nullok", TRUE );
      else if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
      else if (strcmp (cp, "try_first_pass") == 0)
	{ /* will be ignored */ }
      else if (strcmp (cp, "use_authtok") == 0)
	{ /* will be ignored */ }
      else
	print_unknown_option_error ("pam_unix.so", cp);
#endif
    }
  return 1;
}

static int
write_config_unix (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_krb5, with_ldap, with_lum, with_winbind, with_pwcheck,
    with_cracklib;

  if (debug)
    printf ("**** write_config_unix (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_krb5 = is_module_enabled (common_module_list, "pam_krb5.so", op);
  with_ldap = is_module_enabled (common_module_list, "pam_ldap.so", op);
  with_lum = is_module_enabled (common_module_list, "pam_lum.so", op);
  with_winbind = is_module_enabled (common_module_list, "pam_winbind.so", op);
  with_pwcheck = is_module_enabled (common_module_list, "pam_pwcheck.so", op);
  with_cracklib = is_module_enabled (common_module_list, "pam_cracklib.so", op);

  switch (op)
  {
    case AUTH:
      if (with_krb5 || with_ldap || with_lum || with_winbind)
	/* Only sufficient if other modules follow */
	fprintf (fp, "auth\tsufficient\tpam_unix.so\t");
      else
	fprintf (fp, "auth\trequired\tpam_unix.so\t");
      break;
    case ACCOUNT:
      if (with_krb5 || with_ldap || with_lum || with_winbind)
	fprintf (fp, "account\trequisite\tpam_unix.so\t");
      else
	fprintf (fp, "account\trequired\tpam_unix.so\t");
      break;
    case PASSWORD:
      if (with_krb5 || with_ldap || with_lum)
	fprintf (fp, "password\tsufficient\tpam_unix.so\t");
      else
	fprintf (fp, "password\trequired\tpam_unix.so\t");
      if (with_pwcheck || with_cracklib)
	fprintf (fp, "use_authtok ");
      break;
    case SESSION:
      fprintf (fp, "session\trequired\tpam_unix.so\t");
      break;
  }

  /* 'audit' superceeds 'debug';
   * if 'audit' is enabled and the user wants to switch back to
   * 'debug' he needs to remove 'audit' first and then add 'debug'
   * again. kind of awkward :/  */
  int with_audit = opt_set->is_enabled (opt_set, "audit");
  int with_debug = opt_set->is_enabled (opt_set, "debug");
  if (with_audit)
  {
    fprintf (fp, "audit ");
    if (with_debug)
    {
      fprintf(stderr, _("INFO: Both options 'audit' and 'debug' found in service %s.\nINFO: 'audit' supersedes 'debug'. If you want to enable 'debug' remove 'audit' first.\n"), type2string (op));
    }
  }
  else if (with_debug)
  {
    fprintf (fp, "debug ");
  }
  
  if (opt_set->is_enabled (opt_set, "nullok"))
    fprintf (fp, "nullok ");

  if (opt_set->is_enabled (opt_set, "shadow"))
    fprintf (fp, "shadow ");

  int with_md5 = opt_set->is_enabled (opt_set, "md5");
  int with_big_crypt = opt_set->is_enabled (opt_set, "big_crypt");
  if (with_md5)
  {
    fprintf (fp, "md5 ");
    if (with_big_crypt)
    {
      fprintf(stderr, _("INFO: Both options 'md5' and 'big_crypt' found in service %s.\nINFO: 'md5' supersedes 'big_crypt'. If you want to enable 'big_crypt' remove 'md5' first.\n"), type2string (op));
    }
  }
  else if (with_big_crypt)
  {
    fprintf (fp, "big_crypt ");
  }

  fprintf (fp, "\n");

  return 0;
}



/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_7( is_enabled, debug, audit, nullok, shadow, md5, big_crypt );
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_unix = { "pam_unix.so", opt_sets,
  &parse_config_unix,
  &def_print_module,
  &write_config_unix,
  &get_opt_set};
