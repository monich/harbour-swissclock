Name:       harbour-swissclock
Summary:    Fancy Clock
Version:    1.0.6
Release:    1
Group:      Applications/Productivity
License:    BSD
Vendor:     meego
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

%prep
%setup -q -n %{name}-%{version}

%build
%qtc_qmake5
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
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png

%changelog
* Tue Oct 04 2016 Slava Monich <slava.monich@jolla.com> 1.0.6
- Added support for landscape and made it default on tablet

* Sun Oct 02 2016 Slava Monich <slava.monich@jolla.com> 1.0.5
- Vectorized some rendering, now it's usable on hires devices

* Sun May 03 2015 Slava Monich <slava.monich@jolla.com> 1.0.4
- Added Deutsche Bahn clock

* Sun Jan 04 2015 Slava Monich <slava.monich@jolla.com> 1.0.3
- Rendering optimization

* Sun Oct 26 2014 Slava Monich <slava.monich@jolla.com> 1.0.2
- Store settings in dconf
- Fixed a problem with the minute hand being a bit too blurry
- Added Helsinki metro clock

* Fri Jun 27 2014 Slava Monich <slava.monich@jolla.com> 1.0.1
- Fixed corrupted background (hopefully)

* Fri Jun 13 2014 Slava Monich <slava.monich@jolla.com> 1.0.0
- Initial version
