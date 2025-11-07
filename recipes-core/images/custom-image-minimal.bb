SUMMARY = "Custom Linux Image for BeagleBone with Ethernet & SSH"
LICENSE = "MIT"

inherit core-image
inherit extrausers

# Thiết lập root password (vd: root/test)
PASSWD = "\$5\$y9Aeg5ctwntRHo/g\$CAKtoTfQg7VPGfVAMGo5ZG/0GJLn3AD0JdoQ.i0dDFC"
EXTRA_USERS_PARAMS = "\
    usermod -p '${PASSWD}' root; \
"

# Cài thêm các package cơ bản 
IMAGE_INSTALL = "packagegroup-core-boot ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_INSTALL:append = " \
    dhcpcd \
    iproute2 \
    iputils \
    openssh \
    net-tools \
    rtl8188eu \
    iw \
    wpa-supplicant \
    python3 \
    python3-pip \
    python3-flask \
    kernel-module-ds1307 \
    kernel-module-bh1750 \
    kernel-module-sht3x \
    kernel-module-ledpump \
    wifi-auto \
    i2c-tools \
    nano \
    packagegroup-core-buildessential \
    opkg \
    ssd1306 \
    python3-json \
    smartfarm \
    tftp-hpa \
    lrzsz \
    bash \
    coreutils \
    util-linux \
    util-linux-lsblk \
    util-linux-fdisk \
    util-linux-sfdisk \
    util-linux-partx \
    util-linux-findmnt \
    e2fsprogs \
    e2fsprogs-e2fsck \
    e2fsprogs-resize2fs \
    e2fsprogs-tune2fs \
    e2fsprogs-mke2fs \
    dosfstools \
    parted \
    rsync \
    tar \
    gzip \
    xz \
    pv \
    u-boot-tools \
    mmc-utils \
    genotp \
    openssl \
    kmscube \
    fb-test \
    fbset \
    python3-pillow \
    python3-numpy \  
    python3-paho-mqtt \
    python3-evdev \
    fontconfig \
    evtest \
    coreutils \
    util-linux \
    libgpiod-tools \
    python3-evdev \
    python3-requests \
    python3-pyserial \
"

# Tăng dung lượng rootfs mặc định
IMAGE_OVERHEAD_FACTOR ?= "1.0"
IMAGE_ROOTFS_SIZE ?= "204800"
IMAGE_ROOTFS_MAXSIZE = "2097152"

# Tên image xuất ra
IMAGE_NAME = "custom-image"
