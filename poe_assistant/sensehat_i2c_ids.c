// sensehat_i2c_ids.c
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>  // for I2C 
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

// HTS221 temperature and humidity sensor  
#define HTS221_ADDR          0x5F
#define HTS221_CTRL_REG1     0x20  // PD | BDU | ODR
#define HTS221_TEMP_OUT_L    0x2A
#define HTS221_TEMP_OUT_H    0x2B
#define HTS221_T0_DEGC_X8    0x32  
#define HTS221_T1_DEGC_X8    0x33
#define HTS221_T1T0_MSB      0x35
#define HTS221_T0_OUT_L      0x3C
#define HTS221_T0_OUT_H      0x3D
#define HTS221_T1_OUT_L      0x3E
#define HTS221_T1_OUT_H      0x3F
#define LPS22HB_ADDR         0x5C
#define LPS22HB_WHO_AM_I     0xB1
#define LPS25H_WHO_AM_I      0xBD

static int i2c_open(const char *dev, uint8_t slave_addr) { // opens the I2C device
    int fd = open(dev, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "open(%s) failed: %s\n", dev, strerror(errno));
        return -1;
    }
    if (ioctl(fd, I2C_SLAVE, slave_addr) < 0) {
        fprintf(stderr, "ioctl(I2C_SLAVE, 0x%02X) failed: %s\n",
                slave_addr, strerror(errno));
        close(fd);
        return -1;
    }
    return fd;
}

static int i2c_read_reg_byte(int fd, uint8_t reg_addr, uint8_t *out_byte) { // XXXXXX COMMENT see cline and read more TODO SPØR NOEN 
    if (write(fd, &reg_addr, 1) != 1) {
        fprintf(stderr, "write(reg=0x%02X) failed: %s\n", reg_addr, strerror(errno));
        return -1;
    }
    uint8_t val = 0;
    if (read(fd, &val, 1) != 1) {
        fprintf(stderr, "read() failed: %s\n", strerror(errno));
        return -1;
    }
    *out_byte = val;
    return 0;
}

static int i2c_write_reg_byte(int fd, uint8_t reg_addr, uint8_t value) {
    uint8_t buf[2] = { reg_addr, value };
    if (write(fd, buf, 2) != 2) {
        fprintf(stderr, "write(reg=0x%02X) failed: %s\n", reg_addr, strerror(errno));
        return -1;
    }
    return 0;
}

static int i2c_read_reg_word_le(int fd, uint8_t reg_l, int16_t *out) { // reads a 16-bit little-endian register pair
    uint8_t lo, hi;
    if (i2c_read_reg_byte(fd, reg_l,     &lo) < 0) return -1;
    if (i2c_read_reg_byte(fd, reg_l + 1, &hi) < 0) return -1;
    *out = (int16_t)((uint16_t)hi << 8 | lo);
    return 0;
}

/*** Pressure Sensor Functions (LPS22HB and LPS25H) ***/
static int lps_read_id(int fd) {
    uint8_t id;
    if (i2c_read_reg_byte(fd, 0x0F, &id) != 0) {
        return -1;
    }
    printf("DEBUG: Pressure sensor ID register=0x%02X\n", id);
    return id;
}

/* sensor_id: LPS22HB_WHO_AM_I or LPS25H_WHO_AM_I */
static int lps_init(int fd, int sensor_id) {
    uint8_t ctrl_val;
    if (sensor_id == LPS25H_WHO_AM_I) {
        // LPS25H: PD=1 (bit7, activates sensor), ODR=1Hz (bits[6:4]=001), BDU=1 (bit2)
        ctrl_val = (1 << 7) | (1 << 4) | (1 << 2);
    } else {
        // LPS22HB: ODR=1Hz (bits[6:4]=001), BDU=1 (bit1); no PD bit needed
        ctrl_val = (1 << 4) | (1 << 1);
    }
    printf("DEBUG: Init pressure sensor (ID=0x%02X) with ctrl=0x%02X\n", sensor_id, ctrl_val);
    return i2c_write_reg_byte(fd, 0x20, ctrl_val);
}

