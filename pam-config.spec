#
# spec file for package pam-config (Version 0.3)
#
# Copyright (c) 2006 SUSE LINUX Products GmbH, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# norootforbuild

Name:           pam-config
Summary:        Modify common PAM configuration files
Version:        0.4
Release:        1
License:        GPL
Autoreqprov:    on
Prefix:         %{_prefix}
Group:          System Environment/Base
Source:         %{name}-%{version}.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
pam-config is a tool to maintain the common PAM configuration files.

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
fi

%postun
if [ $1 == 0 ]; then
  # Deinsall
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
