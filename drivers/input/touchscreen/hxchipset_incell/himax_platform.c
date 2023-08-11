/*  Himax Android Driver Sample Code for QCT platform

    Copyright (C) 2018 Himax Corporation.

    This software is licensed under the terms of the GNU General Public
    License version 2, as published by the Free Software Foundation, and
    may be copied, distributed, and modified under those terms.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*/

#include "himax_platform.h"
#include "himax_common.h"

bool spi_dev_detected = false;
int incell_i2c_error_count = 0;
int incell_irq_enable_count = 0;
struct spi_device *spi;

extern struct himax_ic_data *incell_ic_data;
extern struct himax_ts_data *private_incell_ts;
extern void himax_incell_ts_work(struct himax_ts_data *ts);
extern enum hrtimer_restart himax_incell_ts_timer_func(struct hrtimer *timer);

extern int himax_chip_common_init(void);
extern void himax_chip_common_deinit(void);

int himax_incell_dev_set(struct himax_ts_data *ts)
{
	int ret = 0;
	ts->input_dev = input_allocate_device();

	if (ts->input_dev == NULL) {
		ret = -ENOMEM;
		E("%s: Failed to allocate input device\n", __func__);
		return ret;
	}

	ts->input_dev->name = "himax-touchscreen";
	return ret;
}
int himax_incell_input_register_device(struct input_dev *input_dev)
{
	return input_register_device(input_dev);
}

#if defined(HX_PLATFOME_DEFINE_KEY)
void himax_platform_key(void)
{
	I("Nothing to be done! Plz cancel it!\n");
}
#endif

void himax_incell_vk_parser(struct device_node *dt,
						struct himax_i2c_platform_data *pdata)
{
	u32 data = 0;
	uint8_t cnt = 0, i = 0;
	uint32_t coords[4] = {0};
	struct device_node *node, *pp = NULL;
	struct himax_virtual_key *vk;
	node = of_parse_phandle(dt, "virtualkey", 0);

	if (node == NULL) {
		I(" DT-No vk info in DT\n");
		return;
	} else {
		while ((pp = of_get_next_child(node, pp)))
			cnt++;

		if (!cnt)
			return;

		vk = kzalloc(cnt * (sizeof *vk), GFP_KERNEL);
		pp = NULL;

		while ((pp = of_get_next_child(node, pp))) {
			if (of_property_read_u32(pp, "idx", &data) == 0)
				vk[i].index = data;

			if (of_property_read_u32_array(pp, "range", coords, 4) == 0) {
				vk[i].x_range_min = coords[0], vk[i].x_range_max = coords[1];
				vk[i].y_range_min = coords[2], vk[i].y_range_max = coords[3];
			} else {
				I(" range faile\n");
			}

			i++;
		}

		pdata->virtual_key = vk;

		for (i = 0; i < cnt; i++)
			I(" vk[%d] idx:%d x_min:%d, y_max:%d\n", i, pdata->virtual_key[i].index,
			  pdata->virtual_key[i].x_range_min, pdata->virtual_key[i].y_range_max);
	}
}

int himax_incell_parse_dt(struct himax_ts_data *ts,
					struct himax_i2c_platform_data *pdata)
{
	int rc, coords_size = 0;
	uint32_t coords[4] = {0};
	struct property *prop;
	struct device_node *dt = ts->dev->of_node;
	u32 data = 0;
	prop = of_find_property(dt, "himax,panel-coords", NULL);

	if (prop) {
		coords_size = prop->length / sizeof(u32);

		if (coords_size != 4)
			D(" %s:Invalid panel coords size %d\n", __func__, coords_size);
	}

	if (of_property_read_u32_array(dt, "himax,panel-coords", coords, coords_size) == 0) {
		pdata->abs_x_min = coords[0], pdata->abs_x_max = coords[1];
		pdata->abs_y_min = coords[2], pdata->abs_y_max = coords[3];
		I(" DT-%s:panel-coords = %d, %d, %d, %d\n", __func__, pdata->abs_x_min,
		  pdata->abs_x_max, pdata->abs_y_min, pdata->abs_y_max);
	}

	prop = of_find_property(dt, "himax,display-coords", NULL);

	if (prop) {
		coords_size = prop->length / sizeof(u32);

		if (coords_size != 4)
			D(" %s:Invalid display coords size %d\n", __func__, coords_size);
	}

