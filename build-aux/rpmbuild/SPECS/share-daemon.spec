Name:           share-daemon
Version:        4.0
Release:        1%{?dist}
Summary:        The share library network synchronization daemon.

Group:          System Environment/Libraries
License:        GPLv3+
URL:            http://www.sharelib.net/
Source0:        http://ftp.neo-natura.com/release/share-daemon/share-daemon-4.0.tar.gz

BuildRequires:  help2man, doxygen
#Requires:       java-1.8.0-openjdk
Requires:       libshare

%description


%description    


%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'


%check
make check


%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%doc
%{_sbindir}/shared
%{_sbindir}/shfsyncd
%{_mandir}/man1/shared.1.gz


%changelog
* Sun Jul 29 2019 Neo Natura <support@neo-natura.com> - 4.0
- The RPM release of the libshare network synchronization daemon.
