#ifndef __DEIVER_SWITCH_H
#define __DEIVER_SWITCH_H

#define OLED_ON oled_on()
#define OLED_OFF oled_off()
#define SD_ON SD_on()
#define SD_OFF SD_off()
#define WIFI_ON WIFI_on()
#define WIFI_OFF WIFI_off()
#define SENSORS_ON Sensors_on()
#define SENSORS_OFF Sensors_off()

void oled_on(void );
void oled_off(void );
void SD_on(void );
void SD_off(void );
void WIFI_on();
void WIFI_off();
void Sensors_on();
void Sensors_off();

#endif
