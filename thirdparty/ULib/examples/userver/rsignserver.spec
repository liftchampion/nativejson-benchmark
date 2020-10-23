Name: rsignserver
Summary: general server SOAP for manage Remote Sign
Version: 1.4.2
Release: 1
Epoch: 0
License: LGPL
Group: Applications
URL: http://www.unirel.com
Source0: %{name}-%{version}.tar.gz
Source1: rsignserver.start
BuildRoot: %{_tmppath}/%{name}-%{version}
Provides: rsignserver
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
This package provides the program server for manage one general server SOAP for manage Remote Sign

%prep
%setup

%build
%configure --enable-static --enable-shared
cd src/ulib
make LDFLAGS="-s"
cd ../../examples/userver
make LDFLAGS="-s"
cd ../..

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/srv/RSIGN/bin
mkdir -p %{buildroot}/srv/RSIGN/etc
mkdir -p %{buildroot}/srv/RSIGN/var/RSIGN_command
mkdir -p $RPM_BUILD_ROOT/%{_initrddir}
mkdir -p $RPM_BUILD_ROOT/etc/sysconfig
autoconf/install-sh -c -m 755 examples/userver/rsignserver.start				$RPM_BUILD_ROOT/%{_initrddir}/rsignserver
SRC=tests/examples
DST=%{buildroot}/srv/RSIGN
autoconf/install-sh -c -m 755 examples/userver/.libs/userver_ssl				$DST/bin/rsignserver
autoconf/install-sh -c -m 644 $SRC/userver.cfg										$DST/etc/userver.cfg.dist
autoconf/install-sh -c -m 644 $SRC/mod_http.cfg										$DST/etc/mod_http.cfg.dist
autoconf/install-sh -c -m 644 $SRC/mod_soap_or_rpc_rsign.cfg					$DST/etc/mod_soap_or_rpc_rsign.cfg.dist
autoconf/install-sh -c -m 644 $SRC/rsignserver.cfg									$DST/etc/rsignserver.cfg.dist
autoconf/install-sh -c -m 644 $SRC/rsignserver_rpc.cfg							$DST/etc/rsignserver_rpc.cfg.dist
autoconf/install-sh -c -m 644 $SRC/RSIGN/RSIGN_command/.function				$DST/var/RSIGN_command
autoconf/install-sh -c -m 755 $SRC/RSIGN/RSIGN_command/rsign_SIGN_B64.sh	$DST/var/RSIGN_command
autoconf/install-sh -c -m 755 $SRC/RSIGN/RSIGN_command/rsign_SIGN_BIN.sh	$DST/var/RSIGN_command

cat > $RPM_BUILD_ROOT/etc/sysconfig/rsignserver << EOF
#ld_library_path=/usr/lib64
exe=/srv/RSIGN/bin/rsignserver
confdir=/srv/RSIGN/etc
EOF

%post
/sbin/chkconfig --add rsignserver

%preun
%{_initrddir}/rsignserver stop
/sbin/chkconfig --del rsignserver

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_initrddir}/rsignserver
/etc/sysconfig/rsignserver
/srv/RSIGN/bin/rsignserver
/srv/RSIGN/etc/*.cfg.dist
/srv/RSIGN/var/RSIGN_command/*.sh
/srv/RSIGN/var/RSIGN_command/.function
