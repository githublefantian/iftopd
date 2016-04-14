Name: iftopd
Version: %{?Ver}
Release:	1%{?dist}
Summary: processlist soft

Group: System Environment/Kernel
License: GPL	
#URL:		
Source0: dummy	
Source1: iftopdserv
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
#BuildRequires:	
#Requires:	

%description
iftopd does for network usage what top(1) does for CPU usage. It listens
to network traffic on a named interface and displays a table of current
bandwidth usage by pairs of hosts. It is handy for explaining why the
network links slow.

%prep
#%setup -q
%setup -D -T


%build
#make
autoreconf -fiv
export CFLAGS="%optflags $(ncursesw6-config --cflags)"
export LDFLAGS="$(ncursesw6-config --libs)"
%configure
%__make %{?jobs:-j%jobs} CPPFLAGS=-DUSE_GETIFADDRS


%install
%makeinstall
#rm -rf %{buildroot}
install -s -D  -m 755  %{buildroot}%{_sbindir}/iftop    %{buildroot}%{_sbindir}/iftopd
mv %{buildroot}%{_sbindir}/iftop  %{buildroot}%{_sbindir}/iftopd
%{__install} -p -D -m 755 %{SOURCE1}    %{buildroot}/etc/init.d/iftopd



%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%{_sbindir}/iftopd
%{_mandir}/man8/*
%attr(0755,root,root) /etc/init.d/iftopd


%changelog
* Mon Oct 26 2015 mengxiaosheng@dnion.com
- Use iftop -->  iftoppre3

