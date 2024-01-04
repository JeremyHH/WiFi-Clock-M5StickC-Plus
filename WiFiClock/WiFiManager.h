#ifndef WIFIMANAGER
#define WIFIMANAGER

#include "WiFiType.h"
#include <M5Unified.h>
#include <esp_wifi.h>
#include <WString.h>

void initWiFi();
void initWiFiSmartConfig();
bool waitingWiFiConnection(wl_status_t status, int count);
bool connectionFailed(wl_status_t status);
String getSsidPasswd(String ssidPasswd);
bool isWiFiConnected();
void setWiFiMaxPowerSave();
void disconnectWiFi();

#endif
