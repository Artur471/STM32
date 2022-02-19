#include "hts221.h"
#include "main.h"
void hts221_init(unsigned char av_conf, unsigned char reg1, unsigned char reg2, unsigned char reg3){
	unsigned char rx_buffer[2];

	//Humidity and temperature resolution mode
	hts221_write_reg(0x5F, 0x10, 1, av_conf);

	//Sensor register control register 1, control register 2, control register 3
	hts221_write_reg(0x5F, 0x20, 1, reg1);
	hts221_write_reg(0x5F, 0x21, 1, reg2);
	hts221_write_reg(0x5F, 0x22, 1, reg3);

	//Read humidity coefficients
	hts221_read_reg(0x5F, 0x36, 2, rx_buffer);
	hts221.h0_t0_out = (((signed short)rx_buffer[1]) << 8) | (signed short)rx_buffer[0];

	hts221_read_reg(0x5F, 0x3A, 2, rx_buffer);
	hts221.h1_t0_out = (((signed short)rx_buffer[1]) << 8) | (signed short)rx_buffer[0];

	hts221_read_reg(0x5F, 0x30, 2, rx_buffer);
	hts221.h0_rh = rx_buffer[0] >> 1;
	hts221.h1_rh = rx_buffer[1] >> 1;


	//Read temperature coefficients
	hts221_read_reg(0x5F, 0x3C, 2, rx_buffer);
	hts221.t0_out = (((signed short)rx_buffer[1]) << 8) | (signed short)rx_buffer[0];

	hts221_read_reg(0x5F, 0x3E, 2, rx_buffer);
	hts221.t1_out = (((signed short)rx_buffer[1]) << 8) | (signed short)rx_buffer[0];

	hts221_read_reg(0x5F, 0x32, 2, rx_buffer);
	hts221.t0_degC = rx_buffer[0];
	hts221.t1_degC = rx_buffer[1];

	hts221_read_reg(0x5F, 0x35, 1, rx_buffer);
	hts221.t0_degC = (((unsigned short)(rx_buffer[0] & 0x03)) << 8) | (unsigned short)hts221.t0_degC;
	hts221.t1_degC = (((unsigned short)(rx_buffer[0] & 0x0C)) << 6) | (unsigned short)hts221.t1_degC;

	hts221.t0_degC = hts221.t0_degC >> 3;
	hts221.t1_degC = hts221.t1_degC >> 3;


}

void hts221_write_reg(unsigned char slave_adr, unsigned char register_adr, unsigned char n_bytes, unsigned char value){
	I2C1 -> CR2 = (slave_adr << 1);
	I2C1 -> CR2 |= (0 << 10);
	I2C1 -> CR2 |= ((n_bytes + 1) << 16);
	I2C1 -> CR2 |= I2C_CR2_START;

	while(!(I2C1 -> ISR & I2C_ISR_TXIS));
	if(n_bytes > 1){
		I2C1 -> TXDR = (register_adr | 0x80);
	}
	else {
		I2C1 -> TXDR = register_adr;
	}
	for(char i = 0; i < n_bytes; i++){
		while(!(I2C1 -> ISR & I2C_ISR_TXIS));
		I2C1 -> TXDR = value;
	}
	while(!(I2C1 -> ISR & I2C_ISR_TC));
	I2C1 -> CR2 |= I2C_CR2_STOP;
	while(I2C1 -> ISR & I2C_ISR_BUSY);
}

void hts221_read_reg(unsigned char slave_adr, unsigned char register_adr, unsigned char n_bytes, unsigned char *rx_buffer){
	I2C1 -> CR2 = (slave_adr << 1);
	I2C1 -> CR2 |= (0 << 10);
	I2C1 -> CR2 |= (1 << 16);
	I2C1 -> CR2 |= I2C_CR2_START;

	while(!(I2C1 -> ISR & I2C_ISR_TXIS));
	if(n_bytes > 1){
		I2C1 -> TXDR = (register_adr | 0x80);
	}
	else {
		I2C1 -> TXDR = register_adr;
	}

	while(!(I2C1 -> ISR & I2C_ISR_TC));
	I2C1 -> CR2 = (slave_adr << 1);
	I2C1 -> CR2 |= (1 << 10);
	I2C1 -> CR2 |= (n_bytes << 16);
	I2C1 -> CR2 |= I2C_CR2_START;
	for(char i = 0; i < n_bytes; i++){
		while(!(I2C1 -> ISR & I2C_ISR_RXNE));
		*(rx_buffer+i) = I2C1 -> RXDR;
		//rx_buffer[i] = I2C1 -> RXDR;
	}
	while(!(I2C1 -> ISR & I2C_ISR_TC));
	I2C1 -> CR2 |= I2C_CR2_STOP;
	while(I2C1 -> ISR & I2C_ISR_BUSY);
}

signed short hts221_get_humidity(){
	unsigned char rx_buffer[2];

	hts221_read_reg(0x5F, 0x28, 2, rx_buffer);
	hts221.h_out = (((signed short)rx_buffer[1]) << 8) | (signed short)rx_buffer[0];

	//Linear interpolation
	hts221.humidity = ((int)(hts221.h1_rh - hts221.h0_rh)) * ((int)(hts221.h_out - hts221.h0_t0_out) * 10);
	hts221.humidity = hts221.humidity / ((int)(hts221.h1_t0_out - hts221.h0_t0_out)) + ((int)hts221.h0_rh )* 10;

	return hts221.humidity;
}

signed short hts221_get_temperature(){
	unsigned char rx_buffer[2];

	hts221_read_reg(0x5F, 0x2A, 2, rx_buffer);
	hts221.t_out = (((signed short)rx_buffer[1]) << 8) | (signed short)rx_buffer[0];

	//Linear interpolation
	hts221.temperature = ((int)(hts221.t1_degC - hts221.t0_degC)) * ((int)(hts221.t_out - hts221.t0_out) * 10);
	hts221.temperature = hts221.temperature / ((int)(hts221.t1_out - hts221.t0_out)) + ((int)hts221.t0_degC )* 10;

	return hts221.temperature;
}

unsigned char hts221_read_status_reg(){
	unsigned char rx_buffer[1];

	hts221_read_reg(0x5F, 0x27, 1, rx_buffer);

	return rx_buffer[0];
}
