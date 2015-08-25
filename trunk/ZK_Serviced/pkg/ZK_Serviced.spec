%define debug_package %{nil}
%define __os_install_post %{nil}
%define _prefix /home/muyanxue/rpm/ZK_Serviced
%define srcname ZK_Serviced

summary: ZK_Serviced-daemon
name: ZK_Serviced
version: 1.0.0.1
release: 1%{?dist}
url: clouduser.cn
license: GPL
vendor: <http://www.clouduser.cn>
group: Application/Internet
source: %{srcname}.tar.gz
#provides: 
#requires: 
#buildrequires: 
buildroot: %{_tmppath}/%{name}-%{version}-%{release}-%(%{__id_u} -n)

%description
ZK_Serviced aim at supply a service which update sysinfo into zookeeper
if any question contact at programer.moo@gmail.com

%prep
%setup -q -n %{srcname}

%build
# your package build steps
make

%install
rm -rf %{buildroot}
# your package install steps
# the compiled files dir: %{_builddir}/<package_source_dir> or $RPM_BUILD_DIR/<package_source_dir>
# the dest root dir: %{buildroot} or $RPM_BUILD_ROOT
mkdir -p %{buildroot}/%{_prefix}/
mkdir -p %{buildroot}/%{_prefix}/bin/
mkdir -p %{buildroot}/%{_prefix}/libs/
mkdir -p %{buildroot}/%{_prefix}/logs/
mkdir -p %{buildroot}/%{_sysconfdir}/
mkdir -p %{buildroot}/%{_sysconfdir}/cron.d/

pushd %{_builddir}/%{srcname}/
cp -rf bin %{buildroot}/%{_prefix}/
cp -rf libs %{buildroot}/%{_prefix}/
cp -rf scripts/ZK_Serviced_clean_crontab %{buildroot}/%{_sysconfdir}/cron.d/
popd

%files
#%defattr(-,search,search)
%defattr(-,root,root)
# list your package files here
# the list of the macros:
#   _prefix           /usr
#   _exec_prefix      %{_prefix}
#   _bindir           %{_exec_prefix}/bin
#   _libdir           %{_exec_prefix}/%{_lib}
#   _libexecdir       %{_exec_prefix}/libexec
#   _sbindir          %{_exec_prefix}/sbin
#   _includedir       %{_prefix}/include
#   _datadir          %{_prefix}/share
#   _sharedstatedir   %{_prefix}/com
#   _sysconfdir       /etc
#   _initrddir        %{_sysconfdir}/rc.d/init.d
#   _var              /var
%{_prefix}/bin
%{_prefix}/libs
%{_prefix}/logs

%defattr(-,root,root)
%{_sysconfdir}/cron.d/ZK_Serviced_clean_crontab

%pre
# pre-install scripts

%post
# post-install scripts

%preun
# pre-uninstall scripts
%{_prefix}/bin/stop.sh ZK_Serviced

%postun
# post-uninstall scripts

%clean
rm -rf %{buildroot}
# your package build clean up steps here

%changelog
# list your change log here

