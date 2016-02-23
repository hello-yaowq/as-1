---
layout: post
title: aslinux DTS -- Device Tree System
category: linux
comments: true
---

### unflatten\_device\_tree
Sometimes, I really want to give up, linux is so complex to me. But for the purpose to keep me valueable, I need to do it, though it is a tough things.

According to my opinion, if I want to know how to develop linux bsp, the first things that I need to know is linux DTS. So let me start. This [article](http://blog.csdn.net/ermuzhi/article/details/9289523) from csdn do give a basic introduction of linux DTS, you can read it firstly. so let's use qemu and eclipse to debug it start\_kernel() --> setup\_arch() --> unflatten\_device\_tree().

```c
/* kernel/drivers/okernel/drivers/of/fdt.c */
void __init unflatten_device_tree(void)
{
	__unflatten_device_tree(initial_boot_params, &of_allnodes,
				early_init_dt_alloc_memory_arch);

	/* Get pointer to "/chosen" and "/aliases" nodes for use everywhere */
	of_alias_scan(early_init_dt_alloc_memory_arch);
}
```

look into the code of function __unflatten\_device\_tree, it will firstly check the header of the blob(initial\_boot\_params).

```c
#define FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#define FDT_FIRST_SUPPORTED_VERSION	0x10
#define FDT_LAST_SUPPORTED_VERSION	0x11
#define FDT_SW_MAGIC		(~FDT_MAGIC)

struct fdt_header {
	uint32_t magic;			 /* magic word FDT_MAGIC */
	uint32_t totalsize;		 /* total size of DT block */
	uint32_t off_dt_struct;		 /* offset to structure */
	uint32_t off_dt_strings;	 /* offset to strings */
	uint32_t off_mem_rsvmap;	 /* offset to memory reserve map */
	uint32_t version;		 /* format version */
	uint32_t last_comp_version;	 /* last compatible version */

	/* version 2 fields below */
	uint32_t boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	uint32_t size_dt_strings;	 /* size of the strings block */

	/* version 17 fields below */
	uint32_t size_dt_struct;	 /* size of the structure block */
};
int fdt_check_header(const void *fdt)
{
	if (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		if (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
			return -FDT_ERR_BADVERSION;
		if (fdt_last_comp_version(fdt) > FDT_LAST_SUPPORTED_VERSION)
			return -FDT_ERR_BADVERSION;
	} else if (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		if (fdt_size_dt_struct(fdt) == 0)
			return -FDT_ERR_BADSTATE;
	} else {
		return -FDT_ERR_BADMAGIC;
	}

	return 0;
}
```

OK, here let's use the Emacs open the dtb file vexpress-v2p-ca9.dtb, and display in hexl-mode,below is the part of header. As you could see that the first 4 bytes value is 0xd00dfeed which is just the value of macro FDT\_MAGIC. And then accoring to the fdt_header structure, we can easily know that the dtb version is 0x11 and the last compatible version is 0x10, so that the header check will pass and return value 0.
And the totalsize is 0x36e0(14048), you can check that the size is the same with file vexpress-v2p-ca9.dtb.

```text
vexpress-v2p-ca9.dtb
00000000: d00d feed 0000 36e0 0000 0038 0000 333c  ......6....8..3<
00000010: 0000 0028 0000 0011 0000 0010 0000 0000  ...(............
00000020: 0000 03a4 0000 3304 0000 0000 0000 0000  ......3.........
00000030: 0000 0000 0000 0000 0000 0001 0000 0000  ................
00000040: 0000 0003 0000 0008 0000 0000 5632 502d  ............V2P-
00000050: 4341 3900 0000 0003 0000 0004 0000 0006  CA9.............
00000060: 0000 0191 0000 0003 0000 0004 0000 000e  ................
00000070: 0000 000f 0000 0003 0000 0022 0000 0020  ..........."... 
00000080: 6172 6d2c 7665 7870 7265 7373 2c76 3270  arm,vexpress,v2p
00000090: 2d63 6139 0061 726d 2c76 6578 7072 6573  -ca9.arm,vexpres
000000a0: 7300 0000 0000 0003 0000 0004 0000 002b  s..............+
000000b0: 0000 0001 0000 0003 0000 0004 0000 003c  ...............<
000000c0: 0000 0001 0000 0003 0000 0004 0000 004b  ...............K
000000d0: 0000 0001 0000 0001 6368 6f73 656e 0000  ........chosen..
000000e0: 0000 0002 0000 0001 616c 6961 7365 7300  ........aliases.
000000f0: 0000 0003 0000 002e 0000 0057 2f73 6d62  ...........W/smb
00000100: 2f6d 6f74 6865 7262 6f61 7264 2f69 6f66  /motherboard/iof
00000110: 7067 6140 372c 3030 3030 3030 3030 2f75  pga@7,00000000/u
00000120: 6172 7440 3039 3030 3000 0000 0000 0003  art@09000.......
```

OK, then on offset of stucture is 0x38, offset to strings is 0x333C, so locate to address 0x333c by ^M g, you will find a text message, but now I don't understand, below is part of it. The offset to memory reserve map is 0x03a4, OK, not understood by me now.

```text
00003330: 0000 0002 0000 0002 0000 0009 6d6f 6465  ............mode
00003340: 6c00 6172 6d2c 6862 6900 6172 6d2c 7665  l.arm,hbi.arm,ve
00003350: 7870 7265 7373 2c73 6974 6500 636f 6d70  xpress,site.comp
00003360: 6174 6962 6c65 0069 6e74 6572 7275 7074  atible.interrupt
00003370: 2d70 6172 656e 7400 2361 6464 7265 7373  -parent.#address
00003380: 2d63 656c 6c73 0023 7369 7a65 2d63 656c  -cells.#size-cel
00003390: 6c73 0073 6572 6961 6c30 0073 6572 6961  ls.serial0.seria
```

go on, reading, the next API of __unflatten\_device\_tree is unflatten\_dt\_node, it is very long. The first action is the fdt\_get\_name, here it do check the header again, and then check the node offset, and see there is structure fdt\_node\_header, I think it would be the key point to know what is dtb. from the above analyse, we will know the nh will point to the offset 0x38,the tag would be 1(FDT\_BEGIN\_NODE), the name would be empty, that is the start node, wonderfull.

```c
#define FDT_BEGIN_NODE	0x1		/* Start node: full name */
#define FDT_END_NODE	0x2		/* End node */
#define FDT_PROP	0x3		/* Property: name off,
					   size, content */
#define FDT_NOP		0x4		/* nop */
#define FDT_END		0x9
struct fdt_node_header {
	uint32_t tag;
	char name[0];
};

static inline const void *_fdt_offset_ptr(const void *fdt, int offset)
{
	return (const char *)fdt + fdt_off_dt_struct(fdt) + offset;
}

const char *fdt_get_name(const void *fdt, int nodeoffset, int *len)
{
	const struct fdt_node_header *nh = _fdt_offset_ptr(fdt, nodeoffset);
	int err;

	if (((err = fdt_check_header(fdt)) != 0)
	    || ((err = _fdt_check_node_offset(fdt, nodeoffset)) < 0))
			goto fail;

	if (len)
		*len = strlen(nh->name);

	return nh->name;

 fail:
	if (len)
		*len = err;
	return NULL;
}
```

okay, go back to API unflatten\_dt\_node, check the following code, wow, all is OK, the pathp="" will go through the first if branch so that a root node would be made.

```c
	/* version 0x10 has a more compact unit name here instead of the full
	 * path. we accumulate the full path size using "fpsize", we'll rebuild
	 * it later. We detect this because the first character of the name is
	 * not '/'.
	 */
	if ((*pathp) != '/') {
		new_format = 1;
		if (fpsize == 0) {
			/* root node: special case. fpsize accounts for path
			 * plus terminating zero. root node only has '/', so
			 * fpsize should be 2, but we want to avoid the first
			 * level nodes to have two '/' so we use fpsize 1 here
			 */
			fpsize = 1;
			allocl = 2;
			l = 1;
			pathp = "";
		} else {
			/* account for '/' and path size minus terminal 0
			 * already in 'l'
			 */
			fpsize += l;
			allocl = fpsize;
		}
	}

```

then on, a for loop to set up the node's properties, okay, as above analyse, we know a property node will with tag FDT\_PROP 3, so that by the information of structure fdt_property, it will be possible to loop all its properties.

```c
struct fdt_property {
	uint32_t tag;
	uint32_t len;
	uint32_t nameoff;
	char data[0];
};
	/* process properties */
	for (offset = fdt_first_property_offset(blob, *poffset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(blob, offset))) {
		const char *pname;
		u32 sz;

		if (!(p = fdt_getprop_by_offset(blob, offset, &pname, &sz))) {
			offset = -FDT_ERR_INTERNAL;
			break;
		}

		if (pname == NULL) {
			pr_info("Can't find property name in list !\n");
			break;
		}
		if (strcmp(pname, "name") == 0)
			has_name = 1;
		pp = unflatten_dt_alloc(&mem, sizeof(struct property),
					__alignof__(struct property));
		if (allnextpp) {
			/* We accept flattened tree phandles either in
			 * ePAPR-style "phandle" properties, or the
			 * legacy "linux,phandle" properties.  If both
			 * appear and have different values, things
			 * will get weird.  Don't do that. */
			if ((strcmp(pname, "phandle") == 0) ||
			    (strcmp(pname, "linux,phandle") == 0)) {
				if (np->phandle == 0)
					np->phandle = be32_to_cpup(p);
			}
			/* And we process the "ibm,phandle" property
			 * used in pSeries dynamic device tree
			 * stuff */
			if (strcmp(pname, "ibm,phandle") == 0)
				np->phandle = be32_to_cpup(p);
			pp->name = (char *)pname;
			pp->length = sz;
			pp->value = (__be32 *)p;
			*prev_pp = pp;
			prev_pp = &pp->next;
		}
	}
```

this picture below gives a simple overview about how a DTB was parsed by linux kernel, so I know that the dtb will be copied to the RAM by uboot and passed to kernel as paramter and then parsed by the kernel. And the name of the node or property can be any valid string I think, that's great, by now I know how the DTS was parsed by the kernel, so next step is that how the DTS was used by the kernel.
![linux-dts-dtb](/as/images/vexpress-a9/linux-dts-dtb.png)

###of\_platform\_populate
then on the key API of\_platform\_populate which will to post process of the DTS which has been parsed by the above analyze. Here is a question

```c
*** arch/arm/mach-vexpress/v2m.c:
static void __init v2m_dt_init(void)
{
	of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);
}

***kernel/drivers/of/platform.c
int of_platform_populate(struct device_node *root,
			const struct of_device_id *matches,
			const struct of_dev_auxdata *lookup,
			struct device *parent)
{
	struct device_node *child;
	int rc = 0;

	root = root ? of_node_get(root) : of_find_node_by_path("/");
	if (!root)
		return -EINVAL;

	for_each_child_of_node(root, child) {
		rc = of_platform_bus_create(child, matches, lookup, parent, true);
		if (rc)
			break;
	}

	of_node_put(root);
	return rc;
}
```
