Name: cspclient
Summary: general client SOAP for manage CA
Version: 1.4.2
Release: 1
Epoch: 0
License: LGPL
Group: Applications
URL: http://www.unirel.com
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}
Provides: cspclient
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
This package provides the program client for manage one general server SOAP for manage CA

%prep
%setup

%build
%configure --enable-static --enable-shared
cd src/ulib
make LDFLAGS="-s" 
cd ../../examples/csp
make LDFLAGS="-s" 
cd ../..

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/srv/CSP/bin
mkdir -p %{buildroot}/srv/CSP/etc
autoconf/install-sh -c -m 755 examples/csp/.libs/cspclient %{buildroot}/srv/CSP/bin
autoconf/install-sh -c -m 644 tests/examples/cspclient.cfg %{buildroot}/srv/CSP/etc/cspclient.cfg.dist

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
/srv/CSP/bin/cspclient
/srv/CSP/etc/cspclient.cfg.dist
