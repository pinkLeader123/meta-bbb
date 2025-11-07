DESCRIPTION = "SmartFarm MQTT Python Application"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

SRC_URI = "git://github.com/pinkLeader123/smartFarm_app.git;protocol=https;branch=main"

# Modify these as desired
PV = "1.0+git${SRCPV}"
SRCREV = "8825220fa7dcc735463a741a60cde7b09b7e2ce0"

S = "${WORKDIR}/git"

# NOTE: no Makefile found, unable to determine what needs to be done

do_configure () {
	# Specify any needed configure commands here
	:
}

do_compile () {
	# Specify compilation commands here
	:
}

do_install () {
	install -d ${D}${bindir}
    install -m 0755 ${S}/app/mqtt_app.py ${D}${bindir}/smartfarm
}

FILES:${PN} += "${bindir}/app ${bindir}/mqtt_app.py"
INSANE_SKIP:${PN} = "ldflags"

