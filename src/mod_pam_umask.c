
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

#define OPT_NAME(PREFIX, NAME) PREFIX ## _opt_ ## NAME

#define INIT_OPT(PREFIX,x) static bool_option_t OPT_NAME(PREFIX,x) = { #x, FALSE }

#define OPT_SET_2(PREFIX, OPT_1, OPT_2) INIT_OPT(PREFIX, OPT_1); \
  INIT_OPT(PREFIX, OPT_2);\
  static bool_option_t * PREFIX ## _bool_opts[] = { &OPT_NAME(PREFIX, OPT_1),	\
					     &OPT_NAME(PREFIX, OPT_2),\
					     NULL }

#define CREATE_OPT_SETS_WITH_OPTS_2(OPT_1, OPT_2) \
  OPT_SET_2( auth, OPT_1, OPT_2 );			\
  OPT_SET_2( account, OPT_1, OPT_2 );			\
  OPT_SET_2( password, OPT_1, OPT_2 );			\
  OPT_SET_2( session, OPT_1, OPT_2 );			\
  static option_set_t auth_opts = { auth_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t account_opts = { account_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t password_opts = { password_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t session_opts = { session_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t *opt_sets[] = { &auth_opts, &account_opts, &password_opts, &session_opts, NULL }


static int
parse_config_umask (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("parse_config_umask:\t%s\t(%s)\n", type2string(type), this->name);

  opt_set->enable (opt_set, "use_umask", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	opt_set->enable( opt_set, "debug", TRUE );
      else
	print_unknown_option_error ("pam_umask.so", cp);
    }
  return 1;
}

static int
print_module_umask_type (  pam_module_t *this, write_type_t type  ) {
  printf ("%s:", type2string( type ) );
  option_set_t *opt_set = this->get_opt_set( this, type );
  for_each_bool_opt( opt_set,  &print_bool_opt );
  printf( "\n" );
  return TRUE;
}

static int
print_module_umask ( pam_module_t *this ){
  print_module_umask_type( this, SESSION );
  return TRUE;
}

static int
write_config_umask (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (!opt_set->is_enabled (opt_set, "use_umask"))
    return 0;

  if (op != SESSION)
    return 0;

  fprintf (fp, "session\toptional\tpam_umask.so\t\n");
  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, "debug ");

  fprintf (fp, "\n");

  return 0;
}



/* ---- contruct module object ---- */
static string_option_t *string_opts[] = { NULL };
CREATE_OPT_SETS_WITH_OPTS_2( use_umask, debug );
/* at last construct the complete module object */
pam_module_t mod_pam_umask = { "pam_umask.so", opt_sets,
			       &parse_config_umask,
			       &print_module_umask,
			       &write_config_umask,
			       &get_opt_set};
