SUMMARY = "ledpump I2C RTC driver"
DESCRIPTION = "Custom ledpump kernel module for BeagleBone Black"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

# Chỉ tạo gói module kernel
KERNEL_MODULE_PACKAGE = "kernel-module-ledpump"
PACKAGES = "${PN}-dbg ${KERNEL_MODULE_PACKAGE}"

SRC_URI = " \
    file://ledpump.c \
    file://Makefile \
"

S = "${WORKDIR}"

do_compile[depends] += "virtual/kernel:do_shared_workdir"

KERNEL_MODULE_AUTOLOAD += "ledpump"

do_install() {
    # Cài đặt file .ko
    if [ ! -f ledpump.ko ]; then
        find ${B} -name "ledpump.ko" -exec cp {} . \;
    fi

    install -d ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra
    install -m 0644 ledpump.ko ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/

    # Cài đặt file autoload
    install -d ${D}${sysconfdir}/modules-load.d
    echo "ledpump" > ${D}${sysconfdir}/modules-load.d/ledpump.conf
}

# Định nghĩa file trong gói module
FILES:${KERNEL_MODULE_PACKAGE} += "${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/ledpump.ko"
FILES:${KERNEL_MODULE_PACKAGE} += "${sysconfdir}/modules-load.d/ledpump.conf"

# Đảm bảo gói chính không cung cấp gì
RPROVIDES:${PN} = ""
    