Name:       openrepos-swissclock
Summary:    Fancy Clock
Version:    1.0.10
Release:    1
Group:      Applications/Productivity
License:    BSD
Vendor:     slava
URL:        https://github.com/monich/harbour-swissclock
Source0:    %{name}-%{version}.tar.bz2

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Requires:      sailfishsilica-qt5 >= 0.10.9
BuildRequires: pkgconfig(sailfishapp) >= 1.0.2
BuildRequires: pkgconfig(mlite5)
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Qml)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: desktop-file-utils
BuildRequires: qt5-qttools-linguist

%description
Fancy clock for Sailfish OS

%if "%{?vendor}" == "chum"
Categories:
 - Utility
Icon: https://raw.githubusercontent.com/monich/harbour-swissclock/master/icons/harbour-swissclock.svg
Screenshots:
- https://home.monich.net/chum/openrepos-swissclock/screenshots/screenshot-001.png
- https://home.monich.net/chum/openrepos-swissclock/screenshots/screenshot-002.png
- https://home.monich.net/chum/openrepos-swissclock/screenshots/screenshot-003.png
- https://home.monich.net/chum/openrepos-swissclock/screenshots/screenshot-004.png
- https://home.monich.net/chum/openrepos-swissclock/screenshots/screenshot-005.png
- https://home.monich.net/chum/openrepos-swissclock/screenshots/screenshot-006.png
Url:
  Homepage: https://openrepos.net/content/slava/swiss-clock
%endif

%prep
%setup -q -n %{name}-%{version}

%build
%qtc_qmake5 CONFIG+=openrepos CONFIG+=app_settings
%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

desktop-file-install --delete-original \
  --dir %{buildroot}%{_datadir}/applications \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}/qml
%{_datadir}/%{name}/settings
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/translations/%{name}*.qm
%{_datadir}/jolla-settings/entries/%{name}.json
