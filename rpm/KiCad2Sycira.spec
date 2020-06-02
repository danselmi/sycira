Name:           KiCad2Sycira
Version:        1.0
Release:        1%{?dist}
Summary: symbolic circuit analyzer for the Maxima computer algebra system

License: GPL-2        
URL: https://github.com/danselmi/sycira         
Source0: %{name}-%{version}.tar.bz2  

BuildRequires: git
BuildRequires: make
BuildRequires: gcc-c++
BuildRequires: tinyxml2-devel
BuildRequires: libzip-devel  
Requires: kicad
Requires: wxmaxima
Requires: tinyxml2
Requires: libzip    

%description 
symbolic circuit analyzer for the Maxima computer algebra system

%global debug_package %{nil}
%prep
%setup -q -n %{name}-%{version}
%build
make

%install
%{make_install} PREFIX=%{_bindir}

%files
/usr/bin/KiCad2Sycira
/usr/share/sycira/sycira.lib
/usr/share/sycira/sycira.mac


%changelog
* Mon May 25 2020 fwdev
- 
