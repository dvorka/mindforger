# MindForger RPM spec TEMPLATE - do NOT build it directly
#
# build/docker/fedora-rpm/container-build.sh works on its own copy of this
# file and replaces:
#   @MF_VERSION@     ... MINDFORGER_VERSION_STRING from lib/src/app_info.h
#   @MF_RPM_RELEASE@ ... MF_RPM_RELEASE env (default 1)
# and appends generated %changelog entry - the spec inside the SRPM is then
# complete and self-contained (COPR rebuilds from the SRPM).
#
# %files is derived from INSTALLS in mindforger.pro and app/app.pro - if they
# change, this spec must follow (rpmbuild fails on unpackaged/missing files).

Name:           mindforger
Version:        @MF_VERSION@
Release:        @MF_RPM_RELEASE@%{?dist}
Summary:        Thinking notebook and Markdown IDE
# MindForger is GPL-2.0-or-later, statically linked cmark-gfm is BSD-2-Clause AND MIT
License:        GPL-2.0-or-later AND BSD-2-Clause AND MIT
URL:            https://www.mindforger.com
# tarball of Git tracked files incl. submodules - see build/fedora/fedora-copr.sh
Source0:        %{name}-%{version}.tar.gz

# Qt WebEngine exists only on some architectures
ExclusiveArch:  %{qt5_qtwebengine_arches}

BuildRequires:  gcc-c++
BuildRequires:  make
BuildRequires:  cmake
BuildRequires:  qt5-qtbase-devel
BuildRequires:  qt5-qttools-devel
BuildRequires:  qt5-qtwebengine-devel
BuildRequires:  hunspell-devel
BuildRequires:  libcurl-devel
BuildRequires:  zlib-devel
BuildRequires:  desktop-file-utils
BuildRequires:  libappstream-glib
Provides:       bundled(cmark-gfm)

%description
Search, browse, view and edit your Markdown files. Get as much
as possible from the knowledge in your remarks.

%prep
%autosetup

%build
# cmark-gfm is a git submodule linked statically - same flags as Debian build
cmake -S deps/cmark-gfm -B deps/cmark-gfm/build \
    -DCMARK_TESTS=OFF -DCMARK_SHARED=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build deps/cmark-gfm/build %{?_smp_mflags}
# PREFIX must be passed explicitly - *.pro files do not default it
# no ccache in clean package builds (COPR mock buildroot does not have it)
# app/app.pro links -lhunspell, but Fedora ships versioned libhunspell-X.Y.so
# only - build local symlink is used (the binary links the versioned soname)
mkdir -p build-libs
ln -s %{_libdir}/libhunspell-*.so build-libs/libhunspell.so
%qmake_qt5 PREFIX=%{_prefix} CONFIG+=mfnoccache "LIBS+=-L${PWD}/build-libs" mindforger.pro
%make_build

%install
make install INSTALL_ROOT=%{buildroot}

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/mindforger.desktop
appstream-util validate-relax --nonet %{buildroot}%{_metainfodir}/com.mindforger.mindforger.metainfo.xml

%files
%license LICENSE
%license deps/cmark-gfm/COPYING
%{_bindir}/mindforger
%{_docdir}/mindforger/
%{_mandir}/man1/mindforger.1*
%{_datadir}/icons/mindforger/
%{_datadir}/icons/hicolor/scalable/apps/mindforger.svg
%{_datadir}/icons/hicolor/128x128/apps/mindforger128x128.png
%{_datadir}/applications/mindforger.desktop
%{_metainfodir}/com.mindforger.mindforger.metainfo.xml

%changelog
