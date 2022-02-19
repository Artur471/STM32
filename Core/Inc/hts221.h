#ifndef INC_HTS221_H_
#define INC_HTS221_H_

//HTS221 slave address
#define HTS221_SLAVE_ADR        0x5F

typedef struct HTS221 {
	//HTS221 registers
	unsigned char WHO_AM_I;
	unsigned char AV_CONF;
	unsigned char CTRL_REG1;
	unsigned char CTRL_REG2;
	unsigned char CTRL_REG3;
 	unsigned char STATUS_REG;

	//HTS221 coefficients
	signed short h0_t0_out;
	signed short h1_t0_out;
	unsigned char h0_rh;
	unsigned char h1_rh;
	signed short h_out;

	signed short t0_out;
	signed short t1_out;
	unsigned short t0_degC;
	unsigned short t1_degC;
	signed short t_out;

	int humidity;
	int temperature;
} HTS221_t;

HTS221_t hts221;

// HTS221 function prototype
void hts221_write_reg(unsigned char slave_adr, unsigned char register_adr, unsigned char n_bytes, unsigned char value);
void hts221_read_reg(unsigned char slave_adr, unsigned char register_adr, unsigned char n_bytes, unsigned char *rx_buffer);
void hts221_init(unsigned char av_conf, unsigned char reg1, unsigned char reg2, unsigned char reg3);
signed short hts221_get_humidity();
signed short HTS221_get_humidity();
signed short hts221_get_temperature();
unsigned char hts221_read_status_reg();

#endif /* INC_HTS221_H_ */
