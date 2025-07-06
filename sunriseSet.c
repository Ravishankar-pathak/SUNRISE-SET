#include <stdio.h>
#include <math.h>

#define DEG_TO_RAD (M_PI / 180.0)
#define RAD_TO_DEG (180.0 / M_PI)

// Function declarations
double calculateJulianDay(int year, int month, int day);
void calculateSunriseSunset(double latitude, double longitude, int year, int month, int day, double utc_offset, double *sunrise, double *sunset);
void printTime(double time);

int main() {
    double latitude, longitude, utc_offset;
    int year, month, day;
    double sunrise, sunset;

    // Get input from the user
    printf("Enter GPS Coordinates (latitude and longitude):\n");
    printf("Latitude: ");
    scanf("%lf", &latitude);
    printf("Longitude: ");
    scanf("%lf", &longitude);

    printf("Enter UTC offset (e.g., 5.5 for IST): ");
    scanf("%lf", &utc_offset);

    printf("Enter date (YYYY MM DD): ");
    scanf("%d %d %d", &year, &month, &day);

    // Calculate sunrise and sunset
    calculateSunriseSunset(latitude, longitude, year, month, day, utc_offset, &sunrise, &sunset);

    // Subtract 5 hours and 30 minutes (5.5 hours) from the times
    sunrise -= 5.5;
    sunset -= 5.5;

    // Display results in HH:MM format after adjustment
    printf("Sunrise: ");
    printTime(sunrise);
    printf("Sunset: ");
    printTime(sunset);

    return 0;
}

// Function to calculate the Julian Day
double calculateJulianDay(int year, int month, int day) {
    if (month <= 2) {
        year -= 1;
        month += 12;
    }
    int A = year / 100;
    int B = 2 - A + (A / 4);
    return (int)(365.25 * (year + 4716)) + (int)(30.6001 * (month + 1)) + day + B - 1524.5;
}

// Function to calculate sunrise and sunset
void calculateSunriseSunset(double latitude, double longitude, int year, int month, int day, double utc_offset, double *sunrise, double *sunset) {
    double julian_day = calculateJulianDay(year, month, day);
    double julian_century = (julian_day - 2451545.0) / 36525.0;

    // Solar calculations
    double geom_mean_long_sun = fmod(280.46646 + julian_century * (36000.76983 + julian_century * 0.0003032), 360.0);
    double geom_mean_anom_sun = 357.52911 + julian_century * (35999.05029 - 0.0001537 * julian_century);
    double eccent_earth_orbit = 0.016708634 - julian_century * (0.000042037 + 0.0000001267 * julian_century);

    double sun_eq_of_center = sin(DEG_TO_RAD * geom_mean_anom_sun) * (1.914602 - julian_century * (0.004817 + 0.000014 * julian_century)) +
                              sin(DEG_TO_RAD * (2 * geom_mean_anom_sun)) * (0.019993 - 0.000101 * julian_century) +
                              sin(DEG_TO_RAD * (3 * geom_mean_anom_sun)) * 0.000289;

    double sun_true_long = geom_mean_long_sun + sun_eq_of_center;
    double sun_app_long = sun_true_long - 0.00569 - 0.00478 * sin(DEG_TO_RAD * (125.04 - 1934.136 * julian_century));

    double mean_obliq_ecliptic = 23.0 + (26.0 + ((21.448 - julian_century * (46.815 + julian_century * (0.00059 - julian_century * 0.001813)))) / 60.0) / 60.0;
    double obliq_corr = mean_obliq_ecliptic + 0.00256 * cos(DEG_TO_RAD * (125.04 - 1934.136 * julian_century));

    double sun_declination = RAD_TO_DEG * asin(sin(DEG_TO_RAD * obliq_corr) * sin(DEG_TO_RAD * sun_app_long));

    // Equation of time
    double var_y = pow(tan(DEG_TO_RAD * (obliq_corr / 2.0)), 2);
    double eq_of_time = 4.0 * RAD_TO_DEG * (var_y * sin(2.0 * DEG_TO_RAD * geom_mean_long_sun) -
                                            2.0 * eccent_earth_orbit * sin(DEG_TO_RAD * geom_mean_anom_sun) +
                                            4.0 * eccent_earth_orbit * var_y * sin(DEG_TO_RAD * geom_mean_anom_sun) * cos(2.0 * DEG_TO_RAD * geom_mean_long_sun) -
                                            0.5 * var_y * var_y * sin(4.0 * DEG_TO_RAD * geom_mean_long_sun) -
                                            1.25 * pow(eccent_earth_orbit, 2) * sin(2.0 * DEG_TO_RAD * geom_mean_anom_sun));

    // Hour angle
    double ha_sunrise = RAD_TO_DEG * acos(cos(DEG_TO_RAD * 90.833) / (cos(DEG_TO_RAD * latitude) * cos(DEG_TO_RAD * sun_declination)) -
                                          tan(DEG_TO_RAD * latitude) * tan(DEG_TO_RAD * sun_declination));

    // Solar noon and sunrise/sunset times
    double solar_noon = (720 - 4.0 * longitude - eq_of_time) / 1440.0;
    *sunrise = 24.0 * (solar_noon - ha_sunrise / 360.0) + utc_offset;
    *sunset = 24.0 * (solar_noon + ha_sunrise / 360.0) + utc_offset;
}

// Function to print time in HH:MM format
void printTime(double time) {
    int hours = (int)time;
    int minutes = (int)((time - hours) * 60);
    printf("%02d:%02d\n", hours, minutes);
}