	rc = of_property_read_u32_array(dt, "himax,display-coords", coords, coords_size);

	if (rc && (rc != -EINVAL)) {
		D(" %s:Fail to read display-coords %d\n", __func__, rc);
		return rc;
	}

	pdata->screenWidth  = coords[1];
	pdata->screenHeight = coords[3];
	I(" DT-%s:display-coords = (%d, %d)\n", __func__, pdata->screenWidth,
	  pdata->screenHeight);
	pdata->gpio_irq = of_get_named_gpio(dt, "himax,irq-gpio", 0);

	if (!gpio_is_valid(pdata->gpio_irq)) {
		I(" DT:gpio_irq value is not valid\n");
	}

	pdata->gpio_reset = of_get_named_gpio(dt, "himax,rst-gpio", 0);

	if (!gpio_is_valid(pdata->gpio_reset)) {
		I(" DT:gpio_rst value is not valid\n");
	}

	pdata->gpio_3v3_en = of_get_named_gpio(dt, "himax,3v3-gpio", 0);

	if (!gpio_is_valid(pdata->gpio_3v3_en)) {
		I(" DT:gpio_3v3_en value is not valid\n");
	}

	I(" DT:gpio_irq=%d, gpio_rst=%d, gpio_3v3_en=%d\n", pdata->gpio_irq, pdata->gpio_reset, pdata->gpio_3v3_en);

	if (of_property_read_u32(dt, "report_type", &data) == 0) {
		pdata->protocol_type = data;
		I(" DT:protocol_type=%d\n", pdata->protocol_type);
	}

	himax_incell_vk_parser(dt, pdata);
	return 0;
}

static ssize_t himax_spi_sync(struct himax_ts_data *ts, struct spi_message *message)
{
	int status;

	status = spi_sync(ts->spi, message);

	if (status == 0) {
		status = message->status;
		if (status == 0)
			status = message->actual_length;
	}
	return status;
}

static int himax_spi_read(uint8_t *command, uint8_t command_len, uint8_t *data, uint32_t length, uint8_t toRetry)
{
	struct spi_message message;
	struct spi_transfer xfer[2];
	int retry;
	int error;

	spi_message_init(&message);
	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = command;
	xfer[0].len = command_len;
	spi_message_add_tail(&xfer[0], &message);

	xfer[1].rx_buf = data;
	xfer[1].len = length;
	spi_message_add_tail(&xfer[1], &message);

	for (retry = 0; retry < toRetry; retry++) {
		error = spi_sync(private_incell_ts->spi, &message);
		if (unlikely(error)) {
			E("SPI read error: %d\n", error);
		} else{
			break;
		}
	}

	if (retry == toRetry) {
		E("%s: SPI read error retry over %d\n",
			__func__, toRetry);
		return -EIO;
	}

	return 0;
}

static int himax_spi_write(uint8_t *buf, uint32_t length)
{
	struct spi_transfer	t = {
			.tx_buf		= buf,
			.len		= length,
	};
	struct spi_message	m;
	spi_message_init(&m);
	spi_message_add_tail(&t, &m);

	return himax_spi_sync(private_incell_ts, &m);

}

int himax_spi_bus_read(uint8_t command, uint8_t *data, uint32_t length, uint8_t toRetry)
{
	int result = 0;
	uint8_t spi_format_buf[3];

	mutex_lock(&(private_incell_ts->spi_lock));
	spi_format_buf[0] = 0xF3;
	spi_format_buf[1] = command;
	spi_format_buf[2] = 0x00;

	result = himax_spi_read(&spi_format_buf[0], 3, data, length, toRetry);
	mutex_unlock(&(private_incell_ts->spi_lock));

	return result;
}

int himax_spi_bus_write(uint8_t command, uint8_t *data, uint32_t length, uint8_t toRetry)
{
	uint8_t spi_format_buf[length + 2];
	int i = 0;
	int result = 0;

	mutex_lock(&(private_incell_ts->spi_lock));
	spi_format_buf[0] = 0xF2;
	spi_format_buf[1] = command;

	for (i = 0; i < length; i++)
		spi_format_buf[i + 2] = data[i];

	result = himax_spi_write(spi_format_buf, length + 2);
	mutex_unlock(&(private_incell_ts->spi_lock));

	return result;
}

