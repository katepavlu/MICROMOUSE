// soc estimation - lookup table with interpolation
typedef struct { double x; double y; } coord_t;

extern coord_t c[21];

double interp( coord_t* c, double x, int n );

double batt_volts(int batt_raw);

double batt_soc(double batt_voltage);