#ifndef __DRV2605_H__
#define __DRV2605_H__
/*
** =============================================================================
** Copyright (c)2016  Texas Instruments Inc.
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
** File:
**     drv2605.h
**
** Description:
**     Header file for drv2605.c
**
** =============================================================================
*/
#include <linux/regmap.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <../../../drivers/staging/android/timed_output.h>
#include <linux/hrtimer.h>
#include <linux/wakelock.h>
#include <linux/mutex.h>
#include <linux/cdev.h>
#define HAPTICS_DEVICE_NAME "drv2605"
#define	DRV2605_REG_ID				0x00
#define	DRV2605_ID					0x60
#define	DRV2605_REG_STATUS			0x00
#define	DIAG_MASK					0x08
#define	DIAG_SUCCESS				0x00
#define	DIAG_SHIFT					0x07
#define	INT_MASK					0x1f
#define	PRG_ERR_MASK				0x10
#define	PROCESS_DONE_MASK			0x08
#define	ULVO_MASK					0x04
#define	OVERTEMPRATURE_MASK			0x02
#define	OVERCURRENT_MASK			0x01
#define	DRV2605_REG_MODE			0x01
#define	WORKMODE_MASK				0x03
#define	DRV2605_REG_RTP_INPUT		0x02
#define	DRV2605_REG_LIBSEL			0x03
#define	DRV2605_REG_SEQUENCER_1		0x04
#define	DRV2605_REG_CONTROL1		0x08
#define	ACTUATOR_MASK				0x80
#define	ACTUATOR_SHIFT				7
#define	LOOP_MASK					0x40
#define	LOOP_SHIFT					6
#define	AUTOBRK_OK_MASK				0x10
#define	AUTOBRK_OK_ENABLE			0x10
#define	DRV2605_REG_GO				0x0c
#define	DRV2605_REG_CONTROL2		0x0d
#define	DRV2605_REG_RATED_VOLTAGE	0x16
#define	DRV2605_REG_OVERDRIVE_CLAMP	0x17
#define	DRV2605_REG_CAL_COMP		0x18
#define	DRV2605_REG_CAL_BEMF		0x19
#define	DRV2605_REG_LOOP_CONTROL	0x1a
#define	BEMFGAIN_MASK				0x03
#define	DRV2605_REG_CONTROL3		0x1d
#define	DRV2605_REG_DRIVE_TIME		0x1b
#define	DRIVE_TIME_MASK				0x1f
#define	GO_BIT_POLL_INTERVAL	15
#define	STANDBY_WAKE_DELAY		1
#define	WAKE_STANDBY_DELAY		3
/* Commands */
#define	HAPTIC_CMDID_PLAY_SINGLE_EFFECT		0x01
#define	HAPTIC_CMDID_PLAY_EFFECT_SEQUENCE	0x02
#define	HAPTIC_CMDID_PLAY_TIMED_EFFECT		0x03
#define	HAPTIC_CMDID_GET_DEV_ID				0x04
#define	HAPTIC_CMDID_RUN_DIAG				0x05
#define	HAPTIC_CMDID_AUDIOHAPTIC_ENABLE		0x06
#define	HAPTIC_CMDID_AUDIOHAPTIC_DISABLE	0x07
#define	HAPTIC_CMDID_AUDIOHAPTIC_GETSTATUS	0x08
#define	HAPTIC_CMDID_REG_WRITE				0x09
#define	HAPTIC_CMDID_REG_READ				0x0a
#define	HAPTIC_CMDID_REG_SETBIT				0x0b
#define	HAPTIC_CMDID_PATTERN_RTP			0x0c
#define	HAPTIC_CMDID_RTP_SEQUENCE			0x0d
#define	HAPTIC_CMDID_GET_EFFECT_COUNT		0x10
#define	HAPTIC_CMDID_UPDATE_FIRMWARE		0x11
#define	HAPTIC_CMDID_READ_FIRMWARE			0x12
#define	HAPTIC_CMDID_RUN_CALIBRATION		0x13
#define	HAPTIC_CMDID_CONFIG_WAVEFORM		0x14
#define	HAPTIC_CMDID_SET_SEQUENCER			0x15
#define	HAPTIC_CMDID_REGLOG_ENABLE			0x16
#define	HAPTIC_CMDID_STOP		0xFF
#define	MAX_TIMEOUT		10000 /* 10s */
#define	MAX_READ_BYTES	0xff
#define	DRV2605_SEQUENCER_SIZE	8
enum dev_mode {
	MODE_INTL_TRIG		= 0x00,
	MODE_EXTL_TRIG_E	= 0x01,
	MODE_EXTL_TRIG_L	= 0x02,
	MODE_PWM_ANG		= 0x03,
	MODE_AUDIOHAPTICS	= 0x04,
	MODE_RTP			= 0x05,
	MODE_DIAG			= 0x06,
	MODE_CALIBRATION	= 0x07,
	MODE_STANDBY		= 0x40
};
enum work_mode {
	WORK_IDLE			= 0,
	WORK_SEQER			= 0x01,
	WORK_RTP			= 0x02,
	WORK_CALIBRATION	= 0x03,
	WORK_DIAGNOSTIC		= 0x04
};
#define	YES		1
#define	NO		0
#define	GO		1
#define	STOP	0
#define	POLL_GO_BIT_INTERVAL	2	/* 2 ms */
#define	POLL_STATUS_INTERVAL	10	/* 10 ms */
#define	POLL_GO_BIT_RETRY		20	/* 20 times */
enum actuator_type {
	ERM,
	LRA
};
enum loop_type {
	CLOSE_LOOP,
	OPEN_LOOP
};
struct actuator_data {
	unsigned char mnActuatorType;
	unsigned char mnEffectLib;
	unsigned char mnRatedVoltage;
	unsigned char mnOverDriveClampVoltage;
	unsigned char mnERMLib;
	unsigned char mnLRAFreq;
};
struct drv2605_waveform {
	unsigned char mnEffect;
};
struct drv2605_waveform_sequencer {
	struct drv2605_waveform msWaveform[DRV2605_SEQUENCER_SIZE];
};
struct drv2605_autocal_result {
	int mnFinished;
	unsigned char mnResult;
	unsigned char mnCalComp;
	unsigned char mnCalBemf;
	unsigned char mnCalGain;
};
struct drv2605_diag_result {
	int mnFinished;
	unsigned char mnResult;
};
struct drv2605_platform_data {
	int mnGpioEN;
	unsigned char mnLoop; 
	struct actuator_data msActuator;
};
struct drv2605_data {
	struct drv2605_platform_data msPlatData;
	struct mutex dev_lock;
	unsigned char mnDeviceID;
	struct device *dev;
	struct regmap *mpRegmap;
	struct drv2605_waveform_sequencer msWaveformSequencer;
	unsigned char mnFileCmd;
	volatile int mnVibratorPlaying;
	enum dev_mode mnDevMode;
	enum work_mode mnWorkMode;
	unsigned char mnRTPStrength;
	unsigned char mnCurrentReg;
	struct wake_lock wklock;
	struct hrtimer timer;
	struct mutex lock;
	struct work_struct vibrator_work;
	struct timed_output_dev to_dev;
	struct drv2605_autocal_result mAutoCalResult;
	struct drv2605_diag_result mDiagResult;
};
#define DRV2605_MAGIC_NUMBER	'T'
#define DRV2605_WAVSEQ_PLAY		 			_IOWR(DRV2605_MAGIC_NUMBER, 4, unsigned long)
#define DRV2605_STOP			 			_IOWR(DRV2605_MAGIC_NUMBER, 5, unsigned long)
#define DRV2605_RUN_DIAGNOSTIC			 	_IOWR(DRV2605_MAGIC_NUMBER, 6, unsigned long)
#define DRV2605_GET_DIAGRESULT			 	_IOWR(DRV2605_MAGIC_NUMBER, 7, struct drv2605_diag_result *)
#define DRV2605_RUN_AUTOCAL				 	_IOWR(DRV2605_MAGIC_NUMBER, 8, unsigned long)
#define DRV2605_GET_CALRESULT			 	_IOWR(DRV2605_MAGIC_NUMBER, 9, struct drv2605_autocal_result *)
#endif