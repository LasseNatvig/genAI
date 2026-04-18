// sensehat_i2c_ids.c
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

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

static int i2c_read_reg_byte(int fd, uint8_t reg_addr, uint8_t *out_byte) {
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

int main(void) {
    const char *i2c_dev = "/dev/i2c-1";

    int fd = i2c_open(i2c_dev, HTS221_ADDR);
    if (fd < 0) return 1;

    // Power on, BDU=1 (block data update), ODR=1 Hz
    if (i2c_write_reg_byte(fd, HTS221_CTRL_REG1, 0x85) < 0) { // Copilot:  needed to configure the sensor
        close(fd); 
        return 1;
    }

   // TODO this is in main program since T0 and T1 are used below
   // Read calibration registers
    uint8_t t0_x8, t1_x8, msb;
    int16_t t0_out, t1_out;
    if (i2c_read_reg_byte(fd, HTS221_T0_DEGC_X8, &t0_x8) < 0 ||
        i2c_read_reg_byte(fd, HTS221_T1_DEGC_X8, &t1_x8) < 0 ||
        i2c_read_reg_byte(fd, HTS221_T1T0_MSB,   &msb)   < 0 ||
        i2c_read_reg_word_le(fd, HTS221_T0_OUT_L, &t0_out) < 0 ||
        i2c_read_reg_word_le(fd, HTS221_T1_OUT_L, &t1_out) < 0) {
        close(fd); return 1;
    }

    // T0 and T1 are 10-bit values in units of degC*8; upper 2 bits in msb
    double T0 = (((msb & 0x03) << 8) | t0_x8) / 8.0;
    double T1 = (((msb & 0x0C) << 6) | t1_x8) / 8.0;
    printf("Calibration: T0=%.3f C (raw=%d), T1=%.3f C (raw=%d)\n",
           T0, t0_out, T1, t1_out);
    // *** end of calibration readout ***    


    printf("Reading temperature every 10 s for 5 minutes...\n\n");

    const int INTERVAL_S      = 10;
    const int TOTAL_SAMPLES   = 30;   // 5 min * 6 samples/min
    const int SAMPLES_PER_MIN = 6;

    double minute_sum   = 0.0;
    int    minute_count = 0;
    int    minute_num   = 1;

    for (int i = 0; i < TOTAL_SAMPLES; i++) {
        sleep(INTERVAL_S);

        int16_t raw;
        if (i2c_read_reg_word_le(fd, HTS221_TEMP_OUT_L, &raw) < 0) {
            fprintf(stderr, "Temperature read failed at sample %d\n", i + 1);
            continue;
        }

        double temp = T0 + (double)(raw - t0_out) * (T1 - T0) / (double)(t1_out - t0_out);
        printf("Sample %2d: %.2f C\n", i + 1, temp);
        fflush(stdout);

        minute_sum += temp;
        minute_count++;

        if ((i + 1) % SAMPLES_PER_MIN == 0) {
            printf("==> Minute %d average: %.2f C\n\n", minute_num,
                   minute_sum / minute_count);
            fflush(stdout);
            minute_sum   = 0.0;
            minute_count = 0;
            minute_num++;
        }
    }

    close(fd);
    return 0;
}
