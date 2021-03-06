Name: lcsp
Summary: general client-server SOAP for manage CA on localhost
Version: 1.4.2
Release: 1
Epoch: 0
License: LGPL
Group: Applications
URL: http://www.unirel.com
Source0: %{name}-%{version}.tar.gz
Source1: lcspserver.start
BuildRoot: %{_tmppath}/%{name}-%{version}
Provides: lcspserver lcspclient
Packager: Stefano Casazza <stefano.casazza@unirel.com>
Requires: ULib
Requires: zlib
Requires: expat
Requires: file
Requires: pcre
Requires: uuid
Requires: openssl
Requires: libstdc++
BuildRequires: expat
BuildRequires: zlib-devel
BuildRequires: file-devel
BuildRequires: pcre-devel
BuildRequires: openssl-devel
BuildRequires: libstdc++-devel

%description
This package provides the prgram client and server for manage one general server SOAP for manage CA on localhost

%prep
%setup

%build
%configure --enable-static --enable-shared
cd src/ulib
make LDFLAGS="-s"
cd ../../examples/lcsp
make LDFLAGS="-s"
cd ../userver
make LDFLAGS="-s"
cd ../..

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/srv/LCSP/bin
mkdir -p %{buildroot}/srv/LCSP/etc
mkdir -p %{buildroot}/srv/LCSP/var/DB_CA
mkdir -p %{buildroot}/srv/LCSP/var/LCSP_command
mkdir -p $RPM_BUILD_ROOT/%{_initrddir}
mkdir -p $RPM_BUILD_ROOT/etc/sysconfig
autoconf/install-sh -c -m 755 examples/lcsp/lcspserver.start				$RPM_BUILD_ROOT/%{_initrddir}/lcspserver
SRC=tests/examples
DST=%{buildroot}/srv/LCSP
autoconf/install-sh -c -m 755 examples/lcsp/.libs/lcspclient				$DST/bin
autoconf/install-sh -c -m 644 $SRC/lcspclient.cfg								$DST/etc/lcspclient.cfg.dist
autoconf/install-sh -c -m 755 examples/userver/.libs/userver_ipc			$DST/bin/lcspserver
autoconf/install-sh -c -m 644 $SRC/userver.cfg                          $DST/etc/userver.cfg.dist
autoconf/install-sh -c -m 644 $SRC/mod_http.cfg                         $DST/etc/mod_http.cfg.dist
autoconf/install-sh -c -m 644 $SRC/mod_soap_or_rpc_csp.cfg              $DST/etc/mod_soap_or_rpc_csp.cfg.dist
autoconf/install-sh -c -m 644 $SRC/lcspserver.cfg								$DST/etc/lcspserver.cfg.dist
autoconf/install-sh -c -m 644 $SRC/CSP/DB_CA/openssl.cnf.tmpl				$DST/var/DB_CA
autoconf/install-sh -c -m 644 $SRC/LCSP/LCSP_command/.function				$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_CA.sh				$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_EMIT_CRL.sh		$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_GET_CA.sh		$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_GET_CRL.sh		$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_LIST_CA.sh		$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_LIST_CERTS.sh	$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_REMOVE_CERT.sh	$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_REVOKE_CERT.sh	$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_SIGN_P10.sh		$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_SIGN_SPACK.sh	$DST/var/LCSP_command
autoconf/install-sh -c -m 755 $SRC/LCSP/LCSP_command/csp_ZERO_CERTS.sh	$DST/var/LCSP_command

cat > $RPM_BUILD_ROOT/etc/sysconfig/lcspserver << EOF
#ld_library_path=/usr/lib64
exe=/srv/LCSP/bin/lcspserver
confdir=/srv/LCSP/etc
EOF

%post
/sbin/chkconfig --add lcspserver

%preun
%{_initrddir}/lcspserver stop
/sbin/chkconfig --del lcspserver

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_initrddir}/lcspserver
/etc/sysconfig/lcspserver
/srv/LCSP/bin/lcsp*
/srv/LCSP/etc/*.cfg.dist
/srv/LCSP/var/DB_CA/openssl.cnf.tmpl
/srv/LCSP/var/LCSP_command/*.sh
/srv/LCSP/var/LCSP_command/.function