int himax_spi_bus_master_write(uint8_t *data, uint32_t length, uint8_t toRetry)
{
	uint8_t buf[length];

	struct spi_transfer	t = {
		.tx_buf	= buf,
		.len	= length,
	};
	struct spi_message	m;
	int result = 0;

	mutex_lock(&(private_incell_ts->spi_lock));
	memcpy(buf, data, length);

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	result = himax_spi_sync(private_incell_ts, &m);
	mutex_unlock(&(private_incell_ts->spi_lock));

	return result;
}

int himax_i2c_bus_read(uint8_t command, uint8_t *data, uint32_t length, uint8_t toRetry)
{
	int retry;
	struct i2c_client *client = private_incell_ts->client;
	struct i2c_msg msg[] = {
		{
			.addr = client->addr,
			.flags = 0,
			.len = 1,
			.buf = &command,
		},
		{
			.addr = client->addr,
			.flags = I2C_M_RD,
			.len = length,
			.buf = data,
		}
	};
	mutex_lock(&private_incell_ts->rw_lock);

	for (retry = 0; retry < toRetry; retry++) {
		if (i2c_transfer(client->adapter, msg, 2) == 2)
			break;

		msleep(20);
	}

	if (retry == toRetry) {
		E("%s: i2c_read_block retry over %d\n",
		  __func__, toRetry);
		incell_i2c_error_count = toRetry;
		mutex_unlock(&private_incell_ts->rw_lock);
		return -EIO;
	}

	mutex_unlock(&private_incell_ts->rw_lock);
	return 0;
}

int himax_i2c_bus_write(uint8_t command, uint8_t *data, uint32_t length, uint8_t toRetry)
{
	int retry/*, loop_i*/;
	uint8_t buf[length + 1];
	struct i2c_client *client = private_incell_ts->client;
	struct i2c_msg msg[] = {
		{
			.addr = client->addr,
			.flags = 0,
			.len = length + 1,
			.buf = buf,
		}
	};
	mutex_lock(&private_incell_ts->rw_lock);
	buf[0] = command;
	memcpy(buf + 1, data, length);

	for (retry = 0; retry < toRetry; retry++) {
		if (i2c_transfer(client->adapter, msg, 1) == 1)
			break;

		msleep(20);
	}

	if (retry == toRetry) {
		E("%s: i2c_write_block retry over %d\n",
		  __func__, toRetry);
		incell_i2c_error_count = toRetry;
		mutex_unlock(&private_incell_ts->rw_lock);
		return -EIO;
	}

	mutex_unlock(&private_incell_ts->rw_lock);
	return 0;
}

int himax_bus_read(uint8_t command, uint8_t *data, uint32_t length, uint8_t toRetry)
{
	if (spi_dev_detected == true) {
		return himax_spi_bus_read(command, data, length, toRetry);
	} else {
		return himax_i2c_bus_read(command, data, length, toRetry);
	}
}

int himax_bus_write(uint8_t command, uint8_t *data, uint32_t length, uint8_t toRetry)
{
	if (spi_dev_detected == true) {
		return himax_spi_bus_write(command, data, length, toRetry);
	} else {
		return himax_i2c_bus_write(command, data, length, toRetry);
	}
}

int himax_bus_write_command(uint8_t command, uint8_t toRetry)
{
	return himax_bus_write(command, NULL, 0, toRetry);
}

int himax_i2c_bus_master_write(uint8_t *data, uint32_t length, uint8_t toRetry)
{
	int retry/*, loop_i*/;
	uint8_t buf[length];
	struct i2c_client *client = private_incell_ts->client;
	struct i2c_msg msg[] = {
		{
			.addr = client->addr,
			.flags = 0,
			.len = length,
			.buf = buf,
		}
	};
	mutex_lock(&private_incell_ts->rw_lock);
	memcpy(buf, data, length);

	for (retry = 0; retry < toRetry; retry++) {
		if (i2c_transfer(client->adapter, msg, 1) == 1)
			break;

		msleep(20);
	}

	if (retry == toRetry) {
		E("%s: i2c_write_block retry over %d\n",
		  __func__, toRetry);
		incell_i2c_error_count = toRetry;
		mutex_unlock(&private_incell_ts->rw_lock);
		return -EIO;
	}

	mutex_unlock(&private_incell_ts->rw_lock);
	return 0;
}

