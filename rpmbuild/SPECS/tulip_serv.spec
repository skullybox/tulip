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
if [ ! -d src ]; then echo "add source..."; ln -s ../../src/ .; fi

%build
echo "cleaning source..."
make -C src/ clean-all
echo "building source..."
make -C src CL=$CLIENT

%install
echo "installing to buildroot..."
make -C src/ install

%files
/etc/tulip/tulip

%post

%preun

%changelog
* Mon Jun 12 2017 - info (at) thincatlabs.com
- Initial 1.0 Release




