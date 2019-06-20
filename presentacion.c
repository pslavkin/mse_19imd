presentacion /*{{{*/


			 __  __ ____  _____				___ __  __ ____  
			|	\/  / ___|| ____|			  |_ _|	\/  |  _ \ 
			| |\/| \___ \|  _|	 _____	| || |\/| | | | |
			| |  | |___) | |___	|_____|	| || |  | | |_| |
			|_|  |_|____/|_____|			  |___|_|  |_|____/ 



-> *Driver MPU9250 I2C* <-

-> MSE 2019 <-
-> Implementacion de manejadores de dispositivos <-
-> Pablo Slavkin <-





/*}}}*/
compilacion kernel/*{{{*/

#en .bashrc para compilar para bbb
#alias armmake='make -j4 ARCH=arm CROSS_COMPILE=/opt/arm_linux_gnueabi_gcc/gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-'
alias armmake='make -j4 ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-'
-------------------------
cd /opt/linux_kernel
armmake
-------------------------
cd /opt/linux_kernel
armmake dtbs
-------------------------

# dmesg
[    0.000000] Booting Linux on physical CPU 0x0
[    0.000000] Linux version 4.19.44-dirty (pslavkin@work1) (gcc version 7.4.1 20181213 [linaro-7.4-2019.02 revision 56ec6f 6b99cc167ff0c2f8e1a2eed33b1edc85d4] (Linaro GCC 7.4-2019.02)) #6 SMP Sun May 19 18:45:24 -03 2019
[    0.000000] CPU: ARMv7 Processor [413fc082] revision 2 (ARMv7), cr=10c5387d
[    0.000000] CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache
[    0.000000] OF: fdt: Machine model: TI AM335x BeagleBone Black
[    0.000000] Memory policy: Data cache writeback
.../*}}}*/
uEnv.txt{{{

bootdir	  =
bootfile   = zImage
fdtfile	  = am335x-boneblack-pslavkin.dtb
loadaddr   = 0x82000000
fdtaddr	  = 0x81000000
data0		  = setenv ipaddr				192.168.0.100
data1		  = setenv serverip			192.168.0.1
data2		  = setenv ethact				usb_ether
data3		  = setenv usbnet_devaddr	f8:dc:7a:00:00:02
data4		  = setenv usbnet_hostaddr	f8:dc:7a:00:00:01
data5		  = saveenv
loadfdt	  = tftp  ${fdtaddr}    ${fdtfile}
loaduimage = tftp  ${loadaddr}   ${bootfile}
fdtboot	  = bootz ${loadaddr} - ${fdtaddr}

mmc_args=setenv bootargs root=/dev/nfs rw 
			ip						= 192.168.0.100:192.168.0.1:
									  192.168.0.1:255.255.255.0::usb0
			console				= ttyS0,115200n8
			g_ether.dev_addr	= usbnet_devaddr
			g_ether.host_addr = usbnet_hostaddr
			nfsroot				= 192.168.0.1:/home/pslavkin/mse_2_19imd/file_system
		,	nfsvers				= 3
			loglevel				= 5

uenvcmd= run data0;
			run data1;
			run data2;
			run data3;
			run data4;
			run data5;
			run loaduimage;
			run loadfdt;
			run mmc_args;
			run fdtboot

en la PC
--------------------------------
enxf8dc7a000001: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
link/ether f8:dc:7a:00:00:01 brd ff:ff:ff:ff:ff:ff
inet 192.168.0.1/24 brd 192.168.0.255 scope global enxf8dc7a000001
valid_lft forever preferred_lft forever
inet6 fe80::2914:2ec9:ae06:5371/64 scope link 
valid_lft forever preferred_lft forever


/*}}}*/
am335x-boneblack-pslavkin.dts/*{{{*/

#include "am335x-boneblack.dts"
// pinmux i2c1 
&am33xx_pinmux {
	i2c1_pins: pinmux_i2c1_pins {
		pinctrl-single,pins = <
			AM33XX_IOPAD(0x958, PIN_INPUT_PULLUP | MUX_MODE2) /* spi0_d1.i2c1_sda */
			AM33XX_IOPAD(0x95c, PIN_INPUT_PULLUP | MUX_MODE2) /* spi0_cs0.i2c1_scl */
		>;
	};
};

// habilio el bus i2c1
&i2c1 {
	status			 = "okay";
	pinctrl-names	 = "default";
	clock-frequency = <100000>;
	pinctrl-0		 = <&i2c1_pins>;

	// aca declaro los devices (uno solo en este caso)
	mpu9250_pslavkin: mpu9250_pslavkin@68 {
		compatible = "mse,mpu9250_pslavkin";
		reg		  = <0x68>;
	};
};/*}}}*/
Makefile DTS{{{

dtb-$(CONFIG_SOC_AM33XX) += \
	am335x-baltos-ir2110.dtb \
	am335x-baltos-ir3220.dtb \
	am335x-baltos-ir5221.dtb \
	am335x-base0033.dtb \
	am335x-bone.dtb \
	am335x-boneblack.dtb \
	am335x-boneblack-pslavkin.dtb \
	am335x-boneblack-wireless.dtb \
	am335x-boneblue.dtb \
	.../*}}}*/
modulo - estructuras{{{

static const struct i2c_device_id mpu9250_pslavkin_i2c_id[] = {
	 { "mpu9250_pslavkin", 0 },
	 { }
};
MOGULE_DEVICE_TABLE(i2c, mpu9250_pslavkin_i2c_id);

static const struct of_device_id mpu9250_pslavkin_of_match[] = {
	 { .compatible = "mse,mpu9250_pslavkin" },
	 { }
};
MODULE_DEVICE_TABLE(of, mpu9250_pslavkin_of_match);/*}}}*/
probe y remove{{{
static int mpu9250_pslavkin_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	gClient = client;
	gId	  = id;
	mpu9250_init ( );
}

static int mpu9250_pslavkin_remove(struct i2c_client *client)
{
	 mpu9250_exit();
}

static struct i2c_driver mpu9250_pslavkin_i2c_driver = {
	 .driver = {
		  .name				= "mpu9250_pslavkin",
		  .of_match_table = mpu9250_pslavkin_of_match,
	 },
	 .probe	  = mpu9250_pslavkin_probe,
	 .remove   = mpu9250_pslavkin_remove,
	 .id_table = mpu9250_pslavkin_i2c_id
};

static int __init mpu9250_init(void) 
{
	majorNumber   = register_chrdev(0, DEVICE_NAME, &fops);
	mpu9250Class  = class_create(THIS_MODULE, CLASS_NAME);
	mpu9250Device = device_create(mpu9250Class, NULL, MKDEV(majorNumber, 0),
						  NULL, DEVICE_NAME);
}

module_i2c_driver(mpu9250_pslavkin_i2c_driver);/*}}}*/
lectura del mpu{{{

void readMpuData ( void )
{
	int rv;
	char buf[20];
	char reg = 0x3B;
	rv			= i2c_master_send ( gClient ,&reg ,1  );
	rv			= i2c_master_recv ( gClient ,buf  ,14 );
	accelX	= (short int)( buf[ 0  ] * 256 + buf[ 1  ]);
	accelY	= (short int)( buf[ 2  ] * 256 + buf[ 3  ]);
	accelZ	= (short int)( buf[ 4  ] * 256 + buf[ 5  ]);
	temp		= (short int)( buf[ 6  ] * 256 + buf[ 7  ]);
	gyroX		= (short int)( buf[ 8  ] * 256 + buf[ 9  ]);
	gyroY		= (short int)( buf[ 10 ] * 256 + buf[ 11 ]);
	gyroZ		= (short int)( buf[ 12 ] * 256 + buf[ 13 ]);
	temp=temp/333+21;
}/*}}}*/
read/write{{{
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	readMpuData();
	switch (retrieveState) {
		case ACCEL:
			size_of_message=sprintf(message,"x=%7d y=%7d z=%7d\n",accelX,accelY,accelZ);
			break;
		case TEMP:
		case GYRO:
		case ACCEL_BARS: 
		case GYRO_BARS: 
		case ALL:
	}
	size_of_message++;
	copy_to_user(buffer, message, size_of_message);
	return size_of_message;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) 
{
	copy_from_user(message,buffer,len);
	size_of_message = strlen(message);
	sscanf(message,"%d",(int*)&retrieveState);
	return len;
}/*}}}*/
