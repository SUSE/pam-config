#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

static int
parse_config_mkhomedir (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("**** parse_config_mkhomedir (%s): '%s'\n", type2string(type),
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
      else if (strcmp (cp, "silent") == 0)
	opt_set->enable( opt_set, "silent", TRUE );
      else if (strncmp (cp, "umask=", 6) == 0)
	opt_set->set_opt (opt_set, "umask", strdup (&cp[6]));
      else if (strncmp (cp, "skel=", 5) == 0)
	opt_set->set_opt (opt_set, "skel", strdup (&cp[5]));
      else
	print_unknown_option_error ("pam_mkhomedir.so", cp);
    }
  return 1;
}

static int
write_config_mkhomedir (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  const char *cp;

  if (debug)
    printf ("**** write_config_mkhomedir (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  if (op != SESSION)
    return 0;

  fprintf (fp, "session\toptional\tpam_mkhomedir.so\t");
  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, "debug ");
  if (opt_set->is_enabled (opt_set, "silent"))
    fprintf (fp, "silent ");
  cp = opt_set->get_opt (opt_set, "umask");
  if (cp)
    fprintf (fp, "umask=%s ", cp);
  cp = opt_set->get_opt (opt_set, "skel");
  if (cp)
    fprintf (fp, "skel=%s ", cp);
  fprintf (fp, "\n");

  return 0;
}



/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_3( is_enabled, debug, silent );
DECLARE_STRING_OPTS_2( umask, skel );
DECLARE_OPT_SETS;

/* at last construct the complete module object */
pam_module_t mod_pam_mkhomedir = { "pam_mkhomedir.so", opt_sets,
			       &parse_config_mkhomedir,
			       &def_print_module,
			       &write_config_mkhomedir,
			       &get_opt_set};
