LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

SRC_URI = "git://github.com/pinkLeader123/genotp.git;protocol=https;branch=main"

PV = "1.0+git${SRCPV}"
SRCREV = "1149d4ea305a30f42efc58fa44e29aefff4dd65c"

S = "${WORKDIR}/git"

# Có Makefile trong src
EXTRA_OEMAKE = "CC='${CC}'"

do_configure() {
    :
}

do_compile() {
    # Build binary trong thư mục src
    oe_runmake -C ${S}/src
}

do_install() {
    # Tạo thư mục đích
    install -d ${D}${bindir}

    # Cài file nhị phân
    install -m 0755 ${S}/src/genotp ${D}${bindir}/genotp

    # Cài file Python
    install -m 0755 ${S}/tools/senOTP.py ${D}${bindir}/senOTP.py
}

FILES:${PN} += "${bindir}/genotp ${bindir}/senOTP.py"
INSANE_SKIP:${PN} = "ldflags"