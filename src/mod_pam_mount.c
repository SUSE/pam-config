/** 
 * @file mod_pam_mount.c
 * @brief Support for PAM module pam_mount.
 * @author Sven Schober <sschober@suse.de>
 * @date 2007-07-23
 */

/* Copyright (C) 2007 Sven Schober
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

/** 
 * @brief Function to parse in a line from a service file and store
 * the options to the opt_set of the instance specified by *this.
 * 
 * @param this The pam-module instance to work on.
 * @param args The line from the service file.
 * @param type One of AUTH, SESSION, PASS or ACCOUNT
 * 
 * @return 1
 */

static int
parse_config_mount (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("**** parse_config_mount (%s): '%s'\n", type2string(type),
           args?args:"");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;
      
      if (opt_set->enable (opt_set, cp, TRUE) == FALSE){
	if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
	else if (strcmp (cp, "try_first_pass") == 0)
	{ /* will be ignored */ }
	else
	  print_unknown_option_error ("pam_mount.so", cp);
      }
    }
  return TRUE;
}

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
		      enum write_type op __attribute__((unused)),
		      FILE *unused __attribute__((unused)))
{
  option_set_t *opt_set = this->get_opt_set (this, SESSION);
  int is_written = 0;
  FILE *fp;
  config_content_t *cfg_content;
  int writeit = opt_set->is_enabled (opt_set, "is_enabled");
  if (debug)
    printf ("**** write_config_mount (%s)\n", gl_service);
  
  
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
	  fprintf (fp, "auth\t optional\tpam_mount.so\n");
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
    if (load_config (CONF_AUTH_PC, AUTH, common_module_list, 1) != 0)
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
    fprintf (fp, "session  required       pam_mount.so\n");
  }
  return close_service_file (fp,gl_service);
}



/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_1( is_enabled );
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_mount = { "pam_mount.so", opt_sets,
  &parse_config_mount,
  &def_print_module,
  &write_config_mount,
  &get_opt_set};
