// bh1750.c - BH1750 Light Sensor Driver (Fixed, no kthread, no compile error)
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#define DRIVER_NAME      "bh1750"
#define MISC_DEVICE_NAME "lux_inten"

#define LED_IOC_MAGIC 'k'
#define GET_LUX       _IOR(LED_IOC_MAGIC, 5, char[10])

// Global client pointer (only one device)
static struct i2c_client *g_client;

/* -------------------------------------------------------------------- */
/*  Khởi động cảm biến: Continuously H-Resolution Mode (1 lx)           */
/* -------------------------------------------------------------------- */
static int bh1750_init(struct i2c_client *client)
{
    u8 cmd = 0x10;  // Continuously H-Resolution Mode
    int ret;

    ret = i2c_master_send(client, &cmd, 1);
    if (ret != 1) {
        dev_err(&client->dev, "Failed to send init command: %d\n", ret);
        return ret < 0 ? ret : -EIO;
    }

    msleep(180);  // Wait for first measurement
    return 0;
}

/* -------------------------------------------------------------------- */
/*  Đọc giá trị raw (2 byte) từ BH1750                                  */
/* -------------------------------------------------------------------- */
static int bh1750_read_raw(struct i2c_client *client, u16 *raw)
{
    u8 data[2];
    int ret;

    ret = i2c_master_recv(client, data, 2);
    if (ret != 2) {
        dev_err(&client->dev, "Failed to read data: %d\n", ret);
        return ret < 0 ? ret : -EIO;
    }

    *raw = (u16)((data[0] << 8) | data[1]);
    return 0;
}

/* -------------------------------------------------------------------- */
/*  Misc device file operations                                         */
/* -------------------------------------------------------------------- */
static int misc_open(struct inode *inode, struct file *file)
{
    // Lưu client vào private_data
    file->private_data = g_client;
    return 0;
}

static int misc_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t misc_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    return count;
}

static ssize_t misc_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    return count;
}

static long misc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct i2c_client *client = file->private_data;
    u16 raw;
    unsigned int lux_int, lux_frac;
    char result[10];
    int ret;

    if (!client)
        return -ENODEV;

    if (cmd != GET_LUX)
        return -ENOTTY;

    ret = bh1750_read_raw(client, &raw);
    if (ret < 0)
        return ret;

    // lux = raw / 1.2 → lux * 100 = (raw * 1000) / 12
    unsigned long lux_x100 = (raw * 1000UL) / 12;

    lux_int  = lux_x100 / 100;
    lux_frac = lux_x100 % 100;

    snprintf(result, sizeof(result), "%u.%02u", lux_int, lux_frac);

    if (copy_to_user((char __user *)arg, result, strlen(result) + 1))
        return -EFAULT;

    return 0;
}

static const struct file_operations misc_fops = {
    .owner          = THIS_MODULE,
    .open           = misc_open,
    .release        = misc_release,
    .read           = misc_read,
    .write          = misc_write,
    .unlocked_ioctl = misc_ioctl,
};

static struct miscdevice misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = MISC_DEVICE_NAME,
    .fops  = &misc_fops,
};

/* -------------------------------------------------------------------- */
/*  I2C probe / remove                                                  */
/* -------------------------------------------------------------------- */
static int bh1750_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret;

    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        dev_err(&client->dev, "I2C functionality not supported\n");
        return -ENODEV;
    }

    ret = bh1750_init(client);
    if (ret < 0)
        return ret;

    // Lưu client để dùng trong file ops
    g_client = client;

    ret = misc_register(&misc_device);
    if (ret) {
        dev_err(&client->dev, "Failed to register misc device: %d\n", ret);
        return ret;
    }

    // Gán parent để hiển thị trong /sys
    misc_device.this_device->parent = &client->dev;

    dev_info(&client->dev, "BH1750 driver probed at /dev/%s\n", MISC_DEVICE_NAME);
    return 0;
}

static int bh1750_remove(struct i2c_client *client)
{
    misc_deregister(&misc_device);
    g_client = NULL;
    dev_info(&client->dev, "BH1750 driver removed\n");
    return 0;
}

/* -------------------------------------------------------------------- */
/*  Device tree & I2C ID table                                          */
/* -------------------------------------------------------------------- */
static const struct of_device_id bh1750_of_match[] = {
    { .compatible = "cambienanhsang,bh1750" },
    { }
};
MODULE_DEVICE_TABLE(of, bh1750_of_match);

static const struct i2c_device_id bh1750_id[] = {
    { "bh1750", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, bh1750_id);

static struct i2c_driver bh1750_driver = {
    .driver = {
        .name           = DRIVER_NAME,
        .of_match_table = bh1750_of_match,
    },
    .probe  = bh1750_probe,
    .remove = bh1750_remove,
    .id_table = bh1750_id,
};

module_i2c_driver(bh1750_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dang Van Phuc (fixed & cleaned by assistant)");
MODULE_DESCRIPTION("BH1750 Ambient Light Sensor Driver - On-demand, correct lux");