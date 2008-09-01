/**
 * @file mod_pam_mount.c
 * @brief Support for PAM module pam_mount.
 * @author Sven Schober <sschober@suse.de>
 * @date 2007-07-23
 */

/* Copyright (C) 2007, 2008 Sven Schober
   Author: Sven Schober <sschober@suse.de>

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

/* defined in pam-config.c */
extern char *conf_auth_pc;

/**
 * @brief Writes two lines out to the service file specified by
 * gl_service.
 *
 * The line concerning the auth stack is inserted before
 * the first line in the existing file containing the substring
 * "auth".
 * The other one concerning session is simply appended to the
 * service file.
 *
 * @param this A pointer to the "object" instance this function is
 * working on.
 *
 * @return
 */
static int
write_config_mount (  pam_module_t *this,
		      enum write_type op __attribute__ ((unused)),
		      FILE *unused __attribute__((unused)))
{
  option_set_t *opt_set = this->get_opt_set (this, SESSION);
  int is_written = 0;
  FILE *fp;
  config_content_t *cfg_content;
  int writeit = opt_set->is_enabled (opt_set, "is_enabled");

  if (debug)
    debug_write_call (this, SESSION);

  load_single_config (gl_service, &cfg_content);

  fp = create_service_file (gl_service);
  if (!fp) return 0;

  while (cfg_content != NULL)
  {
    if (writeit)
    {
      if (!is_written)
      {
	/* make sure pam_mount.so get's written to the service file
	 * _before_ the include common-auth line.
	 */
	if (strcasestr (cfg_content->line, "auth") != NULL)
	{
	  fprintf (fp, "auth     optional\tpam_mount.so\n");
	  is_written = 1;
	}
      }
      /* skip old entries */
      if (strcasestr (cfg_content->line, "pam_mount.so") == NULL )
	fprintf (fp, "%s", cfg_content->line);
    }
    else
    {
      /* skip old entries */
      if (strcasestr (cfg_content->line, "pam_mount.so") == NULL)
	fprintf (fp, "%s", cfg_content->line);
      else
	is_written = 1;
    }
    cfg_content = cfg_content->next;
  }
  if (writeit)
  {
    /* As this is a single service module, common-* files are not
     * parsed in. We need to know if pam_thinkfinger.so is enabled,
     * which is a common-* module, so we parse common-auth in
     */

    /* As the user might have supplied a custom confdir via the
     * --confdir option we have to check if conf_auth_pc was set in
     *  pam-config.c:main()
     */
    char *conf_fname;
    if (conf_auth_pc) conf_fname = conf_auth_pc;
    else conf_fname = CONF_AUTH_PC;
    if (load_config (conf_fname, AUTH, common_module_list, 1) != 0)
    {
      fprintf (stderr,
	       _("\nCouldn't load config file '%s', aborted!\n"),
	       CONF_AUTH_PC);
      return 1;
    }
    if (is_module_enabled (common_module_list, "pam_thinkfinger.so", AUTH))
    {
      fprintf (stderr, _("ERROR: Module pam_thinkfinger.so is enabled. Disable it first.\n"));
      return 1;
    }
    /* pam_thinkfinger.so is not enabled so we can safely add
     * pam_mount.so
     */
    fprintf (fp, "session  optional\tpam_mount.so\n");
  }
  return close_service_file (fp,gl_service);
}

GETOPT_START_1(SESSION)
GETOPT_END_1(SESSION)

PRINT_ARGS("mount")
PRINT_XMLHELP("mount")

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_1( is_enabled );
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_mount = { "pam_mount.so", opt_sets, helptext,
			       &def_parse_config,
			       &def_print_module,
			       &write_config_mount,
			       &get_opt_set,
			       &getopt,
			       &print_args,
			       &print_xmlhelp};
