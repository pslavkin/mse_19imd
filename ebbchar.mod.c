#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x70b4438, "module_layout" },
	{ 0x15c53be0, "i2c_del_driver" },
	{ 0x70cbdadc, "i2c_register_driver" },
	{ 0x1e047854, "warn_slowpath_fmt" },
	{ 0xf4fa543b, "arm_copy_to_user" },
	{ 0x91715312, "sprintf" },
	{ 0xc050fbe5, "device_create" },
	{ 0xa8356886, "__class_create" },
	{ 0xded610ba, "__register_chrdev" },
	{ 0x97255bdf, "strlen" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xdcce994, "class_destroy" },
	{ 0xea038f89, "class_unregister" },
	{ 0x400008e, "device_destroy" },
	{ 0xc83c0b1a, "i2c_transfer_buffer_flags" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x7c32d0f0, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cmse,mpu9250_pslavkin");
MODULE_ALIAS("of:N*T*Cmse,mpu9250_pslavkinC*");
MODULE_ALIAS("i2c:mpu9250_pslavkin");

MODULE_INFO(srcversion, "D1F2E8D61AC3029A0E482C7");
