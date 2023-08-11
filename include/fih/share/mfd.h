#ifndef FIH_MFD_H
#define FIH_MFD_H

#define FIH_MFD_MAGIC_VAL  0x5F4D6644  /* _MfD */
#define FIH_MFD_VERN_VAL   0
#define FIH_MFD_NAME_LEN   16
#define FIH_MFD_DATA_LEN   64

#define FIH_MFD_MMC_PATH    "/dev/block/bootdevice/by-name/mfd"
#define FIH_MFD_MMC_LUN     3
#define FIH_MFD_MMC_OFFSET  0
#define FIH_MFD_MMC_SIZE    4096

enum {
	FIH_MFD_ITEM_SWID = 0,
	FIH_MFD_ITEM_PID,
	FIH_MFD_ITEM_BT_MAC,
	FIH_MFD_ITEM_WIFI_MAC,
	FIH_MFD_ITEM_IMEI_1,
	FIH_MFD_ITEM_IMEI_2,
	FIH_MFD_ITEM_MEID_1,
	FIH_MFD_ITEM_MEID_2,
	FIH_MFD_ITEM_WIFI_MAC2,
	FIH_MFD_ITEM_SIMCONFIG,
	FIH_MFD_ITEM_BATT_INFO,
	FIH_MFD_ITEM_SIM_PERSO,
	FIH_MFD_ITEM_BT2_MAC,
	FIH_MFD_ITEM_MAX,
};

struct st_mfd_item {
	char name[FIH_MFD_NAME_LEN];
	char data[FIH_MFD_DATA_LEN];
	unsigned int size;
};

typedef struct st_mfd_list {
	unsigned int magic;
	unsigned int version;
	unsigned int number;
	struct st_mfd_item item[FIH_MFD_ITEM_MAX];
}FIH_MFD_V0;

#endif
