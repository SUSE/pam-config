/* Copyright (C) 2007, 2008 Thorsten Kukuk
   Author: Thorsten Kukuk <kukuk@thkukuk.de>

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


static int
write_config_selinux (pam_module_t * this, enum write_type op, FILE * fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  static int called = 0;
  
  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  if (op != SESSION)
    return 0;

  if(called == 0)
  {
	  fprintf (fp, "session\trequired\tpam_selinux.so\tclose ");
  }
  else if(called == 1)
  {
	  fprintf (fp, "session\trequired\tpam_selinux.so\topen ");
  }
  called++;
  
  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_1(SESSION)
GETOPT_END_1(SESSION)

PRINT_ARGS("selinux")
PRINT_XMLHELP("selinux")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_2 (is_enabled, debug);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;

static int
selinux_parse_config (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set (this, type);

  if (debug)
    printf ("**** def_parse_config [%s] (%s): '%s'\n", this->name,
            type2string (type), args ? args : "");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)             
  {                                           
      char *key = strsep (&args, " \t");        
      char *val;                                
      
      if (args)                                 
		  while (isspace ((int) *args))           
			  ++args;                               
	  
      if (strcmp (key, "use_first_pass") == 0)  
	  { /* will be ignored */ }                       
      else if (strcmp (key, "try_first_pass") == 0)     
	  { /* will be ignored */ }                       
      else if (strcmp (key, "use_authtok") == 0)        
	  { /* will be ignored */ }                       
      else if (strcmp (key, "open") == 0)        
	  { /* will be ignored */ }                       
      else if (strcmp (key, "close") == 0)        
	  { /* will be ignored */ }                       
      else if (NULL != (val = strchr (key, '=')))       
	  {                                                               
          *val++='\0';                                                  
          if (opt_set->set_opt (opt_set, key, strdup (val)) == FALSE)   
			  print_unknown_option_error (this->name, key);               
	  }                                                               
      else if (opt_set->enable (opt_set, key, TRUE) == FALSE)           
		  print_unknown_option_error (this->name, key);                   
  }
  return 1;
}


static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_selinux = { "pam_selinux.so", opt_sets, helptext,
				  &selinux_parse_config,
				  &def_print_module,
				  &write_config_selinux,
				  &get_opt_set,
				  &getopt,
				  &print_args,
				  &print_xmlhelp,
				  -1,
				  -1,
				  -1,
				  PAM_SELINUX_SESSION_PRIORITY
};
