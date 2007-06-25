#include "pam-module.h"

#include <stdio.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
int
err_parse_option( pam_module_t *this __attribute__ ((unused)), char *arguments, config_file_t *conf __attribute__ ((unused)) ){
  if (arguments){
    return 0;
  }
  return 1;
}

int
def_print_module( pam_module_t *this, config_file_t *conf __attribute__ ((unused)) ){
  printf( "default print module:\t%s", this->name );
  return 1;
}

int
def_write_config( pam_module_t *this, enum write_type op __attribute__ ((unused)), config_file_t *conf __attribute__ ((unused)) ){
  printf( "default write module:\t%s", this->name );
  return 1;
}

pam_module_t* lookup( pam_module_t **module_list, char *module ){
  while (*module_list != NULL) {
    if( 0 == strcmp( (*module_list)->name, module ) ){
      return *module_list;
    }
    module_list++;
  };
  return *module_list;
}

int
handle_module( const char *file, char *m, char *arguments , pam_module_t **module_list, config_file_t *conf ){
  pam_module_t *module = lookup( module_list, m );
  if( NULL != module ){
    if( ! module->parse_option( module, arguments, conf ) ){
      fprintf (stderr, _("%s (%s): Arguments will be ignored\n"),  file, m );
    }
  }
  else{
    fprintf (stderr, _("%s: Unknown module %s, ignored!\n"), file, m);
  }
  return 1;
} 

void
print_unknown_option_error (const char *module, const char *option)
{
  /* TRANSLATORS: first argument is name of a PAM module */
  fprintf (stderr, _("Unknown option for %s, ignored: '%s'\n"),
	   module, option);
}
