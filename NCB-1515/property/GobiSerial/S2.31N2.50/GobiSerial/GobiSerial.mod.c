#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=usbserial";

MODULE_ALIAS("usb:v05C6p920Cd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p920Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p68A2d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p68C0d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9071d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9070d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9100d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9102d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9110d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9090d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9091d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p90B0d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p90B1d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p90C0d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p90C1d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9010d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9011d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9012d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9013d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9014d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9015d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9018d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9019d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v03F0p361Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v03F0p371Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9040d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9041d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9051d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9053d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9054d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9055d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9056d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9060d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9061d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "3DC6537ABD95A627BFFFE6E");
