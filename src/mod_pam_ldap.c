
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

#define OPT_NAME(PREFIX, NAME) PREFIX ## _opt_ ## NAME

#define INIT_OPT(PREFIX,x) static bool_option_t OPT_NAME(PREFIX,x) = { #x, FALSE }

#define OPT_SET_2(PREFIX, OPT_1, OPT_2)					\
  INIT_OPT(PREFIX, OPT_1);						\
  INIT_OPT(PREFIX, OPT_2);						\
  static bool_option_t * PREFIX ## _bool_opts[] =			\
    { &OPT_NAME(PREFIX, OPT_1),						\
      &OPT_NAME(PREFIX, OPT_2),						\
      NULL }

#define CREATE_OPT_SETS_WITH_OPTS_2(OPT_1, OPT_2)		\
  OPT_SET_2( auth, OPT_1, OPT_2 );				\
  OPT_SET_2( account, OPT_1, OPT_2 );				\
  OPT_SET_2( password, OPT_1, OPT_2 );				\
  OPT_SET_2( session, OPT_1, OPT_2 );				\
  static option_set_t auth_opts = { auth_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t account_opts = { account_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t password_opts = { password_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t session_opts = { session_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t *opt_sets[] = { &auth_opts, &account_opts, &password_opts, &session_opts, NULL }


static int
parse_config_ldap (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("**** parse_config_ldap (%s): '%s'\n", type2string(type),
           args?args:"");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	opt_set->enable( opt_set, "debug", TRUE );
      else
	print_unknown_option_error ("pam_ldap.so", cp);
    }
  return 1;
}

static int
write_config_ldap (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_winbind, with_ccreds;

  if (debug)
    printf ("**** write_config_ldap (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_winbind = is_module_enabled (supported_module_list,
				    "pam_winbind.so", op);
  with_ccreds = is_module_enabled (supported_module_list,
				   "pam_ccreds.so", op);

  switch (op)
    {
    case ACCOUNT:
      fprintf (fp, "account\tsufficient\tpam_localuser.so\n");
      if (with_winbind)
        fprintf (fp, "account\tsufficient\tpam_ldap.so\tuse_first_pass");
      else
        fprintf (fp, "account\trequired\tpam_ldap.so\tuse_first_pass");
      break;
    case AUTH:
      if (with_ccreds)
        fprintf (fp, "auth\t[authinfo_unavail=ignore success=1 default=2]\tpam_ldap.so\tuse_first_pass");
      else if (with_winbind)
        fprintf (fp, "auth\tsufficient\tpam_ldap.so\tuse_first_pass");
      else
        fprintf (fp, "auth\trequired\tpam_ldap.so\tuse_first_pass");
      break;
    case PASSWORD:
      fprintf (fp, "password\trequired\tpam_ldap.so\ttry_first_pass use_authtok ");
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_ldap.so\t");
      break;
    }

  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, "debug ");
  fprintf (fp, "\n");

  return 0;
}



/* ---- contruct module object ---- */
static string_option_t *string_opts[] = { NULL };
CREATE_OPT_SETS_WITH_OPTS_2( is_enabled, debug );
/* at last construct the complete module object */
pam_module_t mod_pam_ldap = { "pam_ldap.so", opt_sets,
			       &parse_config_ldap,
			       &def_print_module,
			       &write_config_ldap,
			       &get_opt_set};
