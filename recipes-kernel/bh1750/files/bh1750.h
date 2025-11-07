#ifndef _BH1750_H_
#define _BH1750_H_ 

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/slab.h>


static int bh1750_init(struct i2c_client *client); 
static int bh1750_read_lux(struct i2c_client *client, u16 *lux_raw); 

MODULE_LICENSE("GPL");

#endif