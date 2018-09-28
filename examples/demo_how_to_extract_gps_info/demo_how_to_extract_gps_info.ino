#include "sti_gnss_lib.h"
#include "GNSS.h"

/*
  NOTE: To initialize the UART console port with baud rate 19,200
*/
void setup() {
  // put your setup code here, to run once:
  GnssConf.setNavMode(STGNSS_NAV_MODE_AUTO);
  GnssConf.setUpdateRate(STGNSS_POSITION_UPDATE_RATE_1HZ);
  GnssConf.setDopMaskMode(STGNSS_DOP_MASK_AUTO);
  GnssConf.setPdopMask(30.0);
  GnssConf.setHdopMask(30.0);
  GnssConf.setGdopMask(30.0);
  GnssConf.init(); /* do initialization for GNSS */

  Serial.config(STGNSS_UART_8BITS_WORD_LENGTH, STGNSS_UART_1STOP_BITS, STGNSS_UART_NOPARITY);
  Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:

}

/*
  NOTE: "task_called_after_GNSS_update()" will be called about every second
        (for 1Hz update rate), so we display the info. here.
*/
void task_called_after_GNSS_update(void)
{
  char buf[64];
  uint16_t prnList[STGNSS_GPS_NCHAN];

  // Get info. of setallite
  GnssInfo.update();
  if (GnssInfo.isUpdated() == true) {
    Serial.print("\r\n");

    // display the date from GNSS
    GnssInfo.date.formatString(buf);
    Serial.print(buf);
    Serial.print(", ");
    // display the time from GNSS
    GnssInfo.time.formatString(buf);
    Serial.print(buf);
    Serial.print("\r\n");

    // display how many satellites are in the sky and how many of them are
    // used for position fix
    sprintf(buf, "NumGPSInView = %2d, NumGPSInUse = %2d\r\n",
      GnssInfo.satellites.numGPSInView(prnList),
      GnssInfo.satellites.numGPSInUse(NULL));
    Serial.print(buf);

    // an example to show the info. of 1st satellite in prnList
    sprintf(buf, "Satellite %d: elv = %2d, azi = %3d, CNR = %d\r\n",
    	prnList[0],
      GnssInfo.satellites.elevation(CONSTELLATION_GPS, prnList[0]),
      GnssInfo.satellites.azimuth(CONSTELLATION_GPS, prnList[0]),
      GnssInfo.satellites.CNR(CONSTELLATION_GPS, prnList[0]));
    Serial.print(buf);

    // display the longitude
    GnssInfo.location.longitude_formatString(buf);
    Serial.print(buf);
    Serial.print(", ");
    GnssInfo.location.latitude_formatString(buf);
    Serial.print(buf);
    Serial.print(", ");

    // display the altitude in meters
    sprintf(buf, "height = %.2f(m), ", GnssInfo.altitude.meters());
    Serial.print(buf);

    // display the course in degree
    sprintf(buf, "course = %.2f (deg), ", GnssInfo.course.deg());
    Serial.print(buf);

    // display the speed in KM per hour
    sprintf(buf, "speed = %.2f (km/s)\r\n", GnssInfo.speed.kph());
    Serial.print(buf);
  }
}
