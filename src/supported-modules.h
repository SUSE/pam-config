#include "mod_pam_config.h"

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
extern pam_module_t mod_pam_himmelblau;
extern pam_module_t mod_pam_kanidm;
extern pam_module_t mod_pam_krb5;
extern pam_module_t mod_pam_kwallet5;
extern pam_module_t mod_pam_lastlog2;
extern pam_module_t mod_pam_lastlog;
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
extern pam_module_t mod_pam_unix_ng;
extern pam_module_t mod_pam_winbind;
extern pam_module_t mod_pam_wtmpdb;

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

pam_module_t *common_builtin_modules[] = {
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
  &mod_pam_himmelblau,
  &mod_pam_kanidm,
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
  &mod_pam_unix_ng,
  &mod_pam_winbind,
  &mod_pam_wtmpdb,
  NULL
};

pam_module_t **common_module_list = NULL;

static pam_module_t **configurable_modules = NULL;
static size_t configurable_count = 0;

static inline pam_module_t* is_overridden_builtin(const pam_module_t *m) {
  if (!m || !m->name || !configurable_modules) return 0;
  for (size_t i = 0; configurable_modules[i]; ++i) {
    pam_module_t *cm = configurable_modules[i];
    if (cm && cm->name && strcmp(cm->name, m->name) == 0) {
      return cm; /* a configurable with the same soname exists */
    }
  }
  return NULL;
}

static void inject_fallback(pam_module_t *override, pam_module_t *fallback) {
  if (!override || !fallback) return;
  if (override->config->fallback) {
    inject_fallback(override->config->fallback, fallback);
  } else {
    override->config->fallback = fallback;
  }
}

__attribute__((constructor)) static void init_configurable_modules_ctor(void) {
  size_t n = load_configurable_modules(&configurable_modules);
  if (n > 0) {
    configurable_count = (size_t)n;
  }

  size_t builtin_count = 0;
  for (; common_builtin_modules[builtin_count]; builtin_count++);

  size_t cfg_count = 0;
  if (configurable_modules) {
    for (; configurable_modules[cfg_count]; cfg_count++);
  }

  size_t total = builtin_count + cfg_count;
  common_module_list = malloc((total + 1) * sizeof(pam_module_t *));
  if (!common_module_list) {
    return;
  }

  n = 0;
  for (size_t i = 0; i < builtin_count; ++i) {
    pam_module_t *common_module = common_builtin_modules[i];
    pam_module_t *override_module;
    if (!(override_module = is_overridden_builtin(common_module))) {
      common_module_list[n++] = common_module;
    } else {
      inject_fallback(override_module, common_module);
    }
  }
  for (size_t i = 0; i < cfg_count; ++i) {
    common_module_list[n++] = configurable_modules[i];
  }

  common_module_list[n] = NULL;
}

__attribute__((destructor))
static void free_configurable_modules_dtor(void)
{
  if (configurable_modules) {
    free_configurable_modules(configurable_modules);
    configurable_modules = NULL;
    configurable_count = 0;
  }
  if (common_module_list) {
    free(common_module_list);
    common_module_list = NULL;
  }
}

#define DEFINE_MODULE_SORTER(field)                                            \
  static int cmp_##field(const void *a, const void *b) {                       \
    const pam_module_t *const *pa = (const pam_module_t *const *)a;            \
    const pam_module_t *const *pb = (const pam_module_t *const *)b;            \
    const pam_module_t *ma = *pa;                                              \
    const pam_module_t *mb = *pb;                                              \
    return (ma->field - mb->field);                                            \
  }                                                                            \
                                                                               \
  static pam_module_t **sort_##field(pam_module_t **list, size_t n) {          \
    qsort(list, n, sizeof(pam_module_t *), cmp_##field);                       \
    return list;                                                               \
  }

/* Generate four sorter functions */
DEFINE_MODULE_SORTER(priority_auth)
DEFINE_MODULE_SORTER(priority_account)
DEFINE_MODULE_SORTER(priority_password)
DEFINE_MODULE_SORTER(priority_session)

#ifndef ARRAY_LEN
#  define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
#endif

