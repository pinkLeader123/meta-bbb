SUMMARY = "bh1750 I2C driver"
DESCRIPTION = "Custom bh1750 kernel module for BeagleBone Black"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

# Chỉ tạo gói module kernel
KERNEL_MODULE_PACKAGE = "kernel-module-bh1750"
PACKAGES = "${PN}-dbg ${KERNEL_MODULE_PACKAGE}"

SRC_URI = " \
    file://bh1750.c \
    file://bh1750.h \
    file://Makefile \
"

S = "${WORKDIR}"

do_compile[depends] += "virtual/kernel:do_shared_workdir"

KERNEL_MODULE_AUTOLOAD += "bh1750"

do_install() {
    # Cài đặt file .ko
    if [ ! -f bh1750.ko ]; then
        find ${B} -name "bh1750.ko" -exec cp {} . \;
    fi

    install -d ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra
    install -m 0644 bh1750.ko ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/

    # Cài đặt file autoload
    install -d ${D}${sysconfdir}/modules-load.d
    echo "bh1750" > ${D}${sysconfdir}/modules-load.d/bh1750.conf
}

# Định nghĩa file trong gói module
FILES:${KERNEL_MODULE_PACKAGE} += "${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/bh1750.ko"
FILES:${KERNEL_MODULE_PACKAGE} += "${sysconfdir}/modules-load.d/bh1750.conf"

# Đảm bảo gói chính không cung cấp gì
RPROVIDES:${PN} = ""
    