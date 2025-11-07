#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/ioctl.h>

#define GPIO0_BASE_ADDR    0x44E07000
#define GPIO_SIZE          0x1000

#define GPIO_OE            0x134
#define GPIO_SETDATAOUT    0x194
#define GPIO_CLEARDATAOUT  0x190

/* P9_11 = GPIO0_30 */
#define LED_PIN            (1 << 30)

/* P9_13 = GPIO0_31 */
#define PUMP_PIN           (1 << 31)

static void __iomem *gpio0_base = NULL;

/* Ioctl commands - magic 'p' */
#define PUMP_LED_MAGIC 'p'
#define ON_LED      _IO(PUMP_LED_MAGIC, 1)
#define OFF_LED     _IO(PUMP_LED_MAGIC, 2)
#define ON_PUMP     _IO(PUMP_LED_MAGIC, 3)
#define OFF_PUMP    _IO(PUMP_LED_MAGIC, 4)

static long pump_led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    pr_info("PUMP_LED_DRIVER: ioctl cmd 0x%x\n", cmd);

    switch (cmd) {
    case ON_LED:
        writel(LED_PIN, gpio0_base + GPIO_SETDATAOUT);
        pr_info("PUMP_LED_DRIVER: LED ON (P9_11 HIGH)\n");
        break;
    case OFF_LED:
        writel(LED_PIN, gpio0_base + GPIO_CLEARDATAOUT);
        pr_info("PUMP_LED_DRIVER: LED OFF (P9_11 LOW)\n");
        break;
    case ON_PUMP:
        writel(PUMP_PIN, gpio0_base + GPIO_SETDATAOUT);
        pr_info("PUMP_LED_DRIVER: PUMP ON (P9_13 HIGH)\n");
        break;
    case OFF_PUMP:
        writel(PUMP_PIN, gpio0_base + GPIO_CLEARDATAOUT);
        pr_info("PUMP_LED_DRIVER: PUMP OFF (P9_13 LOW)\n");
        break;
    default:
        pr_warn("PUMP_LED_DRIVER: Unknown ioctl cmd 0x%x\n", cmd);
        return -ENOTTY;
    }
    return 0;
}

static int pump_led_open(struct inode *inode, struct file *file)
{
    pr_info("PUMP_LED_DRIVER: device opened\n");
    return 0;
}

static int pump_led_release(struct inode *inode, struct file *file)
{
    pr_info("PUMP_LED_DRIVER: device closed\n");
    return 0;
}

static const struct file_operations pump_led_fops = {
    .owner          = THIS_MODULE,
    .open           = pump_led_open,
    .release        = pump_led_release,
    .unlocked_ioctl = pump_led_ioctl,
    .compat_ioctl   = pump_led_ioctl,  /* for 32-bit userspace on 64-bit kernel */
};

static struct miscdevice pump_led_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "pump_led",
    .fops  = &pump_led_fops,
};

static int __init pump_led_init(void)
{
    int ret;
    u32 reg;
    int i; 

    pr_info("PUMP_LED_DRIVER: initializing\n");

    /* Map GPIO0 */
    gpio0_base = ioremap(GPIO0_BASE_ADDR, GPIO_SIZE);
    if (!gpio0_base) {
        pr_err("PUMP_LED_DRIVER: failed to ioremap GPIO0\n");
        return -ENOMEM;
    }

    /* Configure P9_11 (GPIO0_30) and P9_13 (GPIO0_31) as outputs */
    reg = readl(gpio0_base + GPIO_OE);
    reg &= ~LED_PIN;   /* clear bit 30 -> output */
    reg &= ~PUMP_PIN;  /* clear bit 31 -> output */
    writel(reg, gpio0_base + GPIO_OE);

    /* Ensure both pins start LOW */
    writel(LED_PIN | PUMP_PIN, gpio0_base + GPIO_CLEARDATAOUT);

    /* Blink test: LED 3 times */
    for (i = 0; i < 3; i++) {
        writel(LED_PIN, gpio0_base + GPIO_SETDATAOUT);
        mdelay(200);
        writel(LED_PIN, gpio0_base + GPIO_CLEARDATAOUT);
        mdelay(200);
    }

    /* Register misc device */
    ret = misc_register(&pump_led_misc);
    if (ret) {
        pr_err("PUMP_LED_DRIVER: failed to register misc device: %d\n", ret);
        iounmap(gpio0_base);
        return ret;
    }

    pr_info("PUMP_LED_DRIVER: loaded successfully. Device: /dev/pump_led\n");
    return 0;
}

static void __exit pump_led_exit(void)
{
    pr_info("PUMP_LED_DRIVER: unloading\n");

    /* Turn off everything */
    writel(LED_PIN | PUMP_PIN, gpio0_base + GPIO_CLEARDATAOUT);

    misc_deregister(&pump_led_misc);
    iounmap(gpio0_base);

    pr_info("PUMP_LED_DRIVER: unloaded\n");
}

module_init(pump_led_init);
module_exit(pump_led_exit);

MODULE_AUTHOR("Dang Van Phuc");
MODULE_DESCRIPTION("Misc driver for P9_11 (LED) and P9_13 (PUMP) control");
MODULE_LICENSE("GPL");