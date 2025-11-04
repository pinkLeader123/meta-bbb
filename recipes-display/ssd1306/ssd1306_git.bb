# Recipe created by recipetool
# This is the basis of a recipe and may need further editing in order to be fully functional.
# (Feel free to remove these comments when editing.)

# Unable to find any files that looked like license statements. Check the accompanying
# documentation and source headers and set LICENSE and LIC_FILES_CHKSUM accordingly.
#
# NOTE: LICENSE is being set to "CLOSED" to allow you to at least start building - if
# this is not accurate with respect to the licensing of the software being built (it
# will not be in most cases) you must specify the correct value before using this
# recipe for anything other than initial testing/development!
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

SRC_URI = "git://github.com/Zk47T/ssd1306.git;protocol=https;branch=main \
           file://0001-add-func-display-ip.patch \
           "

# Modify these as desired
PV = "1.0+git${SRCPV}"
SRCREV = "b1397ef7ec704d8631b73b2c0521f9c804954d3a"

S = "${WORKDIR}/git"

# NOTE: no Makefile found, unable to determine what needs to be done

do_configure () {
	# Specify any needed configure commands here
	:
}

do_compile () {
	# Specify compilation commands here
	${CC} ${LDFLAGS} ${S}/src/main.c -o ssd1306
}

do_install () {
	# Specify install commands here
	install -d ${D}${bindir}
    install -m 0755 ssd1306 ${D}${bindir}/ssd1306
}

