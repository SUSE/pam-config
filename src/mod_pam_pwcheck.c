#include "pam-module.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef NOTDEFINIED
static int
parse_pwcheck_options ( pam_module_t *this, char *args, config_file_t *conf)
{
  printf( "parse_pwcheck_options:\t%s\t(%s)\n", type2string( conf->type ), this->name );
  conf->use_pwcheck = TRUE;
  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	conf->pwcheck_debug = 1;
      else if (strcmp (cp, "nullok") == 0)
	conf->pwcheck_nullok = 1;
      else if (strcmp (cp, "cracklib") == 0)
	conf->pwcheck_cracklib = 1;
      else if (strncmp (cp, "cracklib=", 9) == 0)
	{
	  conf->pwcheck_cracklib = 1;
	  conf->pwcheck_cracklib_path = strdup (&cp[9]);
	}
      else if (strncmp (cp, "maxlen=", 7) == 0)
	conf->pwcheck_maxlen = atoi (&cp[7]);
      else if (strncmp (cp, "minlen=", 7) == 0)
	{
	  conf->pwcheck_have_minlen = 1;
	  conf->pwcheck_minlen = atoi (&cp[7]);
	}
      else if (strncmp (cp, "tries=", 6) == 0)
	conf->pwcheck_tries = atoi (&cp[6]);
      else if (strncmp (cp, "remember=", 9) == 0)
	conf->pwcheck_remember = atoi (&cp[9]);
      else if (strncmp (cp, "nisdir=", 7) == 0)
	conf->pwcheck_nisdir = strdup (&cp[7]);
      else if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
      else if (strcmp (cp, "use_authtok") == 0)
	{ /* will be ignored */ }
      else if (strcmp (cp, "no_obscure_checks") == 0)
	conf->pwcheck_no_obscure_checks = 1;
      else
	print_unknown_option_error ("pam_pwcheck.so", cp);
    }
  return TRUE;
}

static int
print_module_pwcheck ( pam_module_t *this, config_file_t *conf ) {
  printf( "print_module_pwcheck:\t%s\n", this->name );
  if (!conf->use_pwcheck)
    return FALSE;

  printf ("%s:", type2string( conf->type ) );
  if (conf->pwcheck_debug)
    printf (" debug");
  if (conf->pwcheck_nullok)
    printf (" nullok");
  if (conf->pwcheck_cracklib_path)
    printf (" cracklib=%s", conf->pwcheck_cracklib_path);
  else if (conf->pwcheck_cracklib)
    printf (" cracklib");
  if (conf->pwcheck_maxlen)
    printf (" maxlen=%d", conf->pwcheck_maxlen);
  if (conf->pwcheck_have_minlen)
    printf (" minlen=%d", conf->pwcheck_minlen);
  if (conf->pwcheck_tries)
    printf (" tries=%d", conf->pwcheck_tries);
  if (conf->pwcheck_remember)
    printf (" remember=%d", conf->pwcheck_remember);
  if (conf->pwcheck_nisdir)
    printf (" nisdir=%s", conf->pwcheck_nisdir);
  if (conf->pwcheck_no_obscure_checks)
    printf( " no_obscure_checks");
  printf ("\n");

  return TRUE;
}
 
pam_module_t mod_pam_pwcheck = { "pam_pwcheck.so", &parse_pwcheck_options, &print_module_pwcheck, &def_write_config };
#endif