static int lps22hb_read_pressure(int fd, double *pressure_hpa) {
    // Check if pressure data is ready (status register 0x27, bit 0 = P_DA)
    uint8_t stat;
    if (i2c_read_reg_byte(fd, 0x27, &stat) != 0) {
        printf("DEBUG: Failed to read status\n");
        return -1;
    }
    printf("DEBUG: LPS22HB status=0x%02X\n", stat);

    if (!(stat & (1 << 0))) {
        printf("DEBUG: Pressure data not ready\n");
        return -1;
    }

    // Read 24-bit pressure value (little-endian)
    uint8_t p_xl, p_l, p_h;
    if (i2c_read_reg_byte(fd, 0x28, &p_xl) != 0) return -1;
    if (i2c_read_reg_byte(fd, 0x29, &p_l) != 0) return -1;
    if (i2c_read_reg_byte(fd, 0x2A, &p_h) != 0) return -1;

    int32_t raw = (int32_t)((uint32_t)p_h << 16 | (uint32_t)p_l << 8 | p_xl);
    *pressure_hpa = (double)raw / 4096.0;
    printf("DEBUG: Pressure raw=0x%06X (%.2f hPa)\n", (uint32_t)raw, *pressure_hpa);
    return 0;
}

int main(void) {
    const char *i2c_dev = "/dev/i2c-1";  // get pointer to the I2C device file
    int fd = i2c_open(i2c_dev, HTS221_ADDR); // open the I2C device
    if (fd < 0) return 1;

    // Configure the sensor by writing to its control register.
    // Power on, BDU=1 (block data update), Output Data Rate ODR = 1 Hz (Ref: mrT-HTS221)
    if (i2c_write_reg_byte(fd, HTS221_CTRL_REG1, 0x85) < 0) { 
        close(fd); 
        return 1;
    }

   // ***************************************************************************************** 
   // TODO, maybe move to function?  This is now in main program since T0 and T1 are used below
   // Reference: HTS221 Datasheet from ST.  T0 and T1 are 10-bit values in units of degC*8; upper 2 bits in msb
   // Reads calibration data from the sensor's factory-programmed registers, which contain calibration values (T0 and T1)
    uint8_t t0_x8, t1_x8, msb;  // T0 in degrees multiplied with 8 (e.g. 400 gives 400/8 = 50 degrees)
    int16_t t0_out, t1_out;
    if (i2c_read_reg_byte(fd, HTS221_T0_DEGC_X8, &t0_x8) < 0 ||  // read lower 8 bits of T0
        i2c_read_reg_byte(fd, HTS221_T1_DEGC_X8, &t1_x8) < 0 ||  // read lower 8 bits of T1
        i2c_read_reg_byte(fd, HTS221_T1T0_MSB,   &msb)   < 0 ||  // read most significant bits (MSB) for both T0 and T1
        // msb contains the most significant bits (MSB) for both T0 and T1
        i2c_read_reg_word_le(fd, HTS221_T0_OUT_L, &t0_out) < 0 || // the raw sensor output (ADC value) at that same calibration value T0, and T1 (next line)
        i2c_read_reg_word_le(fd, HTS221_T1_OUT_L, &t1_out) < 0) { 
        close(fd); return 1;
    }
   
    double T0 = (((msb & 0x03) << 8) | t0_x8) / 8.0;
    double T1 = (((msb & 0x0C) << 6) | t1_x8) / 8.0;
    printf("Calibration: T0=%.3f C (raw=%d), T1=%.3f C (raw=%d)\n", T0, t0_out, T1, t1_out);
    // *** end of calibration readout *********************************************************     

    // Try to detect and initialize pressure sensor (LPS22HB or LPS25H)
    int lps_ok = 0;
    int lps_sensor_id = 0;
    printf("DEBUG: Testing pressure sensor at 0x%02X...\n", LPS22HB_ADDR);
    if (ioctl(fd, I2C_SLAVE, LPS22HB_ADDR) >= 0) {
        lps_sensor_id = lps_read_id(fd);
        if (lps_sensor_id == LPS22HB_WHO_AM_I || lps_sensor_id == LPS25H_WHO_AM_I) {
            const char *name = (lps_sensor_id == LPS25H_WHO_AM_I) ? "LPS25H" : "LPS22HB";
            printf("SUCCESS: %s sensor detected (ID=0x%02X)\n", name, lps_sensor_id);
            lps_ok = 1;
        } else {
            printf("DEBUG: Device at 0x%02X has unknown ID=0x%02X\n",
                   LPS22HB_ADDR, lps_sensor_id);
        }
    } else {
        printf("DEBUG: No sensor detected at 0x%02X\n", LPS22HB_ADDR);
    }

    if (lps_ok) {
        printf("DEBUG: Initializing pressure sensor...\n");
        if (lps_init(fd, lps_sensor_id) != 0) {
            printf("WARNING: Pressure sensor initialization failed\n");
            lps_ok = 0;
        }
        // Switch back to temperature sensor after init
        ioctl(fd, I2C_SLAVE, HTS221_ADDR);
    } else {
        // Detection failed; ensure fd points back to temperature sensor
        ioctl(fd, I2C_SLAVE, HTS221_ADDR);
    }

    printf("Reading temperature%s (5 x 1 s average) for 5 minutes...\n\n",
           lps_ok ? " and pressure" : "");

    const int INNER_SAMPLES   = 5;    // 1-s reads averaged into one report
    const int TOTAL_SAMPLES   = 60;   // 5 min * 12 reports/min
    const int SAMPLES_PER_MIN = 12;

    double minute_temp_sum  = 0.0;
    double minute_press_sum = 0.0;
    int    minute_count = 0;
    int    minute_num   = 1;
    int    total_count = 0;

    for (int i = 0; i < TOTAL_SAMPLES; i++) {
        // Collect INNER_SAMPLES temperature readings, one per second
        double inner_temp_sum = 0.0;
        int    inner_valid    = 0;
        for (int j = 0; j < INNER_SAMPLES; j++) {
            sleep(1);
            int16_t raw;
            if (i2c_read_reg_word_le(fd, HTS221_TEMP_OUT_L, &raw) < 0) {
                fprintf(stderr, "Temperature read failed (sample %d, sub-read %d)\n", i + 1, j + 1);
                continue;
            }
            inner_temp_sum += T0 + (double)(raw - t0_out) * (T1 - T0) / (double)(t1_out - t0_out);
            inner_valid++;
        }
        if (inner_valid == 0) continue;
        double temp = inner_temp_sum / inner_valid;

        // Read pressure if available
        double pressure = 0.0;
        if (lps_ok) {
            // Switch to pressure sensor
            ioctl(fd, I2C_SLAVE, LPS22HB_ADDR);
            if (lps22hb_read_pressure(fd, &pressure) != 0) {
                pressure = 0.0;
            }
            // Switch back to temperature sensor
            ioctl(fd, I2C_SLAVE, HTS221_ADDR);
        }

        if (lps_ok) {
            printf("Sample %2d: %.2f C  P=%.2f hPa\n", i + 1, temp, pressure);
        } else {
            printf("Sample %2d: %.2f C\n", i + 1, temp);
        }
        fflush(stdout);

        minute_temp_sum += temp;
        minute_press_sum += pressure;
        minute_count++;
        total_count++;

        if ((i + 1) % SAMPLES_PER_MIN == 0) {
            if (lps_ok) {
                printf("==> Minute %d average: %.2f C, %.2f hPa\n\n", minute_num,
                       minute_temp_sum / minute_count, minute_press_sum / minute_count);
            } else {
                printf("==> Minute %d average: %.2f C\n\n", minute_num,
                       minute_temp_sum / minute_count);
            }
            fflush(stdout);
            minute_temp_sum  = 0.0;
            minute_press_sum = 0.0;
            minute_count = 0;
            minute_num++;
        }
    }

    close(fd);
    return 0;
}
