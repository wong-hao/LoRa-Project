/*
 Name:		ttn_otaa.ino
 Created:	2021/11/13 17:11:47
 Author:	229077035
*/

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"

#define DHTPIN A1// Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22// DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

#include <payload_crc.h>

 //
 // For normal use, we require that you edit the sketch to replace FILLMEIN
 // with values assigned by the TTN console. However, for regression tests,
 // we want to be able to compile these scripts. The regression tests define
 // COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
 // working but innocuous value.
 //
#ifdef COMPILE_REGRESSION_TEST
# define FILLMEIN 0
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = { 0x2a, 0x7c, 0x0c, 0x64, 0xec, 0x4c, 0xec, 0x8b };
void os_getArtEui(u1_t* buf) { memcpy_P(buf, APPEUI, 8); }

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = { 0x2a, 0x7c, 0x0c, 0x64, 0xec, 0x4c, 0xec, 0x8b };
void os_getDevEui(u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = { 0x15, 0x61, 0x35, 0x45, 0x76, 0xa0, 0x5e, 0x2b, 0x0d, 0xb9, 0x20, 0xda, 0x6f, 0xf3, 0xe3, 0xe1 };
void os_getDevKey(u1_t* buf) { memcpy_P(buf, APPKEY, 16); }

//static uint8_t mydata[] = "Humidity: 64.12%  Temperature: 34.15°C";
static uint8_t mydata[] = "64.12, 34.15";
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

// Pin mapping Dragino Shiled
// Adapted for Feather M0 per p.10 of [feather]
const lmic_pinmap lmic_pins = {
    .nss = 10,// Connected to pin D10
    .rxtx = LMIC_UNUSED_PIN,// For placeholder only, Do not connected on RFM92/RFM95
    .rst = 9,// Needed on RFM92/RFM95? (probably not)
    .dio = {2, 6, 7},// Specify pin numbers for DIO0, 1, 2
// connected to D2, D6, D7 
};

void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

void dhtTem() {
    // Wait a few seconds between measurements.
    delay(TX_INTERVAL);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));
}

