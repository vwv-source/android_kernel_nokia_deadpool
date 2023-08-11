/*
** =============================================================================
** Copyright (c) 2016  Texas Instruments Inc.
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
** File:
**     drv2605.c
**
** Description:
**     DRV2605 chip driver
**
** =============================================================================
*/
#define DEBUG
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/semaphore.h>
#include <linux/device.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/sched.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/jiffies.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/miscdevice.h>
#include "drv2605.h"
static struct drv2605_data *g_DRV2605data = NULL;
static bool g_logEnable = false;
const char *TS2200EffectNameStr[] = {
	"1 Strong Click - 100%",
	"2 Strong Click - 60%",
	"3 Strong Click - 30%",
	"4 Sharp Click - 100%",
	"5 Sharp Click - 60%",
	"6 Sharp Click - 30%",
	"7 Soft Bump - 100%",
	"8 Soft Bump - 60%",
	"9 Soft Bump - 30%",
	"10 Double Click - 100%",
	"11 Double Click - 60%",
	"12 Triple Click - 100%",
	"13 Soft Fuzz - 60%",
	"14 Strong Buzz - 100%",
	"15 750 ms Alert 100%",
	"16 1000 ms Alert 100%",
	"17 Strong Click 1 - 100%",
	"18 Strong Click 2 - 80%",
	"19 Strong Click 3 - 60%",
	"20 Strong Click 4 - 30%",
	"21 Medium Click 1 - 100%",
	"22 Medium Click 2 - 80%",
	"23 Medium Click 3 - 60%",
	"24 Sharp Tick 1 - 100%",
	"25 Sharp Tick 2 - 80%",
	"26 Sharp Tick 3 � 60%",
	"27 Short Double Click Strong 1 � 100%",
	"28 Short Double Click Strong 2 � 80%",
	"29 Short Double Click Strong 3 � 60%",
	"30 Short Double Click Strong 4 � 30%",
	"31 Short Double Click Medium 1 � 100%",
	"32 Short Double Click Medium 2 � 80%",
	"33 Short Double Click Medium 3 � 60%",
	"34 Short Double Sharp Tick 1 � 100%",
	"35 Short Double Sharp Tick 2 � 80%",
	"36 Short Double Sharp Tick 3 � 60%",
	"37 Long Double Sharp Click Strong 1 � 100%",
	"38 Long Double Sharp Click Strong 2 � 80%",
	"39 Long Double Sharp Click Strong 3 � 60%",
	"40 Long Double Sharp Click Strong 4 � 30%",
	"41 Long Double Sharp Click Medium 1 � 100%",
	"42 Long Double Sharp Click Medium 2 � 80%",
	"43 Long Double Sharp Click Medium 3 � 60%",
	"44 Long Double Sharp Tick 1 � 100%",
	"45 Long Double Sharp Tick 2 � 80%",
	"46 Long Double Sharp Tick 3 � 60%",
	"47 Buzz 1 � 100%",
	"48 Buzz 2 � 80%",
	"49 Buzz 3 � 60%",
	"50 Buzz 4 � 40%",
	"51 Buzz 5 � 20%",
	"52 Pulsing Strong 1 � 100%",
	"53 Pulsing Strong 2 � 60%",
	"54 Pulsing Medium 1 � 100%",
	"55 Pulsing Medium 2 � 60%",
	"56 Pulsing Sharp 1 � 100%",
	"57 Pulsing Sharp 2 � 60%",
	"58 Transition Click 1 � 100%",
	"59 Transition Click 2 � 80%",
	"60 Transition Click 3 � 60%",
	"61 Transition Click 4 � 40%",
	"62 Transition Click 5 � 20%",
	"63 Transition Click 6 � 10%",
	"64 Transition Hum 1 � 100%",
	"65 Transition Hum 2 � 80%",
	"66 Transition Hum 3 � 60%",
	"67 Transition Hum 4 � 40%",
	"68 Transition Hum 5 � 20%",
	"69 Transition Hum 6 � 10%",
	"70 Transition Ramp Down Long Smooth 1 � 100 to 0%",
	"71 Transition Ramp Down Long Smooth 2 � 100 to 0%",
	"72 Transition Ramp Down Medium Smooth 1 � 100 to 0%",		
	"73 Transition Ramp Down Medium Smooth 2 � 100 to 0%",
	"74 Transition Ramp Down Short Smooth 1 � 100 to 0%",
	"75 Transition Ramp Down Short Smooth 2 � 100 to 0%",
	"76 Transition Ramp Down Long Sharp 1 � 100 to 0%",
	"77 Transition Ramp Down Long Sharp 2 � 100 to 0%",
	"78 Transition Ramp Down Medium Sharp 1 � 100 to 0%",
	"79 Transition Ramp Down Medium Sharp 2 � 100 to 0%",
	"80 Transition Ramp Down Short Sharp 1 � 100 to 0%",
	"81 Transition Ramp Down Short Sharp 2 � 100 to 0%",
	"82 Transition Ramp Up Long Smooth 1 � 0 to 100%",
	"83 Transition Ramp Up Long Smooth 2 � 0 to 100%",
	"84 Transition Ramp Up Medium Smooth 1 � 0 to 100%",
	"85 Transition Ramp Up Medium Smooth 2 � 0 to 100%",
	"86 Transition Ramp Up Short Smooth 1 � 0 to 100%",
	"87 Transition Ramp Up Short Smooth 2 � 0 to 100%",
	"88 Transition Ramp Up Long Sharp 1 � 0 to 100%",
	"89 Transition Ramp Up Long Sharp 2 � 0 to 100%",
	"90 Transition Ramp Up Medium Sharp 1 � 0 to 100%",
	"91 Transition Ramp Up Medium Sharp 2 � 0 to 100%",
	"92 Transition Ramp Up Short Sharp 1 � 0 to 100%",
	"93 Transition Ramp Up Short Sharp 2 � 0 to 100%",
	"94 Transition Ramp Down Long Smooth 1 � 50 to 0%",
	"95 Transition Ramp Down Long Smooth 2 � 50 to 0%",
	"96 Transition Ramp Down Medium Smooth 1 � 50 to 0%",
	"97 Transition Ramp Down Medium Smooth 2 � 50 to 0%",
	"98 Transition Ramp Down Short Smooth 1 � 50 to 0%",
	"99 Transition Ramp Down Short Smooth 2 � 50 to 0%",
	"100 Transition Ramp Down Long Sharp 1 � 50 to 0%",
	"101 Transition Ramp Down Long Sharp 2 � 50 to 0%",
	"102 Transition Ramp Down Medium Sharp 1 � 50 to 0%",
	"103 Transition Ramp Down Medium Sharp 2 � 50 to 0%",
	"104 Transition Ramp Down Short Sharp 1 � 50 to 0%",
	"105 Transition Ramp Down Short Sharp 2 � 50 to 0%",
	"106 Transition Ramp Up Long Smooth 1 � 0 to 50%",
	"107 Transition Ramp Up Long Smooth 2 � 0 to 50%",
	"108 Transition Ramp Up Medium Smooth 1 � 0 to 50%",
	"109 Transition Ramp Up Medium Smooth 2 � 0 to 50%",
	"110 Transition Ramp Up Short Smooth 1 � 0 to 50%",
	"111 Transition Ramp Up Short Smooth 2 � 0 to 50%", 
	"112 Transition Ramp Up Long Sharp 1 � 0 to 50%",
	"113 Transition Ramp Up Long Sharp 2 � 0 to 50%",
	"114 Transition Ramp Up Medium Sharp 1 � 0 to 50%",
	"115 Transition Ramp Up Medium Sharp 2 � 0 to 50%",
	"116 Transition Ramp Up Short Sharp 1 � 0 to 50%",
	"117 Transition Ramp Up Short Sharp 2 � 0 to 50%",
	"118 Long buzz for programmatic stopping � 100%", 
	"119 Smooth Hum 1 (No kick or brake pulse) � 50%",
	"120 Smooth Hum 2 (No kick or brake pulse) � 40%",
	"121 Smooth Hum 3 (No kick or brake pulse) � 30%",
	"122 Smooth Hum 4 (No kick or brake pulse) � 20%",
	"123 Smooth Hum 5 (No kick or brake pulse) � 10%"
};
static int drv2605_reg_read(struct drv2605_data *pDRV2605,
	unsigned char reg, unsigned char *pVal)
{
	unsigned int val;
	int nResult;
	mutex_lock(&pDRV2605->dev_lock);
	nResult = regmap_read(pDRV2605->mpRegmap, reg, &val);
	if (nResult < 0)
		dev_err(pDRV2605->dev, "%s I2C error %d\n", __func__, nResult);
	else {
		if (g_logEnable)
			dev_dbg(pDRV2605->dev, "%s, Reg[0x%x]=0x%x\n", __func__, reg, val);
		*pVal = (unsigned char)val;
	}
	mutex_unlock(&pDRV2605->dev_lock);
	return nResult;
}
static int drv2605_reg_write(struct drv2605_data *pDRV2605,
	unsigned char reg, unsigned char val)
{
	int nResult;
	mutex_lock(&pDRV2605->dev_lock);
	nResult = regmap_write(pDRV2605->mpRegmap, reg, val);
	if (nResult < 0)
		dev_err(pDRV2605->dev, "%s reg=0x%x, value=0%x error %d\n",
			__func__, reg, val, nResult);
	else {
		if (g_logEnable)
			dev_dbg(pDRV2605->dev, "%s, Reg[0x%x]=0x%x\n", __func__, reg, val);
	} 
	mutex_unlock(&pDRV2605->dev_lock);
	return nResult;
}
static int drv2605_bulk_read(struct drv2605_data *pDRV2605,
	unsigned char reg, unsigned char *buf, unsigned int count)
{
	int nResult, i;
	mutex_lock(&pDRV2605->dev_lock);
	nResult = regmap_bulk_read(pDRV2605->mpRegmap, reg, buf, count);
	if (nResult < 0)
		dev_err(pDRV2605->dev, "%s reg=0%x, count=%d error %d\n",
			__func__, reg, count, nResult);
	else {
		if (g_logEnable) {
			for (i = 0; i < count; i++)
				dev_dbg(pDRV2605->dev, "%s, Reg[0x%x]=0x%x\n",
					__func__, reg + i, buf[i]);
		}
	}
	mutex_unlock(&pDRV2605->dev_lock);
	return nResult;
}
static int drv2605_bulk_write(struct drv2605_data *pDRV2605,
	unsigned char reg, const u8 *buf, unsigned int count)
{
	int nResult, i;
	mutex_lock(&pDRV2605->dev_lock);
	nResult = regmap_bulk_write(pDRV2605->mpRegmap, reg, buf, count);
	if (nResult < 0)
		dev_err(pDRV2605->dev, "%s reg=0%x, count=%d error %d\n",
			__func__, reg, count, nResult);
	else {
		if (g_logEnable)
			for (i = 0; i < count; i++)
				dev_dbg(pDRV2605->dev, "%s, Reg[0x%x]=0x%x\n",
					__func__, reg + i, buf[i]);
	}
	mutex_unlock(&pDRV2605->dev_lock);
	return nResult;
}
static int drv2605_set_bits(struct drv2605_data *pDRV2605,
	unsigned char reg, unsigned char mask, unsigned char val)
{
	int nResult;
	mutex_lock(&pDRV2605->dev_lock);
	nResult = regmap_update_bits(pDRV2605->mpRegmap, reg, mask, val);
	if (nResult < 0)
		dev_err(pDRV2605->dev, "%s reg=%x, mask=0x%x, value=0x%x error %d\n",
			__func__, reg, mask, val, nResult);
	else {
		if (g_logEnable)
			dev_dbg(pDRV2605->dev, "%s, Reg[0x%x]:M=0x%x, V=0x%x\n",
				__func__, reg, mask, val);
	}
	mutex_unlock(&pDRV2605->dev_lock);
	return nResult;
}
static int drv2605_set_RTPStrength(struct drv2605_data *pDRV2605, char nStrength)
{
	int nResult = 0;
	nResult = drv2605_reg_write(pDRV2605, DRV2605_REG_RTP_INPUT, nStrength);
	return nResult;
}
static int drv2605_set_devMode(struct drv2605_data *pDRV2605,
	enum dev_mode mode)
{
	int nResult = 0;
	nResult = drv2605_reg_write(pDRV2605, DRV2605_REG_MODE, mode);
	if (nResult >= 0) {
		if (pDRV2605->mnDevMode == MODE_STANDBY)
			mdelay(2);
		pDRV2605->mnDevMode = mode;
		if (pDRV2605->mnDevMode == MODE_STANDBY)
			mdelay(2);
		dev_dbg(pDRV2605->dev, "%s, mode=0x%x\n", __func__, mode);
	}
	return nResult;
}
static int drv2605_RTPStop(struct drv2605_data *pDRV2605)
{
	int nResult = 0;
	/* perform auto brake */
	nResult = drv2605_set_RTPStrength(pDRV2605, 0);
	if (nResult >= 0) {
		/* wait 20ms for the auto-brake to finish */
		msleep(20);
		nResult = drv2605_set_devMode(pDRV2605, MODE_INTL_TRIG);
	}
	return nResult;
}
static int drv2605_set_go_bit(struct drv2605_data *pDRV2605, unsigned char val)
{
	int nResult = 0;
	unsigned char value = 0;
	int retry = POLL_GO_BIT_RETRY; 
	val &= 0x01;
	nResult = drv2605_reg_write(pDRV2605, DRV2605_REG_GO, val);
	if (nResult < 0) 
		goto end;
	mdelay(POLL_GO_BIT_INTERVAL);
	nResult = drv2605_reg_read(pDRV2605, DRV2605_REG_GO, &value);
	if (nResult < 0)
		goto end;
	if (val) {
		if (value != GO) {
			nResult = -EIO;
			dev_warn(pDRV2605->dev, "%s, GO fail, stop action\n", __func__);
		}
	} else {
		while (retry > 0) {
			nResult = drv2605_reg_read(pDRV2605, DRV2605_REG_GO, &value);
			if (nResult < 0)
				break;
			if(value==0)
				break;
			mdelay(POLL_GO_BIT_INTERVAL);
			retry--;
		}
		if (retry == 0)
			dev_err(pDRV2605->dev,
				"%s, ERROR: clear GO fail\n", __func__);
		else {
			if (g_logEnable)
				dev_dbg(pDRV2605->dev,
					"%s, clear GO, remain=%d\n", __func__, retry);
		}
	}
end:
	return nResult;
}
static int vibrator_get_time(struct timed_output_dev *dev)
{
	struct drv2605_data *pDRV2605 = container_of(dev, struct drv2605_data, to_dev);
	if (hrtimer_active(&pDRV2605->timer)) {
		ktime_t r = hrtimer_get_remaining(&pDRV2605->timer);
		return ktime_to_ms(r);
	}
	return 0;
}
static void drv2605_set_stopflag(struct drv2605_data *pDRV2605)
{
	pDRV2605->mnVibratorPlaying = NO;
	pDRV2605->mnWorkMode = WORK_IDLE;
	wake_unlock(&pDRV2605->wklock);
	if (g_logEnable)
		dev_dbg(pDRV2605->dev, "wklock unlock");
}
static int drv2605_get_diag_result(struct drv2605_data *pDRV2605, unsigned char nStatus)
{
	int nResult = 0;
	pDRV2605->mDiagResult.mnResult = nStatus;
	if ((nStatus & DIAG_MASK) != DIAG_SUCCESS)
		dev_err(pDRV2605->dev, "Diagnostic fail\n");
	return nResult;
}
static int drv2605_get_calibration_result(struct drv2605_data *pDRV2605, unsigned char nStatus)
{
	int nResult = 0;
	unsigned char value;
	pDRV2605->mAutoCalResult.mnResult = nStatus;
	if ((nStatus & DIAG_MASK) != DIAG_SUCCESS)
		dev_err(pDRV2605->dev, "Calibration fail\n");
	else {
		nResult = drv2605_reg_read(pDRV2605, DRV2605_REG_CAL_COMP, &value);
		if (nResult < 0)
			goto end;
		pDRV2605->mAutoCalResult.mnCalComp = value;
		nResult = drv2605_reg_read(pDRV2605, DRV2605_REG_CAL_BEMF, &value);
		if (nResult < 0)
			goto end;
		pDRV2605->mAutoCalResult.mnCalBemf = value;
		nResult = drv2605_reg_read(pDRV2605, DRV2605_REG_LOOP_CONTROL, &value) & BEMFGAIN_MASK;
		if (nResult < 0)
			goto end;
		pDRV2605->mAutoCalResult.mnCalGain = value;
		dev_dbg(pDRV2605->dev, "AutoCal : Comp=0x%x, Bemf=0x%x, Gain=0x%x\n",
			pDRV2605->mAutoCalResult.mnCalComp,
			pDRV2605->mAutoCalResult.mnCalBemf,
			pDRV2605->mAutoCalResult.mnCalGain);
	}
end:
	return nResult;
}
static int drv2605_stop(struct drv2605_data *pDRV2605)
{
	int nResult = 0;
	if (pDRV2605->mnVibratorPlaying == YES) {
		dev_dbg(pDRV2605->dev, "%s\n", __func__);
		if (hrtimer_active(&pDRV2605->timer))
			hrtimer_cancel(&pDRV2605->timer);
		if (pDRV2605->mnWorkMode == WORK_RTP)
			nResult = drv2605_RTPStop(pDRV2605);
		else
			nResult = drv2605_set_go_bit(pDRV2605, STOP);
		drv2605_set_stopflag(pDRV2605);
	}
	return nResult;
}
static void vibrator_enable( struct timed_output_dev *dev, int value)
{
	int nResult = 0;
	struct drv2605_data *pDRV2605 = 
		container_of(dev, struct drv2605_data, to_dev);
	dev_err(pDRV2605->dev, "%s, value=%d\n", __func__, value);
	mutex_lock(&pDRV2605->lock);
	nResult = drv2605_stop(pDRV2605);
	if (nResult < 0)
		goto exit;
	if (value > 0) {
		wake_lock(&pDRV2605->wklock);
		if (g_logEnable)
			dev_dbg(pDRV2605->dev, "wklock lock");
		nResult = drv2605_set_devMode(pDRV2605, MODE_INTL_TRIG);
		if (nResult < 0)
			goto end;
		nResult = drv2605_set_RTPStrength(pDRV2605, 0x7f);
		if (nResult < 0)
			goto end;
		nResult = drv2605_set_devMode(pDRV2605, MODE_RTP);
		if (nResult >= 0) {
			value = (value > MAX_TIMEOUT) ? MAX_TIMEOUT : value;
			hrtimer_start(&pDRV2605->timer, 
				ns_to_ktime((u64)value * NSEC_PER_MSEC), HRTIMER_MODE_REL);
			pDRV2605->mnVibratorPlaying = YES;
			pDRV2605->mnWorkMode = WORK_RTP;
		}
	} else {
		nResult = drv2605_set_devMode(pDRV2605, MODE_STANDBY);
		if (nResult < 0)
			goto exit;
	}
end:
	if (nResult < 0) {
		nResult = drv2605_set_devMode(pDRV2605, MODE_STANDBY);
		wake_unlock(&pDRV2605->wklock);
		if (g_logEnable)
			dev_dbg(pDRV2605->dev, "wklock unlock");
	}
exit:
	mutex_unlock(&pDRV2605->lock);
}
static enum hrtimer_restart vibrator_timer_func(struct hrtimer *timer)
{
	struct drv2605_data *pDRV2605 =
		container_of(timer, struct drv2605_data, timer);
	dev_dbg(pDRV2605->dev, "%s\n", __func__);
	schedule_work(&pDRV2605->vibrator_work);
	return HRTIMER_NORESTART;
}
static void vibrator_work_routine(struct work_struct *work)
{
	struct drv2605_data *pDRV2605 =
		container_of(work, struct drv2605_data, vibrator_work);
	unsigned char status, GOStatus; 
	int nResult = 0;
	mutex_lock(&pDRV2605->lock);
	if (g_logEnable)
		dev_dbg(pDRV2605->dev, "%s, afer mnWorkMode=0x%x\n",
			__func__, pDRV2605->mnWorkMode);
	if (pDRV2605->mnVibratorPlaying == YES) {
		dev_dbg(pDRV2605->dev, "%s\n", __func__);
		if (pDRV2605->mnWorkMode == WORK_RTP) {
			nResult = drv2605_RTPStop(pDRV2605);
			goto standby;
		} else if ((pDRV2605->mnWorkMode == WORK_CALIBRATION)
			|| (pDRV2605->mnWorkMode == WORK_DIAGNOSTIC)
			|| (pDRV2605->mnWorkMode == WORK_SEQER)) {
			nResult = drv2605_reg_read(pDRV2605, DRV2605_REG_GO, &GOStatus);
			if (nResult < 0)
				goto standby;
			if ((GOStatus & 0x01) == STOP) {
				if (pDRV2605->mnWorkMode == WORK_SEQER)
					goto standby;
				nResult = drv2605_reg_read(pDRV2605, DRV2605_REG_STATUS, &status);
				if (nResult < 0)
					goto standby;
				if (pDRV2605->mnWorkMode == WORK_CALIBRATION)
					nResult = drv2605_get_calibration_result(pDRV2605, status);
				else if (pDRV2605->mnWorkMode == WORK_DIAGNOSTIC)
					nResult = drv2605_get_diag_result(pDRV2605, status);
			} else {
				if (!hrtimer_active(&pDRV2605->timer)) {
					if (g_logEnable)
						dev_dbg(pDRV2605->dev, "will check GO bit after %d ms\n", POLL_STATUS_INTERVAL);
					hrtimer_start(&pDRV2605->timer, 
						ns_to_ktime((u64)POLL_STATUS_INTERVAL * NSEC_PER_MSEC), HRTIMER_MODE_REL);
				}
				goto exit;
			}
		}
	}
standby:
	drv2605_set_devMode(pDRV2605, MODE_STANDBY);
	drv2605_set_stopflag(pDRV2605);
exit:
	mutex_unlock(&pDRV2605->lock);
}
static int dev_auto_calibrate(struct drv2605_data *pDRV2605)
{
	int nResult = 0;
	dev_info(pDRV2605->dev, "%s\n", __func__);
	wake_lock(&pDRV2605->wklock);
	if (g_logEnable)
		dev_dbg(pDRV2605->dev, "wklock lock");
	nResult = drv2605_set_devMode(pDRV2605, MODE_INTL_TRIG);
	if (nResult < 0)
		goto end;
	nResult = drv2605_set_devMode(pDRV2605, MODE_CALIBRATION);
	if (nResult < 0)
		goto end;
	nResult = drv2605_set_go_bit(pDRV2605, GO);
	if (nResult < 0)
		goto end;
	dev_dbg(pDRV2605->dev, "calibration start\n");
	pDRV2605->mnVibratorPlaying = YES;
	pDRV2605->mnWorkMode = WORK_CALIBRATION;
	schedule_work(&pDRV2605->vibrator_work);
end:
	if (nResult < 0) {
		nResult = drv2605_set_devMode(pDRV2605, MODE_STANDBY);
		wake_unlock(&pDRV2605->wklock);
		if (g_logEnable)
			dev_dbg(pDRV2605->dev, "wklock unlock");
	}
	return nResult;
}
static int dev_run_diagnostics(struct drv2605_data *pDRV2605)
{
	int nResult = 0;
	dev_info(pDRV2605->dev, "%s\n", __func__);
	wake_lock(&pDRV2605->wklock);
	if (g_logEnable)
		dev_dbg(pDRV2605->dev, "wklock lock");
	nResult = drv2605_set_devMode(pDRV2605, MODE_INTL_TRIG);
	if (nResult < 0)
		goto end;
	nResult = drv2605_set_devMode(pDRV2605, MODE_DIAG);
	if (nResult < 0) 
		goto end;
	nResult = drv2605_set_go_bit(pDRV2605, GO);
	if (nResult < 0)
		goto end;
	dev_dbg(pDRV2605->dev, "Diag start\n");
	pDRV2605->mnVibratorPlaying = YES;
	pDRV2605->mnWorkMode = WORK_DIAGNOSTIC;
	schedule_work(&pDRV2605->vibrator_work);
end:
	if (nResult < 0) {
		nResult = drv2605_set_devMode(pDRV2605, MODE_STANDBY);
		wake_unlock(&pDRV2605->wklock);
		if (g_logEnable)
			dev_dbg(pDRV2605->dev, "wklock unlock");
	}
	return nResult;
}
static int drv2605_playEffect(struct drv2605_data *pDRV2605)
{
	int nResult = 0;
	dev_info(pDRV2605->dev, "%s\n", __func__);
	wake_lock(&pDRV2605->wklock);
	if (g_logEnable)
		dev_dbg(pDRV2605->dev, "wklock lock");
	nResult = drv2605_set_devMode(pDRV2605, MODE_INTL_TRIG);
	if (nResult < 0) 
		goto end;
	nResult = drv2605_set_go_bit(pDRV2605, GO);
	if (nResult < 0) 
		goto end;
	dev_dbg(pDRV2605->dev, "effects start\n");
	pDRV2605->mnVibratorPlaying = YES;
	pDRV2605->mnWorkMode = WORK_SEQER;
	schedule_work(&pDRV2605->vibrator_work);
end:
	if (nResult < 0) {
		nResult = drv2605_set_devMode(pDRV2605, MODE_STANDBY);
		wake_unlock(&pDRV2605->wklock);
		dev_dbg(pDRV2605->dev, "wklock unlock");
	}
	return nResult;
}
static int drv2605_set_waveform(struct drv2605_data *pDRV2605,
	struct drv2605_waveform_sequencer *pSequencer)
{
	int nResult = 0;
	int i = 0;
	unsigned char effects[DRV2605_SEQUENCER_SIZE] = {0};
	unsigned char len = 0;
	for (i = 0; i < DRV2605_SEQUENCER_SIZE; i++) {
		if (pSequencer->msWaveform[i].mnEffect != 0) {
			effects[i] = pSequencer->msWaveform[i].mnEffect;
			if (effects[i] <= 123)
				dev_dbg(pDRV2605->dev, "[%d] %s\n", i, TS2200EffectNameStr[effects[i] - 1]);
			else if (effects[i] >= 0x80)
				dev_dbg(pDRV2605->dev, "[%d] Delay %d ms\n", i, (effects[i] & 0x7f) * 10);
			else {
				dev_err(pDRV2605->dev, "[%d] invalid %d effect index\n", i, effects[i]);
				goto end;
			}
		} else
			break;
		len ++;
	}
	if (len == 0)
		nResult = drv2605_reg_write(pDRV2605, DRV2605_REG_SEQUENCER_1, 0);
	else {
		len = (len < 8) ? (len + 1) : 8;
		nResult = drv2605_bulk_write(pDRV2605, DRV2605_REG_SEQUENCER_1, effects, len);
	}
	if (nResult < 0) { 
		dev_err(pDRV2605->dev, "sequence error\n");
	}
end:
	return nResult;
}
static int drv2605_file_open(struct inode *inode, struct file *file)
{
	if (!try_module_get(THIS_MODULE))
		return -ENODEV;
	file->private_data = (void*)g_DRV2605data;
	return 0;
}
static int drv2605_file_release(struct inode *inode, struct file *file)
{
	file->private_data = (void*)NULL;
	module_put(THIS_MODULE);
	return 0;
}
static long drv2605_file_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct drv2605_data *pDRV2605 = file->private_data;
	//void __user *user_arg = (void __user *)arg;
	int nResult = 0;
	mutex_lock(&pDRV2605->lock);
	dev_dbg(pDRV2605->dev, "ioctl 0x%x\n", cmd);
	switch (cmd) {
	}
	mutex_unlock(&pDRV2605->lock);
	return nResult;
}
static ssize_t drv2605_file_read(struct file* filp, char* buff, size_t length, loff_t* offset)
{
	struct drv2605_data *pDRV2605 = (struct drv2605_data *)filp->private_data;
	int nResult = 0;
	unsigned char value = 0;
	unsigned char *p_kBuf = NULL;
	mutex_lock(&pDRV2605->lock);
	switch (pDRV2605->mnFileCmd) {
	case HAPTIC_CMDID_REG_READ:
		if (length == 1) {
			nResult = drv2605_reg_read(pDRV2605, pDRV2605->mnCurrentReg, &value);
			if (nResult >= 0) {
				nResult = copy_to_user(buff, &value, 1);
				if (0 != nResult) {
					/* Failed to copy all the data, exit */
					dev_err(pDRV2605->dev, "copy to user fail %d\n", nResult);
				}
			}
		} else if (length > 1) {
			p_kBuf = (unsigned char *)kzalloc(length, GFP_KERNEL);
			if (p_kBuf != NULL) {
				nResult = drv2605_bulk_read(pDRV2605,
					pDRV2605->mnCurrentReg, p_kBuf, length);
				if (nResult >= 0) {
					nResult = copy_to_user(buff, p_kBuf, length);
					if (0 != nResult) {
						/* Failed to copy all the data, exit */
						dev_err(pDRV2605->dev, "copy to user fail %d\n", nResult);
					}
				}
				kfree(p_kBuf);
			} else {
				dev_err(pDRV2605->dev, "read no mem\n");
				nResult = -ENOMEM;
			}
		}
		break;
	case HAPTIC_CMDID_RUN_DIAG:
		if (pDRV2605->mnVibratorPlaying)
			length = 0;
		else {
			unsigned char buf[3];
			buf[0] = pDRV2605->mDiagResult.mnResult;
			nResult = copy_to_user(buff, buf, 1);
			if (0 != nResult) {
				/* Failed to copy all the data, exit */
				dev_err(pDRV2605->dev, "copy to user fail %d\n", nResult);
			}
		}
		break;
	case HAPTIC_CMDID_RUN_CALIBRATION:
		if (pDRV2605->mnVibratorPlaying)
			length = 0;
		else {
			unsigned char buf[4];
			buf[0] = pDRV2605->mAutoCalResult.mnResult;
			buf[1] = pDRV2605->mAutoCalResult.mnCalComp;
			buf[2] = pDRV2605->mAutoCalResult.mnCalBemf;
			buf[3] = pDRV2605->mAutoCalResult.mnCalGain;
			nResult = copy_to_user(buff, buf, 4);
			if (0 != nResult) {
				/* Failed to copy all the data, exit */
				dev_err(pDRV2605->dev, "copy to user fail %d\n", nResult);
			}
		}
		break;
	case HAPTIC_CMDID_SET_SEQUENCER:
		if (length == sizeof(struct drv2605_waveform_sequencer)) {
			struct drv2605_waveform_sequencer sequencer;
			unsigned char effects[DRV2605_SEQUENCER_SIZE] = {0};
			int i = 0;
			nResult = drv2605_bulk_read(pDRV2605, DRV2605_REG_SEQUENCER_1,
						effects, DRV2605_SEQUENCER_SIZE);
			if (nResult < 0)
				break;
			for(i = 0; i < DRV2605_SEQUENCER_SIZE; i++) {
				if (effects[i] != 0)
					sequencer.msWaveform[i].mnEffect = effects[i];
				else
					break;
			}
			nResult = copy_to_user(buff, &sequencer, length);
			if (0 != nResult) {
				/* Failed to copy all the data, exit */
				dev_err(pDRV2605->dev, "copy to user fail %d\n", nResult);
			}
		}
		break;
	case HAPTIC_CMDID_REGLOG_ENABLE:
		if (length == 1) {
			nResult = copy_to_user(buff, &g_logEnable, 1);
			if (0 != nResult) {
				/* Failed to copy all the data, exit */
				dev_err(pDRV2605->dev, "copy to user fail %d\n", nResult);
			}
		}
		break;
	default:
		pDRV2605->mnFileCmd = 0;
		break;
	}
	mutex_unlock(&pDRV2605->lock);
    return length;
}
static ssize_t drv2605_file_write(struct file* filp, const char* buff, size_t len, loff_t* off)
{
	struct drv2605_data *pDRV2605 = (struct drv2605_data *)filp->private_data;
	unsigned char *p_kBuf = NULL;
	int nResult = 0;
	mutex_lock(&pDRV2605->lock);
	p_kBuf = (unsigned char *)kzalloc(len, GFP_KERNEL);
	if (p_kBuf == NULL) {
		dev_err(pDRV2605->dev, "write no mem\n");
		goto err;
	}
	nResult = copy_from_user(p_kBuf, buff, len);
	if (0 != nResult) {
		dev_err(pDRV2605->dev,"copy_from_user failed.\n");
		goto err;
	}
	pDRV2605->mnFileCmd = p_kBuf[0];
	switch(pDRV2605->mnFileCmd) {
	case HAPTIC_CMDID_REG_READ:
		if (len == 2)
			pDRV2605->mnCurrentReg = p_kBuf[1];
		else
			dev_err(pDRV2605->dev, " read cmd len %zu err\n", len);
		break;
	case HAPTIC_CMDID_REG_WRITE:
		if ((len - 1) == 2)
			nResult = drv2605_reg_write(pDRV2605, p_kBuf[1], p_kBuf[2]);
		else if ((len - 1) > 2)
			nResult = drv2605_bulk_write(pDRV2605, p_kBuf[1], &p_kBuf[2], len - 2);
		else
			dev_err(pDRV2605->dev, "%s, reg_write len %zu error\n", __func__, len);
		break;
	case HAPTIC_CMDID_REG_SETBIT:
		if (len == 4)
			nResult = drv2605_set_bits(pDRV2605, p_kBuf[1], p_kBuf[2], p_kBuf[3]);
		else
			dev_err(pDRV2605->dev, "setbit len %zu error\n", len);
		break;
	case HAPTIC_CMDID_RUN_DIAG:
		nResult = drv2605_stop(pDRV2605);
		if (nResult < 0)
			break;
		nResult = dev_run_diagnostics(pDRV2605);
		break;
	case HAPTIC_CMDID_RUN_CALIBRATION:
		nResult = drv2605_stop(pDRV2605);
		if (nResult < 0)
			break;
		nResult = dev_auto_calibrate(pDRV2605);
		break;
	case HAPTIC_CMDID_SET_SEQUENCER:
		if (len == (1 + sizeof(struct drv2605_waveform_sequencer))) {
			struct drv2605_waveform_sequencer sequencer;
			memcpy(&sequencer, &p_kBuf[1], sizeof(struct drv2605_waveform_sequencer));
			nResult = drv2605_set_waveform(pDRV2605, &sequencer);
		} else
			dev_dbg(pDRV2605->dev, "pass cmd, prepare for read\n");
		break;
	case HAPTIC_CMDID_PLAY_EFFECT_SEQUENCE:
		nResult = drv2605_stop(pDRV2605);
		if (nResult < 0)
			break;
		nResult = drv2605_playEffect(pDRV2605);
		break;
	case HAPTIC_CMDID_STOP:
		nResult = drv2605_stop(pDRV2605);
		break;
	case HAPTIC_CMDID_REGLOG_ENABLE:
		if (len == 2)
			g_logEnable = p_kBuf[1];
		break;
	default:
		dev_err(pDRV2605->dev, "%s, unknown cmd\n", __func__);
		break;
	}
err:
	if (p_kBuf != NULL)
		kfree(p_kBuf);
    mutex_unlock(&pDRV2605->lock);
    return len;
}
static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.read = drv2605_file_read,
	.write = drv2605_file_write,
	.unlocked_ioctl = drv2605_file_unlocked_ioctl,
	.open = drv2605_file_open,
	.release = drv2605_file_release,
};
static struct miscdevice drv2605_misc =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = HAPTICS_DEVICE_NAME,
	.fops = &fops,
};
 
