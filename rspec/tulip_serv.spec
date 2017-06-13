# MTS RPM SPEC FILE

Summary: Tulip Server
Name: tulip
Version: 1.0
Release: 1
License: Commercial License 
Group: misc
URL: http://www.thincatlabs.com
Distribution: RedHat Linux
Vendor: Thin Cat Labs
Packager: Thin Cat Labs <info@thincatlabs.com>

%description
Tulip Server service

%prep

%build
if [ ! -x ~/tulipc/out/tulip ];
then
make -C src clean-all
make -C src CL=$CLIENT
fi

%install
make -C src/ install

%files
/etc/tulip/tulip
/etc/tulip/config.cfg

%post
chkconfig --add tulip
chkconfig tulip on

%preun
service tulip stop
chkconfig tulip off
chkconfig --del tulip

%changelog
* Mon Jun 12 2017 - info (at) thincatlabs.com
- Initial 1.0 Release




