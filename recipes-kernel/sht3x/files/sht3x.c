// sht3x.c - Fixed, no kthread, on-demand, Yocto-compatible
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#define DRIVER_NAME      "sht3x"
#define MISC_DEVICE_NAME "hum_temp"

#define LED_IOC_MAGIC 'k'
#define GET_TEMP      _IOR(LED_IOC_MAGIC, 4, char[10])
#define GET_HUM       _IOR(LED_IOC_MAGIC, 3, char[10])

// Global client pointer
static struct i2c_client *g_client;

/* -------------------------------------------------------------------- */
/*  Read raw temperature and humidity from SHT3x                        */
/* -------------------------------------------------------------------- */
static int sht3x_read(struct i2c_client *client, s16 *temp, s16 *humid)
{
    u8 cmd[] = {0x2C, 0x06};  // Single shot, high repeatability
    u8 data[6];
    int ret;

    ret = i2c_master_send(client, cmd, 2);
    if (ret != 2) {
        dev_err(&client->dev, "Failed to send command: %d\n", ret);
        return ret < 0 ? ret : -EIO;
    }

    msleep(15);  // ~12.5ms needed

    ret = i2c_master_recv(client, data, 6);
    if (ret != 6) {
        dev_err(&client->dev, "Failed to read data: %d\n", ret);
        return ret < 0 ? ret : -EIO;
    }

    *temp  = (s16)((data[0] << 8) | data[1]);
    *humid = (s16)((data[3] << 8) | data[4]);
    return 0;
}

/* -------------------------------------------------------------------- */
/*  Misc device file operations                                         */
/* -------------------------------------------------------------------- */
static int misc_open(struct inode *inode, struct file *file)
{
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
    s16 temp_raw, humid_raw;
    int temp, humid;
    char result[10];
    int ret;

    if (!client)
        return -ENODEV;

    ret = sht3x_read(client, &temp_raw, &humid_raw);
    if (ret < 0)
        return ret;

    temp  = ((s32)temp_raw * 17500) / 65535 - 4500;  // °C * 100
    humid = ((s32)humid_raw * 10000) / 65535;        // %RH * 100

    switch (cmd) {
    case GET_TEMP:
        snprintf(result, sizeof(result), "%d.%02d", abs(temp / 100), abs(temp % 100));
        break;
    case GET_HUM:
        snprintf(result, sizeof(result), "%d.%02d",100 - abs(humid / 100), abs(humid % 100));
        break;
    default:
        return -ENOTTY;
    }

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
static int sht3x_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret;

    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        dev_err(&client->dev, "I2C functionality not supported\n");
        return -ENODEV;
    }

    g_client = client;

    ret = misc_register(&misc_device);
    if (ret) {
        dev_err(&client->dev, "Failed to register misc device: %d\n", ret);
        return ret;
    }

    // ĐÚNG: gán parent
    misc_device.this_device->parent = &client->dev;

    dev_info(&client->dev, "SHT3x driver probed at /dev/%s\n", MISC_DEVICE_NAME);
    return 0;
}

static int sht3x_remove(struct i2c_client *client)
{
    misc_deregister(&misc_device);
    g_client = NULL;
    dev_info(&client->dev, "SHT3x driver removed\n");
    return 0;
}

/* -------------------------------------------------------------------- */
/*  Device tree & I2C ID table                                          */
/* -------------------------------------------------------------------- */
static const struct of_device_id sht3x_of_match[] = {
    { .compatible = "cambiennhietdo,sht3x" },
    { }
};
MODULE_DEVICE_TABLE(of, sht3x_of_match);

static const struct i2c_device_id sht3x_id[] = {
    { "sht3x", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, sht3x_id);

static struct i2c_driver sht3x_driver = {
    .driver = {
        .name           = DRIVER_NAME,
        .of_match_table = sht3x_of_match,
    },
    .probe  = sht3x_probe,
    .remove = sht3x_remove,
    .id_table = sht3x_id,
};

module_i2c_driver(sht3x_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dang Van Phuc (fixed by assistant)");
MODULE_DESCRIPTION("SHT3x Temperature & Humidity Sensor Driver - On-demand, Yocto-ready");