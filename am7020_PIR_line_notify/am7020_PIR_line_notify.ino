#include "config.h"
#include <HttpClient.h>
#include <TinyGsmClient.h>

#ifdef DEBUG_DUMP_AT_COMMAND
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger, AM7020_RESET);

#else
TinyGsm modem(SerialAT, AM7020_RESET);

#endif

TinyGsmClient tcpClient(modem);
HttpClient    httpClient = HttpClient(tcpClient, HTTP_SERVER, HTTP_PORT);

void nbConnect(void);

void setup()
{
    SerialMon.begin(MONITOR_BAUDRATE);
    SerialAT.begin(AM7020_BAUDRATE);

    nbConnect();
}

void loop()
{
    char   buff[100];
    int    state_code;
    String body;

    if (!modem.isNetworkConnected()) {
        nbConnect();
    }
    if (digitalRead(4)) {
        SerialMon.println(F("HTTP Get..."));
        sprintf(buff, "/%s", HTTP_API);
        httpClient.get(buff);
        state_code = httpClient.responseStatusCode();
        body       = httpClient.responseBody();

        SerialMon.print(F("GET state code = "));
        SerialMon.println(state_code);
        SerialMon.print(F("body = "));
        SerialMon.println(body);

        while (digitalRead(4))
            ;
    }
}

void nbConnect(void)
{
    SerialMon.println(F("Initializing modem..."));
    while (!modem.init() || !modem.nbiotConnect(APN, BAND)) {
        SerialMon.print(F("."));
    }

    SerialMon.print(F("Waiting for network..."));
    while (!modem.waitForNetwork()) {
        SerialMon.print(F("."));
    }
    SerialMon.println(F(" success"));
}
