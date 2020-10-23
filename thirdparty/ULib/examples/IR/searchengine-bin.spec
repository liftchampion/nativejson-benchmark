Name: searchengine-bin
Summary: search engine to manage search
Version: 1.4.2 
Release: 1
Epoch: 0
License: LGPL
Group: Applications
URL: http://www.unirel.com
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}
Provides: searchengine-bin
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
This package provides the programs search engine for general search 

%prep
%setup

%build
%configure --enable-static --enable-shared
cd src/ulib
make LDFLAGS="-s" 
cd ../../examples/IR
make LDFLAGS="-s"
cd ../..

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/srv/searchengine/bin
autoconf/install-sh -c -m 755 examples/IR/.libs/index1		%{buildroot}/srv/searchengine/bin
autoconf/install-sh -c -m 755 examples/IR/.libs/query			%{buildroot}/srv/searchengine/bin
autoconf/install-sh -c -m 755 examples/IR/.libs/update		%{buildroot}/srv/searchengine/bin
autoconf/install-sh -c -m 755 examples/IR/.libs/db_check		%{buildroot}/srv/searchengine/bin
autoconf/install-sh -c -m 755 examples/IR/.libs/db_dump		%{buildroot}/srv/searchengine/bin

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
/srv/searchengine/bin/*
%doc tests/examples/index.cfg
%doc tests/examples/index_dir.cfg
