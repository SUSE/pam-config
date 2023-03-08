extern pam_module_t mod_pam_access;
extern pam_module_t mod_pam_apparmor;
extern pam_module_t mod_pam_ccreds;
extern pam_module_t mod_pam_cracklib;
extern pam_module_t mod_pam_deny;
extern pam_module_t mod_pam_ecryptfs;
extern pam_module_t mod_pam_env;
extern pam_module_t mod_pam_exec;
extern pam_module_t mod_pam_faildelay;
extern pam_module_t mod_pam_fp;
extern pam_module_t mod_pam_fprint;
extern pam_module_t mod_pam_fprintd;
extern pam_module_t mod_pam_fscrypt;
extern pam_module_t mod_pam_gnome_keyring;
extern pam_module_t mod_pam_group;
extern pam_module_t mod_pam_krb5;
extern pam_module_t mod_pam_kwallet5;
extern pam_module_t mod_pam_lastlog;
extern pam_module_t mod_pam_lastlog2;
extern pam_module_t mod_pam_ldap;
extern pam_module_t mod_pam_limits;
extern pam_module_t mod_pam_localuser;
extern pam_module_t mod_pam_make;
extern pam_module_t mod_pam_mkhomedir;
extern pam_module_t mod_pam_mktemp;
extern pam_module_t mod_pam_nam;
extern pam_module_t mod_pam_passwdqc;
extern pam_module_t mod_pam_pkcs11;
extern pam_module_t mod_pam_pwcheck;
extern pam_module_t mod_pam_pwhistory;
extern pam_module_t mod_pam_pwquality;
extern pam_module_t mod_pam_selinux;
extern pam_module_t mod_pam_ssh;
extern pam_module_t mod_pam_sss;
extern pam_module_t mod_pam_succeed_if;
extern pam_module_t mod_pam_systemd_home;
extern pam_module_t mod_pam_thinkfinger;
extern pam_module_t mod_pam_time;
extern pam_module_t mod_pam_umask;
extern pam_module_t mod_pam_unix2;
extern pam_module_t mod_pam_unix;
extern pam_module_t mod_pam_winbind;

/* modules for single services only.  */
extern pam_module_t mod_pam_ck_connector;
extern pam_module_t mod_pam_cryptpass;
extern pam_module_t mod_pam_csync;
extern pam_module_t mod_pam_google_authenticator;
extern pam_module_t mod_pam_keyinit;
extern pam_module_t mod_pam_loginuid;
extern pam_module_t mod_pam_mount;
extern pam_module_t mod_pam_systemd;
extern pam_module_t mod_pam_u2f;

pam_module_t *common_module_list[] = {
  &mod_pam_access,
  &mod_pam_apparmor,
  &mod_pam_ccreds,
  &mod_pam_cracklib,
  &mod_pam_deny,
  &mod_pam_ecryptfs,
  &mod_pam_env,
  &mod_pam_exec,
  &mod_pam_faildelay,
  &mod_pam_fp,
  &mod_pam_fprint,
  &mod_pam_fprintd,
  &mod_pam_fscrypt,
  &mod_pam_gnome_keyring,
  &mod_pam_group,
  &mod_pam_krb5,
  &mod_pam_kwallet5,
  &mod_pam_lastlog2,
  &mod_pam_ldap,
  &mod_pam_limits,
  &mod_pam_localuser,
  &mod_pam_make,
  &mod_pam_mkhomedir,
  &mod_pam_mktemp,
  &mod_pam_nam,
  &mod_pam_passwdqc,
  &mod_pam_pkcs11,
  &mod_pam_pwcheck,
  &mod_pam_pwhistory,
  &mod_pam_pwquality,
  &mod_pam_selinux,
  &mod_pam_ssh,
  &mod_pam_sss,
  &mod_pam_succeed_if,
  &mod_pam_systemd,
  &mod_pam_systemd_home,
  &mod_pam_thinkfinger,
  &mod_pam_umask,
  &mod_pam_unix,
  &mod_pam_unix2,
  &mod_pam_winbind,
  NULL
};

static pam_module_t *module_list_account[] = {
  &mod_pam_access,
  &mod_pam_systemd_home,
  &mod_pam_unix2,
  &mod_pam_unix,
  &mod_pam_krb5,
  &mod_pam_localuser,
  &mod_pam_sss,
  &mod_pam_ldap,
  &mod_pam_nam,
  &mod_pam_winbind,
  &mod_pam_time,
  NULL
};

static pam_module_t *module_list_auth[] = {
  &mod_pam_env,
  &mod_pam_faildelay,
  &mod_pam_group,
  &mod_pam_pkcs11,
  &mod_pam_fp,
  &mod_pam_fprint,
  &mod_pam_fprintd,
  &mod_pam_thinkfinger,
  &mod_pam_gnome_keyring,
  &mod_pam_kwallet5, /* optional modules MUST be executed before sufficient modules which also need a password. */
  &mod_pam_systemd_home,
  &mod_pam_ssh,
  &mod_pam_unix2,
  &mod_pam_unix,
  &mod_pam_ecryptfs,
  &mod_pam_fscrypt,
  &mod_pam_krb5,
  &mod_pam_sss,
  &mod_pam_ldap,
  &mod_pam_nam,
  &mod_pam_winbind,
                   /* Attention: if you add another module behind krb5
					  you MUST change mod_pam_krb5.c */
  NULL
};

static pam_module_t *module_list_password[] = {
  &mod_pam_winbind,
  &mod_pam_pwcheck,
  &mod_pam_passwdqc,
  &mod_pam_pwquality,
  &mod_pam_cracklib,
  &mod_pam_pwhistory,
  &mod_pam_gnome_keyring,
  &mod_pam_kwallet5,
  &mod_pam_ecryptfs,
  &mod_pam_systemd_home,
  &mod_pam_unix2,
  &mod_pam_unix,
  &mod_pam_fscrypt,
  &mod_pam_make,
  &mod_pam_exec,
  &mod_pam_krb5,
  &mod_pam_sss,
  &mod_pam_ldap,
  &mod_pam_nam,    /* Attention: if you add another module behind krb5
					  you MUST change mod_pam_krb5.c */
  NULL
};

static pam_module_t *module_list_session[] = {
  &mod_pam_selinux,
  &mod_pam_mkhomedir,
  &mod_pam_systemd_home,
  &mod_pam_systemd,
  &mod_pam_lastlog2,
  &mod_pam_limits,
  &mod_pam_unix2,
  &mod_pam_unix,
  &mod_pam_apparmor,
  &mod_pam_krb5,
  &mod_pam_sss,
  &mod_pam_ldap,
  &mod_pam_winbind,
  &mod_pam_nam,
  &mod_pam_umask,
  &mod_pam_ssh,
  &mod_pam_selinux,
  &mod_pam_gnome_keyring,
  &mod_pam_kwallet5,
  &mod_pam_exec,
  &mod_pam_ecryptfs,
  &mod_pam_fscrypt,
  &mod_pam_env,
  &mod_pam_mktemp,
  NULL
};

pam_module_t *service_module_list[] = {
  &mod_pam_ck_connector,
  &mod_pam_cryptpass,
  &mod_pam_csync,
  &mod_pam_google_authenticator,
  &mod_pam_keyinit,
  &mod_pam_lastlog,
  &mod_pam_loginuid,
  &mod_pam_mount,
  &mod_pam_u2f,
  NULL
};
