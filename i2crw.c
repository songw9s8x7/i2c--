int gtp_i2c_read(struct i2c_client *client, u8 *buf, int len)
{
	struct goodix_ts_data *ts = i2c_get_clientdata(client);
	struct i2c_msg msgs[2];
	int ret = -EIO;
	int retries = 0;

	GTP_DEBUG_FUNC();

	msgs[0].flags = !I2C_M_RD;
	msgs[0].addr = client->addr;
	msgs[0].len = GTP_ADDR_LENGTH;
	msgs[0].buf = &buf[0];

	msgs[1].flags = I2C_M_RD;
	msgs[1].addr = client->addr;
	msgs[1].len = len - GTP_ADDR_LENGTH;
	msgs[1].buf = &buf[GTP_ADDR_LENGTH];

	while (retries < 5) {
		ret = i2c_transfer(client->adapter, msgs, 2);
		if (ret == 2)
			break;
		retries++;
	}
	if (retries >= 5) {
		GTP_DEBUG("I2C communication timeout, resetting chip...");
		if (init_done)
			gtp_reset_guitar(ts, 10);
		else
			dev_warn(&client->dev,
				"<GTP> gtp_reset_guitar exit init_done=%d:\n",
				init_done);
	}
	return ret;
}


int gtp_i2c_write(struct i2c_client *client, u8 *buf, int len)
{
	struct goodix_ts_data *ts = i2c_get_clientdata(client);
	struct i2c_msg msg;
	int ret = -EIO;
	int retries = 0;

	GTP_DEBUG_FUNC();

	msg.flags = !I2C_M_RD;
	msg.addr = client->addr;
	msg.len = len;
	msg.buf = buf;

	while (retries < 5) {
		ret = i2c_transfer(client->adapter, &msg, 1);
		if (ret == 1)
			break;
		retries++;
	}
	if ((retries >= 5)) {
		GTP_DEBUG("I2C communication timeout, resetting chip...");
		if (init_done)
			gtp_reset_guitar(ts, 10);
		else
			dev_warn(&client->dev,
				"<GTP> gtp_reset_guitar exit init_done=%d:\n",
				init_done);
	}
	return ret;
}

/*有时候我们需要对读取的数据进行保证这样我们再封装一个i2c读函数，这个函数会读两次
i2c传输过来的值，然后当两次都相等的时候才会返回i2c的值*/
int gtp_i2c_read_dbl_check(struct i2c_client *client,
			u16 addr, u8 *rxbuf, int len)
{
	u8 buf[16] = {0};
	u8 confirm_buf[16] = {0};
	u8 retry = 0;

	while (retry++ < 3) {
		memset(buf, 0xAA, 16);
		buf[0] = (u8)(addr >> 8);
		buf[1] = (u8)(addr & 0xFF);
		gtp_i2c_read(client, buf, len + 2);

		memset(confirm_buf, 0xAB, 16);
		confirm_buf[0] = (u8)(addr >> 8);
		confirm_buf[1] = (u8)(addr & 0xFF);
		gtp_i2c_read(client, confirm_buf, len + 2);

		if (!memcmp(buf, confirm_buf, len + 2))
			break;
	}
	if (retry < 3) {
		memcpy(rxbuf, confirm_buf + 2, len);
		return SUCCESS;
	} else {
		dev_err(&client->dev,
			"i2c read 0x%04X, %d bytes, double check failed!",
			addr, len);
		return FAIL;
	}
}



