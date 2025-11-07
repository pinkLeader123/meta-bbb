#ifndef _SHT3_H_
#define _SHT3_H_ 

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/slab.h>


static int sht3x_read(struct i2c_client *client, s16 *temp, s16 *humid); 

MODULE_LICENSE("GPL");

#endif