/* Merge base + configurable into varname[] */
#define BUILD_SORTED_LIST(varname, stack)                                      \
  static pam_module_t **varname = NULL;                                        \
  static void init_##varname(void) __attribute__((constructor));               \
  static void init_##varname(void) {                                           \
    size_t base_n = ARRAY_LEN(stack##_modules);                                \
    if (base_n && (stack##_modules)[base_n - 1] == NULL) {                     \
      base_n--;                                                                \
    }                                                                          \
    /* worst case we add all configurable modules */                           \
    size_t cap = base_n + configurable_count + 1;                              \
    varname = (pam_module_t **)malloc(cap * sizeof(varname[0]));               \
    if (!varname) {                                                            \
      return;                                                                  \
    }                                                                          \
    /* copy base, but skip overridden built-ins */                             \
    size_t n = 0;                                                              \
    for (size_t i = 0; i < base_n; ++i) {                                      \
      pam_module_t *m = (stack##_modules)[i];                                  \
      if (!is_overridden_builtin(m)) {                                         \
        varname[n++] = m;                                                      \
      }                                                                        \
    }                                                                          \
    /* append eligible configurables */                                        \
    if (configurable_modules) {                                                \
      for (size_t i = 0; configurable_modules[i]; ++i) {                       \
        pam_module_t *m = configurable_modules[i];                             \
        if (m && m->config && m->config->stack##_line &&                       \
            m->priority_##stack >= 0) {                                        \
          varname[n++] = m;                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    /* sort and NULL terminate */                                              \
    sort_priority_##stack(varname, n);                                         \
    varname[n] = NULL;                                                         \
  }

/* Modifying the order of these modules no longer effects the order of placement
 * in pam. Use the module ordering in module_priorities.h instead.
 */
static pam_module_t *account_modules[] = {
  &mod_pam_access,
  &mod_pam_systemd_home,
  &mod_pam_himmelblau,
  &mod_pam_unix2,
  &mod_pam_unix,
  &mod_pam_unix_ng,
  &mod_pam_krb5,
  &mod_pam_localuser,
  &mod_pam_sss,
  &mod_pam_ldap,
  &mod_pam_nam,
  &mod_pam_winbind,
  &mod_pam_time,
  &mod_pam_kanidm,
  NULL
};
BUILD_SORTED_LIST(module_list_account, account)

/* Modifying the order of these modules no longer effects the order of placement
 * in pam. Use the module ordering in module_priorities.h instead.
 */
static pam_module_t *auth_modules[] = {
  &mod_pam_faildelay,
  &mod_pam_group,
  &mod_pam_pkcs11,
  &mod_pam_fp,
  &mod_pam_fprint,
  &mod_pam_fprintd,
  &mod_pam_thinkfinger,
  &mod_pam_himmelblau,
  &mod_pam_gnome_keyring,
  &mod_pam_kwallet5, /* optional modules MUST be executed before sufficient modules which also need a password. */
  &mod_pam_systemd_home,
  &mod_pam_ssh,
  &mod_pam_ecryptfs,
  &mod_pam_fscrypt,
  &mod_pam_unix2,
  &mod_pam_unix,
  &mod_pam_unix_ng,
  &mod_pam_krb5,
  &mod_pam_sss,
  &mod_pam_ldap,
  &mod_pam_nam,
  &mod_pam_winbind,
  &mod_pam_kanidm,
                   /* Attention: if you add another module behind krb5
					  you MUST change mod_pam_krb5.c */
  NULL
};
BUILD_SORTED_LIST(module_list_auth, auth)

/* Modifying the order of these modules no longer effects the order of placement
 * in pam. Use the module ordering in module_priorities.h instead.
 */
static pam_module_t *password_modules[] = {
  &mod_pam_himmelblau,
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
  &mod_pam_unix_ng,
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
BUILD_SORTED_LIST(module_list_password, password)

/* Modifying the order of these modules no longer effects the order of placement
 * in pam. Use the module ordering in module_priorities.h instead.
 */
static pam_module_t *session_modules[] = {
  &mod_pam_selinux,
  &mod_pam_limits,
  &mod_pam_ecryptfs,
  &mod_pam_fscrypt,
  &mod_pam_mkhomedir,
  &mod_pam_systemd_home,
  &mod_pam_systemd,
  &mod_pam_unix2,
  &mod_pam_unix,
  &mod_pam_unix_ng,
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
  &mod_pam_mktemp,
  &mod_pam_himmelblau,
  &mod_pam_kanidm,
  &mod_pam_env,
  NULL
};
BUILD_SORTED_LIST(module_list_session, session)

static pam_module_t *module_list_session_nl[] = {
  &mod_pam_ecryptfs,
  &mod_pam_fscrypt,
  &mod_pam_selinux,
  &mod_pam_mkhomedir,
  &mod_pam_systemd_home,
  // &mod_pam_systemd,
  &mod_pam_limits,
  &mod_pam_unix2,
  &mod_pam_unix,
  &mod_pam_unix_ng,
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
  &mod_pam_mktemp,
  &mod_pam_env,
  NULL
};

static pam_module_t *module_list_postlogin_account[] = {
  NULL
};

static pam_module_t *module_list_postlogin_auth[] = {
  NULL
};

static pam_module_t *module_list_postlogin_password[] = {
  NULL
};

static pam_module_t *module_list_postlogin_session[] = {
  &mod_pam_wtmpdb,
  &mod_pam_lastlog2,
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
