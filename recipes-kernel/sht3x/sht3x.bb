SUMMARY = "sht3x I2C driver"
DESCRIPTION = "Custom sht3x kernel module for BeagleBone Black"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

# Chỉ tạo gói module kernel
KERNEL_MODULE_PACKAGE = "kernel-module-sht3x"
PACKAGES = "${PN}-dbg ${KERNEL_MODULE_PACKAGE}"

SRC_URI = " \
    file://sht3x.c \
    file://sht3x.h \
    file://Makefile \
"

S = "${WORKDIR}"

do_compile[depends] += "virtual/kernel:do_shared_workdir"

KERNEL_MODULE_AUTOLOAD += "sht3x"

do_install() {
    # Cài đặt file .ko
    if [ ! -f sht3x.ko ]; then
        find ${B} -name "sht3x.ko" -exec cp {} . \;
    fi

    install -d ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra
    install -m 0644 sht3x.ko ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/

    # Cài đặt file autoload
    install -d ${D}${sysconfdir}/modules-load.d
    echo "sht3x" > ${D}${sysconfdir}/modules-load.d/sht3x.conf
}

# Định nghĩa file trong gói module
FILES:${KERNEL_MODULE_PACKAGE} += "${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/sht3x.ko"
FILES:${KERNEL_MODULE_PACKAGE} += "${sysconfdir}/modules-load.d/sht3x.conf"

# Đảm bảo gói chính không cung cấp gì
RPROVIDES:${PN} = ""
    