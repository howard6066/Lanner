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

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x957c3338, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xb029b28b, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0x16cbe8aa, __VMLINUX_SYMBOL_STR(usb_serial_generic_write) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x37befc70, __VMLINUX_SYMBOL_STR(jiffies_to_msecs) },
	{ 0x265d2f94, __VMLINUX_SYMBOL_STR(usb_serial_generic_open) },
	{ 0xb5d944ba, __VMLINUX_SYMBOL_STR(usb_bulk_msg) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x87a4b3f3, __VMLINUX_SYMBOL_STR(usb_serial_deregister_drivers) },
	{ 0xe7431462, __VMLINUX_SYMBOL_STR(usb_serial_register_drivers) },
	{ 0x776abf1b, __VMLINUX_SYMBOL_STR(usb_serial_suspend) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x62a524fd, __VMLINUX_SYMBOL_STR(usb_clear_halt) },
	{ 0x46a617a8, __VMLINUX_SYMBOL_STR(usb_set_interface) },
	{ 0x64e6b913, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x4bda8bcd, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x6397c183, __VMLINUX_SYMBOL_STR(usb_submit_urb) },
	{ 0x1916e38c, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0x680ec266, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x7f02188f, __VMLINUX_SYMBOL_STR(__msecs_to_jiffies) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0x7c0c25d3, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0xd228382c, __VMLINUX_SYMBOL_STR(usb_autopm_put_interface) },
	{ 0x8b10a48, __VMLINUX_SYMBOL_STR(usb_control_msg) },
	{ 0xd722bcef, __VMLINUX_SYMBOL_STR(usb_autopm_get_interface) },
	{ 0x550d9137, __VMLINUX_SYMBOL_STR(__dynamic_dev_dbg) },
	{ 0x78764f4e, __VMLINUX_SYMBOL_STR(pv_irq_ops) },
	{ 0x6bf1c17f, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0xe5815f8a, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irq) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x9063f84c, __VMLINUX_SYMBOL_STR(usb_kill_urb) },
	{ 0x5add155a, __VMLINUX_SYMBOL_STR(usb_serial_generic_resume) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

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

MODULE_INFO(srcversion, "EDEE055DF9EC5EC3BCB74D3");
