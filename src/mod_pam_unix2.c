
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

#define OPT_NAME(PREFIX, NAME) PREFIX ## _opt_ ## NAME

#define INIT_OPT(PREFIX,x) bool_option_t OPT_NAME(PREFIX,x) = { #x, FALSE }

#define OPT_SET_3(PREFIX, OPT_1, OPT_2, OPT_3) INIT_OPT(PREFIX, OPT_1);\
  INIT_OPT(PREFIX, OPT_2);\
  INIT_OPT(PREFIX, OPT_3);\
  bool_option_t * PREFIX ## _bool_opts[] = { &OPT_NAME(PREFIX, OPT_1),\
					     &OPT_NAME(PREFIX, OPT_2),\
					     &OPT_NAME(PREFIX, OPT_3),\
					     NULL }

#define CREATE_OPT_SETS_WITH_OPTS_3(OPT_1, OPT_2, OPT_3) OPT_SET_3( auth, OPT_1, OPT_2, OPT_3 );\
							 OPT_SET_3( account, OPT_1, OPT_2, OPT_3);\
							 OPT_SET_3( password, OPT_1, OPT_2, OPT_3);\
							 OPT_SET_3( session, OPT_1, OPT_2, OPT_3);\
							 option_set_t auth_opts = { auth_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt };\
							 option_set_t account_opts = { account_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt };\
							 option_set_t password_opts = { password_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt };\
							 option_set_t session_opts = { session_bool_opts, string_opts, &is_enabled, &enable, &get_opt, &set_opt };\
							 option_set_t *opt_sets[] = { &auth_opts, &account_opts, &password_opts, &session_opts, NULL }


static int
parse_config_unix2 (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("parse_config_unix2:\t%s\t(%s)\n", type2string(type), this->name);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	opt_set->enable( opt_set, "debug", TRUE );
      else if (strcmp (cp, "nullok") == 0)
	opt_set->enable( opt_set, "nullok", TRUE );
      else if (strcmp (cp, "trace") == 0)
	opt_set->enable( opt_set, "trace", TRUE );
      else if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
      else if (strcmp (cp, "use_authtok") == 0)
	{ /* will be ignored */ }
      else if (strncmp (cp, "call_modules=", 13) == 0){
	/* XXX strip krb5 and ldap modules from it
	conf->unix2_call_modules = strdup (&cp[13]);*/
      }
      else
	print_unknown_option_error ("pam_unix2.so", cp);
    }
  return 1;
}

static int
print_module_unix2_type (  pam_module_t *this, write_type_t type  ) {
  printf ("%s:", type2string( type ) );
  option_set_t *opt_set = this->get_opt_set( this, type );
  for_each_bool_opt( opt_set,  &print_bool_opt );
  printf( "\n" );
  return TRUE;
}

static int
print_module_unix2 ( pam_module_t *this ){
  print_module_unix2_type( this, AUTH );
  print_module_unix2_type( this, ACCOUNT );
  print_module_unix2_type( this, PASSWORD );
  print_module_unix2_type( this, SESSION );
  return TRUE;
}


/* ---- contruct module object ---- */
string_option_t *string_opts[] = { NULL };
CREATE_OPT_SETS_WITH_OPTS_3( debug, nullok, trace );
/* at last construct the complete module object */
pam_module_t mod_pam_unix2 = { "pam_unix2.so", opt_sets,
			       &parse_config_unix2,
			       &print_module_unix2,
			       &def_write_config,
			       &get_opt_set};
