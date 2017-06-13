#! /bin/bash
mkdir -p rpmbuild/BUILD
echo '%_topdir %(pwd)/rpmbuild' > ~/.rpmmacros
rpmbuild -ba rpmbuild/SPECS/tulip_serv.spec

