/*******************************************************************************
 * The Things Network - ABP Feather
 *
 * Example of using an Adafruit Feather M0 and DHT22 with a
 * single-channel TheThingsNetwork gateway.
 *
 * This uses ABP (Activation by Personalization), where session keys for
 * communication would be assigned/generated by TTN and hard-coded on the device.
 *
 * Learn Guide: https://learn.adafruit.com/lora-pi
 *
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 * Copyright (c) 2018 Brent Rubell, Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *******************************************************************************/

/***************************************************************************
  This is a library for the CCS811 air

  This sketch reads the sensor

  Designed specifically to work with the Adafruit CCS811 breakout
  ----> http://www.adafruit.com/products/3566

  These sensors use I2C to communicate. The device's I2C address is 0x5A

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

#include <payload_crc.h>

 // include the DHT22 Sensor Library
#include "DHT.h"

// DHT digital pin and sensor type
#define DHTPIN A1
#define DHTTYPE DHT22

#include "Adafruit_CCS811.h"

Adafruit_CCS811 ccs;

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

#define ED 1//ED flag

// LoRaWAN NwkSKey, network session key
static const PROGMEM u1_t NWKSKEY[][16] = {{0x43, 0x57, 0x9e, 0xa9, 0x9c, 0xf9, 0x25, 0x62, 0x04, 0xd4, 0x77, 0x8f, 0x63, 0xa6, 0x1c, 0x0c},
                                           {0x90, 0x89, 0x18, 0x26, 0xc9, 0x31, 0x82, 0x4a, 0xec, 0x95, 0x8b, 0x38, 0x6f, 0x05, 0xc0, 0xce}};

// LoRaWAN AppSKey, application session key
static const u1_t PROGMEM APPSKEY[][16] = {{0xe9, 0xb5, 0x3b, 0x90, 0x85, 0x77, 0xe0, 0xcf, 0x4a, 0x7d, 0xbe, 0x49, 0x0d, 0x40, 0xb0, 0x45},
                                           {0x20, 0x04, 0x23, 0x0c, 0xc7, 0xaa, 0x82, 0xf2, 0xfe, 0x50, 0xb6, 0x93, 0x4b, 0x9e, 0x0c, 0x44}};

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
// The library converts the address to network byte order as needed.
#ifndef COMPILE_REGRESSION_TEST
static const u4_t DEVADDR[] = {0x00deea15,
                               0x0019a01d0};
#else
static const u4_t DEVADDR = 0;
#endif

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in arduino-lmic/project_config/lmic_project_config.h,
// otherwise the linker will complain).
void os_getArtEui(u1_t* buf) { }
void os_getDevEui(u1_t* buf) { }
void os_getDevKey(u1_t* buf) { }

// payload to send to TTN gateway
//static uint8_t payload[] = "Humidity: 64.12%  Temperature";
static uint8_t payload[] = {0x01, 0x67, 0x00, 0x00, 0x02, 0x68, 0x00, 0x03, 0x02, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00};
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

// init. DHT
DHT dht(DHTPIN, DHTTYPE);

void printVersionFragment(char sep, uint8_t v) {
    if (sep != 0) {
        Serial.print(sep);
    }
    Serial.print(unsigned(v));
}

void printVersion(uint32_t v) {
    printVersionFragment(0, uint8_t(v >> 24u));
    printVersionFragment('.', uint8_t(v >> 16u));
    printVersionFragment('.', uint8_t(v >> 8u));
    if (uint8_t(v) != 0) {
        printVersionFragment('.', uint8_t(v));
    }
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

            Serial.print(F("FRMPayload: ")); //�����·�����PHYPayload
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

        switch (ControlOption) {
            case 0: {
                break;
            }
            case 1: {
                if (LMIC.dataLen) {
                    if (DebugOption) {
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
            }
            default: {
                printf("ControlOption is illegal! This program will be shut down!\n");
                return;
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
        switch (SensorOption) {
            case 0: {
                break;
            }
            case 1: {
                // read the temperature from the DHT22
                float temperature = dht.readTemperature();
                Serial.print("Temperature: ");
                Serial.print(temperature);
                Serial.println(" *C");

                // Read temperature as Fahrenheit (isFahrenheit = true)
                float temperaturef = dht.readTemperature(true);

                // read the humidity from the DHT22
                float rHumidity = dht.readHumidity();
                Serial.print("%RH ");
                Serial.println(rHumidity);


                // Check if any reads failed and exit early (to try again).
                if (isnan(temperature) || isnan(temperaturef) || isnan(rHumidity)) {
                    Serial.println(F("Failed to read from DHT sensor!"));
                    return;
                }

                u2_t CO2 = ccs.geteCO2();
                u2_t TVOC = ccs.getTVOC();

                if (ccs.available()) {
                    if (!ccs.readData()) {
                        Serial.print("CO2: ");
                        Serial.print(CO2);
                        Serial.print("ppm, TVOC: ");
                        Serial.println(TVOC);
                    } else {
                        Serial.println("ERROR!");
                        while (1)
                            ;
                    }
                }

                u2_t tem1 = (temperature * 10);
                payload[2] = tem1 >> 8; //Uint16 to Uint8: https://stackoverflow.com/a/1289360/12650926
                payload[3] = tem1 & 0xff;
                //Uint8 to Uint16: u2_t temp1 = (payload[2] << 8) + (payload[3] & 0xff); //Uint8 to Uint16: https://stackoverflow.com/a/59123471/12650926

                payload[6] = rHumidity * 2;

                u2_t CO21 = (CO2 * 10);//TODO: when mutiply 100 times, the value stackoverflow
                payload[9] = CO21 >> 8;
                payload[10] = CO21 & 0xff;

                u2_t TVOC1 = (TVOC * 100);
                payload[13] = TVOC1 >> 8;
                payload[14] = TVOC1 & 0xff;

                break;
            }
            default: {
                printf("SensorOption is illegal! This program will be shut down!\n");
                return;
            }
        }

        // prepare upstream data transmission at the next possible time.
        // transmit on port 1 (the first parameter); you can use any value from 1 to 223 (others are reserved).
        // don't request an ack (the last parameter, if not zero, requests an ack from the network).
        // Remember, acks consume a lot of network resources; don't ask for an ack unless you really need it.
        LMIC_setTxData2(1, payload, sizeof(payload), 0);
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
    delay(5000);
    while (!Serial);
    Serial.begin(115200);
    delay(100);
    Serial.println(F("Starting"));

    switch (SensorOption) {
        case 0: {
            break;
        }
        case 1: {
            dht.begin();

            if (!ccs.begin()) {
                Serial.println("Failed to start sensor! Please check your wiring.");
                while (1);
            }

            // Wait for the sensor to be ready
            while (!ccs.available());

            break;
        }
        default: {
            printf("SensorOption is illegal! This program will be shut down!\n");
            return;
        }
    }

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    //Handle downlink messages loss (https://www.thethingsnetwork.org/forum/t/downlink-messages-not-always-received-on-device/21856/6?u=learner)
    LMIC_setClockError(MAX_CLOCK_ERROR * 40 / 100);

    // Set static session parameters. Instead of dynamically establishing a session
    // by joining the network, precomputed session parameters are be provided.
    // On AVR, these values are stored in flash and only copied to RAM
    // once. Copy them to a temporary buffer here, LMIC_setSession will
    // copy them into a buffer of its own again.
    uint8_t appskey[sizeof(APPSKEY[ED])];
    uint8_t nwkskey[sizeof(NWKSKEY[ED])];
    memcpy_P(appskey, APPSKEY[ED], sizeof(APPSKEY[ED]));
    memcpy_P(nwkskey, NWKSKEY[ED], sizeof(NWKSKEY[ED]));
    LMIC_setSession(0x13, DEVADDR[ED], nwkskey, appskey);

#if defined(CFG_eu868)
    // Set up the channels used by the Things Network, which corresponds
    // to the defaults of most gateways. Without this, only three base
    // channels from the LoRaWAN specification are used, which certainly
    // works, so it is good for debugging, but can overload those
    // frequencies, so be sure to configure the full frequency range of
    // your network here (unless your network autoconfigures them).
    // Setting up channels should happen after LMIC_setSession, as that
    // configures the minimal channel set. The LMIC doesn't let you change
    // the three basic settings, but we show them here.
    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);      // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);      // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);      // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);      // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);      // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);      // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);      // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK, DR_FSK), BAND_MILLI);      // g2-band
    // TTN defines an additional channel at 869.525Mhz using SF9 for class B
    // devices' ping slots. LMIC does not have an easy way to define set this
    // frequency and support for class B is spotty and untested, so this
    // frequency is not configured here.
#elif defined(CFG_us915) || defined(CFG_au915)
    // NA-US and AU channels 0-71 are configured automatically
    // but only one group of 8 should (a subband) should be active
    // TTN recommends the second sub band, 1 in a zero based count.
    // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
    LMIC_selectSubBand(1);
#elif defined(CFG_as923)
    // Set up the channels used in your country. Only two are defined by default,
    // and they cannot be changed.  Use BAND_CENTI to indicate 1% duty cycle.
    // LMIC_setupChannel(0, 923200000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    // LMIC_setupChannel(1, 923400000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);

    // ... extra definitions for channels 2..n here
#elif defined(CFG_kr920)
    // Set up the channels used in your country. Three are defined by default,
    // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
    // BAND_MILLI.
    // LMIC_setupChannel(0, 922100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
    // LMIC_setupChannel(1, 922300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
    // LMIC_setupChannel(2, 922500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);

    // ... extra definitions for channels 3..n here.
#elif defined(CFG_in866)
    // Set up the channels used in your country. Three are defined by default,
    // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
    // BAND_MILLI.
    // LMIC_setupChannel(0, 865062500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
    // LMIC_setupChannel(1, 865402500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
    // LMIC_setupChannel(2, 865985000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);

    // ... extra definitions for channels 3..n here.

#elif defined(CFG_cn490)
    // CN channels 0-95 are configured automatically
    // but only one group of 8 should (a subband) should be active
    LMIC_selectSubBand(10);

#else
# error Region not supported
#endif

    // Disable link check validation
    //https://forum.mcci.io/t/lmic-setlinkcheckmode-questions/96/2?u=wong-hao
    //related counter variable: LMIC.adrAckReq
    LMIC_setLinkCheckMode(0);

    // CN470 uses SF12 for its RX2 window.
    LMIC.dn2Dr = DR_SF12;

    // Set ADR mode (if mobile turn off)
    LMIC_setAdrMode(0);

    // Set data rate and transmit power for uplink
    LMIC_setDrTxpow(DR_SF10, 19);

    printf("LoRaWAN Version: 0x% 08x\n", LMIC_LORAWAN_SPEC_VERSION);
    Serial.print(F("LMIC version "));
    printVersion(ARDUINO_LMIC_VERSION);
    Serial.println();

    printf("Error control option 'ControlOption': % d\n", ControlOption);
    printf("CRC intert option 'CRCOption': % d\n", CRCOption);
    printf("Sensor connection option 'SensorOption': % d\n", SensorOption);
    printf("Debug info option 'DebugOption': % d\n\n", DebugOption);

    // Start job
    do_send(&sendjob);


}

void loop() {
    os_runloop_once();
}