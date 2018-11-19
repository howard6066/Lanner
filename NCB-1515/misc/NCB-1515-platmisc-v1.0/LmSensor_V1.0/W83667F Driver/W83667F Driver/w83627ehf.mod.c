#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
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
	{ 0x14522340, "module_layout" },
	{ 0x79fcc8d1, "device_remove_file" },
	{ 0x1fedf0f4, "__request_region" },
	{ 0xd2037915, "dev_set_drvdata" },
	{ 0xd691cba2, "malloc_sizes" },
	{ 0x52a971fe, "hwmon_device_unregister" },
	{ 0x2bd43d13, "dynamic_debug_enabled2" },
	{ 0x3758301, "mutex_unlock" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x7d11c268, "jiffies" },
	{ 0xff7559e4, "ioport_resource" },
	{ 0x4bf79039, "__mutex_init" },
	{ 0xea147363, "printk" },
	{ 0x23269a13, "strict_strtoul" },
	{ 0x6943a5ac, "platform_device_alloc" },
	{ 0xda5ebcf1, "platform_device_add" },
	{ 0xb4390f9a, "mcount" },
	{ 0x85abc85f, "strncmp" },
	{ 0xef1c781c, "vid_which_vrm" },
	{ 0xfee8a795, "mutex_lock" },
	{ 0x222dc54b, "platform_get_resource" },
	{ 0x222936ca, "platform_device_unregister" },
	{ 0x773507, "platform_driver_register" },
	{ 0x7477dbc6, "device_create_file" },
	{ 0x8831d9bf, "platform_device_add_resources" },
	{ 0x7a2a837d, "strict_strtol" },
	{ 0x52ebb126, "param_get_ushort" },
	{ 0x9cb480f4, "dynamic_debug_enabled" },
	{ 0x27f96468, "pv_cpu_ops" },
	{ 0xf18b1317, "dev_driver_string" },
	{ 0xac16e9c, "platform_device_add_data" },
	{ 0x7c61340c, "__release_region" },
	{ 0x2044fa9e, "kmem_cache_alloc_trace" },
	{ 0x37a0cba, "kfree" },
	{ 0xa044500, "platform_driver_unregister" },
	{ 0xa92a43c, "dev_get_drvdata" },
	{ 0xf5e284b6, "hwmon_device_register" },
	{ 0x903c239, "vid_from_reg" },
	{ 0xdfb7bada, "acpi_check_resource_conflict" },
	{ 0x736839da, "platform_device_put" },
	{ 0xe0bc24a1, "param_set_ushort" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=hwmon-vid";


MODULE_INFO(srcversion, "9F25467E8BDC8A03E7922F0");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 4,
};