static int Haptics_init(struct drv2605_data *pDRV2605)
{
	int nResult = 0;
	pDRV2605->to_dev.name = "vibrator";
	pDRV2605->to_dev.get_time = vibrator_get_time;
	pDRV2605->to_dev.enable = vibrator_enable;
	nResult = timed_output_dev_register(&(pDRV2605->to_dev));
	if (nResult < 0) {
		dev_err(pDRV2605->dev, "drv2605: fail to create timed output dev\n");
		return nResult;
	}
	nResult = misc_register(&drv2605_misc);
	if (nResult) {
		dev_err(pDRV2605->dev, "drv2605 misc fail: %d\n", nResult);
		return nResult;
	}
	hrtimer_init(&pDRV2605->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	pDRV2605->timer.function = vibrator_timer_func;
	INIT_WORK(&pDRV2605->vibrator_work, vibrator_work_routine);
	wake_lock_init(&pDRV2605->wklock, WAKE_LOCK_SUSPEND, "vibrator");
	mutex_init(&pDRV2605->lock);
	return 0;
}
static int dev_init_platform_data(struct drv2605_data *pDRV2605)
{
	struct drv2605_platform_data *pDrv2605Platdata = &pDRV2605->msPlatData;
	struct actuator_data actuator = pDrv2605Platdata->msActuator;
	int nResult = 0;
	nResult = drv2605_set_devMode(pDRV2605, MODE_INTL_TRIG);
	if (nResult < 0)
		goto end;
	if (actuator.mnActuatorType == ERM) {
		nResult = drv2605_set_bits(pDRV2605, DRV2605_REG_LOOP_CONTROL, 0x80, 0x00);
		if (pDrv2605Platdata->mnLoop == CLOSE_LOOP)
			nResult = drv2605_set_bits(pDRV2605, DRV2605_REG_CONTROL3, 0x20, 0x00);
		else if (pDrv2605Platdata->mnLoop == OPEN_LOOP)
			nResult = drv2605_set_bits(pDRV2605, DRV2605_REG_CONTROL3, 0x20, 0x20);
		nResult = drv2605_reg_write(pDRV2605, DRV2605_REG_LIBSEL, actuator.mnEffectLib);
	} else if (actuator.mnActuatorType == LRA) {
		nResult = drv2605_set_bits(pDRV2605, DRV2605_REG_LOOP_CONTROL, 0x80, 0x80);
		if (pDrv2605Platdata->mnLoop == CLOSE_LOOP) {
			nResult = drv2605_set_bits(pDRV2605, DRV2605_REG_CONTROL3, 0x01, 0x00);
			nResult = drv2605_reg_write(pDRV2605, DRV2605_REG_LIBSEL, 0x06);
		} else if (pDrv2605Platdata->mnLoop == OPEN_LOOP)
			nResult = drv2605_set_bits(pDRV2605, DRV2605_REG_CONTROL3, 0x01, 0x01);
	}
	if (actuator.mnRatedVoltage != 0)
		drv2605_reg_write(pDRV2605, DRV2605_REG_RATED_VOLTAGE, actuator.mnRatedVoltage);
	else
		dev_err(pDRV2605->dev, "%s, ERROR Rated ZERO\n", __func__);
	if (actuator.mnOverDriveClampVoltage != 0)
		drv2605_reg_write(pDRV2605, DRV2605_REG_OVERDRIVE_CLAMP, actuator.mnOverDriveClampVoltage);
	else
		dev_err(pDRV2605->dev,"%s, ERROR OverDriveVol ZERO\n", __func__);
	if (actuator.mnActuatorType == LRA) {
		unsigned char DriveTime = 5 * (1000 - actuator.mnLRAFreq) / actuator.mnLRAFreq;
		nResult = drv2605_set_bits(pDRV2605, DRV2605_REG_DRIVE_TIME,
			DRIVE_TIME_MASK, DriveTime);
		dev_info(pDRV2605->dev, "%s, LRA = %d, DriveTime=0x%x\n", 
			__func__, actuator.mnLRAFreq, DriveTime);
	}
end:
	return nResult;
}
static int drv2605_parse_dt(struct device *dev, struct drv2605_data *pDRV2605)
{
	struct device_node *np = dev->of_node;
	struct drv2605_platform_data *pPlatData = &pDRV2605->msPlatData;
	int rc= 0, nResult = 0;
	unsigned int value;
	pPlatData->mnGpioEN = of_get_named_gpio(np, "ti,enable-gpio", 0);
	if (pPlatData->mnGpioEN < 0) {
		dev_err(pDRV2605->dev, "Looking up %s property in node %s failed %d\n",
			"ti,enable-gpio", np->full_name, pPlatData->mnGpioEN);
		nResult = -EINVAL;
	} else
		dev_dbg(pDRV2605->dev, "ti,reset-gpio=%d\n", pPlatData->mnGpioEN);
	if (nResult >= 0) {
		rc = of_property_read_u32(np, "ti,smart-loop", &value);
		if (rc) {
			dev_err(pDRV2605->dev, "Looking up %s property in node %s failed %d\n",
				"ti,smart-loop", np->full_name, rc);
			nResult = -EINVAL;
		} else {
			pPlatData->mnLoop = value & 0x01;
			dev_dbg(pDRV2605->dev, "ti,smart-loop=%d\n", pPlatData->mnLoop);
		}
	}
	if (nResult >= 0) {
		rc = of_property_read_u32(np, "ti,actuator", &value);
		if (rc) {
			dev_err(pDRV2605->dev, "Looking up %s property in node %s failed %d\n",
				"ti,actuator", np->full_name, rc);
			nResult = -EINVAL;
		} else {
			pPlatData->msActuator.mnActuatorType = value & 0x01;
			dev_dbg(pDRV2605->dev, "ti,actuator=%d\n",
				pPlatData->msActuator.mnActuatorType);
		}
	}
	if (nResult >= 0) {
		rc = of_property_read_u32(np, "ti,rated-voltage", &value);
		if (rc) {
			dev_err(pDRV2605->dev, "Looking up %s property in node %s failed %d\n",
				"ti,rated-voltage", np->full_name, rc);
			nResult = -EINVAL;
		}else{
			pPlatData->msActuator.mnRatedVoltage = value;
			dev_dbg(pDRV2605->dev, "ti,rated-voltage=0x%x\n",
				pPlatData->msActuator.mnRatedVoltage);
		}
	}
	if (nResult >= 0) {
		rc = of_property_read_u32(np, "ti,odclamp-voltage", &value);
		if (rc) {
			dev_err(pDRV2605->dev, "Looking up %s property in node %s failed %d\n",
				"ti,odclamp-voltage", np->full_name, rc);
			nResult = -EINVAL;
		} else {
			pPlatData->msActuator.mnOverDriveClampVoltage = value;
			dev_dbg(pDRV2605->dev, "ti,odclamp-voltage=0x%x\n", 
				pPlatData->msActuator.mnOverDriveClampVoltage);
		}
	}
	if (nResult >= 0) {
		if (pPlatData->msActuator.mnActuatorType == LRA) {
			rc = of_property_read_u32(np, "ti,lra-frequency", &value);
			if (rc) {
				dev_err(pDRV2605->dev, "Looking up %s property in node %s failed %d\n",
					"ti,lra-frequency", np->full_name, rc);
				nResult = -EINVAL;
			} else {
				if ((value >= 125) && (value <= 300)) {
					pPlatData->msActuator.mnLRAFreq = value;
					dev_dbg(pDRV2605->dev, "ti,lra-frequency=%d\n",
						pPlatData->msActuator.mnLRAFreq);
				} else {
					nResult = -EINVAL;
					dev_err(pDRV2605->dev, "ERROR, ti,lra-frequency=%d, out of range\n", 
						pPlatData->msActuator.mnLRAFreq);
				}
			}
		} else if (pPlatData->msActuator.mnActuatorType == ERM) {
			rc = of_property_read_u32(np, "ti,erm-lib", &value);
			if (rc) {
				dev_err(pDRV2605->dev, "Looking up %s property in node %s failed %d\n",
					"ti,lra-frequency", np->full_name, rc);
				nResult = -EINVAL;
			} else {
				if ((value >= 1) && (value <= 5)) {
					pPlatData->msActuator.mnERMLib = value;
					dev_dbg(pDRV2605->dev, "ti,erm-lib = %d\n",
						pPlatData->msActuator.mnERMLib);
				} else {
					nResult = -EINVAL;
					dev_err(pDRV2605->dev, "ERROR, ti,erm-lib=%d, out of range\n", 
						pPlatData->msActuator.mnERMLib);
				}
			}
		}
	}
	return nResult;
}
static struct regmap_config drv2605_i2c_regmap = {
	.reg_bits = 8,
	.val_bits = 8,
	.cache_type = REGCACHE_NONE,
};
static int drv2605_i2c_probe(struct i2c_client* client, const struct i2c_device_id* id)
{
	struct drv2605_data *pDRV2605;
	int nResult = 0;
	dev_info(&client->dev, "%s enter\n", __func__);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "%s:I2C check failed\n", __func__);
		return -ENODEV;
	}
	pDRV2605 = devm_kzalloc(&client->dev, sizeof(struct drv2605_data), GFP_KERNEL);
	if (pDRV2605 == NULL) {
		dev_err(&client->dev, "%s:no memory\n", __func__);
		return -ENOMEM;
	}
	pDRV2605->dev = &client->dev;
	i2c_set_clientdata(client,pDRV2605);
	dev_set_drvdata(&client->dev, pDRV2605);
	pDRV2605->mpRegmap = devm_regmap_init_i2c(client, &drv2605_i2c_regmap);
	if (IS_ERR(pDRV2605->mpRegmap)) {
		nResult = PTR_ERR(pDRV2605->mpRegmap);
		dev_err(pDRV2605->dev, "%s:Failed to allocate register map: %d\n",
			__func__, nResult);
		return nResult;
	}
	if (client->dev.of_node) {
		dev_dbg(pDRV2605->dev, "of node parse\n");
		nResult = drv2605_parse_dt(&client->dev, pDRV2605);
	}
	if ((nResult < 0) || !gpio_is_valid(pDRV2605->msPlatData.mnGpioEN)) {
		dev_err(pDRV2605->dev, "%s: platform data error\n",__func__);
		return -EINVAL;
	}
	if (gpio_is_valid(pDRV2605->msPlatData.mnGpioEN)) {
		nResult = gpio_request(pDRV2605->msPlatData.mnGpioEN, "DRV2605-EN");
		if (nResult < 0) {
			dev_err(pDRV2605->dev, "%s: GPIO %d request EN error\n",
				__func__, pDRV2605->msPlatData.mnGpioEN);
			return nResult;
		}
		gpio_direction_output(pDRV2605->msPlatData.mnGpioEN, 0);
		mdelay(5);
		gpio_direction_output(pDRV2605->msPlatData.mnGpioEN, 1);
		mdelay(2);
		pDRV2605->mnDevMode = MODE_STANDBY;
	}
	mutex_init(&pDRV2605->dev_lock);
	nResult = drv2605_reg_read(pDRV2605, DRV2605_REG_ID, &pDRV2605->mnDeviceID);
	if (nResult < 0)
		goto exit_gpio_request_failed;
	else
		dev_info(pDRV2605->dev, "%s, ID status (0x%x)\n", __func__, pDRV2605->mnDeviceID);
	if ((pDRV2605->mnDeviceID & 0xe0) != DRV2605_ID) {
		dev_err(pDRV2605->dev, "%s, device_id(0x%x) fail\n",
			__func__, pDRV2605->mnDeviceID);
		goto exit_gpio_request_failed;
	}
	nResult = dev_init_platform_data(pDRV2605);
	if (nResult < 0)
		goto exit_gpio_request_failed;
	g_DRV2605data = pDRV2605;
	Haptics_init(pDRV2605);
	dev_info(pDRV2605->dev, "drv2605 probe succeeded\n");
	return 0;