void onEvent(ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev) {
    case EV_SCAN_TIMEOUT:
        Serial.println(F("EV_SCAN_TIMEOUT"));
        break;
    case EV_BEACON_FOUND:
        Serial.println(F("EV_BEACON_FOUND"));
        break;
    case EV_BEACON_MISSED:
        Serial.println(F("EV_BEACON_MISSED"));
        break;
    case EV_BEACON_TRACKED:
        Serial.println(F("EV_BEACON_TRACKED"));
        break;
    case EV_JOINING:
        Serial.println(F("EV_JOINING"));
        break;
    case EV_JOINED:
        Serial.println(F("EV_JOINED"));
        {
            u4_t netid = 0;
            devaddr_t devaddr = 0;
            u1_t nwkKey[16];
            u1_t artKey[16];
            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            Serial.print("netid: ");
            Serial.println(netid, DEC);
            Serial.print("devaddr: ");
            Serial.println(devaddr, HEX);
            Serial.print("AppSKey: ");
            for (size_t i = 0; i < sizeof(artKey); ++i) {
                if (i != 0)
                    Serial.print("-");
                printHex2(artKey[i]);
            }
            Serial.println("");
            Serial.print("NwkSKey: ");
            for (size_t i = 0; i < sizeof(nwkKey); ++i) {
                if (i != 0)
                    Serial.print("-");
                printHex2(nwkKey[i]);
            }
            Serial.println();
        }
        // Disable link check validation (automatically enabled
        // during join, but because slow data rates change max TX
    // size, we don't use it in this example.
        //https://forum.mcci.io/t/lmic-setlinkcheckmode-questions/96/2?u=wong-hao
        LMIC_setLinkCheckMode(0);
        break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_RFU1:
        ||     Serial.println(F("EV_RFU1"));
        ||     break;
        */
    case EV_JOIN_FAILED:
        Serial.println(F("EV_JOIN_FAILED"));
        break;
    case EV_REJOIN_FAILED:
        Serial.println(F("EV_REJOIN_FAILED"));
        break;
    case EV_TXCOMPLETE:
        Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
        if (LMIC.txrxFlags & TXRX_ACK)
            Serial.println(F("Received ack"));
        if (LMIC.dataLen) {
            Serial.print(F("Received "));
            Serial.print(LMIC.dataLen);
            Serial.print(F(" bytes of payload: "));

            Serial.print(F("FRMPayload: ")); //网关下发的是PHYPayload
            for (int loopcount = 0; loopcount < LMIC.dataLen; loopcount++) {//https://www.thethingsnetwork.org/forum/t/downlink-to-node-with-lmic/5127/12?u=learner
                printf("%02X", LMIC.frame[LMIC.dataBeg + loopcount]);
            }
            Serial.println();
        }
        // Schedule next transmission
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
        break;
    case EV_LOST_TSYNC:
        Serial.println(F("EV_LOST_TSYNC"));
        break;
    case EV_RESET:
        Serial.println(F("EV_RESET"));
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        Serial.println(F("EV_RXCOMPLETE"));
        break;
    case EV_LINK_DEAD:
        Serial.println(F("EV_LINK_DEAD"));
        break;
    case EV_LINK_ALIVE:
        Serial.println(F("EV_LINK_ALIVE"));
        break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_SCAN_FOUND:
        ||    Serial.println(F("EV_SCAN_FOUND"));
        ||    break;
        */
    case EV_TXSTART:
        Serial.println(F("EV_TXSTART"));

        printf("Error control option 'ControlOption': % d\n", ControlOption);
        printf("CRC intert option 'CRCOption': % d\n", CRCOption);

        if (ControlOption) {
            if (LMIC.dataLen) {

                u1_t sf = getSf(LMIC.rps) + 6;// 1 == SF7
                u1_t bw = getBw(LMIC.rps);
                u1_t cr = getCr(LMIC.rps);
                u1_t pw = LMIC.adrTxPow;
                u2_t fcntUp = (u2_t) LMIC.seqnoUp - 1;
                printf("%" LMIC_PRId_ostime_t ": TXMODE, freq=%" PRIu32 ", len=%d, SF=%d, PW=%d, BW=%d, CR=4/%d, FCNT=%d, IH=%d\n",
                       os_getTime(), LMIC.freq, LMIC.dataLen, sf,
                       pw,
                       bw == BW125 ? 125 : (bw == BW250 ? 250 : 500),
                       cr == CR_4_5 ? 5 : (cr == CR_4_6 ? 6 : (cr == CR_4_7 ? 7 : 8)),
                       fcntUp,
                       getIh(LMIC.rps));

                printf("upRepeat now : % d\n", LMIC.upRepeat);

                Serial.print(F("Actual Sent "));
                Serial.print(LMIC.pendTxLen, DEC);
                Serial.print(F(" bytes of FRMPayload:"));
                for (int loopcount = 0; loopcount < LMIC.pendTxLen; loopcount++) {
                    printf("%02X", LMIC.pendTxData[LMIC.dataBeg + loopcount]);
                }
                Serial.println();

                Serial.print(F("Actual Sent "));
                Serial.print(LMIC.dataLen, DEC);
                Serial.print(F(" bytes of PHYPayload: "));
                for (int loopcount = 0; loopcount < LMIC.dataLen; loopcount++) {
                    printf("%02X", LMIC.frame[LMIC.dataBeg + loopcount]);
                }
                Serial.println();

                printf("Actual Payload MIC Hex: ", LMIC.dataLen);
                for (int loopcount = LMIC.dataLen - 4; loopcount < LMIC.dataLen; loopcount++) {
                    printf("%02X", LMIC.frame[LMIC.dataBeg + loopcount]);
                }
                Serial.println();

                u2_t payload_crc16_calc;
                payload_crc16_calc = sx1302_lora_payload_crc(LMIC.frame, LMIC.dataLen);
                printf("Actual Payload CRC Hex (0x%04X), Payload CRC DEC (%u)\n", payload_crc16_calc, payload_crc16_calc);

                Serial.println();

            }
        }

        break;
    case EV_TXCANCELED:
        Serial.println(F("EV_TXCANCELED"));
        break;
    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;
    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        break;

    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned)ev);
        break;
    }
}

void do_send(osjob_t* j) {
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else {
        dhtTem();

        // Prepare upstream data transmission at the next possible time.
        // confirmed / confirmation by the server will be requested
        LMIC_setTxData2(1, mydata, sizeof(mydata) - 1, 0);
        Serial.println(F("Packet queued"));
    }

    // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
    Serial.begin(115200);
    Serial.println(F("Starting"));

#ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
#endif

    Serial.println(F("DHTxx test!"));

    dht.begin();

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

#if defined(CFG_cn490)
    // CN channels 0-95 are configured automatically
    // but only one group of 8 should (a subband) should be active
    //TODO: LMIC_selectSubBand沿用的us915且不会改，故只能设置0-7八组信道
    LMIC_selectSubBand(7);

#else
# error Region not supported
#endif

    // Set ADR mode (if mobile turn off)
    LMIC_setAdrMode(0);

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}

