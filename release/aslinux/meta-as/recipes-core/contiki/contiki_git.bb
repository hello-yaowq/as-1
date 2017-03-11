DESCRIPTION = "The Open Source OS for the Internet of Things"
HOMEPAGE = "http://www.contiki-os.org/"
SECTION = "IoT/Network"
LICENSE = "3-clause BSD"

# get the whole git is really very slow
#SRCREV = "248770b0850495cce651aecc941b3c27e7dd4141"
#SRC_URI = "git://github.com/contiki-os/contiki.git;protocal=git;"

S = "${WORKDIR}/"

BBCLASSEXTEND = "native"

do_fetch() {
  if ! [ -e ${TMPDIR}/../downloads/contiki_3.x.tar.gz.done ] ; then
    wget https://github.com/contiki-os/contiki/archive/3.x.tar.gz -O  ${TMPDIR}/../downloads/contiki_3.x.tar.gz
    touch ${TMPDIR}/../downloads/contiki_3.x.tar.gz.done
  fi
}

do_unpack() {
  if ! [ -e ${S}/contiki ] ; then
    tar xf ${TMPDIR}/../downloads/contiki_3.x.tar.gz -C ${S}
  fi
}

do_compile() {
  cd ${S}/contiki/examples/webserver
  make V=1 LD="${CC} ${LDFLAGS}"

}

do_install() {
  mkdir -p ${S}/image/usr/bin
  cp -f ${S}/contiki/examples/webserver/webserver-example.minimal-net ${S}/image/usr/bin
}
