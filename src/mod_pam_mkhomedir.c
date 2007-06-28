
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

#define OPT_NAME(PREFIX, NAME) PREFIX ## _opt_ ## NAME

#define INIT_OPT(PREFIX,x) static bool_option_t OPT_NAME(PREFIX,x) = { #x, FALSE }

#define OPT_SET_3(PREFIX, OPT_1, OPT_2, OPT_3)	\
  INIT_OPT(PREFIX, OPT_1); \
  INIT_OPT(PREFIX, OPT_2); \
  INIT_OPT(PREFIX, OPT_3); \
  static bool_option_t * PREFIX ## _bool_opts[] = { &OPT_NAME(PREFIX, OPT_1),\
					     &OPT_NAME(PREFIX, OPT_2),\
					     &OPT_NAME(PREFIX, OPT_3),\
					     NULL }

#define CREATE_OPT_SETS_WITH_OPTS_3(OPT_1, OPT_2, OPT_3)	\
  OPT_SET_3( auth, OPT_1, OPT_2, OPT_3 );			\
  OPT_SET_3( account, OPT_1, OPT_2, OPT_3 );			\
  OPT_SET_3( password, OPT_1, OPT_2, OPT_3 );			\
  OPT_SET_3( session, OPT_1, OPT_2, OPT_3 );			\
  static option_set_t auth_opts = { auth_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t account_opts = { account_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t password_opts = { password_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t session_opts = { session_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t *opt_sets[] = { &auth_opts, &account_opts, &password_opts, &session_opts, NULL }


static int
parse_config_mkhomedir (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("**** parse_config_mkhomedir (%s): '%s'\n", type2string(type),
           args?args:"");

  opt_set->enable (opt_set, "use_mkhomedir", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	opt_set->enable( opt_set, "debug", TRUE );
      else if (strcmp (cp, "silent") == 0)
	opt_set->enable( opt_set, "silent", TRUE );
      else
	print_unknown_option_error ("pam_mkhomedir.so", cp);
    }
  return 1;
}

static int
print_module_mkhomedir_type (  pam_module_t *this, write_type_t type  ) {
  printf ("%s:", type2string( type ) );
  option_set_t *opt_set = this->get_opt_set( this, type );
  for_each_bool_opt( opt_set,  &print_bool_opt );
  printf( "\n" );
  return TRUE;
}

static int
print_module_mkhomedir ( pam_module_t *this ){
  print_module_mkhomedir_type( this, SESSION );
  return TRUE;
}

static int
write_config_mkhomedir (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    printf ("**** write_config_mkhomedir (...)\n");

  if (!opt_set->is_enabled (opt_set, "use_mkhomedir"))
    return 0;

  if (op != SESSION)
    return 0;

  fprintf (fp, "session\toptional\tpam_mkhomedir.so\t");
  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, "debug ");
  if (opt_set->is_enabled (opt_set, "silent"))
    fprintf (fp, "silent ");
  fprintf (fp, "\n");

  return 0;
}



/* ---- contruct module object ---- */
static string_option_t *string_opts[] = { NULL };
CREATE_OPT_SETS_WITH_OPTS_3( use_mkhomedir, debug, silent );
/* at last construct the complete module object */
pam_module_t mod_pam_mkhomedir = { "pam_mkhomedir.so", opt_sets,
			       &parse_config_mkhomedir,
			       &print_module_mkhomedir,
			       &write_config_mkhomedir,
			       &get_opt_set};