int himax_bus_master_write(uint8_t *data, uint32_t length, uint8_t toRetry)
{
	if (spi_dev_detected == true) {
		return himax_spi_bus_master_write(data, length, toRetry);
	} else {
		return himax_i2c_bus_master_write(data, length, toRetry);
	}
}

void himax_incell_int_enable(int enable)
{
	int irqnum = 0;

	if (spi_dev_detected == true) {
		irqnum = private_incell_ts->hx_irq;
	} else {
		irqnum = private_incell_ts->client->irq;
	}

	if (enable == 1 && incell_irq_enable_count == 0) {
		enable_irq(irqnum);
		incell_irq_enable_count++;
		private_incell_ts->irq_enabled = 1;
	} else if (enable == 0 && incell_irq_enable_count == 1) {
		disable_irq_nosync(irqnum);
		incell_irq_enable_count--;
		private_incell_ts->irq_enabled = 0;
	}

	I("incell_irq_enable_count = %d\n", incell_irq_enable_count);
}

#ifdef HX_RST_PIN_FUNC
void himax_incell_rst_gpio_set(int pinnum, uint8_t value)
{
	gpio_direction_output(pinnum, value);
}
#endif

uint8_t himax_incell_int_gpio_read(int pinnum)
{
	return gpio_get_value(pinnum);
}

#if defined(CONFIG_HMX_DB)
static int himax_regulator_configure(struct himax_i2c_platform_data *pdata)
{
	int retval;
	struct i2c_client *client = NULL;
	
	if (spi_dev_detected == true) {
		pdata->vcc_dig = regulator_get(private_incell_ts->dev, "vdd");
	} else {
		client = private_incell_ts->client;
		pdata->vcc_dig = regulator_get(&client->dev, "vdd");
	}

	if (IS_ERR(pdata->vcc_dig)) {
		E("%s: Failed to get regulator vdd\n",
		  __func__);
		retval = PTR_ERR(pdata->vcc_dig);
		return retval;
	}

	if (spi_dev_detected == true) {
		pdata->vcc_ana = regulator_get(private_incell_ts->dev, "avdd");
	} else {
		pdata->vcc_ana = regulator_get(&client->dev, "avdd");
	}

	if (IS_ERR(pdata->vcc_ana)) {
		E("%s: Failed to get regulator avdd\n",
		  __func__);
		retval = PTR_ERR(pdata->vcc_ana);
		regulator_put(pdata->vcc_ana);
		return retval;
	}

	return 0;
};

static int himax_power_on(struct himax_i2c_platform_data *pdata, bool on)
{
	int retval;

	if (on) {
		retval = regulator_enable(pdata->vcc_dig);

		if (retval) {
			E("%s: Failed to enable regulator vdd\n",
			  __func__);
			return retval;
		}

		msleep(100);
		retval = regulator_enable(pdata->vcc_ana);

		if (retval) {
			E("%s: Failed to enable regulator avdd\n",
			  __func__);
			regulator_disable(pdata->vcc_dig);
			return retval;
		}
	} else {
		regulator_disable(pdata->vcc_dig);
		regulator_disable(pdata->vcc_ana);
	}

	return 0;
}

