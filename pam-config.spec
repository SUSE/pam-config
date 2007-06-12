#
# spec file for package pam-config (Version 0.6)
#
# Copyright (c) 2006 SUSE LINUX Products GmbH, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# norootforbuild
# usedforbuild    aaa_base acl attr audit-libs autoconf automake bash bind-libs bind-utils binutils bison bzip2 coreutils cpio cpp cpp41 cracklib cvs cyrus-sasl db diffutils e2fsprogs file filesystem fillup findutils flex gawk gcc gcc41 gdbm gdbm-devel gettext gettext-devel glibc glibc-devel glibc-locale gpm grep groff gzip info insserv klogd less libacl libattr libcom_err libgcc41 libltdl libnscd libstdc++41 libtool libvolume_id libxcrypt libzio m4 make man mktemp module-init-tools ncurses ncurses-devel net-tools netcfg openldap2-client openssl pam pam-modules patch perl permissions popt procinfo procps psmisc pwdutils rcs readline rpm sed strace sysvinit tar tcpd texinfo timezone unzip util-linux vim zlib zlib-devel

Name:           pam-config
Summary:        Modify common PAM configuration files
Version:        0.6
Release:        1
License:        GPL
Autoreqprov:    on
Prefix:         %{_prefix}
Group:          System/Management
Source:         %{name}-%{version}.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
pam-config is a command line utility to maintain the common PAM
configuration files included by most PAM application configuration
files. It can be used to configure a system for different network or
hardware based authentication schemes. pam-config can also
add/adjust/remove other PAM modules and their options.



%prep
%setup -q

%build
%configure
make

%install
make install DESTDIR=$RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_sysconfdir}/pam.d
for i in account auth password session ; do
  touch $RPM_BUILD_ROOT/%{_sysconfdir}/pam.d/common-$i-pc
done

%clean
rm -rf $RPM_BUILD_ROOT

%post
if [ ! -f /etc/pam.d/common-auth-pc ] ; then
  pam-config --debug --initialize
else
  pam-config --debug --update ||:
fi

%postun
if [ $1 == 0 ]; then
  # Deinstall
  dir=/etc/security
  for conf in pam_unix2.conf pam_pwcheck.conf ; do
    if [ -f $dir/$conf.pam-config-backup -a ! -f $dir/$conf ]; then
      mv -v $dir/$conf.pam-config-backup $dir/$conf
    fi
  done
  dir=/etc/pam.d
  for pamd in common-account common-auth common-password common-session ; do
    if [ -f $dir/$pamd.pam-config-backup -a -L $dir/$pamd ]; then
      rm -v $dir/$pamd && mv -v $dir/$pamd.pam-config-backup $dir/$pamd
    fi
    # common-*-pc are %ghost, so we have to move them away...
    if [ -f $dir/$pamd-pc ]; then
      mv -v $dir/$pamd-pc $dir/$pamd-pc.bak
    fi
  done
fi

%files
%defattr(-,root,root)
%{_sbindir}/pam-config
%doc %{_mandir}/man8/pam-config.8*
%ghost %config %{_sysconfdir}/pam.d/common-account-pc
%ghost %config %{_sysconfdir}/pam.d/common-auth-pc
%ghost %config %{_sysconfdir}/pam.d/common-password-pc
%ghost %config %{_sysconfdir}/pam.d/common-session-pc

%changelog -n pam-config
* Wed Aug 23 2006 - kukuk@suse.de
- Fix stacking of modules, add query option for YaST2 (version 0.6)
* Tue Aug 22 2006 - kukuk@suse.de
- Bug fixes, add --update option (version 0.5)
* Tue Aug 22 2006 - kukuk@suse.de
- Add support for ccreds and pkcs11 (version 0.4)
* Mon Aug 21 2006 - kukuk@suse.de
- Add support for ldap and krb5 (version 0.3)
* Mon Aug 21 2006 - kukuk@suse.de
- Add documentation (version 0.2)
* Mon Aug 21 2006 - kukuk@suse.de
- Initial release
