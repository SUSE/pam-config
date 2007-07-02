extern pam_module_t mod_pam_ccreds;
extern pam_module_t mod_pam_env;
extern pam_module_t mod_pam_krb5;
extern pam_module_t mod_pam_ldap;
extern pam_module_t mod_pam_limits;
extern pam_module_t mod_pam_localuser;
extern pam_module_t mod_pam_mkhomedir;
extern pam_module_t mod_pam_pwcheck;
extern pam_module_t mod_pam_umask;
extern pam_module_t mod_pam_unix2;
extern pam_module_t mod_pam_winbind;

pam_module_t *common_module_list[] = {
  &mod_pam_ccreds,
  &mod_pam_env,
  &mod_pam_krb5,
  &mod_pam_ldap,
  &mod_pam_limits,
  &mod_pam_localuser,
  &mod_pam_mkhomedir,
  &mod_pam_pwcheck,
  &mod_pam_umask,
  &mod_pam_unix2,
  &mod_pam_winbind,
  NULL
};

static pam_module_t *module_list_account[] = {
  &mod_pam_unix2,
  &mod_pam_krb5,
  &mod_pam_localuser,
  &mod_pam_ldap,
  &mod_pam_winbind,
  NULL
};

static pam_module_t *module_list_auth[] = {
  &mod_pam_env,
  &mod_pam_unix2,
  &mod_pam_krb5,
  &mod_pam_ldap,
  &mod_pam_winbind,
  NULL
};

static pam_module_t *module_list_password[] = {
  &mod_pam_winbind,
  &mod_pam_pwcheck,
  &mod_pam_unix2,
  &mod_pam_krb5,
  &mod_pam_ldap,
  NULL
};

static pam_module_t *module_list_session[] = {
  &mod_pam_mkhomedir,
  &mod_pam_limits,
  &mod_pam_unix2,
  &mod_pam_krb5,
  &mod_pam_ldap,
  &mod_pam_winbind,
  &mod_pam_umask,
  &mod_pam_env,
  NULL
};
