extern pam_module_t mod_pam_unix2;
/* extern pam_module_t mod_pam_pwcheck; */

pam_module_t *supported_module_list[] = {
  &mod_pam_unix2,
  /* &mod_pam_pwcheck, */
  NULL
};