int himax_incell_gpio_power_config(struct himax_i2c_platform_data *pdata)
{
	int error;
	struct i2c_client *client = NULL;

	if (spi_dev_detected == false) {
		client = private_incell_ts->client;
	}
	error = himax_regulator_configure(pdata);

	if (error) {
		E("Failed to intialize hardware\n");
		goto err_regulator_not_on;
	}

#ifdef HX_RST_PIN_FUNC

	if (gpio_is_valid(pdata->gpio_reset)) {
		/* configure touchscreen reset out gpio */
		error = gpio_request(pdata->gpio_reset, "hmx_reset_gpio");

		if (error) {
			E("unable to request gpio [%d]\n",
			  pdata->gpio_reset);
			goto err_regulator_on;
		}

		error = gpio_direction_output(pdata->gpio_reset, 0);

		if (error) {
			E("unable to set direction for gpio [%d]\n",
			  pdata->gpio_reset);
			goto err_gpio_reset_req;
		}
	}

#endif
	error = himax_power_on(pdata, true);

	if (error) {
		E("Failed to power on hardware\n");
		goto err_gpio_reset_req;
	}

	if (gpio_is_valid(pdata->gpio_irq)) {
		/* configure touchscreen irq gpio */
		error = gpio_request(pdata->gpio_irq, "hmx_gpio_irq");

		if (error) {
			E("unable to request gpio [%d]\n",
			  pdata->gpio_irq);
			goto err_power_on;
		}

		error = gpio_direction_input(pdata->gpio_irq);

		if (error) {
			E("unable to set direction for gpio [%d]\n",
			  pdata->gpio_irq);
			goto err_gpio_irq_req;
		}

		if (spi_dev_detected == true) {
			private_incell_ts->hx_irq = gpio_to_irq(pdata->gpio_irq);
		} else {
			client->irq = gpio_to_irq(pdata->gpio_irq);
		}
	} else {
		E("irq gpio not provided\n");
		goto err_power_on;
	}

	msleep(20);
#ifdef HX_RST_PIN_FUNC

	if (gpio_is_valid(pdata->gpio_reset)) {
		error = gpio_direction_output(pdata->gpio_reset, 1);

		if (error) {
			E("unable to set direction for gpio [%d]\n",
			  pdata->gpio_reset);
			goto err_gpio_irq_req;
		}
	}

#endif
	return 0;
err_gpio_irq_req:

	if (gpio_is_valid(pdata->gpio_irq))
		gpio_free(pdata->gpio_irq);

err_power_on:
	himax_power_on(pdata, false);
err_gpio_reset_req:
#ifdef HX_RST_PIN_FUNC

	if (gpio_is_valid(pdata->gpio_reset))
		gpio_free(pdata->gpio_reset);

err_regulator_on:
#endif
err_regulator_not_on:
	return error;
}

#else
int himax_incell_gpio_power_config(struct himax_i2c_platform_data *pdata)
{
	int error = 0;
	struct i2c_client *client = NULL;

	if (spi_dev_detected == false) {
		client = private_incell_ts->client;
	}
	
#ifdef HX_RST_PIN_FUNC
	if (pdata->gpio_reset >= 0) {
		error = gpio_request(pdata->gpio_reset, "himax-reset");

		if (error < 0) {
			E("%s: request reset pin failed\n", __func__);
			return error;
		}

		error = gpio_direction_output(pdata->gpio_reset, 0);

		if (error) {
			E("unable to set direction for gpio [%d]\n",
			  pdata->gpio_reset);
			return error;
		}
	}

#endif

	if (pdata->gpio_3v3_en >= 0) {
		error = gpio_request(pdata->gpio_3v3_en, "himax-3v3_en");

		if (error < 0) {
			E("%s: request 3v3_en pin failed\n", __func__);
			return error;
		}

		gpio_direction_output(pdata->gpio_3v3_en, 1);
		I("3v3_en pin =%d\n", gpio_get_value(pdata->gpio_3v3_en));
	}

	if (gpio_is_valid(pdata->gpio_irq)) {
		/* configure touchscreen irq gpio */
		error = gpio_request(pdata->gpio_irq, "himax_gpio_irq");

		if (error) {
			E("unable to request gpio [%d]\n", pdata->gpio_irq);
			return error;
		}

		error = gpio_direction_input(pdata->gpio_irq);

		if (error) {
			E("unable to set direction for gpio [%d]\n", pdata->gpio_irq);
			return error;
		}

		if (spi_dev_detected == true) {
			private_incell_ts->hx_irq = gpio_to_irq(pdata->gpio_irq);
		} else {
			client->irq = gpio_to_irq(pdata->gpio_irq);
		}
	} else {
		E("irq gpio not provided\n");
		return error;
	}

	msleep(20);
#ifdef HX_RST_PIN_FUNC

	if (pdata->gpio_reset >= 0) {
		error = gpio_direction_output(pdata->gpio_reset, 1);

		if (error) {
			E("unable to set direction for gpio [%d]\n",
			  pdata->gpio_reset);
			return error;
		}
	}

#endif
	return error;
}

#endif

static void himax_ts_isr_func(struct himax_ts_data *ts)
{
	himax_incell_ts_work(ts);
}

irqreturn_t himax_incell_ts_thread(int irq, void *ptr)
{
	himax_ts_isr_func((struct himax_ts_data *)ptr);

	return IRQ_HANDLED;
}

