
void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */

  analogPWMPeriod(20, 2500); // 400KHz => one cycle is 2,500ns
  analogWrite(20, 51);	// 255 * 20% = 51
}

void loop() {
  // put your main code here, to run repeatedly:

}
