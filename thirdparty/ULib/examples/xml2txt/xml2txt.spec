Name: xml2txt
Summary: Expects xml as input, outputs text only
Version: 1.4.2
Release: 1
Epoch: 0
License: LGPL
Group: Applications
URL: http://www.unirel.com
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}
Provides: xml2txt
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
This package provides the program for to print only text from xml files

%prep
%setup

%build
%configure --enable-static --enable-shared
cd src/ulib
make LDFLAGS="-s"
cd ../../examples/xml2txt
make LDFLAGS="-s"
cd ../..

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/srv/searchengine/bin
autoconf/install-sh -c -m 755 examples/xml2txt/.libs/xml2txt	%{buildroot}/srv/searchengine/bin

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
/srv/searchengine/bin/xml2txt
