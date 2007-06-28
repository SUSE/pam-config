extern pam_module_t mod_pam_unix2;
extern pam_module_t mod_pam_umask;
extern pam_module_t mod_pam_mkhomedir;

pam_module_t *supported_module_list[] = {
  &mod_pam_unix2,
  &mod_pam_umask,
  &mod_pam_mkhomedir,
  NULL
};

static pam_module_t *module_list_auth[] = {
  &mod_pam_unix2,
  NULL
};

static pam_module_t *module_list_account[] = {
  &mod_pam_unix2,
  NULL
};

static pam_module_t *module_list_password[] = {
  &mod_pam_unix2,
   NULL
};

static pam_module_t *module_list_session[] = {
  &mod_pam_mkhomedir,
  &mod_pam_unix2,
  &mod_pam_umask,
  NULL
};
