DESCRIPTION = "Auto WiFi connect at boot (SysVinit)"
LICENSE = "CLOSED"

SRC_URI = " \
    file://wifi-connect.sh \
"

S = "${WORKDIR}"

do_install() {
    # Cài script chính
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/wifi-connect.sh ${D}${bindir}/wifi-connect.sh

    # Tạo symlink vào init.d
    install -d ${D}${sysconfdir}/init.d
    ln -sf ${bindir}/wifi-connect.sh ${D}${sysconfdir}/init.d/wifi-auto
}

FILES:${PN} += " \
    ${bindir}/wifi-connect.sh \
    ${sysconfdir}/init.d/wifi-auto \
"

# Cho phép khởi động tự động (tùy sysvinit)
INITSCRIPT_NAME = "wifi-auto"
INITSCRIPT_PARAMS = "start 99 5 . stop 01 0 1 6 ."

inherit update-rc.d
