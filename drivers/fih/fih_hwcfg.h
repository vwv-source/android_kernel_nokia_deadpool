#ifndef __FIH_HWCFG_H
#define __FIH_HWCFG_H

#define HWCFG_INFO_SIZE 256
#define HWCFG_INFO_MAGIC "FIH_HWID_INFO"
#define HWCFG_INFO_MAGIC_SIZE 13

enum {
	FIH_HWCFG_INT_PROJECT = 0,
	FIH_HWCFG_INT_HWREV,
	FIH_HWCFG_INT_BSP,
	FIH_HWCFG_INT_RF,
//	FIH_SWID_MODULE, //NOT USED
	FIH_HWCFG_INT_SIM,
	FIH_HWCFG_INT_DTM,
	FIH_HWCFG_INT_DTN,
	FIH_HWCFG_INT_HWID,
	FIH_HWCFG_INT_MAX,
	//
	FIH_HWCFG_CHAR_PROJECT,
	FIH_HWCFG_CHAR_FACTORY,
	FIH_HWCFG_CHAR_PHASE_SW,
	FIH_HWCFG_CHAR_PHASE_HW,
	FIH_HWCFG_CHAR_SKU,
	FIH_HWCFG_CHAR_RF_BAND,
	FIH_HWCFG_CHAR_BSP,
	FIH_HWCFG_CHAR_MAX,
	/* max */
	FIH_HWCFG_MAX
};

//Header size = 32 byte
struct st_hwcfg_header {
	char	magic_tag[16];  //CONSTAST: FIH_HWID_INFO
	int		table_size;
	char	reserved[12];
};

//Total len = 1024 byte
struct st_hwcfg_info {
    int hwid;
    char project;
    char hw_rev;
    char rf;
    char bsp;
    char dtm;
    char dtn;
    char sim;
		//reserved
	char RESERVED[21];
		//info
	char	project_name[16];
	char	factory_name[16];
	char	phase_sw[16];
	char	phase_hw[16];
	char	sku_name[16];
	char	rf_band[255];
		//RESERVED2
	char reserved_tag_2[17];
		//BB-PCBA description
	char  pcba_description[640];
};

void* fih_hwcfg_fetch(int idx);
void fih_hwcfg_setup(void);

#endif /* __FIH_HWCFG_H */
