/*  Himax Android Driver Sample Code for debug nodes

    Copyright (C) 2018 Himax Corporation.

    This software is licensed under the terms of the GNU General Public
    License version 2, as published by the Free Software Foundation, and
    may be copied, distributed, and modified under those terms.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*/
#ifndef H_HIMAX_DEBUG
#define H_HIMAX_DEBUG

#include "himax_platform.h"
#include "himax_common.h"


#ifdef HX_ESD_RECOVERY
	extern u8 HX_INCELL_ESD_RESET_ACTIVATE;
	extern 	int hx_incell_EB_event_flag;
	extern 	int hx_incell_EC_event_flag;
	extern 	int hx_incell_ED_event_flag;
#endif

#define HIMAX_PROC_DEBUG_LEVEL_FILE	"debug_level"
#define HIMAX_PROC_VENDOR_FILE		"vendor"
#define HIMAX_PROC_ATTN_FILE		"attn"
#define HIMAX_PROC_INT_EN_FILE		"int_en"
#define HIMAX_PROC_LAYOUT_FILE		"layout"
#define HIMAX_PROC_CRC_TEST_FILE		"CRC_test"

static struct proc_dir_entry *himax_proc_debug_level_file;
static struct proc_dir_entry *himax_proc_vendor_file;
static struct proc_dir_entry *himax_proc_attn_file;
static struct proc_dir_entry *himax_proc_int_en_file;
static struct proc_dir_entry *himax_proc_layout_file;
static struct proc_dir_entry *himax_proc_CRC_test_file;

int himax_touch_proc_init(void);
void himax_touch_proc_deinit(void);
extern int himax_incell_int_en_set(void);

#define HIMAX_PROC_REGISTER_FILE	"register"
struct proc_dir_entry *himax_incell_proc_register_file = NULL;
uint8_t incell_byte_length = 0;
uint8_t incell_register_command[4];
uint8_t incell_cfg_flag = 0;

#define HIMAX_PROC_DIAG_FILE	"diag"
struct proc_dir_entry *himax_incell_proc_diag_file = NULL;
#define HIMAX_PROC_DIAG_ARR_FILE	"diag_arr"
struct proc_dir_entry *himax_incell_proc_diag_arrange_file = NULL;
struct file *incell_diag_sram_fn;
uint8_t incell_write_counter = 0;
uint8_t incell_write_max_count = 30;
#define IIR_DUMP_FILE "/sdcard/HX_IIR_Dump.txt"
#define DC_DUMP_FILE "/sdcard/HX_DC_Dump.txt"
#define BANK_DUMP_FILE "/sdcard/HX_BANK_Dump.txt"
#ifdef HX_TP_PROC_2T2R
	static uint8_t x_channel_2;
	static uint8_t y_channel_2;
	static uint32_t *diag_mutual_2;
	int32_t *incell_getMutualBuffer_2(void);
	uint8_t 	incell_getXChannel_2(void);
	uint8_t 	incell_getYChannel_2(void);
	void 	setMutualBuffer_2(void);
	void 	incell_setXChannel_2(uint8_t x);
	void 	incell_setYChannel_2(uint8_t y);
#endif
uint8_t incell_x_channel 		= 0;
uint8_t incell_y_channel 		= 0;
int32_t *incell_diag_mutual = NULL;
int32_t *incell_diag_mutual_new = NULL;
int32_t *incell_diag_mutual_old = NULL;
uint8_t incell_diag_max_cnt = 0;
uint8_t incell_hx_state_info[2] = {0};
uint8_t incell_diag_coor[128];
int32_t incell_diag_self[100] = {0};
int32_t incell_diag_self_new[100] = {0};
int32_t incell_diag_self_old[100] = {0};
int32_t *incell_getMutualBuffer(void);
int32_t *incell_getMutualNewBuffer(void);
int32_t *incell_getMutualOldBuffer(void);
int32_t *incell_getSelfBuffer(void);
int32_t *incell_getSelfNewBuffer(void);
int32_t *incell_getSelfOldBuffer(void);
uint8_t 	incell_getXChannel(void);
uint8_t 	incell_getYChannel(void);
void 	setMutualBuffer(void);
void 	setMutualNewBuffer(void);
void 	setMutualOldBuffer(void);
void 	incell_setXChannel(uint8_t x);
void 	incell_setYChannel(uint8_t y);

#define HIMAX_PROC_DEBUG_FILE	"debug"
struct proc_dir_entry *himax_incell_proc_debug_file = NULL;
#define HIMAX_PROC_FW_DEBUG_FILE	"FW_debug"
struct proc_dir_entry *himax_incell_proc_fw_debug_file = NULL;
#define HIMAX_PROC_DD_DEBUG_FILE	"DD_debug"
struct proc_dir_entry *himax_incell_proc_dd_debug_file = NULL;
bool	incell_fw_update_complete = false;
int incell_handshaking_result = 0;
unsigned char incell_debug_level_cmd = 0;
uint8_t incell_cmd_set[8];
uint8_t incell_mutual_set_flag = 0;

#define HIMAX_PROC_FLASH_DUMP_FILE	"flash_dump"
struct proc_dir_entry *himax_incell_proc_flash_dump_file = NULL;
static int Flash_Size = 131072;
static uint8_t *flash_buffer;
static uint8_t flash_command;
static uint8_t flash_read_step;
static uint8_t flash_progress;
static uint8_t flash_dump_complete;
static uint8_t flash_dump_fail;
static uint8_t sys_operation;
static bool    flash_dump_going;
static uint8_t getFlashDumpComplete(void);
static uint8_t getFlashDumpFail(void);
static uint8_t getFlashDumpProgress(void);
static uint8_t getFlashReadStep(void);
uint8_t incell_getFlashCommand(void);
uint8_t incell_getSysOperation(void);
static void setFlashCommand(uint8_t command);
static void setFlashReadStep(uint8_t step);
void incell_setFlashBuffer(void);
void incell_setFlashDumpComplete(uint8_t complete);
void incell_setFlashDumpFail(uint8_t fail);
void incell_setFlashDumpProgress(uint8_t progress);
void incell_setSysOperation(uint8_t operation);
void incell_setFlashDumpGoing(bool going);
bool incell_getFlashDumpGoing(void);


uint32_t **incell_raw_data_array;
uint8_t INCELL_X_NUM = 0, INCELL_Y_NUM = 0;
uint8_t incell_sel_type = 0x0D;

#define HIMAX_PROC_RESET_FILE		"reset"
struct proc_dir_entry *himax_incell_proc_reset_file 		= NULL;

#define HIMAX_PROC_SENSE_ON_OFF_FILE "SenseOnOff"
struct proc_dir_entry *himax_incell_proc_SENSE_ON_OFF_file = NULL;

#ifdef HX_ESD_RECOVERY
	#define HIMAX_PROC_ESD_CNT_FILE "ESD_cnt"
	struct proc_dir_entry *himax_incell_proc_ESD_cnt_file = NULL;
#endif

#endif