static void himax_incell_ts_work_func(struct work_struct *work)
{
	struct himax_ts_data *ts = container_of(work, struct himax_ts_data, work);
	himax_incell_ts_work(ts);
}

int himax_incell_spi_int_register_trigger(void)
{
	int ret = 0;
	struct himax_ts_data *ts = private_incell_ts;

	if (incell_ic_data->HX_INT_IS_EDGE) {
		I("%s edge triiger falling\n ", __func__);
		ret = request_threaded_irq(ts->hx_irq, NULL, himax_incell_ts_thread, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, HIMAX_incell_common_NAME, ts);
	} else {
		I("%s level trigger low\n ", __func__);
		ret = request_threaded_irq(ts->hx_irq, NULL, himax_incell_ts_thread, IRQF_TRIGGER_LOW | IRQF_ONESHOT, HIMAX_incell_common_NAME, ts);
	}

	return ret;
}

int himax_incell_i2c_int_register_trigger(void)
{
	int ret = 0;
	struct himax_ts_data *ts = private_incell_ts;
	struct i2c_client *client = private_incell_ts->client;

	if (incell_ic_data->HX_INT_IS_EDGE) {
		I("%s edge triiger falling\n ", __func__);
		ret = request_threaded_irq(client->irq, NULL, himax_incell_ts_thread, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, client->name, ts);
	}

	else {
		I("%s level trigger low\n ", __func__);
		ret = request_threaded_irq(client->irq, NULL, himax_incell_ts_thread, IRQF_TRIGGER_LOW | IRQF_ONESHOT, client->name, ts);
	}

	return ret;
}

int himax_incell_int_register_trigger(void)
{
	if (spi_dev_detected == true) {
		return himax_incell_spi_int_register_trigger();
	} else {
		return himax_incell_i2c_int_register_trigger();
	}
}

int himax_incell_int_en_set(void)
{
	int ret = NO_ERR;
	ret = himax_incell_int_register_trigger();
	return ret;
}

