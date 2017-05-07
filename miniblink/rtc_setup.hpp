

void rtc_setup();
extern volatile uint32_t rtc_counter;
int rtc_get_hms(int &h, int &m, int &s);
void rtc_get_dmy(int &d, int &m, int &y);
void rtc_set_ts(int Y, int M, int D, int h, int m, int s);
