int lidbg_init_input(struct input_dev **input_dev,struct lidbg_input_data *pinput)
{
    	int ret;
	char phys[32];
	lidbg("%s:----------------wsx-------------------\n",__FUNCTION__);
	*input_dev = input_allocate_device();
	if (input_dev == NULL) {
		lidbg("Failed to allocate input device.\n");
		return -ENOMEM;
	}
	(*input_dev)->evbit[0] =
		BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) ; 
	set_bit(BTN_TOOL_FINGER, (*input_dev)->keybit);
	__set_bit(INPUT_PROP_DIRECT, (*input_dev)->propbit);
	input_mt_init_slots(*input_dev, 5);/* in case of "out of memory" */ 
	input_set_abs_params(*input_dev, ABS_MT_POSITION_X,
				0, pinput->abs_x_max, 0, 0);
	input_set_abs_params(*input_dev, ABS_MT_POSITION_Y,
				0, pinput->abs_y_max, 0, 0);
	input_set_abs_params(*input_dev, ABS_MT_WIDTH_MAJOR,
				0, 255, 0, 0);
	input_set_abs_params(*input_dev, ABS_MT_TOUCH_MAJOR,
				0, 255, 0, 0);
	input_set_abs_params(*input_dev, ABS_MT_TRACKING_ID,
				0, 255, 0, 0);
	snprintf(phys, 32, "input/ts");
	(*input_dev)->name = "Goodix-CTP";
	(*input_dev)->phys = phys;
	(*input_dev)->id.bustype = BUS_I2C;
	(*input_dev)->id.vendor = 0xDEAD;
	(*input_dev)->id.product = 0xBEEF;
	(*input_dev)->id.version = 10427;
	lidbg("before register\n");
	ret = input_register_device(*input_dev);

	if (ret) {
		lidbg("---------wsx------input device failed.\n");
		input_free_device(*input_dev);
		input_dev = NULL;
		return ret;
	}
	lidbg("after register\n");
	return 0;

}

