/*******************************************************************************
 * The Things Network - Sensor Data Example
 *
 * Example of sending a valid LoRaWAN packet with DHT22 temperature and
 * humidity data to The Things Networ using a Feather M0 LoRa.
 *
 * Learn Guide: https://learn.adafruit.com/the-things-network-for-feather
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
#define FILLMEIN 0
#else
#warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
#define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
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
// practice, a key taken from the TTN console can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = { 0x15, 0x61, 0x35, 0x45, 0x76, 0xa0, 0x5e, 0x2b, 0x0d, 0xb9, 0x20, 0xda, 0x6f, 0xf3, 0xe3, 0xe1 };
void os_getDevKey(u1_t* buf) { memcpy_P(buf, APPKEY, 16); }

// payload to send to TTN gateway
//static uint8_t payload[] = "Humidity: 64.12%  Temperature";
static uint8_t payload[] = {0x01, 0x67, 0x00, 0x00, 0x02, 0x68, 0x00, 0x03, 0x02, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00};
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 20;

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

void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

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
        LMIC_setLinkCheckMode(0);//related counter variable: LMIC.adrAckReq
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

                payload[2] = tem1 >> 8;
                payload[3] = tem1 & 0xff;
                //接收端获得原始数据: u2_t temp1 = (payload[2] << 8) + (payload[3] & 0xff);

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
        LMIC_setTxData2(1, payload, sizeof(payload) - 1, 0);
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
    delay(5000);
    while (!Serial);
    Serial.begin(115200);
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

    //LMIC_setClockError(MAX_CLOCK_ERROR * 40 / 100);

    // Disable link-check mode and ADR, because ADR tends to complicate testing.
    LMIC_setLinkCheckMode(0);
    // Set the data rate to Spreading Factor 7.  This is the fastest supported rate for 125 kHz channels, and it
    // minimizes air time and battery power. Set the transmission power to 14 dBi (25 mW).
    LMIC_setDrTxpow(DR_SF7, 14);
    // in the US, with TTN, it saves join time if we start on subband 1 (channels 8-15). This will
    // get overridden after the join by parameters from the network. If working with other
    // networks or in other regions, this will need to be changed.
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

    printf("LoRaWAN Version: 0x% 08x\n", LMIC_LORAWAN_SPEC_VERSION);
    Serial.print(F("LMIC version "));
    printVersion(ARDUINO_LMIC_VERSION);
    Serial.println();

    printf("Error control option 'ControlOption': % d\n", ControlOption);
    printf("CRC intert option 'CRCOption': % d\n", CRCOption);
    printf("Sensor connection option 'SensorOption': % d\n\n", SensorOption);

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop() {
    // we call the LMIC's runloop processor. This will cause things to happen based on events and time. One
    // of the things that will happen is callbacks for transmission complete or received messages. We also
    // use this loop to queue periodic data transmissions.  You can put other things here in the `loop()` routine,
    // but beware that LoRaWAN timing is pretty tight, so if you do more than a few milliseconds of work, you
    // will want to call `os_runloop_once()` every so often, to keep the radio running.
    os_runloop_once();
}