exit_gpio_request_failed:
	if (gpio_is_valid(pDRV2605->msPlatData.mnGpioEN))
		gpio_free(pDRV2605->msPlatData.mnGpioEN);
	mutex_destroy(&pDRV2605->dev_lock);
	dev_err(pDRV2605->dev, "%s failed, err=%d\n", __func__, nResult);
	return nResult;
}
static int drv2605_i2c_remove(struct i2c_client* client)
{
	struct drv2605_data *pDRV2605 = i2c_get_clientdata(client);
	if(pDRV2605->msPlatData.mnGpioEN)
		gpio_free(pDRV2605->msPlatData.mnGpioEN);
	misc_deregister(&drv2605_misc);
	mutex_destroy(&pDRV2605->lock);
	mutex_destroy(&pDRV2605->dev_lock);
	return 0;
}
static const struct i2c_device_id drv2605_i2c_id[] = {
	{"drv2605", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, drv2605_i2c_id);
#if defined(CONFIG_OF)
static const struct of_device_id drv2605_of_match[] = {
	{.compatible = "ti,drv2605"},
	{},
};
MODULE_DEVICE_TABLE(of, drv2605_of_match);
#endif
static struct i2c_driver drv2605_i2c_driver = {
	.driver = {
			.name = "drv2605",
			.owner = THIS_MODULE,
#if defined(CONFIG_OF)
			.of_match_table = of_match_ptr(drv2605_of_match),
#endif
		},
	.probe = drv2605_i2c_probe,
	.remove = drv2605_i2c_remove,
	.id_table = drv2605_i2c_id,
};
module_i2c_driver(drv2605_i2c_driver);
MODULE_AUTHOR("Texas Instruments Inc.");
MODULE_DESCRIPTION("DRV2605 I2C Haptics driver");
MODULE_LICENSE("GPL v2");