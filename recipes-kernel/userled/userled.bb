SUMMARY = "userled I2C RTC driver"
DESCRIPTION = "Custom userled kernel module for BeagleBone Black"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

# Chỉ tạo gói module kernel
KERNEL_MODULE_PACKAGE = "kernel-module-userled"
PACKAGES = "${PN}-dbg ${KERNEL_MODULE_PACKAGE}"

SRC_URI = " \
    file://userled.c \
    file://Makefile \
"

S = "${WORKDIR}"

do_compile[depends] += "virtual/kernel:do_shared_workdir"

KERNEL_MODULE_AUTOLOAD += "userled"

do_install() {
    # Cài đặt file .ko
    if [ ! -f userled.ko ]; then
        find ${B} -name "userled.ko" -exec cp {} . \;
    fi

    install -d ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra
    install -m 0644 userled.ko ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/

    # Cài đặt file autoload
    install -d ${D}${sysconfdir}/modules-load.d
    echo "userled" > ${D}${sysconfdir}/modules-load.d/userled.conf
}

# Định nghĩa file trong gói module
FILES:${KERNEL_MODULE_PACKAGE} += "${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/userled.ko"
FILES:${KERNEL_MODULE_PACKAGE} += "${sysconfdir}/modules-load.d/userled.conf"

# Đảm bảo gói chính không cung cấp gì
RPROVIDES:${PN} = ""
    