int himax_incell_spi_ts_register_interrupt(void)
{
	struct himax_ts_data *ts = private_incell_ts;
	/* struct i2c_client *client = private_incell_ts->client; */
	int ret = 0;
	ts->irq_enabled = 0;

	/* Work functon */
	if (private_incell_ts->hx_irq) {/*INT mode*/
		ts->use_irq = 1;
		ret = himax_incell_int_register_trigger();

		if (ret == 0) {
			ts->irq_enabled = 1;
			incell_irq_enable_count = 1;
			I("%s: irq enabled at qpio: %d\n", __func__, private_incell_ts->hx_irq);
#ifdef HX_SMART_WAKEUP
			irq_set_irq_wake(private_incell_ts->hx_irq , 1);
#endif
		} else {
			ts->use_irq = 0;
			E("%s: request_irq failed\n", __func__);
		}
	} else {
		I("%s: private_incell_ts->hx_irq is empty, use polling mode.\n", __func__);
	}

	if (!ts->use_irq) {/*if use polling mode need to disable HX_ESD_RECOVERY function*/
		ts->himax_wq = create_singlethread_workqueue("himax_touch");
		INIT_WORK(&ts->work, himax_incell_ts_work_func);
		hrtimer_init(&ts->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
		ts->timer.function = himax_incell_ts_timer_func;
		hrtimer_start(&ts->timer, ktime_set(1, 0), HRTIMER_MODE_REL);
		I("%s: polling mode enabled\n", __func__);
	}

	return ret;
}

int himax_incell_i2c_ts_register_interrupt(void)
{
	struct himax_ts_data *ts = private_incell_ts;
	struct i2c_client *client = private_incell_ts->client;
	int ret = 0;
	ts->irq_enabled = 0;

	/* Work functon */
	if (client->irq) {/*INT mode*/
		ts->use_irq = 1;
		ret = himax_incell_int_register_trigger();

		if (ret == 0) {
			ts->irq_enabled = 1;
			incell_irq_enable_count = 1;
			I("%s: irq enabled at qpio: %d\n", __func__, client->irq);
#ifdef HX_SMART_WAKEUP
			irq_set_irq_wake(client->irq, 1);
#endif
		} else {
			ts->use_irq = 0;
			E("%s: request_irq failed\n", __func__);
		}
	} else {
		I("%s: client->irq is empty, use polling mode.\n", __func__);
	}

	if (!ts->use_irq) {/*if use polling mode need to disable HX_ESD_RECOVERY function*/
		ts->himax_wq = create_singlethread_workqueue("himax_touch");
		INIT_WORK(&ts->work, himax_incell_ts_work_func);
		hrtimer_init(&ts->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
		ts->timer.function = himax_incell_ts_timer_func;
		hrtimer_start(&ts->timer, ktime_set(1, 0), HRTIMER_MODE_REL);
		I("%s: polling mode enabled\n", __func__);
	}

	return ret;
}

int himax_incell_ts_register_interrupt(void)
{
	if (spi_dev_detected == true) {
		return himax_incell_spi_ts_register_interrupt();
	} else {
		return himax_incell_i2c_ts_register_interrupt();
	}
}

static int himax_common_suspend(struct device *dev)
{
	struct himax_ts_data *ts = dev_get_drvdata(dev);
	I("%s: enter \n", __func__);
	himax_incell_chip_common_suspend(ts);
	return 0;
}

static int himax_common_resume(struct device *dev)
{
	struct himax_ts_data *ts = dev_get_drvdata(dev);
	I("%s: enter \n", __func__);
	himax_chip_incell_common_resume(ts);
	return 0;
}

#if defined(CONFIG_FB)
int incell_spi_fb_notifier_callback(struct notifier_block *self,
							unsigned long event, void *data)
{
	struct fb_event *evdata = data;
	int *blank;
	struct himax_ts_data *ts =
	    container_of(self, struct himax_ts_data, fb_notif);
	I(" %s\n", __func__);

	if (evdata && evdata->data && event == FB_EVENT_BLANK && ts != NULL &&
	    ts->dev != NULL) {
		blank = evdata->data;

		switch (*blank) {
		case FB_BLANK_UNBLANK:
			himax_common_resume(ts->dev);
			break;

		case FB_BLANK_POWERDOWN:
		case FB_BLANK_HSYNC_SUSPEND:
		case FB_BLANK_VSYNC_SUSPEND:
		case FB_BLANK_NORMAL:
			himax_common_suspend(ts->dev);
			break;
		}
	}

	return 0;
}

int incell_i2c_fb_notifier_callback(struct notifier_block *self,
							unsigned long event, void *data)
{
	struct fb_event *evdata = data;
	int *blank;
	struct himax_ts_data *ts =
	    container_of(self, struct himax_ts_data, fb_notif);
	I(" %s\n", __func__);

	if (evdata && evdata->data && event == FB_EVENT_BLANK && ts &&
	    ts->client) {
		blank = evdata->data;

		switch (*blank) {
		case FB_BLANK_UNBLANK:
			himax_common_resume(&ts->client->dev);
			break;
		case FB_BLANK_POWERDOWN:
		case FB_BLANK_HSYNC_SUSPEND:
		case FB_BLANK_VSYNC_SUSPEND:
		case FB_BLANK_NORMAL:
			himax_common_suspend(&ts->client->dev);
			break;
		}
	}

	return 0;
}

int incell_fb_notifier_callback(struct notifier_block *self,
							unsigned long event, void *data)
{
	if (spi_dev_detected == true) {
		return incell_spi_fb_notifier_callback(self, event, data);
	} else {
		return incell_i2c_fb_notifier_callback(self, event, data);
	}
}
#endif

int himax_chip_incell_spi_common_probe(struct spi_device *spi)
{
	struct himax_ts_data *ts;
	int ret = 0;

	I("Enter %s \n", __func__);
	if (spi->master->flags & SPI_MASTER_HALF_DUPLEX) {
		dev_err(&spi->dev,
				"%s: Full duplex not supported by host\n", __func__);
		return -EIO;
	}


	ts = kzalloc(sizeof(struct himax_ts_data), GFP_KERNEL);
	if (ts == NULL) {
		E("%s: allocate himax_ts_data failed\n", __func__);
		ret = -ENOMEM;
		goto err_alloc_data_failed;
	}

	private_incell_ts = ts;
	spi->bits_per_word = 8;
	spi->mode = SPI_MODE_3;

	ts->spi = spi;
	mutex_init(&(ts->spi_lock));
	mutex_init(&(ts->himax_mutex));
	ts->dev = &spi->dev;
	dev_set_drvdata(&spi->dev, ts);
	spi_set_drvdata(spi, ts);

	ret = himax_chip_common_init();

	err_alloc_data_failed:
		return ret;

}

int himax_chip_incell_spi_common_remove(struct spi_device *spi)
{
	struct himax_ts_data *ts = spi_get_drvdata(spi);

	ts->spi = NULL;
	/* spin_unlock_irq(&ts->spi_lock); */
	spi_set_drvdata(spi, NULL);

	himax_chip_common_deinit();

	return 0;
}

int himax_chip_incell_i2c_common_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret = 0;
	struct himax_ts_data *ts;

	I("%s:Enter \n", __func__);

	/* Check I2C functionality */
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		E("%s: i2c check functionality error\n", __func__);
		ret = -ENODEV;
		goto err_check_functionality_failed;
	}

	ts = kzalloc(sizeof(struct himax_ts_data), GFP_KERNEL);
	if (ts == NULL) {
		E("%s: allocate himax_ts_data failed\n", __func__);
		ret = -ENOMEM;
		goto err_alloc_data_failed;
	}

	i2c_set_clientdata(client, ts);
	ts->client = client;
	ts->dev = &client->dev;
	mutex_init(&ts->rw_lock);
    mutex_init(&ts->himax_mutex);
	private_incell_ts = ts;

	ret = himax_chip_common_init();

	err_alloc_data_failed:
	err_check_functionality_failed:

	return ret;
}

