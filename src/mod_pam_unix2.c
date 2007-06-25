#include "pam-module.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int
parse_unix2_options (pam_module_t *this, char *args, config_file_t *conf)
{
  printf( "parse_unix2_options:\t%s\n", this->name );
  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	conf->unix2_debug = 1;
      else if (strcmp (cp, "nullok") == 0)
	conf->unix2_nullok = 1;
      else if (strcmp (cp, "trace") == 0)
	conf->unix2_trace = 1;
      else if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
      else if (strcmp (cp, "use_authtok") == 0)
	{ /* will be ignored */ }
      else if (strncmp (cp, "call_modules=", 13) == 0)
	/* XXX strip krb5 and ldap modules from it */
	conf->unix2_call_modules = strdup (&cp[13]);
      else
	print_unknown_option_error ("pam_unix2.so", cp);
    }
  return 1;
}

pam_module_t mod_pam_unix2 = { "pam_unix2.so", &parse_unix2_options, &def_print_module, &def_write_config };
