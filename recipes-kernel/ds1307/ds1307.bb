SUMMARY = "DS1307 I2C RTC driver"
DESCRIPTION = "Custom DS1307 kernel module for BeagleBone Black"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

# Chỉ tạo gói module kernel
KERNEL_MODULE_PACKAGE = "kernel-module-ds1307"
PACKAGES = "${PN}-dbg ${KERNEL_MODULE_PACKAGE}"

SRC_URI = " \
    file://ds1307.c \
    file://ds1307.h \
    file://Makefile \
"

S = "${WORKDIR}"

do_compile[depends] += "virtual/kernel:do_shared_workdir"

KERNEL_MODULE_AUTOLOAD += "ds1307"

do_install() {
    # Cài đặt file .ko
    if [ ! -f ds1307.ko ]; then
        find ${B} -name "ds1307.ko" -exec cp {} . \;
    fi

    install -d ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra
    install -m 0644 ds1307.ko ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/

    # Cài đặt file autoload
    install -d ${D}${sysconfdir}/modules-load.d
    echo "ds1307" > ${D}${sysconfdir}/modules-load.d/ds1307.conf
}

# Định nghĩa file trong gói module
FILES:${KERNEL_MODULE_PACKAGE} += "${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/ds1307.ko"
FILES:${KERNEL_MODULE_PACKAGE} += "${sysconfdir}/modules-load.d/ds1307.conf"

# Đảm bảo gói chính không cung cấp gì
RPROVIDES:${PN} = ""