int himax_chip_incell_i2c_common_remove(struct i2c_client *client)
{
	himax_chip_common_deinit();

	return 0;
}

static const struct i2c_device_id himax_common_ts_id[] = {
	{HIMAX_incell_common_NAME, 0 },
	{}
};

static const struct dev_pm_ops himax_common_pm_ops = {
#if (!defined(CONFIG_FB))
	.suspend = himax_common_suspend,
	.resume  = himax_common_resume,
#endif
};

#ifdef CONFIG_OF
static struct of_device_id himax_match_table[] = {
	{.compatible = "himax,hx83102_spi" },
	{.compatible = "himax,hx83102_i2c" },
	{},
};
#else
#define himax_match_table NULL
#endif

static struct spi_driver himax_incell_spi_common_driver = {
	.driver = {
		.name =		HIMAX_incell_common_NAME,
		.owner =	THIS_MODULE,
		.of_match_table = himax_match_table,
	},
	.probe =	himax_chip_incell_spi_common_probe,
	.remove =	himax_chip_incell_spi_common_remove,
};

static struct i2c_driver himax_incell_i2c_common_driver = {
	.id_table	= himax_common_ts_id,
	.probe		= himax_chip_incell_i2c_common_probe,
	.remove		= himax_chip_incell_i2c_common_remove,
	.driver		= {
		.name = HIMAX_incell_common_NAME,
		.owner = THIS_MODULE,
		.of_match_table = himax_match_table,
#ifdef CONFIG_PM
		.pm				= &himax_common_pm_ops,
#endif
	},
};

static int __init himax_incell_common_init(void)
{
	I("Himax incell common touch panel driver init\n");

	I("saved_command_line = %s\n", saved_command_line);

	if ((strstr(saved_command_line, "fih,mdss_dsi_hx83102_truly_hd_plus_video") != NULL)
		|| (strstr(saved_command_line, "fih,mdss_dsi_hx83102_inno_hd_plus_video") != NULL)) {

		if (strstr(saved_command_line, "fih,mdss_dsi_hx83102_inno_hd_plus_video") != NULL) {
			spi_dev_detected = true;
		}

		if (spi_dev_detected == true) {
			spi_register_driver(&himax_incell_spi_common_driver);
		} else {
			i2c_add_driver(&himax_incell_i2c_common_driver);
		}

		return 0;
	} else {
		I("Not match lk himax lcm name\n");
		return -1;
	}
}

static void __exit himax_incell_common_exit(void)
{
	if (spi_dev_detected == true) {
		if (spi) {
			spi_unregister_device(spi);
			spi = NULL;
		}
		spi_unregister_driver(&himax_incell_spi_common_driver);
	} else {
		i2c_del_driver(&himax_incell_i2c_common_driver);
	}
}

module_init(himax_incell_common_init);
module_exit(himax_incell_common_exit);

MODULE_DESCRIPTION("Himax_common driver");
MODULE_LICENSE("GPL");

