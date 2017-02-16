# bitbake recipes for all the release of as
LICENSE = "GPLv2"

DEPENDS += "readline python"

S = "${WORKDIR}/"
ASDIR= "`readlink -f ${TMPDIR}/../../../../..`"

# for rpm package /lib/as
FILES_${PN} += "/lib/as/*"

do_clean () {
  if [ -e ${S}/as ] ; then
    cd ${S}/as/release/aslua
    make clean-dist
  fi
}

do_compile () {
#	die " die for debug env: ${CXX}"
	ln -fs ${ASDIR} ${S}/as

	export verbose=1

	cd ${S}/as/release/aslua
	make 91
	make 92
#	make 81
#	make 82
	
#	cd ${S}/as/release/ascore
#	make 91
#	make 92

	cd ${S}/as/release/asboot
	make 91
	make 92
}

do_install () {
  mkdir -p ${D}/bin ${D}/lib/as/pyas
  cp ${S}/as/release/aslua/out/posix.exe  ${D}/bin/aslua
#  cp ${S}/as/release/ascore/out/posix.exe ${D}/bin/ascore
  cp ${S}/as/release/asboot/out/posix.exe ${D}/bin/asboot
  cp ${ASDIR}/com/as.tool/as.one.py/*.py  ${D}/lib/as
#  cp ${ASDIR}/com/as.tool/as.one.py/pyas/*.so  ${D}/lib/as/pyas
  cp ${ASDIR}/com/as.tool/as.one.py/pyas/*.py  ${D}/lib/as/pyas
  cp ${ASDIR}/com/as.tool/as.one.py/pyas/*.lua  ${D}/lib/as/pyas
  
  cd ${TMPDIR}/deploy/images/${MACHINE}

  if ! [ -d rootfs ]; then
    mkdir rootfs
    tar xf core-image-minimal-${MACHINE}.tar.bz2 -C ./rootfs
  fi
  
  cp ${D}/* ./rootfs/ -frv

  # need: sudo apt-get install android-tools-fsutils
  # size 4G
  # make_ext4fs -s -L system -l 4294967296 system.ext4 ./rootfs
}
