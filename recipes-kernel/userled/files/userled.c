#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/kthread.h>

#define GPIO1_BASE_ADDR 0x4804C000
#define GPIO_SIZE 0x1000
#define GPIO_OE 0x134
#define GPIO_SETDATAOUT 0x194
#define GPIO_CLEARDATAOUT 0x190

/* USR0 = GPIO1_21, USR1 = GPIO1_22, USR2 = GPIO1_23, USR3 = GPIO1_24 */
#define USR0_PIN (1 << 21)
#define USR1_PIN (1 << 22)
#define USR2_PIN (1 << 23)
#define USR3_PIN (1 << 24)

static void __iomem *gpio1_base = NULL;
static struct task_struct *led_thread = NULL;

static int led_pattern(void *data)
{
    int i,j; 
    int cycle;
    u32 pins[4] = {USR0_PIN, USR1_PIN, USR2_PIN, USR3_PIN};
    u32 all_pins = USR0_PIN | USR1_PIN | USR2_PIN | USR3_PIN;

    while (!kthread_should_stop()) {
        /* 5 cycles of left-to-right then right-to-left chase */
        for (cycle = 0; cycle < 5; ++cycle) {
            /* Left to right */
            for (j = 0; j < 4; ++j) {
                writel(all_pins, gpio1_base + GPIO_CLEARDATAOUT);
                writel(pins[j], gpio1_base + GPIO_SETDATAOUT);
                mdelay(200);
            }
            /* Right to left */
            for (j = 3; j >= 0; --j) {
                writel(all_pins, gpio1_base + GPIO_CLEARDATAOUT);
                writel(pins[j], gpio1_base + GPIO_SETDATAOUT);
                mdelay(200);
            }
        }
        /* All 4 LEDs blink 5 times */
        for (i = 0; i < 5; ++i) {
            writel(all_pins, gpio1_base + GPIO_SETDATAOUT);
            mdelay(200);
            writel(all_pins, gpio1_base + GPIO_CLEARDATAOUT);
            mdelay(200);
        }
    }
    return 0;
}

static int usr_led_open(struct inode *inode, struct file *file)
{
    pr_info("USR_LED_DRIVER: device opened\n");
    return 0;
}

static int usr_led_release(struct inode *inode, struct file *file)
{
    pr_info("USR_LED_DRIVER: device closed\n");
    return 0;
}

static const struct file_operations usr_led_fops = {
    .owner = THIS_MODULE,
    .open = usr_led_open,
    .release = usr_led_release,
};

static struct miscdevice usr_led_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "usr_leds",
    .fops = &usr_led_fops,
};

static int __init usr_led_init(void)
{
    int ret;
    u32 reg;

    pr_info("USR_LED_DRIVER: initializing\n");

    /* Map GPIO1 */
    gpio1_base = ioremap(GPIO1_BASE_ADDR, GPIO_SIZE);
    if (!gpio1_base) {
        pr_err("USR_LED_DRIVER: failed to ioremap GPIO1\n");
        return -ENOMEM;
    }

    /* Configure USR0-USR3 (GPIO1_21-24) as outputs */
    reg = readl(gpio1_base + GPIO_OE);
    reg &= ~(USR0_PIN | USR1_PIN | USR2_PIN | USR3_PIN); /* clear bits -> output */
    writel(reg, gpio1_base + GPIO_OE);

    /* Ensure all pins start LOW */
    writel(USR0_PIN | USR1_PIN | USR2_PIN | USR3_PIN, gpio1_base + GPIO_CLEARDATAOUT);

    /* Start the LED pattern thread */
    led_thread = kthread_run(led_pattern, NULL, "usr_led_thread");
    if (IS_ERR(led_thread)) {
        pr_err("USR_LED_DRIVER: failed to create kthread\n");
        iounmap(gpio1_base);
        return PTR_ERR(led_thread);
    }

    /* Register misc device */
    ret = misc_register(&usr_led_misc);
    if (ret) {
        pr_err("USR_LED_DRIVER: failed to register misc device: %d\n", ret);
        kthread_stop(led_thread);
        iounmap(gpio1_base);
        return ret;
    }

    pr_info("USR_LED_DRIVER: loaded successfully. Device: /dev/usr_leds\n");
    return 0;
}

static void __exit usr_led_exit(void)
{
    pr_info("USR_LED_DRIVER: unloading\n");

    if (led_thread) {
        kthread_stop(led_thread);
        led_thread = NULL;
    }

    /* Turn off all LEDs */
    if (gpio1_base) {
        writel(USR0_PIN | USR1_PIN | USR2_PIN | USR3_PIN, gpio1_base + GPIO_CLEARDATAOUT);
    }

    misc_deregister(&usr_led_misc);
    if (gpio1_base) {
        iounmap(gpio1_base);
        gpio1_base = NULL;
    }

    pr_info("USR_LED_DRIVER: unloaded\n");
}

module_init(usr_led_init);
module_exit(usr_led_exit);

MODULE_AUTHOR("Dang Van Phuc");
MODULE_DESCRIPTION("Misc driver for USR0-USR3 LED pattern control");
MODULE_LICENSE("GPL");