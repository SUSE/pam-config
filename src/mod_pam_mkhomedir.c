#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

static int
write_config_mkhomedir (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  if (op != SESSION)
    return 0;

  fprintf (fp, "session  optional\tpam_mkhomedir.so\t");

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_1(SESSION)
GETOPT_END_1(SESSION)

PRINT_ARGS("mkhomedir")
PRINT_XMLHELP("mkhomedir")

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_3( is_enabled, debug, silent );
DECLARE_STRING_OPTS_2( umask, skel );
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_mkhomedir = { "pam_mkhomedir.so", opt_sets, helptext,
				   &def_parse_config,
				   &def_print_module,
				   &write_config_mkhomedir,
				   &get_opt_set,
				   &getopt,
				   &print_args,
				   &print_xmlhelp};
