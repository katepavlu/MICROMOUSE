#include "./batt_soc.h"

coord_t c[21] = 
{
    {6550, 0.00},
    {7220, 0.05},
    {7370, 0.10}, 
    {7410, 0.15}, 
    {7450, 0.20},
    {7490, 0.25},
    {7530, 0.30},
    {7570, 0.35},
    {7590, 0.40},
    {7630, 0.45},
    {7670, 0.50},
    {7710, 0.55},
    {7750, 0.60},
    {7830, 0.65},
    {7910, 0.70},
    {7970, 0.75},
    {8050, 0.80},
    {8160, 0.85},
    {8220, 0.90},
    {8300, 0.95},
    {8400, 1.00}
};

double interp( coord_t* c, double x, int n )
{
    int i;

    for( i = 0; i < n-1; i++ )
    {
        if ( c[i].x <= x && c[i+1].x >= x )
        {
            double diffx = x - c[i].x;
            double diffn = c[i+1].x - c[i].x;

            return c[i].y + ( c[i+1].y - c[i].y ) * diffx / diffn; 
        }
    }

    return 0.0; // Not in Range
}

double batt_volts(int batt_raw)
{
    return batt_raw*(4700.0+2200.0)/2200.0;
}

double batt_soc(double batt_voltage)
{
    return interp( c, batt_voltage, 21 );
}