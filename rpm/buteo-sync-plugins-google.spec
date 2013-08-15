Name: buteo-sync-plugins-google
Version: 0.1.0
Release: 1
Summary: Buteo Google Synchronization plugins
Group: System/Libraries
URL: https://github.com/nemomobile/buteo-sync-plugins-google
License: LGPLv2.1
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Contacts)
BuildRequires: pkgconfig(Qt5Network)
BuildRequires: pkgconfig(Qt5Test)
BuildRequires: pkgconfig(signon-oauth2plugin)
BuildRequires: pkgconfig(buteosyncfw5)
# Change to below pkgconfig dependencies when we don't have broken packages
# from mer anymore
BuildRequires: signon-qt5-devel
#BuildRequires: pkgconfig(signon-plugins)
#BuildRequires: pkgconfig(libsignon-qt5)
BuildRequires: pkgconfig(accounts-qt5)
BuildRequires: qtcontacts-sqlite-qt5

%description
%{summary}.

%files
%defattr(-,root,root,-)
%config %{_sysconfdir}/buteo/profiles/client/googlecontacts.xml
%config %{_sysconfdir}/buteo/profiles/sync/googlecontacts.xml
%{_libdir}/buteo-plugins-qt5/*.so


%package tests
Summary: Tests for %{name}
Group: System/Libraries
Requires: %{name} = %{version}-%{release}

%description tests
%{summary}.

%files tests
%defattr(-,root,root,-)
/opt/tests/%{name}


%prep
%setup -q


%build
%qmake5 -recursive
make %{?_smp_mflags}


%install
make INSTALL_ROOT=%{buildroot} install
