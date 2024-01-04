#ifndef PREFSMANAGER
#define PREFSMANAGER

#include <WString.h>

void wipeWiFiData();
String getPrefsSsidPasswd(String ssidPasswd);
void saveWiFiDataInStorage(String ssid, String password);
bool dontHaveWiFiDataInPrefs();

#endif
