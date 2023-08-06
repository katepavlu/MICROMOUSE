/**
 * Battery State Of Charge Estimation
 * implemented via lookup table with linear interpolation
 * shamelessly stolen from StackOverflow and RCWorld
*/
#ifndef BATT_SOC
#define BATT_SOC

// coordinate struct for the lookup table
typedef struct { double x; double y; } coord_t;

// lookup table
extern coord_t c[21];

// interpolation function
double interp( coord_t* c, double x, int n );

// convert adc voltage to battery voltage (voltage divider)
double batt_volts(int batt_raw);

// convert battery voltage to state of charge (0.0-1.0)
double batt_soc(double batt_voltage);
#endif