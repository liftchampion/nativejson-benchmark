Name: web_server
Summary: general HTTP web server for manage CA
Version: 1.4.2
Release: 1
Epoch: 0
License: LGPL
Group: Applications
URL: http://www.unirel.com
Source0: %{name}-%{version}.tar.gz
Source1: web_server.start
BuildRoot: %{_tmppath}/%{name}-%{version}
Provides: web_server
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
This package provides the program for manage one general HTTP web server for manage CA

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
mkdir -p %{buildroot}/srv/WEB/bin
mkdir -p %{buildroot}/srv/WEB/etc
mkdir -p %{buildroot}/srv/WEB/var/cgi-bin
mkdir -p %{buildroot}/srv/WEB/var/icons
mkdir -p $RPM_BUILD_ROOT/%{_initrddir}
mkdir -p $RPM_BUILD_ROOT/etc/sysconfig
SRC=tests/examples
DST=%{buildroot}/srv/WEB
autoconf/install-sh -c -m 755 examples/userver/web_server.start	$RPM_BUILD_ROOT/%{_initrddir}/web_server
autoconf/install-sh -c -m 755 examples/userver/.libs/userver_tcp	$DST/bin/web_server
autoconf/install-sh -c -m 644 $SRC/userver.cfg							$DST/etc/userver.cfg.dist
autoconf/install-sh -c -m 644 $SRC/web_server.cfg						$DST/etc/web_server.cfg.dist
autoconf/install-sh -c -m 644 $SRC/CSP/WEB/cgi-bin/.env				$DST/var/cgi-bin
autoconf/install-sh -c -m 644 $SRC/CSP/WEB/cgi-bin/get-ca.sh		$DST/var/cgi-bin
autoconf/install-sh -c -m 644 $SRC/CSP/WEB/cgi-bin/get-crl.sh		$DST/var/cgi-bin
autoconf/install-sh -c -m 644 $SRC/icons/gopher-unknown.gif			$DST/var/icons
autoconf/install-sh -c -m 644 $SRC/icons/menu.png						$DST/var/icons

cat > $RPM_BUILD_ROOT/etc/sysconfig/web_server << EOF
#ld_library_path=/usr/lib64
exe=/srv/WEB/bin/web_server
confdir=/srv/WEB/etc
EOF

%post
/sbin/chkconfig --add web_server

%preun
%{_initrddir}/web_server stop
/sbin/chkconfig --del web_server

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_initrddir}/web_server
/etc/sysconfig/web_server
/srv/WEB/bin/web_server
/srv/WEB/etc/*.cfg.dist
/srv/WEB/var/cgi-bin/*.sh
/srv/WEB/var/cgi-bin/.env
/srv/WEB/var/icons/*.gif
/srv/WEB/var/icons/*.png
