/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2019 Semtech

Description:
    Configure Lora concentrator and forward packets to a server
    Use GPS for packet timestamping.
    Send a becon at a regular interval without server intervention

License: Revised BSD License, see LICENSE.TXT file include in the project
*/


/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

/* fix an issue between POSIX and C99 */
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <stdint.h>         /* C99 types */
#include <stdbool.h>        /* bool type */
#include <stdio.h>          /* printf, fprintf, snprintf, fopen, fputs */
#include <inttypes.h>       /* PRIx64, PRIu64... */

#include <string.h>         /* memset */
#include <signal.h>         /* sigaction */
#include <time.h>           /* time, clock_gettime, strftime, gmtime */
#include <sys/time.h>       /* timeval */
#include <unistd.h>         /* getopt, access */
#include <stdlib.h>         /* atoi, exit */
#include <errno.h>          /* error messages */
#include <math.h>           /* modf */

#include <sys/socket.h>     /* socket specific definitions */
#include <netinet/in.h>     /* INET constants and stuff */
#include <arpa/inet.h>      /* IP address conversion stuff */
#include <netdb.h>          /* gai_strerror */

#include <pthread.h>

#include "trace.h" //.. / packet_forwarder / inc / trace.h
#include "jitqueue.h" //.. / packet_forwarder / src / jitqueue.c
#include "parson.h" //.. / libtools / src / parson.c
#include "base64.h" //.. / libtools / src / base64.c
#include "loragw_hal.h" //.. / libloragw / src / loragw_hal.c
#include "loragw_aux.h" //.. / libloragw / src / loragw_aux.c
#include "loragw_reg.h" //.. / libloragw / src / loragw_reg.c
#include "loragw_gps.h" //.. / libloragw / src / loragw_gps.c

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof((a)[0]))
#define STRINGIFY(x)    #x
#define STR(x)          STRINGIFY(x)

#define RAND_RANGE(min, max) (rand() % (max + 1 - min) + min)

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ---------------------------------------------------- */

#ifndef VERSION_STRING
#define VERSION_STRING "undefined"
#endif

#define JSON_CONF_DEFAULT   "global_conf.json"

#define DEFAULT_SERVER      127.0.0.1   /* hostname also supported */
#define DEFAULT_PORT_UP     1780
#define DEFAULT_PORT_DW     1782
#define DEFAULT_KEEPALIVE   5           /* default time interval for downstream keep-alive packet */
#define DEFAULT_STAT        30          /* default time interval for statistics */
#define PUSH_TIMEOUT_MS     100
#define PULL_TIMEOUT_MS     200
#define GPS_REF_MAX_AGE     30          /* maximum admitted delay in seconds of GPS loss before considering latest GPS sync unusable */
#define FETCH_SLEEP_MS      10          /* nb of ms waited when a fetch return no packets */
#define BEACON_POLL_MS      50          /* time in ms between polling of beacon TX status */

#define PROTOCOL_VERSION    2           /* v1.6 */
#define PROTOCOL_JSON_RXPK_FRAME_FORMAT 1

#define XERR_INIT_AVG       16          /* nb of measurements the XTAL correction is averaged on as initial value */
#define XERR_FILT_COEF      256         /* coefficient for low-pass XTAL error tracking */

//PROTOCOL packet identifier ��ʶ��
#define PKT_PUSH_DATA   0 //PUSH_DATA packet
#define PKT_PUSH_ACK    1 //PUSH_ACK packet
#define PKT_PULL_DATA   2 //PULL_DATA packet 
#define PKT_PULL_RESP   3 //PULL_RESP packet
#define PKT_PULL_ACK    4 //PULL_ACK packet
#define PKT_TX_ACK      5 //TX_ACK packet

#define NB_PKT_MAX      255 /* max number of packets per fetch/send cycle */

#define MIN_LORA_PREAMB 6 /* minimum Lora preamble length for this application */
#define STD_LORA_PREAMB 8
#define MIN_FSK_PREAMB  3 /* minimum FSK preamble length for this application */
#define STD_FSK_PREAMB  5

#define STATUS_SIZE     200
#define TX_BUFF_SIZE    ((540 * NB_PKT_MAX) + 30 + STATUS_SIZE)
#define ACK_BUFF_SIZE   64

#define UNIX_GPS_EPOCH_OFFSET 315964800 /* Number of seconds ellapsed between 01.Jan.1970 00:00:00
                                                                          and 06.Jan.1980 00:00:00 */ //UTC��GPSʱ��Ĳ�ֵ

#define UNIX_GPS_EPOCH_OFFSET_MILLISECOND 315964800000 //Transfer NTP time to GPS timestampe without GPS

                                                                          //'Beaconing parameters' in 'global.config'�����global����û�в�����Ĭ��ʹ����Щ
#define DEFAULT_BEACON_FREQ_HZ      869525000
#define DEFAULT_BEACON_FREQ_NB      1
#define DEFAULT_BEACON_FREQ_STEP    0
#define DEFAULT_BEACON_DATARATE     9
#define DEFAULT_BEACON_BW_HZ        125000
#define DEFAULT_BEACON_POWER        14
#define DEFAULT_BEACON_INFODESC     0

/* -------------------------------------------------------------------------- */
/* --- PRIVATE TYPES -------------------------------------------------------- */

/* spectral scan */
typedef struct spectral_scan_s {
    bool enable;            /* enable spectral scan thread */
    uint32_t freq_hz_start; /* first channel frequency, in Hz */
    uint8_t nb_chan;        /* number of channels to scan (200kHz between each channel) */
    uint16_t nb_scan;       /* number of scan points for each frequency scan */
    uint32_t pace_s;        /* number of seconds between 2 scans in the thread */
} spectral_scan_t;

/* -------------------------------------------------------------------------- */
/* --- PRIVATE VARIABLES (GLOBAL) ------------------------------------------- */

/* signal handling variables */
volatile bool exit_sig = false; /* 1 -> application terminates cleanly (shut down hardware, close open files, etc) */
volatile bool quit_sig = false; /* 1 -> application terminates without shutting down the hardware */

/* packets filtering configuration variables */ //�����ˣ�parse_gateway_configuration�����
static bool fwd_valid_pkt = true; /* packets with PAYLOAD CRC OK are forwarded */
static bool fwd_error_pkt = false; /* packets with PAYLOAD CRC ERROR are NOT forwarded */
static bool fwd_nocrc_pkt = false; /* packets with NO PAYLOAD CRC are NOT forwarded */

/* network configuration variables */ //�������ã�parse_gateway_configuration���
static uint64_t lgwm = 0; /* Lora gateway MAC address */
static char serv_addr[64] = STR(DEFAULT_SERVER); /* address of the server (host name or IPv4/IPv6) */
static char serv_port_up[8] = STR(DEFAULT_PORT_UP); /* server port for upstream traffic */
static char serv_port_down[8] = STR(DEFAULT_PORT_DW); /* server port for downstream traffic */
static char between_addr[64] = STR(DEFAULT_SERVER); //��һ����ݮ�ɵĵ�ַ
static char between_port_up[8] = STR(DEFAULT_PORT_UP);
static char between_port_down[8] = STR(DEFAULT_PORT_DW);  //����
static int keepalive_time = DEFAULT_KEEPALIVE; /* send a PULL_DATA request every X seconds, negative = disabled */

/* statistics collection configuration variables */ //ͳ�ƣ�parse_gateway_configuration���
static unsigned stat_interval = DEFAULT_STAT; /* time interval (in sec) at which statistics are collected and displayed */

/* gateway <-> MAC protocol variables */ //main���
static uint32_t net_mac_h; /* Most Significant Nibble, network order */ //ǰ���ֽ�
static uint32_t net_mac_l; /* Least Significant Nibble, network order */ //����ֽ�

/* network sockets */ //�׽��֣�main���
static int sock_up; /* socket for upstream traffic */

/* network protocol variables */
static struct timeval push_timeout_half = { 0, (PUSH_TIMEOUT_MS * 500) }; /* cut in half, critical for throughput */ //parse_gateway_configuration���
static struct timeval pull_timeout = { 0, (PULL_TIMEOUT_MS * 1000) }; /* non critical for throughput */

/* hardware access control and correction */ //Ӳ�����룬thread_valid���
pthread_mutex_t mx_concent = PTHREAD_MUTEX_INITIALIZER; /* control access to the concentrator */ //Concentrator
static pthread_mutex_t mx_xcorr = PTHREAD_MUTEX_INITIALIZER; /* control access to the XTAL correction */ //�ⲿ����
static bool xtal_correct_ok = false; /* set true when XTAL correction is stable enough */
static double xtal_correct = 1.0;

/* GPS configuration and synchronization */ //GPS����
static char gps_tty_path[64] = "\0"; /* path of the TTY port GPS is connected on */  //parse_gateway_configuration��ã������Ƿ�����gps��Ψһ��׼��TTY: Ӳ���ն��豸��
static int gps_tty_fd = -1; /* file descriptor of the GPS TTY port */ //main���
static bool gps_enabled = false; /* is GPS enabled on that gateway ? */ //main���

/* GPS time reference */
static pthread_mutex_t mx_timeref = PTHREAD_MUTEX_INITIALIZER; /* control access to GPS time reference */
static bool gps_ref_valid; /* is GPS reference acceptable (ie. not too old) */
static struct tref time_reference_gps; /* time reference used for GPS <-> timestamp conversion */

/* Reference coordinates, for broadcasting (beacon) */
static struct coord_s reference_coord;

/* Enable faking the GPS coordinates of the gateway */ //α�����GPS��parse_gateway_configuration���
static bool gps_fake_enable; /* enable the feature */

/* measurements to establish statistics */ //ͳ����Ϣ��thread_up���
static pthread_mutex_t mx_meas_up = PTHREAD_MUTEX_INITIALIZER; /* control access to the upstream measurements */
static uint32_t meas_nb_rx_rcv = 0; /* count packets received */
static uint32_t meas_nb_rx_ok = 0; /* count packets received with PAYLOAD CRC OK */
static uint32_t meas_nb_rx_bad = 0; /* count packets received with PAYLOAD CRC ERROR */
static uint32_t meas_nb_rx_nocrc = 0; /* count packets received with NO PAYLOAD CRC */
static uint32_t meas_up_pkt_fwd = 0; /* number of radio packet forwarded to the server */
static uint32_t meas_up_network_byte = 0; /* sum of UDP bytes sent for upstream traffic */
static uint32_t meas_up_payload_byte = 0; /* sum of radio payload bytes sent for upstream traffic */
static uint32_t meas_up_dgram_sent = 0; /* number of datagrams sent for upstream traffic */
static uint32_t meas_up_ack_rcv = 0; /* number of datagrams acknowledged for upstream traffic */

static pthread_mutex_t mx_meas_dw = PTHREAD_MUTEX_INITIALIZER; /* control access to the downstream measurements */

//thread_down���
//PULL_DATA
static uint32_t meas_dw_pull_sent = 0; /* number of PULL requests sent for downstream traffic */
//PULL_ACK
static uint32_t meas_dw_ack_rcv = 0; /* number of PULL requests acknowledged for downstream traffic */
//PULL_RESP
static uint32_t meas_dw_dgram_rcv = 0; /* count PULL response packets received for downstream traffic */
static uint32_t meas_dw_network_byte = 0; /* sum of UDP bytes sent for upstream traffic */
static uint32_t meas_dw_payload_byte = 0; /* sum of radio payload bytes sent for upstream traffic */

//thread_jit���
static uint32_t meas_nb_tx_ok = 0; /* count packets emitted successfully */
static uint32_t meas_nb_tx_fail = 0; /* count packets were TX failed for other reasons */

//thread_down���
//jit_enqueue CLASS A/B/C
static uint32_t meas_nb_tx_requested = 0; /* count TX request from server (downlinks) */

//send_tx_ack���
static uint32_t meas_nb_tx_rejected_collision_packet = 0; /* count packets were TX request were rejected due to collision with another packet already programmed */
static uint32_t meas_nb_tx_rejected_collision_beacon = 0; /* count packets were TX request were rejected due to collision with a beacon already programmed */
static uint32_t meas_nb_tx_rejected_too_late = 0; /* count packets were TX request were rejected because it is too late to program it */
static uint32_t meas_nb_tx_rejected_too_early = 0; /* count packets were TX request were rejected because timestamp is too much in advance */

//thread_down���
//jit_enqueue BEACON
static uint32_t meas_nb_beacon_queued = 0; /* count beacon inserted in jit queue */

//thread_jit���
static uint32_t meas_nb_beacon_sent = 0; /* count beacon actually sent to concentrator */

//thread_down���
static uint32_t meas_nb_beacon_rejected = 0; /* count beacon rejected for queuing */

//gps_process_coords���
static pthread_mutex_t mx_meas_gps = PTHREAD_MUTEX_INITIALIZER; /* control access to the GPS statistics */
static bool gps_coord_valid; /* could we get valid GPS coordinates ? */
static struct coord_s meas_gps_coord; /* GPS position of the gateway */
static struct coord_s meas_gps_err; /* GPS position of the gateway */

//main���
static pthread_mutex_t mx_stat_rep = PTHREAD_MUTEX_INITIALIZER; /* control access to the status report */
static bool report_ready = false; /* true when there is a new report to send to the server */
//main��ã�thread_up����
static char status_report[STATUS_SIZE]; /* status report as a JSON object */

/* beacon parameters */ //class B beacon�ű������parse_gateway_configuration���
static uint32_t beacon_period = 0; /* set beaconing period, must be a sub-multiple of 86400, the nb of sec in a day */
static uint32_t beacon_freq_hz = DEFAULT_BEACON_FREQ_HZ; /* set beacon TX frequency, in Hz */
static uint8_t beacon_freq_nb = DEFAULT_BEACON_FREQ_NB; /* set number of beaconing channels beacon */
static uint32_t beacon_freq_step = DEFAULT_BEACON_FREQ_STEP; /* set frequency step between beacon channels, in Hz */
static uint8_t beacon_datarate = DEFAULT_BEACON_DATARATE; /* set beacon datarate (SF) */
static uint32_t beacon_bw_hz = DEFAULT_BEACON_BW_HZ; /* set beacon bandwidth, in Hz */
static int8_t beacon_power = DEFAULT_BEACON_POWER; /* set beacon TX power, in dBm */
static uint8_t beacon_infodesc = DEFAULT_BEACON_INFODESC; /* set beacon information descriptor */

/* auto-quit function */ //�Զ��˳������ѽ��ã�parse_gateway_configuration���
static uint32_t autoquit_threshold = 0; /* enable auto-quit after a number of non-acknowledged PULL_DATA (0 = disabled)*/

/* Just In Time TX scheduling */
static struct jit_queue_s jit_queue[LGW_RF_CHAIN_NB];

/* Gateway specificities */ //�������棻parse_SX130x_configuration���
static int8_t antenna_gain = 0;

/* TX capabilities */ //parse_SX130x_configuration��ã���concentrator����thread_jit����downlink����·
static struct lgw_tx_gain_lut_s txlut[LGW_RF_CHAIN_NB]; /* TX gain table */ //txlut[i]��ָSX130x_conf.radio_%i.tx_gain_lut���� snprintf(param_name, sizeof param_name, "radio_%i.tx_gain_lut", i);
static uint32_t tx_freq_min[LGW_RF_CHAIN_NB]; /* lowest frequency supported by TX chain */
static uint32_t tx_freq_max[LGW_RF_CHAIN_NB]; /* highest frequency supported by TX chain */
static bool tx_enable[LGW_RF_CHAIN_NB] = { false }; /* Is TX enabled for a given RF chain ? */

//thread_up���
static uint32_t nb_pkt_log[LGW_IF_CHAIN_NB][8]; /* [CH][SF] */
static uint32_t nb_pkt_received_lora = 0;
static uint32_t nb_pkt_received_fsk = 0;

//parse_debug_configuration�����
static struct lgw_conf_debug_s debugconf; //debugconfָ��debug_conf����
static uint32_t nb_pkt_received_ref[16]; //global count

/* Interface type */
static lgw_com_type_t com_type = LGW_COM_SPI;

/* Spectral Scan */
static spectral_scan_t spectral_scan_params = {
    .enable = false,
    .freq_hz_start = 0,
    .nb_chan = 0,
    .nb_scan = 0,
    .pace_s = 10
};

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DECLARATION ---------------------------------------- */ //��������

static void usage(void);

static void sig_handler(int sigio);

static int parse_SX130x_configuration(const char* conf_file);

static int parse_gateway_configuration(const char* conf_file);

static int parse_debug_configuration(const char* conf_file);

static uint16_t crc16(const uint8_t* data, unsigned size);

static double difftimespec(struct timespec end, struct timespec beginning);

static void gps_process_sync(void);

static void gps_process_coords(void);

static int get_tx_gain_lut_index(uint8_t rf_chain, int8_t rf_power, uint8_t* lut_index);

/* threads */ //���������������߳��Թ������κ����Ρ�GPS��Ƶ��ɨ��
void thread_up(void);

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DEFINITION ----------------------------------------- */

void lora_crc16_copy(const char data, int* crc) {
    int next = 0;
    next = (((data >> 0) & 1) ^ ((*crc >> 12) & 1) ^ ((*crc >> 8) & 1));
    next += ((((data >> 1) & 1) ^ ((*crc >> 13) & 1) ^ ((*crc >> 9) & 1)) << 1);
    next += ((((data >> 2) & 1) ^ ((*crc >> 14) & 1) ^ ((*crc >> 10) & 1)) << 2);
    next += ((((data >> 3) & 1) ^ ((*crc >> 15) & 1) ^ ((*crc >> 11) & 1)) << 3);
    next += ((((data >> 4) & 1) ^ ((*crc >> 12) & 1)) << 4);
    next += ((((data >> 5) & 1) ^ ((*crc >> 13) & 1) ^ ((*crc >> 12) & 1) ^ ((*crc >> 8) & 1)) << 5);
    next += ((((data >> 6) & 1) ^ ((*crc >> 14) & 1) ^ ((*crc >> 13) & 1) ^ ((*crc >> 9) & 1)) << 6);
    next += ((((data >> 7) & 1) ^ ((*crc >> 15) & 1) ^ ((*crc >> 14) & 1) ^ ((*crc >> 10) & 1)) << 7);
    next += ((((*crc >> 0) & 1) ^ ((*crc >> 15) & 1) ^ ((*crc >> 11) & 1)) << 8);
    next += ((((*crc >> 1) & 1) ^ ((*crc >> 12) & 1)) << 9);
    next += ((((*crc >> 2) & 1) ^ ((*crc >> 13) & 1)) << 10);
    next += ((((*crc >> 3) & 1) ^ ((*crc >> 14) & 1)) << 11);
    next += ((((*crc >> 4) & 1) ^ ((*crc >> 15) & 1) ^ ((*crc >> 12) & 1) ^ ((*crc >> 8) & 1)) << 12);
    next += ((((*crc >> 5) & 1) ^ ((*crc >> 13) & 1) ^ ((*crc >> 9) & 1)) << 13);
    next += ((((*crc >> 6) & 1) ^ ((*crc >> 14) & 1) ^ ((*crc >> 10) & 1)) << 14);
    next += ((((*crc >> 7) & 1) ^ ((*crc >> 15) & 1) ^ ((*crc >> 11) & 1)) << 15);
    (*crc) = next;
}



uint16_t sx1302_lora_payload_crc_copy(const uint8_t* data, uint8_t size) {
    int i;
    int crc = 0;

    for (i = 0; i < size; i++) {
        lora_crc16_copy(data[i], &crc);
    }

    //printf("CRC16: 0x%02X 0x%02X (%X)\n", (uint8_t)(crc >> 8), (uint8_t)crc, crc);
    return (uint16_t)crc;
}


static void usage(void) //���������й�
{
    printf("~~~ Library version string~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf(" %s\n", lgw_version_info()); //���ڱ�ʶ�����Ŀ�汾/ѡ��
    printf("~~~ Available options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf(" -h  print this help\n");
    printf(" -c <filename>  use config file other than 'global_conf.json'\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    //pi@raspberrypi:~/gw1302s/bin $ ./lora_pkt_fwd -h
    //~~~ Library version string~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Version: 2.0.1;
    //~~~ Available options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //-h  print this help
    //-c <filename>	use config file other than 'global_conf.json'
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
}

static void sig_handler(int sigio) { //�ж��˳����򷽷�
    if (sigio == SIGQUIT) { //δ�ر�Ӳ��
        quit_sig = true;
    }
    else if ((sigio == SIGINT) || (sigio == SIGTERM)) { //��ȷ�˳�
        exit_sig = true;
    }
    return;
}

static int parse_SX130x_configuration(const char* conf_file) {
    int i, j, number;
    char param_name[32]; /* used to generate variable parameter names */ //��Ϊ�м����
    const char* str; /* used to store string value from JSON object */
    const char conf_obj_name[] = "SX130x_conf";
    JSON_Value* root_val = NULL;
    JSON_Value* val = NULL;
    JSON_Object* conf_obj = NULL; //conf_obj��SX130x_conf
    JSON_Object* conf_txgain_obj;
    JSON_Object* conf_ts_obj;
    JSON_Object* conf_sx1261_obj = NULL;
    JSON_Object* conf_scan_obj = NULL;
    JSON_Object* conf_lbt_obj = NULL;
    JSON_Object* conf_lbtchan_obj = NULL;
    JSON_Array* conf_txlut_array = NULL;
    JSON_Array* conf_lbtchan_array = NULL;
    JSON_Array* conf_demod_array = NULL;

    struct lgw_conf_board_s boardconf; //boardconfָ��SX130x_conf����
    struct lgw_conf_rxrf_s rfconf; //rfconfָ��SX130x_conf.radio_%i����
    struct lgw_conf_rxif_s ifconf; //ifconfָ��SX130x_conf.chan_multiSF_i/SX130x_conf.chan_Lora_std/SX130x_conf.chan_FSK����
    struct lgw_conf_demod_s demodconf; //demodconfָ��SX130x_conf.chan_multiSF_All����
    struct lgw_conf_ftime_s tsconf; //tsconfָ��SX130x_conf.fine_timestamp����
    struct lgw_conf_sx1261_s sx1261conf;
    uint32_t sf, bw, fdev;
    bool sx1250_tx_lut;
    size_t size;

    /* try to parse JSON */ //�ж��Ƿ���JSON�ļ�
    root_val = json_parse_file_with_comments(conf_file);
    if (root_val == NULL) {
        MSG("ERROR: %s is not a valid JSON file\n", conf_file);
        exit(EXIT_FAILURE);
    }

    /* point to the gateway configuration object */ //�ж�json�ļ����Ƿ���SX130x_conf����
    conf_obj = json_object_get_object(json_value_get_object(root_val), conf_obj_name);
    if (conf_obj == NULL) {
        MSG("INFO: %s does not contain a JSON object named %s\n", conf_file, conf_obj_name);
        return -1;
    }
    else {
        MSG("INFO: %s does contain a JSON object named %s, parsing SX1302 parameters\n", conf_file, conf_obj_name);
    }

    /* set board configuration */
    memset(&boardconf, 0, sizeof boardconf); /* initialize configuration structure */ //boardconfָ��SX130x_conf����
    str = json_object_get_string(conf_obj, "com_type"); //SX130x_conf.com_type
    if (str == NULL) {
        MSG("ERROR: com_type must be configured in %s\n", conf_file);
        return -1;
    }
    else if (!strncmp(str, "SPI", 3) || !strncmp(str, "spi", 3)) { //"com_type": "SPI"
        boardconf.com_type = LGW_COM_SPI;
    }
    else if (!strncmp(str, "USB", 3) || !strncmp(str, "usb", 3)) {
        boardconf.com_type = LGW_COM_USB;
    }
    else {
        MSG("ERROR: invalid com type: %s (should be SPI or USB)\n", str);
        return -1;
    }
    com_type = boardconf.com_type;
    str = json_object_get_string(conf_obj, "com_path"); //SX130x_conf.com_path
    if (str != NULL) {
        strncpy(boardconf.com_path, str, sizeof boardconf.com_path);
        boardconf.com_path[sizeof boardconf.com_path - 1] = '\0'; /* ensure string termination */
    }
    else {
        MSG("ERROR: com_path must be configured in %s\n", conf_file);
        return -1;
    }
    val = json_object_get_value(conf_obj, "lorawan_public"); /* fetch value (if possible) */ //boardconf.lorawan_public
    if (json_value_get_type(val) == JSONBoolean) {
        boardconf.lorawan_public = (bool)json_value_get_boolean(val);
    }
    else {
        MSG("WARNING: Data type for lorawan_public seems wrong, please check\n");
        boardconf.lorawan_public = false;
    }
    val = json_object_get_value(conf_obj, "clksrc"); /* fetch value (if possible) */ //boardconf.clksrc
    if (json_value_get_type(val) == JSONNumber) {
        boardconf.clksrc = (uint8_t)json_value_get_number(val);
    }
    else {
        MSG("WARNING: Data type for clksrc seems wrong, please check\n");
        boardconf.clksrc = 0;
    }
    val = json_object_get_value(conf_obj, "full_duplex"); /* fetch value (if possible) */ //boardconf.full_duplex
    if (json_value_get_type(val) == JSONBoolean) {
        boardconf.full_duplex = (bool)json_value_get_boolean(val);
    }
    else {
        MSG("WARNING: Data type for full_duplex seems wrong, please check\n");
        boardconf.full_duplex = false;
    }
    MSG("INFO: com_type %s, com_path %s, lorawan_public %d, clksrc %d, full_duplex %d\n", (boardconf.com_type == LGW_COM_SPI) ? "SPI" : "USB", boardconf.com_path, boardconf.lorawan_public, boardconf.clksrc, boardconf.full_duplex);
    /* all parameters parsed, submitting configuration to the HAL */ //to set the configuration of the concentrator (sx1302)
    if (lgw_board_setconf(&boardconf) != LGW_HAL_SUCCESS) {
        MSG("ERROR: Failed to configure board\n");
        return -1;
    }

    /* set antenna gain configuration */
    val = json_object_get_value(conf_obj, "antenna_gain"); /* fetch value (if possible) */ //antenna_gain
    if (val != NULL) {
        if (json_value_get_type(val) == JSONNumber) {
            antenna_gain = (int8_t)json_value_get_number(val);
        }
        else {
            MSG("WARNING: Data type for antenna_gain seems wrong, please check\n");
            antenna_gain = 0;
        }
    }
    MSG("INFO: antenna_gain %d dBi\n", antenna_gain);

    /* set timestamp configuration */ ////�ж�SX130x_conf�������Ƿ���fine_timestamp����
    conf_ts_obj = json_object_get_object(conf_obj, "fine_timestamp"); //conf_obj��SX130x_conf
    if (conf_ts_obj == NULL) {
        MSG("INFO: %s does not contain a JSON object for fine timestamp\n", conf_file);
    }
    else {
        val = json_object_get_value(conf_ts_obj, "enable"); /* fetch value (if possible) */  //tsconf.nable_precision_ts���ж�SX130x_conf.fine_timestamp�Ƿ�����
        if (json_value_get_type(val) == JSONBoolean) {
            tsconf.enable = (bool)json_value_get_boolean(val); //tsconfָ��SX130x_conf.fine_timestamp����
        }
        else {
            MSG("WARNING: Data type for fine_timestamp.enable seems wrong, please check\n");
            tsconf.enable = false;
        }
        if (tsconf.enable == true) {
            str = json_object_get_string(conf_ts_obj, "mode");
            if (str == NULL) {
                MSG("ERROR: fine_timestamp.mode must be configured in %s\n", conf_file);
                return -1;
            }
            else if (!strncmp(str, "high_capacity", 13) || !strncmp(str, "HIGH_CAPACITY", 13)) {
                tsconf.mode = LGW_FTIME_MODE_HIGH_CAPACITY;
            }
            else if (!strncmp(str, "all_sf", 6) || !strncmp(str, "ALL_SF", 6)) {
                tsconf.mode = LGW_FTIME_MODE_ALL_SF;
            }
            else {
                MSG("ERROR: invalid fine timestamp mode: %s (should be high_capacity or all_sf)\n", str);
                return -1;
            }
            MSG("INFO: Configuring precision timestamp with %s mode\n", str);

            /* all parameters parsed, submitting configuration to the HAL */ //Configure the fine timestamp
            if (lgw_ftime_setconf(&tsconf) != LGW_HAL_SUCCESS) {
                MSG("ERROR: Failed to configure fine timestamp\n");
                return -1;
            }
        }
        else {
            MSG("INFO: Configuring legacy timestamp\n");
        }
    }

    /* set SX1261 configuration */
    memset(&sx1261conf, 0, sizeof sx1261conf); /* initialize configuration structure */
    conf_sx1261_obj = json_object_get_object(conf_obj, "sx1261_conf"); /* fetch value (if possible) */
    if (conf_sx1261_obj == NULL) { //û��sx1261
        MSG("INFO: no configuration for SX1261\n");
    }
    else {
        /* Global SX1261 configuration */
        str = json_object_get_string(conf_sx1261_obj, "spi_path");
        if (str != NULL) {
            strncpy(sx1261conf.spi_path, str, sizeof sx1261conf.spi_path);
            sx1261conf.spi_path[sizeof sx1261conf.spi_path - 1] = '\0'; /* ensure string termination */
        }
        else {
            MSG("INFO: SX1261 spi_path is not configured in %s\n", conf_file);
        }
        val = json_object_get_value(conf_sx1261_obj, "rssi_offset"); /* fetch value (if possible) */
        if (json_value_get_type(val) == JSONNumber) {
            sx1261conf.rssi_offset = (int8_t)json_value_get_number(val);
        }
        else {
            MSG("WARNING: Data type for sx1261_conf.rssi_offset seems wrong, please check\n");
            sx1261conf.rssi_offset = 0;
        }

        /* Spectral Scan configuration */ //Ƶ��ɨ�����Ҫ��sx1261
        conf_scan_obj = json_object_get_object(conf_sx1261_obj, "spectral_scan"); /* fetch value (if possible) */
        if (conf_scan_obj == NULL) {
            MSG("INFO: no configuration for Spectral Scan\n");
        }
        else {
            val = json_object_get_value(conf_scan_obj, "enable"); /* fetch value (if possible) */
            if (json_value_get_type(val) == JSONBoolean) {
                /* Enable background spectral scan thread in packet forwarder */
                spectral_scan_params.enable = (bool)json_value_get_boolean(val);
            }
            else {
                MSG("WARNING: Data type for spectral_scan.enable seems wrong, please check\n");
            }
            if (spectral_scan_params.enable == true) {
                /* Enable the sx1261 radio hardware configuration to allow spectral scan */
                sx1261conf.enable = true;
                MSG("INFO: Spectral Scan with SX1261 is enabled\n");

                /* Get Spectral Scan Parameters */
                val = json_object_get_value(conf_scan_obj, "freq_start"); /* fetch value (if possible) */
                if (json_value_get_type(val) == JSONNumber) {
                    spectral_scan_params.freq_hz_start = (uint32_t)json_value_get_number(val);
                }
                else {
                    MSG("WARNING: Data type for spectral_scan.freq_start seems wrong, please check\n");
                }
                val = json_object_get_value(conf_scan_obj, "nb_chan"); /* fetch value (if possible) */
                if (json_value_get_type(val) == JSONNumber) {
                    spectral_scan_params.nb_chan = (uint8_t)json_value_get_number(val);
                }
                else {
                    MSG("WARNING: Data type for spectral_scan.nb_chan seems wrong, please check\n");
                }
                val = json_object_get_value(conf_scan_obj, "nb_scan"); /* fetch value (if possible) */
                if (json_value_get_type(val) == JSONNumber) {
                    spectral_scan_params.nb_scan = (uint16_t)json_value_get_number(val);
                }
                else {
                    MSG("WARNING: Data type for spectral_scan.nb_scan seems wrong, please check\n");
                }
                val = json_object_get_value(conf_scan_obj, "pace_s"); /* fetch value (if possible) */
                if (json_value_get_type(val) == JSONNumber) {
                    spectral_scan_params.pace_s = (uint32_t)json_value_get_number(val);
                }
                else {
                    MSG("WARNING: Data type for spectral_scan.pace_s seems wrong, please check\n");
                }
            }
        }

        /* LBT configuration */ //Listen-Before-Talk����Ҫ��sx1261
        conf_lbt_obj = json_object_get_object(conf_sx1261_obj, "lbt"); /* fetch value (if possible) */
        if (conf_lbt_obj == NULL) {
            MSG("INFO: no configuration for LBT\n");
        }
        else {
            val = json_object_get_value(conf_lbt_obj, "enable"); /* fetch value (if possible) */
            if (json_value_get_type(val) == JSONBoolean) {
                sx1261conf.lbt_conf.enable = (bool)json_value_get_boolean(val);
            }
            else {
                MSG("WARNING: Data type for lbt.enable seems wrong, please check\n");
            }
            if (sx1261conf.lbt_conf.enable == true) {
                /* Enable the sx1261 radio hardware configuration to allow spectral scan */
                sx1261conf.enable = true;
                MSG("INFO: Listen-Before-Talk with SX1261 is enabled\n");

                val = json_object_get_value(conf_lbt_obj, "rssi_target"); /* fetch value (if possible) */
                if (json_value_get_type(val) == JSONNumber) {
                    sx1261conf.lbt_conf.rssi_target = (int8_t)json_value_get_number(val);
                }
                else {
                    MSG("WARNING: Data type for lbt.rssi_target seems wrong, please check\n");
                    sx1261conf.lbt_conf.rssi_target = 0;
                }
                /* set LBT channels configuration */
                conf_lbtchan_array = json_object_get_array(conf_lbt_obj, "channels");
                if (conf_lbtchan_array != NULL) {
                    sx1261conf.lbt_conf.nb_channel = json_array_get_count(conf_lbtchan_array);
                    MSG("INFO: %u LBT channels configured\n", sx1261conf.lbt_conf.nb_channel);
                }
                for (i = 0; i < (int)sx1261conf.lbt_conf.nb_channel; i++) {
                    /* Sanity check */
                    if (i >= LGW_LBT_CHANNEL_NB_MAX) {
                        MSG("ERROR: LBT channel %d not supported, skip it\n", i);
                        break;
                    }
                    /* Get LBT channel configuration object from array */
                    conf_lbtchan_obj = json_array_get_object(conf_lbtchan_array, i);

                    /* Channel frequency */
                    val = json_object_dotget_value(conf_lbtchan_obj, "freq_hz"); /* fetch value (if possible) */
                    if (val != NULL) {
                        if (json_value_get_type(val) == JSONNumber) {
                            sx1261conf.lbt_conf.channels[i].freq_hz = (uint32_t)json_value_get_number(val);
                        }
                        else {
                            MSG("WARNING: Data type for lbt.channels[%d].freq_hz seems wrong, please check\n", i);
                            sx1261conf.lbt_conf.channels[i].freq_hz = 0;
                        }
                    }
                    else {
                        MSG("ERROR: no frequency defined for LBT channel %d\n", i);
                        return -1;
                    }

                    /* Channel bandiwdth */
                    val = json_object_dotget_value(conf_lbtchan_obj, "bandwidth"); /* fetch value (if possible) */
                    if (val != NULL) {
                        if (json_value_get_type(val) == JSONNumber) {
                            bw = (uint32_t)json_value_get_number(val);
                            switch (bw) {
                            case 500000: sx1261conf.lbt_conf.channels[i].bandwidth = BW_500KHZ; break;
                            case 250000: sx1261conf.lbt_conf.channels[i].bandwidth = BW_250KHZ; break;
                            case 125000: sx1261conf.lbt_conf.channels[i].bandwidth = BW_125KHZ; break;
                            default: sx1261conf.lbt_conf.channels[i].bandwidth = BW_UNDEFINED;
                            }
                        }
                        else {
                            MSG("WARNING: Data type for lbt.channels[%d].freq_hz seems wrong, please check\n", i);
                            sx1261conf.lbt_conf.channels[i].bandwidth = BW_UNDEFINED;
                        }
                    }
                    else {
                        MSG("ERROR: no bandiwdth defined for LBT channel %d\n", i);
                        return -1;
                    }

                    /* Channel scan time */
                    val = json_object_dotget_value(conf_lbtchan_obj, "scan_time_us"); /* fetch value (if possible) */
                    if (val != NULL) {
                        if (json_value_get_type(val) == JSONNumber) {
                            if ((uint16_t)json_value_get_number(val) == 128) {
                                sx1261conf.lbt_conf.channels[i].scan_time_us = LGW_LBT_SCAN_TIME_128_US;
                            }
                            else if ((uint16_t)json_value_get_number(val) == 5000) {
                                sx1261conf.lbt_conf.channels[i].scan_time_us = LGW_LBT_SCAN_TIME_5000_US;
                            }
                            else {
                                MSG("ERROR: scan time not supported for LBT channel %d, must be 128 or 5000\n", i);
                                return -1;
                            }
                        }
                        else {
                            MSG("WARNING: Data type for lbt.channels[%d].scan_time_us seems wrong, please check\n", i);
                            sx1261conf.lbt_conf.channels[i].scan_time_us = 0;
                        }
                    }
                    else {
                        MSG("ERROR: no scan_time_us defined for LBT channel %d\n", i);
                        return -1;
                    }

                    /* Channel transmit time */
                    val = json_object_dotget_value(conf_lbtchan_obj, "transmit_time_ms"); /* fetch value (if possible) */
                    if (val != NULL) {
                        if (json_value_get_type(val) == JSONNumber) {
                            sx1261conf.lbt_conf.channels[i].transmit_time_ms = (uint16_t)json_value_get_number(val);
                        }
                        else {
                            MSG("WARNING: Data type for lbt.channels[%d].transmit_time_ms seems wrong, please check\n", i);
                            sx1261conf.lbt_conf.channels[i].transmit_time_ms = 0;
                        }
                    }
                    else {
                        MSG("ERROR: no transmit_time_ms defined for LBT channel %d\n", i);
                        return -1;
                    }
                }
            }
        }

        /* all parameters parsed, submitting configuration to the HAL */
        if (lgw_sx1261_setconf(&sx1261conf) != LGW_HAL_SUCCESS) {
            MSG("ERROR: Failed to configure the SX1261 radio\n");
            return -1;
        }
    }

    /* set configuration for RF chains */
    for (i = 0; i < LGW_RF_CHAIN_NB; ++i) { //��Ϊnumber of RF chains = 2�����Դ�radio_0���ν�����radio_1
        memset(&rfconf, 0, sizeof rfconf); /* initialize configuration structure */ //rfconfָ��SX130x_conf.radio_%i����
        snprintf(param_name, sizeof param_name, "radio_%i", i); /* compose parameter path inside JSON structure */ //param_name��Ϊ�м����
        val = json_object_get_value(conf_obj, param_name); /* fetch value (if possible) */ //�ж�json�ļ����Ƿ���SX130x_conf.radio_0��SX130x_conf.radio_1...�ȶ���
        if (json_value_get_type(val) != JSONObject) {
            MSG("INFO: no configuration for radio %i\n", i);
            continue;
        }
        /* there is an object to configure that radio, let's parse it */
        snprintf(param_name, sizeof param_name, "radio_%i.enable", i);
        val = json_object_dotget_value(conf_obj, param_name);  //rfconf.enable���ж�SX130x_conf.radio_%i�Ƿ�����
        if (json_value_get_type(val) == JSONBoolean) {
            rfconf.enable = (bool)json_value_get_boolean(val);
        }
        else {
            rfconf.enable = false;
        }
        if (rfconf.enable == false) { /* radio disabled, nothing else to parse */
            MSG("INFO: radio %i disabled\n", i);
        }
        else { /* radio enabled, will parse the other parameters */
            snprintf(param_name, sizeof param_name, "radio_%i.freq", i); //param_name��Ϊ�м��������radio_%i.freq
            rfconf.freq_hz = (uint32_t)json_object_dotget_number(conf_obj, param_name); //rfconf.freq_hz
            snprintf(param_name, sizeof param_name, "radio_%i.rssi_offset", i); //param_name����Ϊ�м��������radio_%i.rssi_offset
            rfconf.rssi_offset = (float)json_object_dotget_number(conf_obj, param_name); //rfconf.rssi_offset
            snprintf(param_name, sizeof param_name, "radio_%i.rssi_tcomp.coeff_a", i); //ͬ��
            rfconf.rssi_tcomp.coeff_a = (float)json_object_dotget_number(conf_obj, param_name);
            snprintf(param_name, sizeof param_name, "radio_%i.rssi_tcomp.coeff_b", i);
            rfconf.rssi_tcomp.coeff_b = (float)json_object_dotget_number(conf_obj, param_name);
            snprintf(param_name, sizeof param_name, "radio_%i.rssi_tcomp.coeff_c", i);
            rfconf.rssi_tcomp.coeff_c = (float)json_object_dotget_number(conf_obj, param_name);
            snprintf(param_name, sizeof param_name, "radio_%i.rssi_tcomp.coeff_d", i);
            rfconf.rssi_tcomp.coeff_d = (float)json_object_dotget_number(conf_obj, param_name);
            snprintf(param_name, sizeof param_name, "radio_%i.rssi_tcomp.coeff_e", i);
            rfconf.rssi_tcomp.coeff_e = (float)json_object_dotget_number(conf_obj, param_name);
            snprintf(param_name, sizeof param_name, "radio_%i.type", i);
            str = json_object_dotget_string(conf_obj, param_name);
            if (!strncmp(str, "SX1255", 6)) { //radio_%i.type = SX1255
                rfconf.type = LGW_RADIO_TYPE_SX1255;
            }
            else if (!strncmp(str, "SX1257", 6)) { //radio_%i.type = SX1257
                rfconf.type = LGW_RADIO_TYPE_SX1257;
            }
            else if (!strncmp(str, "SX1250", 6)) { //radio_%i.type = SX1250
                rfconf.type = LGW_RADIO_TYPE_SX1250;
            }
            else {
                MSG("WARNING: invalid radio type: %s (should be SX1255 or SX1257 or SX1250)\n", str);
            }
            snprintf(param_name, sizeof param_name, "radio_%i.single_input_mode", i);
            val = json_object_dotget_value(conf_obj, param_name); //rfconf.single_input_mode
            if (json_value_get_type(val) == JSONBoolean) {
                rfconf.single_input_mode = (bool)json_value_get_boolean(val);
            }
            else {
                rfconf.single_input_mode = false;
            }

            snprintf(param_name, sizeof param_name, "radio_%i.tx_enable", i); //param_name����Ϊ�м��������radio_%i.tx_enable
            val = json_object_dotget_value(conf_obj, param_name); //rfconf.tx_enable���ж�SX130x_conf.radio_%i.tx_enable�Ƿ�����
            if (json_value_get_type(val) == JSONBoolean) {
                rfconf.tx_enable = (bool)json_value_get_boolean(val);
                tx_enable[i] = rfconf.tx_enable; /* update global context for later check */
                if (rfconf.tx_enable == true) { //���rfchaini����tx����
                    /* tx is enabled on this rf chain, we need its frequency range */
                    snprintf(param_name, sizeof param_name, "radio_%i.tx_freq_min", i); //tx_freq_min[i]
                    tx_freq_min[i] = (uint32_t)json_object_dotget_number(conf_obj, param_name);
                    snprintf(param_name, sizeof param_name, "radio_%i.tx_freq_max", i); //tx_freq_max[i]
                    tx_freq_max[i] = (uint32_t)json_object_dotget_number(conf_obj, param_name);
                    if ((tx_freq_min[i] == 0) || (tx_freq_max[i] == 0)) {
                        MSG("WARNING: no frequency range specified for TX rf chain %d\n", i);
                    }

                    /* set configuration for tx gains */
                    memset(&txlut[i], 0, sizeof txlut[i]); /* initialize configuration structure */ //txlut[i]��ָSX130x_conf.radio_%i.tx_gain_lut����
                    snprintf(param_name, sizeof param_name, "radio_%i.tx_gain_lut", i);
                    conf_txlut_array = json_object_dotget_array(conf_obj, param_name);
                    if (conf_txlut_array != NULL) {
                        txlut[i].size = json_array_get_count(conf_txlut_array);
                        /* Detect if we have a sx125x or sx1250 configuration */
                        conf_txgain_obj = json_array_get_object(conf_txlut_array, 0); //conf_txgain_obj��SX130x_conf.radio_%i.tx_gain_lut
                        val = json_object_dotget_value(conf_txgain_obj, "pwr_idx");
                        if (val != NULL) { //��SX130x_conf.radio_%i.tx_gain_lut.pwr_idx����SX1250
                            printf("INFO: Configuring Tx Gain LUT for rf_chain %u with %u indexes for sx1250\n", i, txlut[i].size);
                            sx1250_tx_lut = true;
                        }
                        else { //û��SX130x_conf.radio_%i.tx_gain_lut.pwr_idx����SX125x
                            printf("INFO: Configuring Tx Gain LUT for rf_chain %u with %u indexes for sx125x\n", i, txlut[i].size);
                            sx1250_tx_lut = false;
                        }
                        /* Parse the table */
                        for (j = 0; j < (int)txlut[i].size; j++) {
                            /* Sanity check */
                            if (j >= TX_GAIN_LUT_SIZE_MAX) {
                                printf("ERROR: TX Gain LUT [%u] index %d not supported, skip it\n", i, j);
                                break;
                            }
                            /* Get TX gain object from LUT */
                            conf_txgain_obj = json_array_get_object(conf_txlut_array, j);
                            /* rf power */
                            val = json_object_dotget_value(conf_txgain_obj, "rf_power"); //txlut[i].lut[j].rf_power��ָSX130x_conf.radio_%i.tx_gain_lut��j������.rf_power
                            if (json_value_get_type(val) == JSONNumber) {
                                txlut[i].lut[j].rf_power = (int8_t)json_value_get_number(val);
                            }
                            else {
                                printf("WARNING: Data type for %s[%d] seems wrong, please check\n", "rf_power", j);
                                txlut[i].lut[j].rf_power = 0;
                            }
                            /* PA gain */
                            val = json_object_dotget_value(conf_txgain_obj, "pa_gain"); //txlut[i].lut[j].pa_gain
                            if (json_value_get_type(val) == JSONNumber) {
                                txlut[i].lut[j].pa_gain = (uint8_t)json_value_get_number(val);
                            }
                            else {
                                printf("WARNING: Data type for %s[%d] seems wrong, please check\n", "pa_gain", j);
                                txlut[i].lut[j].pa_gain = 0;
                            }
                            if (sx1250_tx_lut == false) { //��sx125x
                                /* DIG gain */
                                val = json_object_dotget_value(conf_txgain_obj, "dig_gain"); //txlut[i].lut[j].dig_gain
                                if (json_value_get_type(val) == JSONNumber) {
                                    txlut[i].lut[j].dig_gain = (uint8_t)json_value_get_number(val);
                                }
                                else {
                                    printf("WARNING: Data type for %s[%d] seems wrong, please check\n", "dig_gain", j);
                                    txlut[i].lut[j].dig_gain = 0;
                                }
                                /* DAC gain */
                                val = json_object_dotget_value(conf_txgain_obj, "dac_gain"); //txlut[i].lut[j].dac_gain
                                if (json_value_get_type(val) == JSONNumber) {
                                    txlut[i].lut[j].dac_gain = (uint8_t)json_value_get_number(val);
                                }
                                else {
                                    printf("WARNING: Data type for %s[%d] seems wrong, please check\n", "dac_gain", j);
                                    txlut[i].lut[j].dac_gain = 3; /* This is the only dac_gain supported for now */
                                }
                                /* MIX gain */
                                val = json_object_dotget_value(conf_txgain_obj, "mix_gain"); //txlut[i].lut[j].mix_gain
                                if (json_value_get_type(val) == JSONNumber) {
                                    txlut[i].lut[j].mix_gain = (uint8_t)json_value_get_number(val);
                                }
                                else {
                                    printf("WARNING: Data type for %s[%d] seems wrong, please check\n", "mix_gain", j);
                                    txlut[i].lut[j].mix_gain = 0;
                                }
                            }
                            else { //sx1250
                             /* TODO: rework this, should not be needed for sx1250 */
                                txlut[i].lut[j].mix_gain = 5;

                                /* power index */
                                val = json_object_dotget_value(conf_txgain_obj, "pwr_idx"); //txlut[i].lut[j].pwr_idx
                                if (json_value_get_type(val) == JSONNumber) {
                                    txlut[i].lut[j].pwr_idx = (uint8_t)json_value_get_number(val);
                                }
                                else {
                                    printf("WARNING: Data type for %s[%d] seems wrong, please check\n", "pwr_idx", j);
                                    txlut[i].lut[j].pwr_idx = 0;
                                }
                            }
                        }
                        /* all parameters parsed, submitting configuration to the HAL */
                        if (txlut[i].size > 0) {
                            if (lgw_txgain_setconf(i, &txlut[i]) != LGW_HAL_SUCCESS) { //to set the configuration of the concentrator gain table
                                MSG("ERROR: Failed to configure concentrator TX Gain LUT for rf_chain %u\n", i);
                                return -1;
                            }
                        }
                        else {
                            MSG("WARNING: No TX gain LUT defined for rf_chain %u\n", i);
                        }
                    }
                    else {
                        MSG("WARNING: No TX gain LUT defined for rf_chain %u\n", i);
                    }
                }
            }
            else {
                rfconf.tx_enable = false; //radio_iû����
            }
            MSG("INFO: radio %i enabled (type %s), center frequency %u, RSSI offset %f, tx enabled %d, single input mode %d\n", i, str, rfconf.freq_hz, rfconf.rssi_offset, rfconf.tx_enable, rfconf.single_input_mode);
        }
        /* all parameters parsed, submitting configuration to the HAL */
        if (lgw_rxrf_setconf(i, &rfconf) != LGW_HAL_SUCCESS) { //to set the configuration of the radio channels
            MSG("ERROR: invalid configuration for radio %i\n", i);
            return -1;
        }
    }

    /* set configuration for demodulators */
    memset(&demodconf, 0, sizeof demodconf); /* initialize configuration structure */
    val = json_object_get_value(conf_obj, "chan_multiSF_All"); /* fetch value (if possible) */ //conf_objΪ��ȡ���Ĵ������������
    if (json_value_get_type(val) != JSONObject) { //�������Ͳ���
        MSG("INFO: no configuration for LoRa multi-SF spreading factors enabling\n");
    }
    else { //����������ȷ
        conf_demod_array = json_object_dotget_array(conf_obj, "chan_multiSF_All.spreading_factor_enable"); //��ȡspreading_factor_enable�������������
        if ((conf_demod_array != NULL) && ((size = json_array_get_count(conf_demod_array)) <= LGW_MULTI_NB)) {
            for (i = 0; i < (int)size; i++) {
                number = json_array_get_number(conf_demod_array, i);
                if (number < 5 || number > 12) { //���������ֲ���5��12֮��
                    MSG("WARNING: failed to parse chan_multiSF_All.spreading_factor_enable (wrong value at idx %d)\n", i);
                    demodconf.multisf_datarate = 0xFF; /* enable all SFs */
                    break;
                }
                else { //������������5��12֮��
                 /* set corresponding bit in the bitmask SF5 is LSB -> SF12 is MSB */
                    demodconf.multisf_datarate |= (1 << (number - 5));
                }
            }
        }
        else { //���������������ָ���С��0���ߴ���8
            MSG("WARNING: failed to parse chan_multiSF_All.spreading_factor_enable\n");
            demodconf.multisf_datarate = 0xFF; /* enable all SFs */ //����λ���붼Ϊ1
        }
        /* all parameters parsed, submitting configuration to the HAL */
        if (lgw_demod_setconf(&demodconf) != LGW_HAL_SUCCESS) {
            MSG("ERROR: invalid configuration for demodulation parameters\n");
            return -1;
        }
    }

    /* set configuration for Lora multi-SF channels (bandwidth cannot be set) */
    for (i = 0; i < LGW_MULTI_NB; ++i) { //���֧��8��
        memset(&ifconf, 0, sizeof ifconf); /* initialize configuration structure */ //ifconfָ��SX130x_conf.chan_multiSF_i����
        snprintf(param_name, sizeof param_name, "chan_multiSF_%i", i); /* compose parameter path inside JSON structure */
        val = json_object_get_value(conf_obj, param_name); /* fetch value (if possible) */
        if (json_value_get_type(val) != JSONObject) {
            MSG("INFO: no configuration for Lora multi-SF channel %i\n", i);
            continue;
        }
        /* there is an object to configure that Lora multi-SF channel, let's parse it */
        snprintf(param_name, sizeof param_name, "chan_multiSF_%i.enable", i);
        val = json_object_dotget_value(conf_obj, param_name);  //ifconf.enable���ж�SX130x_conf.chan_multiSF_%i�Ƿ�����
        if (json_value_get_type(val) == JSONBoolean) {
            ifconf.enable = (bool)json_value_get_boolean(val);
        }
        else {
            ifconf.enable = false;
        }
        if (ifconf.enable == false) { /* Lora multi-SF channel disabled, nothing else to parse */
            MSG("INFO: Lora multi-SF channel %i disabled\n", i);
        }
        else { /* Lora multi-SF channel enabled, will parse the other parameters */
            snprintf(param_name, sizeof param_name, "chan_multiSF_%i.radio", i); //ifconf.rf_chain
            ifconf.rf_chain = (uint32_t)json_object_dotget_number(conf_obj, param_name);
            snprintf(param_name, sizeof param_name, "chan_multiSF_%i.if", i); //ifconf.freq_hz
            ifconf.freq_hz = (int32_t)json_object_dotget_number(conf_obj, param_name);
            // TODO: handle individual SF enabling and disabling (spread_factor)
            MSG("INFO: Lora multi-SF channel %i>  radio %i, IF %i Hz, 125 kHz bw, SF 5 to 12\n", i, ifconf.rf_chain, ifconf.freq_hz);
        }
        /* all parameters parsed, submitting configuration to the HAL */
        if (lgw_rxif_setconf(i, &ifconf) != LGW_HAL_SUCCESS) { //to set the configuration of the 0-7 IF+modem channels
            MSG("ERROR: invalid configuration for Lora multi-SF channel %i\n", i);
            return -1;
        }
    }

    /* set configuration for Lora standard channel */
    memset(&ifconf, 0, sizeof ifconf); /* initialize configuration structure */
    val = json_object_get_value(conf_obj, "chan_Lora_std"); /* fetch value (if possible) */
    if (json_value_get_type(val) != JSONObject) {
        MSG("INFO: no configuration for Lora standard channel\n");
    }
    else {
        val = json_object_dotget_value(conf_obj, "chan_Lora_std.enable"); //ifconf.enable���ж�SX130x_conf.chan_Lora_std�Ƿ�����
        if (json_value_get_type(val) == JSONBoolean) {
            ifconf.enable = (bool)json_value_get_boolean(val);
        }
        else {
            ifconf.enable = false;
        }
        if (ifconf.enable == false) {
            MSG("INFO: Lora standard channel %i disabled\n", i);
        }
        else {
            ifconf.rf_chain = (uint32_t)json_object_dotget_number(conf_obj, "chan_Lora_std.radio"); //ifconf.rf_chain
            ifconf.freq_hz = (int32_t)json_object_dotget_number(conf_obj, "chan_Lora_std.if"); //ifconf.freq_hz
            bw = (uint32_t)json_object_dotget_number(conf_obj, "chan_Lora_std.bandwidth"); //bw���м����
            switch (bw) { //ifconf.bandwidth
            case 500000: ifconf.bandwidth = BW_500KHZ; break;
            case 250000: ifconf.bandwidth = BW_250KHZ; break;
            case 125000: ifconf.bandwidth = BW_125KHZ; break;
            default: ifconf.bandwidth = BW_UNDEFINED;
            }
            sf = (uint32_t)json_object_dotget_number(conf_obj, "chan_Lora_std.spread_factor"); //sf���м����
            switch (sf) { //ifconf.datarate
            case  5: ifconf.datarate = DR_LORA_SF5;  break;
            case  6: ifconf.datarate = DR_LORA_SF6;  break;
            case  7: ifconf.datarate = DR_LORA_SF7;  break;
            case  8: ifconf.datarate = DR_LORA_SF8;  break;
            case  9: ifconf.datarate = DR_LORA_SF9;  break;
            case 10: ifconf.datarate = DR_LORA_SF10; break;
            case 11: ifconf.datarate = DR_LORA_SF11; break;
            case 12: ifconf.datarate = DR_LORA_SF12; break;
            default: ifconf.datarate = DR_UNDEFINED;
            }
            val = json_object_dotget_value(conf_obj, "chan_Lora_std.implicit_hdr");
            if (json_value_get_type(val) == JSONBoolean) {
                ifconf.implicit_hdr = (bool)json_value_get_boolean(val);
            }
            else {
                ifconf.implicit_hdr = false;
            }
            if (ifconf.implicit_hdr == true) { //�������ʽ��ͷ
                val = json_object_dotget_value(conf_obj, "chan_Lora_std.implicit_payload_length"); //ifconf.implicit_payload_length�����
                if (json_value_get_type(val) == JSONNumber) {
                    ifconf.implicit_payload_length = (uint8_t)json_value_get_number(val);
                }
                else {
                    MSG("ERROR: payload length setting is mandatory for implicit header mode\n");
                    return -1;
                }
                val = json_object_dotget_value(conf_obj, "chan_Lora_std.implicit_crc_en"); //ifconf.implicit_crc_en�����
                if (json_value_get_type(val) == JSONBoolean) {
                    ifconf.implicit_crc_en = (bool)json_value_get_boolean(val);
                }
                else {
                    MSG("ERROR: CRC enable setting is mandatory for implicit header mode\n");
                    return -1;
                }
                val = json_object_dotget_value(conf_obj, "chan_Lora_std.implicit_coderate"); //ifconf.implicit_coderate�����
                if (json_value_get_type(val) == JSONNumber) {
                    ifconf.implicit_coderate = (uint8_t)json_value_get_number(val);
                }
                else {
                    MSG("ERROR: coding rate setting is mandatory for implicit header mode\n");
                    return -1;
                }
            }

            MSG("INFO: Lora std channel> radio %i, IF %i Hz, %u Hz bw, SF %u, %s\n", ifconf.rf_chain, ifconf.freq_hz, bw, sf, (ifconf.implicit_hdr == true) ? "Implicit header" : "Explicit header");
        }
        if (lgw_rxif_setconf(8, &ifconf) != LGW_HAL_SUCCESS) { //to set the configuration of the 8th IF+modem channels
            MSG("ERROR: invalid configuration for Lora standard channel\n");
            return -1;
        }
    }

    /* set configuration for FSK channel */
    memset(&ifconf, 0, sizeof ifconf); /* initialize configuration structure */ //ifconfָ��SX130x_conf.chan_FSK����
    val = json_object_get_value(conf_obj, "chan_FSK"); /* fetch value (if possible) */
    if (json_value_get_type(val) != JSONObject) {
        MSG("INFO: no configuration for FSK channel\n");
    }
    else {
        val = json_object_dotget_value(conf_obj, "chan_FSK.enable"); //ifconf.enable���ж�SX130x_conf.chan_FSK�Ƿ�����
        if (json_value_get_type(val) == JSONBoolean) {
            ifconf.enable = (bool)json_value_get_boolean(val);
        }
        else {
            ifconf.enable = false;
        }
        if (ifconf.enable == false) {
            MSG("INFO: FSK channel %i disabled\n", i);
        }
        else {
            ifconf.rf_chain = (uint32_t)json_object_dotget_number(conf_obj, "chan_FSK.radio"); //ifconf.rf_chain
            ifconf.freq_hz = (int32_t)json_object_dotget_number(conf_obj, "chan_FSK.if"); //ifconf.freq_hz
            bw = (uint32_t)json_object_dotget_number(conf_obj, "chan_FSK.bandwidth");  //bw���м������SX130x_conf.chan_FSK.freq_deviation��ģ����û��
            fdev = (uint32_t)json_object_dotget_number(conf_obj, "chan_FSK.freq_deviation"); //fdev���м����
            ifconf.datarate = (uint32_t)json_object_dotget_number(conf_obj, "chan_FSK.datarate"); //ifconf.datarate

            /* if chan_FSK.bandwidth is set, it has priority over chan_FSK.freq_deviation */
            if ((bw == 0) && (fdev != 0)) {
                bw = 2 * fdev + ifconf.datarate;
            }
            if (bw == 0)      ifconf.bandwidth = BW_UNDEFINED;
#if 0 /* TODO */
            else if (bw <= 7800)   ifconf.bandwidth = BW_7K8HZ;
            else if (bw <= 15600)  ifconf.bandwidth = BW_15K6HZ;
            else if (bw <= 31200)  ifconf.bandwidth = BW_31K2HZ;
            else if (bw <= 62500)  ifconf.bandwidth = BW_62K5HZ;
#endif
            else if (bw <= 125000) ifconf.bandwidth = BW_125KHZ;
            else if (bw <= 250000) ifconf.bandwidth = BW_250KHZ;
            else if (bw <= 500000) ifconf.bandwidth = BW_500KHZ;
            else ifconf.bandwidth = BW_UNDEFINED;

            MSG("INFO: FSK channel> radio %i, IF %i Hz, %u Hz bw, %u bps datarate\n", ifconf.rf_chain, ifconf.freq_hz, bw, ifconf.datarate);
        }
        if (lgw_rxif_setconf(9, &ifconf) != LGW_HAL_SUCCESS) { //to set the configuration of the 9th IF+modem channels
            MSG("ERROR: invalid configuration for FSK channel\n");
            return -1;
        }
    }
    json_value_free(root_val);

    return 0;
}

static int parse_gateway_configuration(const char* conf_file) {
    const char conf_obj_name[] = "gateway_conf";
    JSON_Value* root_val;
    JSON_Object* conf_obj = NULL;
    JSON_Value* val = NULL; /* needed to detect the absence of some fields */
    const char* str; /* pointer to sub-strings in the JSON data */
    unsigned long long ull = 0; //�洢MAC��ַ���м����

    /* try to parse JSON */ //�ж��Ƿ���JSON�ļ�
    root_val = json_parse_file_with_comments(conf_file);
    if (root_val == NULL) {
        MSG("ERROR: %s is not a valid JSON file\n", conf_file);
        exit(EXIT_FAILURE);
    }

    /* point to the gateway configuration object */ //�ж�json�ļ����Ƿ���gateway_conf����
    conf_obj = json_object_get_object(json_value_get_object(root_val), conf_obj_name);
    if (conf_obj == NULL) {
        MSG("INFO: %s does not contain a JSON object named %s\n", conf_file, conf_obj_name);
        return -1;
    }
    else {
        MSG("INFO: %s does contain a JSON object named %s, parsing gateway parameters\n", conf_file, conf_obj_name);
    }

    /* gateway unique identifier (aka MAC address) (optional) */
    str = json_object_get_string(conf_obj, "gateway_ID");
    if (str != NULL) {
        sscanf(str, "%llx", &ull); //64λ16��������
        lgwm = ull;
        MSG("INFO: gateway MAC address is configured to %016llX\n", ull);
    }

    /* server hostname or IP address (optional) */
    str = json_object_get_string(conf_obj, "server_address"); //serv_addr
    if (str != NULL) {
        strncpy(serv_addr, str, sizeof serv_addr);
        serv_addr[sizeof serv_addr - 1] = '\0'; /* ensure string termination */
        MSG("INFO: server hostname or IP address is configured to \"%s\"\n", serv_addr);
    }

    /* get up and down ports (optional) */
    val = json_object_get_value(conf_obj, "serv_port_up"); //serv_port_up
    if (val != NULL) {
        snprintf(serv_port_up, sizeof serv_port_up, "%u", (uint16_t)json_value_get_number(val));
        MSG("INFO: upstream port is configured to \"%s\"\n", serv_port_up);
    }
    val = json_object_get_value(conf_obj, "serv_port_down"); //serv_port_down
    if (val != NULL) {
        snprintf(serv_port_down, sizeof serv_port_down, "%u", (uint16_t)json_value_get_number(val));
        MSG("INFO: downstream port is configured to \"%s\"\n", serv_port_down);
    }

    str = json_object_get_string(conf_obj, "between_address"); //��һ����ݮ�ɵĵ�ַ
    if (str != NULL) {
        strncpy(between_addr, str, sizeof between_addr);
        between_addr[sizeof between_addr - 1] = '\0'; /* ensure string termination */
        MSG("INFO: between hostname or IP address is configured to \"%s\"\n", between_addr);
    }

    val = json_object_get_value(conf_obj, "between_port_up"); //serv_port_up
    if (val != NULL) {
        snprintf(between_port_up, sizeof between_port_up, "%u", (uint16_t)json_value_get_number(val));
        MSG("INFO: between up port is configured to \"%s\"\n", between_port_up);
    }
    val = json_object_get_value(conf_obj, "between_port_down"); //serv_port_down
    if (val != NULL) {
        snprintf(between_port_down, sizeof between_port_down, "%u", (uint16_t)json_value_get_number(val));
        MSG("INFO: between down port is configured to \"%s\"\n", between_port_down);
    }

    /* get keep-alive interval (in seconds) for downstream (optional) */
    val = json_object_get_value(conf_obj, "keepalive_interval"); //keepalive_time
    if (val != NULL) {
        keepalive_time = (int)json_value_get_number(val);
        MSG("INFO: downstream keep-alive interval is configured to %u seconds\n", keepalive_time);
    }

    /* get interval (in seconds) for statistics display (optional) */
    val = json_object_get_value(conf_obj, "stat_interval"); //stat_interval
    if (val != NULL) {
        stat_interval = (unsigned)json_value_get_number(val);
        MSG("INFO: statistics display interval is configured to %u seconds\n", stat_interval);
    }

    /* get time-out value (in ms) for upstream datagrams (optional) */
    val = json_object_get_value(conf_obj, "push_timeout_ms"); //push_timeout_half.tv_usec/500
    if (val != NULL) {
        push_timeout_half.tv_usec = 500 * (long int)json_value_get_number(val);
        MSG("INFO: upstream PUSH_DATA time-out is configured to %u ms\n", (unsigned)(push_timeout_half.tv_usec / 500));
    }

    /* packet filtering parameters */
    val = json_object_get_value(conf_obj, "forward_crc_valid"); //fwd_valid_pkt
    if (json_value_get_type(val) == JSONBoolean) {
        fwd_valid_pkt = (bool)json_value_get_boolean(val);
    }
    MSG("INFO: packets received with a valid CRC will%s be forwarded\n", (fwd_valid_pkt ? "" : " NOT"));
    val = json_object_get_value(conf_obj, "forward_crc_error"); //fwd_error_pkt
    if (json_value_get_type(val) == JSONBoolean) {
        fwd_error_pkt = (bool)json_value_get_boolean(val);
    }
    MSG("INFO: packets received with a CRC error will%s be forwarded\n", (fwd_error_pkt ? "" : " NOT"));
    val = json_object_get_value(conf_obj, "forward_crc_disabled"); //fwd_nocrc_pkt
    if (json_value_get_type(val) == JSONBoolean) {
        fwd_nocrc_pkt = (bool)json_value_get_boolean(val);
    }
    MSG("INFO: packets received with no CRC will%s be forwarded\n", (fwd_nocrc_pkt ? "" : " NOT"));

    /* GPS module TTY path (optional) */
    str = json_object_get_string(conf_obj, "gps_tty_path"); //gps_tty_path
    if (str != NULL) {
        strncpy(gps_tty_path, str, sizeof gps_tty_path);
        gps_tty_path[sizeof gps_tty_path - 1] = '\0'; /* ensure string termination */
        MSG("INFO: GPS serial port path is configured to \"%s\"\n", gps_tty_path);
    }

    /* get reference coordinates */
    val = json_object_get_value(conf_obj, "ref_latitude"); //reference_coord.lat
    if (val != NULL) {
        reference_coord.lat = (double)json_value_get_number(val);
        MSG("INFO: Reference latitude is configured to %f deg\n", reference_coord.lat);
    }
    val = json_object_get_value(conf_obj, "ref_longitude"); //reference_coord.lon
    if (val != NULL) {
        reference_coord.lon = (double)json_value_get_number(val);
        MSG("INFO: Reference longitude is configured to %f deg\n", reference_coord.lon);
    }
    val = json_object_get_value(conf_obj, "ref_altitude"); //reference_coord.alt
    if (val != NULL) {
        reference_coord.alt = (short)json_value_get_number(val);
        MSG("INFO: Reference altitude is configured to %i meters\n", reference_coord.alt);
    }

    /* Gateway GPS coordinates hardcoding (aka. faking) option */
    val = json_object_get_value(conf_obj, "fake_gps"); //gps_fake_enable���ж�fake_gps�Ƿ�����
    if (json_value_get_type(val) == JSONBoolean) {
        gps_fake_enable = (bool)json_value_get_boolean(val);
        if (gps_fake_enable == true) {
            MSG("INFO: fake GPS is enabled\n");
        }
        else {
            MSG("INFO: fake GPS is disabled\n");
        }
    }

    /* Beacon signal period (optional) */ //���涼��class B beacon�������
    val = json_object_get_value(conf_obj, "beacon_period"); //beacon_period 
    if (val != NULL) {
        beacon_period = (uint32_t)json_value_get_number(val);
        if ((beacon_period > 0) && (beacon_period < 6)) {
            MSG("ERROR: invalid configuration for Beacon period, must be >= 6s\n");
            return -1;
        }
        else {
            MSG("INFO: Beaconing period is configured to %u seconds\n", beacon_period);
        }
    }

    /* Beacon TX frequency (optional) */
    val = json_object_get_value(conf_obj, "beacon_freq_hz"); //beacon_freq_hzs
    if (val != NULL) {
        beacon_freq_hz = (uint32_t)json_value_get_number(val);
        MSG("INFO: Beaconing signal will be emitted at %u Hz\n", beacon_freq_hz);
    }

    /* Number of beacon channels (optional) */
    val = json_object_get_value(conf_obj, "beacon_freq_nb"); //beacon_freq_nb
    if (val != NULL) {
        beacon_freq_nb = (uint8_t)json_value_get_number(val);
        MSG("INFO: Beaconing channel number is set to %u\n", beacon_freq_nb);
    }

    /* Frequency step between beacon channels (optional) */
    val = json_object_get_value(conf_obj, "beacon_freq_step");//beacon_freq_step
    if (val != NULL) {
        beacon_freq_step = (uint32_t)json_value_get_number(val);
        MSG("INFO: Beaconing channel frequency step is set to %uHz\n", beacon_freq_step);
    }

    /* Beacon datarate (optional) */
    val = json_object_get_value(conf_obj, "beacon_datarate"); //beacon_datarate
    if (val != NULL) {
        beacon_datarate = (uint8_t)json_value_get_number(val);
        MSG("INFO: Beaconing datarate is set to SF%d\n", beacon_datarate);
    }

    /* Beacon modulation bandwidth (optional) */
    val = json_object_get_value(conf_obj, "beacon_bw_hz"); //beacon_bw_hz
    if (val != NULL) {
        beacon_bw_hz = (uint32_t)json_value_get_number(val);
        MSG("INFO: Beaconing modulation bandwidth is set to %dHz\n", beacon_bw_hz);
    }

    /* Beacon TX power (optional) */
    val = json_object_get_value(conf_obj, "beacon_power"); //beacon_power
    if (val != NULL) {
        beacon_power = (int8_t)json_value_get_number(val);
        MSG("INFO: Beaconing TX power is set to %ddBm\n", beacon_power);
    }

    /* Beacon information descriptor (optional) */
    val = json_object_get_value(conf_obj, "beacon_infodesc"); //beacon_infodesc
    if (val != NULL) {
        beacon_infodesc = (uint8_t)json_value_get_number(val);
        MSG("INFO: Beaconing information descriptor is set to %u\n", beacon_infodesc);
    }

    /* Auto-quit threshold (optional) */
    val = json_object_get_value(conf_obj, "autoquit_threshold"); //autoquit_threshold
    if (val != NULL) {
        autoquit_threshold = (uint32_t)json_value_get_number(val);
        MSG("INFO: Auto-quit after %u non-acknowledged PULL_DATA\n", autoquit_threshold);
    }

    /* free JSON parsing data structure */
    json_value_free(root_val);
    return 0;
}

static int parse_debug_configuration(const char* conf_file) {
    int i;
    const char conf_obj_name[] = "debug_conf";
    JSON_Value* root_val;
    JSON_Object* conf_obj = NULL;
    JSON_Array* conf_array = NULL;
    JSON_Object* conf_obj_array = NULL;
    const char* str; /* pointer to sub-strings in the JSON data */

    /* Initialize structure */
    memset(&debugconf, 0, sizeof debugconf); //debugconfָ��debug_conf����

    /* try to parse JSON */ //�ж��Ƿ���JSON�ļ�
    root_val = json_parse_file_with_comments(conf_file);
    if (root_val == NULL) {
        MSG("ERROR: %s is not a valid JSON file\n", conf_file);
        exit(EXIT_FAILURE);
    }

    /* point to the gateway configuration object */ //�ж�json�ļ����Ƿ���debug_conf����
    conf_obj = json_object_get_object(json_value_get_object(root_val), conf_obj_name);
    if (conf_obj == NULL) {
        MSG("INFO: %s does not contain a JSON object named %s\n", conf_file, conf_obj_name);
        json_value_free(root_val);
        return -1;
    }
    else {
        MSG("INFO: %s does contain a JSON object named %s, parsing debug parameters\n", conf_file, conf_obj_name);
    }

    /* Get reference payload configuration */
    conf_array = json_object_get_array(conf_obj, "ref_payload"); //�鿴�Ƿ���debug_conf.ref_payload����
    if (conf_array != NULL) {
        debugconf.nb_ref_payload = json_array_get_count(conf_array);
        MSG("INFO: got %u debug reference payload\n", debugconf.nb_ref_payload); //��������16�������ʵ���м���ref_payload

        for (i = 0; i < (int)debugconf.nb_ref_payload; i++) { //���ÿ��ʵ��ref_payload��id
            conf_obj_array = json_array_get_object(conf_array, i);
            /* id */
            str = json_object_get_string(conf_obj_array, "id"); //debugconf.ref_payload[i].id
            if (str != NULL) {
                sscanf(str, "0x%08X", &(debugconf.ref_payload[i].id));
                MSG("INFO: reference payload ID %d is 0x%08X\n", i, debugconf.ref_payload[i].id);
            }

            /* global count */
            nb_pkt_received_ref[i] = 0;
        }
    }

    /* Get log file configuration */
    str = json_object_get_string(conf_obj, "log_file"); //debugconf.log_file_name
    if (str != NULL) {
        strncpy(debugconf.log_file_name, str, sizeof debugconf.log_file_name);
        debugconf.log_file_name[sizeof debugconf.log_file_name - 1] = '\0'; /* ensure string termination */
        MSG("INFO: setting debug log file name to %s\n", debugconf.log_file_name);
    }

    /* Commit configuration */
    if (lgw_debug_setconf(&debugconf) != LGW_HAL_SUCCESS) { //Configure the debug context
        MSG("ERROR: Failed to configure debug\n");
        json_value_free(root_val);
        return -1;
    }

    /* free JSON parsing data structure */
    json_value_free(root_val);
    return 0;
}

static uint16_t crc16(const uint8_t* data, unsigned size) { //��txpkt����crc����
    const uint16_t crc_poly = 0x1021;
    const uint16_t init_val = 0x0000;
    uint16_t x = init_val;
    unsigned i, j;

    if (data == NULL) {
        return 0;
    }

    for (i = 0; i < size; ++i) {
        x ^= (uint16_t)data[i] << 8;
        for (j = 0; j < 8; ++j) {
            x = (x & 0x8000) ? (x << 1) ^ crc_poly : (x << 1);
        }
    }

    return x;
}

static double difftimespec(struct timespec end, struct timespec beginning) { //����ʱ���
    double x;

    x = 1E-9 * (double)(end.tv_nsec - beginning.tv_nsec);
    x += (double)(end.tv_sec - beginning.tv_sec);

    return x;
}



/* -------------------------------------------------------------------------- */
/* --- MAIN FUNCTION -------------------------------------------------------- */

int main(int argc, char** argv)
{
    struct sigaction sigact; /* SIGQUIT&SIGINT&SIGTERM signal handling */
    int i; /* loop variable and temporary variable for return value */
    int x;
    int l, m;

    /* configuration file related */ //�����ļ�
    const char defaut_conf_fname[] = JSON_CONF_DEFAULT;
    const char* conf_fname = defaut_conf_fname; /* pointer to a string we won't touch */

    /* threads */
    pthread_t thrid_up;


    /* network socket creation */ //socket�׽�������ͨ��
    struct addrinfo hints;
    struct addrinfo* result; /* store result of getaddrinfo */
    struct addrinfo* q; /* pointer to move into *result data */
    char host_name[64];
    char port_name[64];

    /* variables to get local copies of measurements */
    uint32_t cp_nb_rx_rcv;
    uint32_t cp_nb_rx_ok;
    uint32_t cp_nb_rx_bad;
    uint32_t cp_nb_rx_nocrc;
    uint32_t cp_up_pkt_fwd;
    uint32_t cp_up_network_byte;
    uint32_t cp_up_payload_byte;
    uint32_t cp_up_dgram_sent;
    uint32_t cp_up_ack_rcv;
    uint32_t cp_dw_pull_sent;
    uint32_t cp_dw_ack_rcv;
    uint32_t cp_dw_dgram_rcv;
    uint32_t cp_dw_network_byte;
    uint32_t cp_dw_payload_byte;
    uint32_t cp_nb_tx_ok;
    uint32_t cp_nb_tx_fail;
    uint32_t cp_nb_tx_requested = 0;
    uint32_t cp_nb_tx_rejected_collision_packet = 0;
    uint32_t cp_nb_tx_rejected_collision_beacon = 0;
    uint32_t cp_nb_tx_rejected_too_late = 0;
    uint32_t cp_nb_tx_rejected_too_early = 0;
    uint32_t cp_nb_beacon_queued = 0;
    uint32_t cp_nb_beacon_sent = 0;
    uint32_t cp_nb_beacon_rejected = 0;

    /* GPS coordinates variables */ //gps����
    bool coord_ok = false;
    struct coord_s cp_gps_coord = { 0.0, 0.0, 0 }; //gps��ַ����

    /* SX1302 data variables */
    uint32_t trig_tstamp;
    uint32_t inst_tstamp;
    uint64_t eui;
    float temperature;

    /* statistics variable */ //ͳ�Ʊ���
    time_t t;
    char stat_timestamp[24]; //NTP time
    float rx_ok_ratio;
    float rx_bad_ratio;
    float rx_nocrc_ratio;
    float up_ack_ratio;
    float dw_ack_ratio;

    /* Parse command line options */ //���������м��
    while ((i = getopt(argc, argv, "hc:")) != -1)
    {
        switch (i)
        {
        case 'h': //./lora_pkt_fwd -h
            usage();
            return EXIT_SUCCESS;
            break;

        case 'c': //./lora_pkt_fwd -c global_conf.json
            conf_fname = optarg;
            break;

        default:
            printf("ERROR: argument parsing options, use -h option for help\n");
            usage();
            return EXIT_FAILURE;
        }
    }

    /* display version informations */ //��ת������������ʾ
    MSG("*** Packet Forwarder ***\nVersion: " VERSION_STRING "\n"); //������ʾ�汾
    MSG("*** SX1302 HAL library version info ***\n%s\n***\n", lgw_version_info());

    /* display host endianness */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    MSG("INFO: Little endian host\n");
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    MSG("INFO: Big endian host\n");
#else
    MSG("INFO: Host endianness unknown\n");
#endif

    /* load configuration files */ //�������������json��������
    if (access(conf_fname, R_OK) == 0) { /* if there is a global conf, parse it  */ //"global_conf.json"
        MSG("INFO: found configuration file %s, parsing it\n", conf_fname);
        x = parse_SX130x_configuration(conf_fname); //����SX130x_conf����
        if (x != 0) {
            exit(EXIT_FAILURE);
        }
        x = parse_gateway_configuration(conf_fname); //����gateway_conf����
        if (x != 0) {
            exit(EXIT_FAILURE);
        }
        x = parse_debug_configuration(conf_fname); //����debug_conf����
        if (x != 0) {
            MSG("INFO: no debug configuration\n");
        }
    }
    else {
        MSG("ERROR: [main] failed to find any configuration file named %s\n", conf_fname);
        exit(EXIT_FAILURE);
    }

    /* Start GPS a.s.a.p., to allow it to lock */ //a.s.a.p = as soon as possible
    if (gps_tty_path[0] != '\0') { /* do not try to open GPS device if no path set */ //�����Ƿ�����gps��Ψһ��׼��TTY: Ӳ���ն��豸��
        i = lgw_gps_enable(gps_tty_path, "ubx7", 0, &gps_tty_fd); /* HAL only supports u-blox 7 for now */ //ublox7��һ������gpsģ��
        if (i != LGW_GPS_SUCCESS) {
            printf("WARNING: [main] impossible to open %s for GPS sync (check permissions)\n", gps_tty_path);
            gps_enabled = false; //gps_enabled��������mainֱ���ж�
            gps_ref_valid = false;  //gps_ref_valid��thread_valid��һ���ж�
        }
        else {
            printf("INFO: [main] TTY port %s open for GPS synchronization\n", gps_tty_path);
            gps_enabled = true;
            gps_ref_valid = false;
        }
    }

    /* get timezone info */
    tzset(); //ʱ����ݺ���

    /* sanity check on configuration variables */
    // TODO

    /* process some of the configuration variables */ //�ֽ���
    net_mac_h = htonl((uint32_t)(0xFFFFFFFF & (lgwm >> 32)));
    net_mac_l = htonl((uint32_t)(0xFFFFFFFF & lgwm));

    /* prepare hints to open network sockets */ //��ΪupstreamҲΪdownstream�����
    memset(&hints, 0, sizeof hints); //hints
    hints.ai_family = AF_INET; //IPV4 /* WA: Forcing IPv4 as AF_UNSPEC(IPV4 and IPV6) makes connection on localhost to fail */
    hints.ai_socktype = SOCK_DGRAM; //UDP
    //hints.ai_protocolȡĬ��ֵ0��ϵͳ���Զ����ݳ�Ӧ��ʹ��UDPЭ��

    /* look for server address w/ upstream port */
    i = getaddrinfo(serv_addr, serv_port_up, &hints, &result);
    //serv_addr��serv_port_up��parse_gateway_configuration�ó�����hints��ȡ��Ϣ�洢��result
    //��ΪIP + port -> socket
    if (i != 0) {
        MSG("ERROR: [up] getaddrinfo on address %s (PORT %s) returned %s\n", serv_addr, serv_port_up, gai_strerror(i));
        exit(EXIT_FAILURE);
    }

    /* try to open socket for upstream traffic */
    for (q = result; q != NULL; q = q->ai_next) { //qָ��result��q�����Զ�������getaddrinfo�õ��ģ���Ϊһ���������ܲ�ֹһ��IP��ַ�����ԣ���Ҫ����res�е�next�����£��Ƿ�����һ���ڵ�
        sock_up = socket(q->ai_family, q->ai_socktype, q->ai_protocol); //�����׽���sock_up
        if (sock_up == -1) continue; /* try next field */
        else break; /* success, get out of loop */ //�õ�sock_up������forѭ����û�б�Ҫѭ������������q==NULL
    }
    if (q == NULL) { //һֱѭ�����˽�������q==NULL��û���sock_up
        MSG("ERROR: [up] failed to open socket to any of server %s addresses (port %s)\n", serv_addr, serv_port_up);
        i = 1;
        for (q = result; q != NULL; q = q->ai_next) {
            getnameinfo(q->ai_addr, q->ai_addrlen, host_name, sizeof host_name, port_name, sizeof port_name, NI_NUMERICHOST);
            //��getaddrinfo������ת: socket -> IP + port
            MSG("INFO: [up] result %i host:%s service:%s\n", i, host_name, port_name);
            ++i;
        }
        exit(EXIT_FAILURE); //�쳣�˳�����
    }

    /* connect so we can send/receive packet with the server only */
    i = connect(sock_up, q->ai_addr, q->ai_addrlen); //��������socket��qΪforѭ��break��ʱ��ֵ
    if (i != 0) {
        MSG("ERROR: [up] connect returned %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result); //�ͷŵ��ý�������ͨ��


    if (com_type == LGW_COM_SPI) {
        /* Board reset */ //ִ��reset_lgw.sh�ű�
        if (system("./reset_lgw.sh start") != 0) {
            printf("ERROR: failed to reset SX1302, check your reset_lgw.sh script\n");
            exit(EXIT_FAILURE);
        }
    }

    for (l = 0; l < LGW_IF_CHAIN_NB; l++) {
        for (m = 0; m < 8; m++) {
            nb_pkt_log[l][m] = 0; //��ʼ��
        }
    }

    /* starting the concentrator */ //���й̶���ʾ�����ڶ���
    i = lgw_start();
    if (i == LGW_HAL_SUCCESS) {
        MSG("INFO: [main] concentrator started, packet can now be received\n");
    }
    else {
        MSG("ERROR: [main] failed to start the concentrator\n");
        exit(EXIT_FAILURE);
    }

    /* get the concentrator EUI */ //Gateway ID\GwEUI��//���й̶���ʾ���һ��
    i = lgw_get_eui(&eui);
    if (i != LGW_HAL_SUCCESS) {
        printf("ERROR: failed to get concentrator EUI\n");
    }
    else {
        printf("INFO: concentrator EUI: 0x%016" PRIx64 "\n", eui);
    }

    /* spawn threads to manage upstream and downstream */
    i = pthread_create(&thrid_up, NULL, (void* (*)(void*))thread_up, NULL);
    if (i != 0) {
        MSG("ERROR: [main] impossible to create upstream thread\n");
        exit(EXIT_FAILURE);
    }

    /* configure signal handling */ //����sig_handler
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = sig_handler;
    sigaction(SIGQUIT, &sigact, NULL); /* Ctrl-\ */
    sigaction(SIGINT, &sigact, NULL); /* Ctrl-C */
    sigaction(SIGTERM, &sigact, NULL); /* default "kill" command */

    /* main loop task : statistics collection */ //����threads�õ���ͳ����Ϣ
    while (!exit_sig && !quit_sig) { //�����ֶ�ֹͣʱ
        /* wait for next reporting interval */
        wait_ms(1000 * stat_interval);

        /* get timestamp for statistics */
        t = time(NULL);
        strftime(stat_timestamp, sizeof stat_timestamp, "%F %T %Z", gmtime(&t)); //get NTP time

        /* access upstream statistics, copy and reset them */ //��������ͳ��
        pthread_mutex_lock(&mx_meas_up);
        cp_nb_rx_rcv = meas_nb_rx_rcv;
        cp_nb_rx_ok = meas_nb_rx_ok;
        cp_nb_rx_bad = meas_nb_rx_bad;
        cp_nb_rx_nocrc = meas_nb_rx_nocrc;
        cp_up_pkt_fwd = meas_up_pkt_fwd;
        cp_up_network_byte = meas_up_network_byte;
        cp_up_payload_byte = meas_up_payload_byte;
        cp_up_dgram_sent = meas_up_dgram_sent;
        cp_up_ack_rcv = meas_up_ack_rcv;
        meas_nb_rx_rcv = 0;
        meas_nb_rx_ok = 0;
        meas_nb_rx_bad = 0;
        meas_nb_rx_nocrc = 0;
        meas_up_pkt_fwd = 0;
        meas_up_network_byte = 0;
        meas_up_payload_byte = 0;
        meas_up_dgram_sent = 0;
        meas_up_ack_rcv = 0;
        pthread_mutex_unlock(&mx_meas_up);
        if (cp_nb_rx_rcv > 0) {
            rx_ok_ratio = (float)cp_nb_rx_ok / (float)cp_nb_rx_rcv;
            rx_bad_ratio = (float)cp_nb_rx_bad / (float)cp_nb_rx_rcv;
            rx_nocrc_ratio = (float)cp_nb_rx_nocrc / (float)cp_nb_rx_rcv;
        }
        else {
            rx_ok_ratio = 0.0;
            rx_bad_ratio = 0.0;
            rx_nocrc_ratio = 0.0;
        }
        if (cp_up_dgram_sent > 0) {
            up_ack_ratio = (float)cp_up_ack_rcv / (float)cp_up_dgram_sent;
        }
        else {
            up_ack_ratio = 0.0;
        }

        /* access downstream statistics, copy and reset them */ //��������ͳ��
        pthread_mutex_lock(&mx_meas_dw);
        //thread_down + thread_jit
        cp_dw_pull_sent = meas_dw_pull_sent;
        cp_dw_ack_rcv = meas_dw_ack_rcv;
        cp_dw_dgram_rcv = meas_dw_dgram_rcv;
        cp_dw_network_byte = meas_dw_network_byte;
        cp_dw_payload_byte = meas_dw_payload_byte;
        cp_nb_tx_ok = meas_nb_tx_ok;
        cp_nb_tx_fail = meas_nb_tx_fail;
        cp_nb_tx_requested += meas_nb_tx_requested;
        cp_nb_tx_rejected_collision_packet += meas_nb_tx_rejected_collision_packet;
        cp_nb_tx_rejected_collision_beacon += meas_nb_tx_rejected_collision_beacon;
        cp_nb_tx_rejected_too_late += meas_nb_tx_rejected_too_late;
        cp_nb_tx_rejected_too_early += meas_nb_tx_rejected_too_early;
        cp_nb_beacon_queued += meas_nb_beacon_queued;
        cp_nb_beacon_sent += meas_nb_beacon_sent;
        cp_nb_beacon_rejected += meas_nb_beacon_rejected;
        meas_dw_pull_sent = 0;
        meas_dw_ack_rcv = 0;
        meas_dw_dgram_rcv = 0;
        meas_dw_network_byte = 0;
        meas_dw_payload_byte = 0;
        meas_nb_tx_ok = 0;
        meas_nb_tx_fail = 0;
        meas_nb_tx_requested = 0;
        meas_nb_tx_rejected_collision_packet = 0;
        meas_nb_tx_rejected_collision_beacon = 0;
        meas_nb_tx_rejected_too_late = 0;
        meas_nb_tx_rejected_too_early = 0;
        meas_nb_beacon_queued = 0;
        meas_nb_beacon_sent = 0;
        meas_nb_beacon_rejected = 0;
        pthread_mutex_unlock(&mx_meas_dw);
        if (cp_dw_pull_sent > 0) {
            dw_ack_ratio = (float)cp_dw_ack_rcv / (float)cp_dw_pull_sent;
        }
        else {
            dw_ack_ratio = 0.0;
        }

        //GPS�Ƿ����öԺ��汨����ʾ��Ӱ��
        /* access GPS statistics, copy them */ //��gps
        if (gps_enabled == true) {
            pthread_mutex_lock(&mx_meas_gps);
            coord_ok = gps_coord_valid; //reference����
            cp_gps_coord = meas_gps_coord;
            pthread_mutex_unlock(&mx_meas_gps);
        }

        /* overwrite with reference coordinates if function is enabled */ //fake gps
        if (gps_fake_enable == true) {
            cp_gps_coord = reference_coord; //����global.config���reference_coord��Ϊfake gps location
        }

        /* display a report */
        printf("\n##### %s #####\n", stat_timestamp); //stat_timestamp��Ŀǰ��NTPʱ��
        printf("### [UPSTREAM] ###\n");
        printf("# RF packets received by concentrator: %u\n", cp_nb_rx_rcv);
        printf("# CRC_OK: %.2f%%, CRC_FAIL: %.2f%%, NO_CRC: %.2f%%\n", 100.0 * rx_ok_ratio, 100.0 * rx_bad_ratio, 100.0 * rx_nocrc_ratio);
        printf("# RF packets forwarded: %u (%u bytes)\n", cp_up_pkt_fwd, cp_up_payload_byte);
        printf("# PUSH_DATA datagrams sent: %u (%u bytes)\n", cp_up_dgram_sent, cp_up_network_byte);
        printf("# PUSH_DATA acknowledged: %.2f%%\n", 100.0 * up_ack_ratio);
        printf("### [DOWNSTREAM] ###\n");
        printf("# PULL_DATA sent: %u (%.2f%% acknowledged)\n", cp_dw_pull_sent, 100.0 * dw_ack_ratio);
        printf("# PULL_RESP(onse) datagrams received: %u (%u bytes)\n", cp_dw_dgram_rcv, cp_dw_network_byte);
        printf("# RF packets sent to concentrator: %u (%u bytes)\n", (cp_nb_tx_ok + cp_nb_tx_fail), cp_dw_payload_byte);
        printf("# TX errors: %u\n", cp_nb_tx_fail);
        if (cp_nb_tx_requested != 0) {
            printf("# TX rejected (collision packet): %.2f%% (req:%u, rej:%u)\n", 100.0 * cp_nb_tx_rejected_collision_packet / cp_nb_tx_requested, cp_nb_tx_requested, cp_nb_tx_rejected_collision_packet);
            printf("# TX rejected (collision beacon): %.2f%% (req:%u, rej:%u)\n", 100.0 * cp_nb_tx_rejected_collision_beacon / cp_nb_tx_requested, cp_nb_tx_requested, cp_nb_tx_rejected_collision_beacon);
            printf("# TX rejected (too late): %.2f%% (req:%u, rej:%u)\n", 100.0 * cp_nb_tx_rejected_too_late / cp_nb_tx_requested, cp_nb_tx_requested, cp_nb_tx_rejected_too_late);
            printf("# TX rejected (too early): %.2f%% (req:%u, rej:%u)\n", 100.0 * cp_nb_tx_rejected_too_early / cp_nb_tx_requested, cp_nb_tx_requested, cp_nb_tx_rejected_too_early);
        }
        printf("### SX1302 Status ###\n");
        pthread_mutex_lock(&mx_concent);
        i = lgw_get_instcnt(&inst_tstamp); //SX1302�³��ģ�SX1301û�У�Get concentrator count����jit_enqueue/jit_peek
        i |= lgw_get_trigcnt(&trig_tstamp); //|=��λ��Get PPM pulse concentrator count����lgw_gps_sync
        //�ܵ���˵��count��upstream�õ�������downstream
        pthread_mutex_unlock(&mx_concent);
        if (i != LGW_HAL_SUCCESS) { //lgw_get_instcnt��lgw_get_trigcnt������һ��δ�ɹ�
            printf("# SX1302 counter unknown\n");
        }
        else {
            printf("# SX1302 counter (INST): %u\n", inst_tstamp); //SX1302�³��ģ�SX1301û��
            printf("# SX1302 counter (PPS):  %u\n", trig_tstamp);
        }
        printf("# BEACON queued: %u\n", cp_nb_beacon_queued);
        printf("# BEACON sent so far: %u\n", cp_nb_beacon_sent);
        printf("# BEACON rejected: %u\n", cp_nb_beacon_rejected);
        printf("### [JIT] ###\n");
        /* get timestamp captured on PPM pulse  */
        jit_print_queue(&jit_queue[0], false, DEBUG_LOG);
        printf("#--------\n");
        jit_print_queue(&jit_queue[1], false, DEBUG_LOG);
        printf("### [GPS] ###\n");
        if (gps_enabled == true) { //����gps
            /* no need for mutex, display is not critical */
            if (gps_ref_valid == true) { //thread_valid����gps�ο�ʱ��
                printf("# Valid time reference (age: %li sec)\n", (long)difftime(time(NULL), time_reference_gps.systime));
            }
            else {
                printf("# Invalid time reference (age: %li sec)\n", (long)difftime(time(NULL), time_reference_gps.systime));
            }
            if (coord_ok == true) { //����gps��ַ
                printf("# GPS coordinates: latitude %.5f, longitude %.5f, altitude %i m\n", cp_gps_coord.lat, cp_gps_coord.lon, cp_gps_coord.alt);
            }
            else {
                printf("# no valid GPS coordinates available yet\n");
            }
        }
        else if (gps_fake_enable == true) {  //���ü�gps
            printf("# GPS *FAKE* coordinates: latitude %.5f, longitude %.5f, altitude %i m\n", cp_gps_coord.lat, cp_gps_coord.lon, cp_gps_coord.alt);
        }
        else { //δ����gps
            printf("# GPS sync is disabled\n");
        }
        pthread_mutex_lock(&mx_concent);
        i = lgw_get_temperature(&temperature); //�¶�
        pthread_mutex_unlock(&mx_concent);
        if (i != LGW_HAL_SUCCESS) {
            printf("### Concentrator temperature unknown ###\n");
        }
        else {
            printf("### Concentrator temperature: %.0f C ###\n", temperature);
        }
        printf("##### END #####\n");


        pthread_mutex_unlock(&mx_stat_rep);
    }

    //�ֶ�ֹͣ�����
    /* wait for all threads with a COM with the concentrator board to finish (1 fetch cycle max) */
    i = pthread_join(thrid_up, NULL); //�������ķ�ʽ�ȴ�thread_up����
    if (i != 0) {
        printf("ERROR: failed to join upstream thread with %d - %s\n", i, strerror(errno));
    }

    /* if an exit signal was received, try to quit properly */ //�رճ���
    if (exit_sig) {
        /* shut down network sockets */
        shutdown(sock_up, SHUT_RDWR);
        /* stop the hardware */
        i = lgw_stop();
        if (i == LGW_HAL_SUCCESS) {
            MSG("INFO: concentrator stopped successfully\n");
        }
        else {
            MSG("WARNING: failed to stop concentrator successfully\n");
        }
    }

    if (com_type == LGW_COM_SPI) {
        /* Board reset */
        if (system("./reset_lgw.sh stop") != 0) {
            printf("ERROR: failed to reset SX1302, check your reset_lgw.sh script\n");
            exit(EXIT_FAILURE);
        }
    }

    MSG("INFO: Exiting packet forwarder program\n");
    exit(EXIT_SUCCESS);
}

/* -------------------------------------------------------------------------- */
/* --- THREAD 1: RECEIVING PACKETS AND FORWARDING THEM ---------------------- */

void thread_up(void) { //PUSH_DATA packet
    int i, j, k; /* loop variables */
    unsigned pkt_in_dgram; /* nb on Lora packet in the current datagram */
    char stat_timestamp[24]; //NTP time
    time_t t;

    /* allocate memory for packet fetching and processing */
    struct lgw_pkt_rx_s rxpkt[NB_PKT_MAX]; /* array containing inbound packets + metadata */
    struct lgw_pkt_rx_s* p; /* pointer on a RX packet */ //Gateway��Node���յ������ݰ�����ָrxpkt
    int nb_pkt; //number of packets retrieved

    /* local copy of GPS time reference */
    bool ref_ok = false; /* determine if GPS time reference must be used or not */ //GPS reference
    struct tref local_ref; /* time reference used for UTC <-> timestamp conversion */ //�ṩʱ��ת���Ĳο�

    /* data buffers */
    uint8_t buff_up[TX_BUFF_SIZE]; /* buffer to compose the upstream packet */
    int buff_index;
    uint8_t buff_ack[32]; /* buffer to receive acknowledges */

    /* protocol variables */
    uint8_t token_h; /* random token for acknowledgement matching */ //����PUSH_ACK
    uint8_t token_l; /* random token for acknowledgement matching */

    /* ping measurement variables */
    struct timespec send_time;
    struct timespec recv_time;

    /* GPS synchronization variables */
    struct timespec pkt_utc_time; //UTC absolute times
    struct tm* x; /* broken-up UTC time */
    struct timespec pkt_gps_time; //GPS absolute time
    uint64_t pkt_gps_time_ms;

    /* report management variable */ //stat
    bool send_report = false;

    /* mote info variables */ //End Device
    uint32_t mote_addr = 0; //unsigned int 
    uint16_t mote_fcnt = 0;

    /* set upstream socket RX timeout */ //����ʱ���׽��ֳ�ʱ
    i = setsockopt(sock_up, SOL_SOCKET, SO_RCVTIMEO, (void*)&push_timeout_half, sizeof push_timeout_half);
    if (i != 0) {
        MSG("ERROR: [up] setsockopt returned %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* pre-fill the data buffer with fixed fields */ //PUSH_DATA packet
    buff_up[0] = PROTOCOL_VERSION; //protocol version = 2
    buff_up[3] = PKT_PUSH_DATA; //PUSH_DATA identifier 0x00
    *(uint32_t*)(buff_up + 4) = net_mac_h; //Gateway unique identifier (MAC address)
    *(uint32_t*)(buff_up + 8) = net_mac_l;

    while (!exit_sig && !quit_sig) { //δ�õ��˳��źž�һֱ����

        /* fetch packets */
        pthread_mutex_lock(&mx_concent);
        nb_pkt = lgw_receive(NB_PKT_MAX, rxpkt); //��Node�������NB_PKT_MAX�����ݰ���rxpkt pointer to an array of struct that will receive the packet metadata and payload pointers
        pthread_mutex_unlock(&mx_concent);
        if (nb_pkt == LGW_HAL_ERROR) {
            MSG("ERROR: [up] failed packet fetch, exiting\n"); //ͬʱ��������lora_pkt_fwd����
            exit(EXIT_FAILURE);
        }

        /* check if there are status report to send */
        send_report = report_ready; /* copy the variable so it doesn't change mid-function */
        /* no mutex, we're only reading */

        /* wait a short time if no packets, nor status report */
        if ((nb_pkt == 0) && (send_report == false)) {
            wait_ms(FETCH_SLEEP_MS); //�˳�����ѭ��
            continue;
        }

        /* get a copy of GPS time reference (avoid 1 mutex per packet) */ //ΪPacket RX time (GPS based)��׼��
        if ((nb_pkt > 0) && (gps_enabled == true)) { //Ҫ���gps reference time��������gps
            pthread_mutex_lock(&mx_timeref);
            ref_ok = gps_ref_valid; //thread_valid����gps reference
            local_ref = time_reference_gps; //thread_gps���ʱ��ת���Ĳο�
            pthread_mutex_unlock(&mx_timeref);
        }
        else {
            ref_ok = false;
        }

        /* get timestamp for statistics */ //ʱ�����ֻ��DEBUG������
        t = time(NULL);
        strftime(stat_timestamp, sizeof stat_timestamp, "%F %T %Z", gmtime(&t)); //get NTP time
        MSG_DEBUG(DEBUG_PKT_FWD, "\nCurrent time: %s \n", stat_timestamp); //stat_timestamp��Ŀǰ��NTPʱ�䣻ֻ��DEBUG��������

        /* start composing datagram with the header */
        token_h = (uint8_t)rand(); /* random token */ //random token
        token_l = (uint8_t)rand(); /* random token */
        buff_up[1] = token_h;
        buff_up[2] = token_l;
        buff_index = 12; /* 12-byte header */

        /* start of JSON structure */ //Upstream JSON data structure //datagram
        memcpy((void*)(buff_up + buff_index), (void*)"{\"rxpk\":[", 9);
        buff_index += 9;

        /* serialize Lora packets metadata and payload */
        pkt_in_dgram = 0; //��ʼ��datagram�е�packet����Ϊ0��packet���г�datagram��ÿ��{}��һ��packet��[...]��һ��datagram
        for (i = 0; i < nb_pkt; ++i) {
            p = &rxpkt[i]; //rxpkt[i]���յ��ĵ�i�����ݰ�datagram

            //�ɽ��ղ��������߽���ȶԿɵõ�p->payload = PHYPayload��ֻ�����߽���Ҳ���Եõ�
            /* Get mote information from current packet (addr, fcnt) */ //mote information��end device����Ϣ
            /* FHDR - DevAddr */ //mote_addr: DevAddr[4 bytes]
            //�ֽ���Little Endian to Big Endian
            if (p->size >= 8) {
                mote_addr = p->payload[1];  //��PHYPayload�ĵڶ����ֽڿ�ʼ��Ҳ����MACPayload��ʼ
                mote_addr |= p->payload[2] << 8;
                mote_addr |= p->payload[3] << 16;
                mote_addr |= p->payload[4] << 24;
                /* FHDR - FCnt */ //mote_fcnt: FCnt[2 bytes]: �ն˽ڵ���������/���������ã�����ֵ�ɽڵ㷢�ʹ����������������޹�
                mote_fcnt = p->payload[6];
                mote_fcnt |= p->payload[7] << 8;
            }
            else {
                mote_addr = 0;
                mote_fcnt = 0;
            }

            /* basic packet filtering */ //crc�ж�
            pthread_mutex_lock(&mx_meas_up);
            meas_nb_rx_rcv += 1;
            switch (p->status) {
            case STAT_CRC_OK:
                meas_nb_rx_ok += 1;
                if (!fwd_valid_pkt) { //������¸���gobal.config����ʹcrc��ȷҲ��ת����NS
                    pthread_mutex_unlock(&mx_meas_up);
                    continue; /* skip that packet */ //������switch����һ��ѭ����,switch�г��ֵ�continue��ʵ�Ƕ����ѭ�������Ƶ�
                }
                break; //switch-break
            case STAT_CRC_BAD:
                meas_nb_rx_bad += 1;
                if (!fwd_error_pkt) {
                    pthread_mutex_unlock(&mx_meas_up);
                    continue; /* skip that packet */
                }
                break;
            case STAT_NO_CRC:
                meas_nb_rx_nocrc += 1;
                if (!fwd_nocrc_pkt) {
                    pthread_mutex_unlock(&mx_meas_up);
                    continue; /* skip that packet */
                }
                break;
            default:
                MSG("WARNING: [up] received packet with unknown status %u (size %u, modulation %u, BW %u, DR %u, RSSI %.1f)\n", p->status, p->size, p->modulation, p->bandwidth, p->datarate, p->rssic);
                pthread_mutex_unlock(&mx_meas_up);
                continue; /* skip that packet */
                // exit(EXIT_FAILURE);
            }

            //ֻ�п���ת�������ݰ����ܳ�����json up��Ϣ��
            meas_up_pkt_fwd += 1; //RF packets forwarded
            meas_up_payload_byte += p->size; //RF packets forwarded size: ���߽���õ���LoRa���ṹ�е�PHYPayload��С
            pthread_mutex_unlock(&mx_meas_up);
            printf("\nINFO: Received pkt from mote: %08X (fcnt=%u)\n", mote_addr, mote_fcnt);

            /* Start of packet, add inter-packet separator if necessary */ //{"jver":*,...,"data":""}Ϊһ��packet
            if (pkt_in_dgram == 0) { //forѭ���ĵ�һ��ѭ������datagram��ĵ�һ��packet
                buff_up[buff_index] = '{';
                ++buff_index;
            }
            else { //inter-packet separator //JSON up: {"rxpk":[{},...,{}]}��ÿ��{}��һ��packet�������crc_bad��no_crc����ת��ʱ����
                buff_up[buff_index] = ',';
                buff_up[buff_index + 1] = '{';
                buff_index += 2;
                //JSON up: {"rxpk":[{"jver":1,"tmst":21762225,"time":"2021-02-11T15:43:12.000000000Z","tmms":1297093392000,"chan":4,"rfch":1,"freq":487.100000,"mid": 9,"stat":1,"modu":"LORA","datr":"SF7BW125","codr":"4/5","rssis":-66,"lsnr":-7.2,"foff":23844,"rssi":-61,"size":18,"data":"QHbECwCAAAACG3rABSmarECY"},
                                  //{"jver":1,"tmst":21762336,"time":"2021-02-11T15:43:12.000000000Z","tmms":1297093392000,"chan":2,"rfch":0,"freq":486.700000,"mid": 8,"stat":1,"modu":"LORA","datr":"SF7BW125","codr":"4/5","rssis":-3,"lsnr":13.2,"foff":-226,"rssi":-3,"size":18,"data":"QHbECwCAAAACG3rABSmarECY"}]}
            }

            /* JSON rxpk frame format version, 8 useful chars */ //Upstream JSON data structure
            j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, "\"jver\":%d", PROTOCOL_JSON_RXPK_FRAME_FORMAT);
            if (j > 0) {
                buff_index += j;
            }
            else {
                MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                exit(EXIT_FAILURE);
            }

            /* RAW timestamp, 8-17 useful chars */ //�ڲ��������ʱ���timestamp
            //��tmst������ӵ�buff_up
            j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"tmst\":%u", p->count_us);
            if (j > 0) {
                buff_index += j;
            }
            else {
                MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                exit(EXIT_FAILURE);
            }

            /* Packet RX time (GPS based), 37 useful chars */ //��þ���ʱ��
            if (ref_ok == true) { //get a copy of GPS time reference����Ҫ�ɹ�
                /* convert packet timestamp to UTC absolute time */ //timestamp -> UTC time����p->count_us -> pkt_utc_time
                j = lgw_cnt2utc(local_ref, p->count_us, &pkt_utc_time);
                if (j == LGW_GPS_SUCCESS) {
                    /* split the UNIX timestamp to its calendar components */
                    x = gmtime(&(pkt_utc_time.tv_sec));
                    //convert a SX1302 counter value to GPS UTC time when we receive an uplink, in order to fill the ��time�� field of JSON ��rxpk�� structure
                    j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"time\":\"%04i-%02i-%02iT%02i:%02i:%02i.%06liZ\"", (x->tm_year) + 1900, (x->tm_mon) + 1, x->tm_mday, x->tm_hour, x->tm_min, x->tm_sec, (pkt_utc_time.tv_nsec) / 1000); /* ISO 8601 format */
                    if (j > 0) {
                        buff_index += j;
                    }
                    else {
                        MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                        exit(EXIT_FAILURE);
                    }
                }
                /* convert packet timestamp to GPS absolute time */ //timestamp -> GPS time����p->count_us -> pkt_gps_time
                j = lgw_cnt2gps(local_ref, p->count_us, &pkt_gps_time);
                if (j == LGW_GPS_SUCCESS) {
                    pkt_gps_time_ms = pkt_gps_time.tv_sec * 1E3 + pkt_gps_time.tv_nsec / 1E6;
                    //��tmms������ӵ�buff_up
                    j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"tmms\":%" PRIu64 "", pkt_gps_time_ms); /* GPS time in milliseconds since 06.Jan.1980 */
                    if (j > 0) {
                        buff_index += j;
                    }
                    else {
                        MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                        exit(EXIT_FAILURE);
                    }
                }
            }

            //Transfer NTP time to UTC time without GPS

            char time0[100];
            strftime(time0, sizeof time0, "\"%Y-%m-%dT%H:%M:%S.000000000Z\"", gmtime(&t)); //https://stackoverflow.com/a/63574158/12650926: setup UTC time from NTP server in C, fixed millisecond part (����Ҳû��)

            j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"time\":%s", time0); //stat_timestamp���ַ������ͣ���ʹ������ɹ�������buff_upҲ����Ϊ����ISO 8601��ʽ������NS���գ��Ӷ�û��PUSH_ACK����
            if (j > 0) {
                buff_index += j;
            }
            else {
                MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                exit(EXIT_FAILURE);
            }

            //Transfer NTP time to UTC time without GPS ������
            //char time0[100] = "\"";
            //char time1[100];
            //char time2[100];
            //char time3[100] = ".000000000Z\""; //���벿��Ū���̶�ֵ����������Ҫ��Ҳ�ò���

            //sscanf(stat_timestamp, "%s", time1); //���ַ�������ת��ΪISO 8601��ʽ
            //sscanf(stat_timestamp, "%*s%s", time2);
            //strcat(time0,time1);
            //strcat(time0,"T");
            //strcat(time0,time2);
            //strcat(time0,time3);
            //j = snprintf((char *)(buff_up + buff_index), TX_BUFF_SIZE-buff_index, ",\"time\":%s", time0); //stat_timestamp���ַ������ͣ���ʹ������ɹ�������buff_upҲ����Ϊ����ISO 8601��ʽ������NS���գ��Ӷ�û��PUSH_ACK����



            //Transfer NTP time to GPS timestampe without GPS

            uint64_t UNIXtimestamp = (unsigned)time(NULL); //get UNIX timestamp: https://stackoverflow.com/questions/11765301/how-do-i-get-the-unix-timestamp-in-c-as-an-int
            uint64_t GPStimestamp = UNIXtimestamp * 1000 - UNIX_GPS_EPOCH_OFFSET_MILLISECOND; //convert UNIX timestamp to GPS timestamp: ���벻Ҫ�ˣ�����Ҳ����Ҫ
            j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"tmms\":%llu", GPStimestamp);
            if (j > 0) {
                buff_index += j;
            }
            else {
                MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                exit(EXIT_FAILURE);
            }

            /* Fine timestamp */
            if (p->ftime_received == true) {
                j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"ftime\":%u", p->ftime);
                if (j > 0) {
                    buff_index += j;
                }
                else {
                    MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                    exit(EXIT_FAILURE);
                }
            }

            /* Packet concentrator channel, RF chain & RX frequency, 34-36 useful chars */ //Packet concentrator channel = IF chain
            j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"chan\":%1u,\"rfch\":%1u,\"freq\":%.6lf,\"mid\":%2u", p->if_chain, p->rf_chain, ((double)p->freq_hz / 1e6), p->modem_id);
            if (j > 0) {
                buff_index += j;
            }
            else {
                MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                exit(EXIT_FAILURE);
            }

            /* Packet status, 9-10 useful chars */ //����ֱ�ӵõ���pkt.payload_crc_errorֵ�ж϶��� (������crc16����)
            switch (p->status) {
            case STAT_CRC_OK:
                memcpy((void*)(buff_up + buff_index), (void*)",\"stat\":1", 9);
                buff_index += 9;
                break;
            case STAT_CRC_BAD:
                memcpy((void*)(buff_up + buff_index), (void*)",\"stat\":-1", 10);
                buff_index += 10;
                break;
            case STAT_NO_CRC:
                memcpy((void*)(buff_up + buff_index), (void*)",\"stat\":0", 9);
                buff_index += 9;
                break;
            default:
                MSG("ERROR: [up] received packet with unknown status 0x%02X\n", p->status);
                memcpy((void*)(buff_up + buff_index), (void*)",\"stat\":?", 9);
                buff_index += 9;
                exit(EXIT_FAILURE);
            }

            //copy sx1302_parse����
            uint16_t payload_crc16_calc;
            payload_crc16_calc = sx1302_lora_payload_crc_copy(p->payload, p->size);
            if (payload_crc16_calc != p->crc) {
                printf("ERROR: Payload CRC16 check failed (got:0x%04X calc:0x%04X)\n", p->crc, payload_crc16_calc); //p->crc����ֱ�ӵõ���pkt.rx_crc16_value��ֵ���� (���crc16����ʧ�ܽ�������ֵ��ֻ��pkt.payload_crc_error=0�Ž��н�ȫ�Լ�����)
            }
            else {
                printf("Payload CRC check OK (0x%04X)\n", p->crc);
            }


            /* Packet modulation, 13-14 useful chars */
            if (p->modulation == MOD_LORA) {
                memcpy((void*)(buff_up + buff_index), (void*)",\"modu\":\"LORA\"", 14);
                buff_index += 14;

                /* Lora datarate & bandwidth, 16-19 useful chars */
                switch (p->datarate) {
                case DR_LORA_SF5:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"datr\":\"SF5", 12);
                    buff_index += 12;
                    break;
                case DR_LORA_SF6:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"datr\":\"SF6", 12);
                    buff_index += 12;
                    break;
                case DR_LORA_SF7:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"datr\":\"SF7", 12);
                    buff_index += 12;
                    break;
                case DR_LORA_SF8:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"datr\":\"SF8", 12);
                    buff_index += 12;
                    break;
                case DR_LORA_SF9:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"datr\":\"SF9", 12);
                    buff_index += 12;
                    break;
                case DR_LORA_SF10:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"datr\":\"SF10", 13);
                    buff_index += 13;
                    break;
                case DR_LORA_SF11:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"datr\":\"SF11", 13);
                    buff_index += 13;
                    break;
                case DR_LORA_SF12:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"datr\":\"SF12", 13);
                    buff_index += 13;
                    break;
                default:
                    MSG("ERROR: [up] lora packet with unknown datarate 0x%02X\n", p->datarate);
                    memcpy((void*)(buff_up + buff_index), (void*)",\"datr\":\"SF?", 12);
                    buff_index += 12;
                    exit(EXIT_FAILURE);
                }
                switch (p->bandwidth) {
                case BW_125KHZ:
                    memcpy((void*)(buff_up + buff_index), (void*)"BW125\"", 6);
                    buff_index += 6;
                    break;
                case BW_250KHZ:
                    memcpy((void*)(buff_up + buff_index), (void*)"BW250\"", 6);
                    buff_index += 6;
                    break;
                case BW_500KHZ:
                    memcpy((void*)(buff_up + buff_index), (void*)"BW500\"", 6);
                    buff_index += 6;
                    break;
                default:
                    MSG("ERROR: [up] lora packet with unknown bandwidth 0x%02X\n", p->bandwidth);
                    memcpy((void*)(buff_up + buff_index), (void*)"BW?\"", 4);
                    buff_index += 4;
                    exit(EXIT_FAILURE);
                }

                /* Packet ECC coding rate, 11-13 useful chars */ //������/������
                switch (p->coderate) {
                case CR_LORA_4_5:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"codr\":\"4/5\"", 13);
                    buff_index += 13;
                    break;
                case CR_LORA_4_6:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"codr\":\"4/6\"", 13);
                    buff_index += 13;
                    break;
                case CR_LORA_4_7:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"codr\":\"4/7\"", 13);
                    buff_index += 13;
                    break;
                case CR_LORA_4_8:
                    memcpy((void*)(buff_up + buff_index), (void*)",\"codr\":\"4/8\"", 13);
                    buff_index += 13;
                    break;
                case 0: /* treat the CR0 case (mostly false sync) */
                    memcpy((void*)(buff_up + buff_index), (void*)",\"codr\":\"OFF\"", 13);
                    buff_index += 13;
                    break;
                default:
                    MSG("ERROR: [up] lora packet with unknown coderate 0x%02X\n", p->coderate);
                    memcpy((void*)(buff_up + buff_index), (void*)",\"codr\":\"?\"", 11);
                    buff_index += 11;
                    exit(EXIT_FAILURE);
                }

                /* Signal RSSI, payload size */
                j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"rssis\":%.0f", roundf(p->rssis));
                if (j > 0) {
                    buff_index += j;
                }
                else {
                    MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                    exit(EXIT_FAILURE);
                }

                /* Lora SNR */
                j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"lsnr\":%.1f", p->snr);
                if (j > 0) {
                    buff_index += j;
                }
                else {
                    MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                    exit(EXIT_FAILURE);
                }

                /* Lora frequency offset */
                j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"foff\":%d", p->freq_offset);
                if (j > 0) {
                    buff_index += j;
                }
                else {
                    MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                    exit(EXIT_FAILURE);
                }
            }
            else if (p->modulation == MOD_FSK) {
                memcpy((void*)(buff_up + buff_index), (void*)",\"modu\":\"FSK\"", 13);
                buff_index += 13;

                /* FSK datarate, 11-14 useful chars */ //Ƶ��ƫ�Ƶ���/Ƶ�Ƽ���
                j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"datr\":%u", p->datarate);
                if (j > 0) {
                    buff_index += j;
                }
                else {
                    MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                    exit(EXIT_FAILURE);
                }
            }
            else {
                MSG("ERROR: [up] received packet with unknown modulation 0x%02X\n", p->modulation);
                exit(EXIT_FAILURE);
            }

            /* Channel RSSI, payload size, 18-23 useful chars */
            j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, ",\"rssi\":%.0f,\"size\":%u", roundf(p->rssic), p->size);
            if (j > 0) {
                buff_index += j;
            }
            else {
                MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 4));
                exit(EXIT_FAILURE);
            }

            printf("PHYPayload: "); //�ճ�test_loragw_hal_rx��Ĵ�����ȷ�����͵�p->payload = PHYPayload
            for (int count = 0; count < p->size; count++) {
                printf("%02X", p->payload[count]);
            }
            printf("\n");

            /* Packet base64-encoded payload, 14-350 useful chars */ //base64����
            memcpy((void*)(buff_up + buff_index), (void*)",\"data\":\"", 9);
            buff_index += 9;;

            //rxpkt->payloadʹ��base64����: p->payload (PHYPayload) �� data
            j = bin_to_b64(p->payload, p->size, (char*)(buff_up + buff_index), 341); /* 255 bytes = 340 chars in b64 + null char */

            if (j >= 0) {
                buff_index += j;
            }
            else {
                MSG("ERROR: [up] bin_to_b64 failed line %u\n", (__LINE__ - 5));
                exit(EXIT_FAILURE);
            }
            buff_up[buff_index] = '"';
            ++buff_index;

            /* End of packet serialization */ //packet����
            buff_up[buff_index] = '}'; //�����������}����{"jver":*,...,"data":""}Ϊһ��packet
            ++buff_index;
            ++pkt_in_dgram; //{"rxpk":[{},...,{}]}��ÿ��{}��һ��packet

            //��¼����
            if (p->modulation == MOD_LORA) { //LoRa����
                /* Log nb of packets per channel, per SF */
                nb_pkt_log[p->if_chain][p->datarate - 5] += 1; //SF:5~12������(SF-5):0~7
                nb_pkt_received_lora += 1;

                /* Log nb of packets for ref_payload (DEBUG) */ //Total number of LoRa packet received
                for (k = 0; k < debugconf.nb_ref_payload; k++) {
                    if ((p->payload[0] == (uint8_t)(debugconf.ref_payload[k].id >> 24)) &&
                        (p->payload[1] == (uint8_t)(debugconf.ref_payload[k].id >> 16)) &&
                        (p->payload[2] == (uint8_t)(debugconf.ref_payload[k].id >> 8)) &&
                        (p->payload[3] == (uint8_t)(debugconf.ref_payload[k].id >> 0))) {
                        nb_pkt_received_ref[k] += 1;
                    }
                }
            }
            else if (p->modulation == MOD_FSK) { //FSK����
                nb_pkt_log[p->if_chain][0] += 1; //������Concentrator "IF" channel    = if_chain��SF=5ʱ���յ�
                nb_pkt_received_fsk += 1;
            }
        }


        /* DEBUG: print the number of packets received per channel and per SF */ //������DEBUG����ʾ�����¼�Ľ���
        {
            int l, m;
            MSG_PRINTF(DEBUG_PKT_FWD, "\n");
            for (l = 0; l < (LGW_IF_CHAIN_NB - 1); l++) { //if chain: 0~8, sf: 5~12
                MSG_PRINTF(DEBUG_PKT_FWD, "CH%d: ", l);
                for (m = 0; m < 8; m++) {
                    MSG_PRINTF(DEBUG_PKT_FWD, "\t%d", nb_pkt_log[l][m]);
                }
                MSG_PRINTF(DEBUG_PKT_FWD, "\n");
            }
            MSG_PRINTF(DEBUG_PKT_FWD, "FSK: \t%d", nb_pkt_log[9][0]); ////if chain "FSK channel": 9, sf:5
            MSG_PRINTF(DEBUG_PKT_FWD, "\n");
            MSG_PRINTF(DEBUG_PKT_FWD, "Total number of LoRa packet received: %u\n", nb_pkt_received_lora);
            MSG_PRINTF(DEBUG_PKT_FWD, "Total number of FSK packet received: %u\n", nb_pkt_received_fsk);
            for (l = 0; l < debugconf.nb_ref_payload; l++) {
                MSG_PRINTF(DEBUG_PKT_FWD, "Total number of LoRa packet received from 0x%08X: %u\n", debugconf.ref_payload[l].id, nb_pkt_received_ref[l]);
            }
        }

        /* restart fetch sequence without sending empty JSON if all packets have been filtered out */  //�ж��Ƿ������ݰ�
        if (pkt_in_dgram == 0) { //����packet��û��ͨ��CRCУ��
            if (send_report == true) { //ֻ��"stat"
                /* need to clean up the beginning of the payload */
                buff_index -= 8; /* removes "rxpk":[ */
            }
            else { //ʲô������
             /* all packet have been filtered out and no report, restart loop */
                continue; //�˳���ѭ������ֹͣ���Ǹ�
            }
        }
        else { //��packet��ʱ��ֻ�� "rxpk"
         /* end of packet array */
            buff_up[buff_index] = ']'; //����������ģ����һ�������ڶ����]
            ++buff_index;
            /* add separator if needed */
            if (send_report == true) { //��packet��ʱ��ȷ�"rxpk"�ַ�"stat"
                buff_up[buff_index] = ','; //�����������}���һ��,
                ++buff_index;
            }
        }

        /* add status report if a new one is available */ //status report��"stat" object����ӵ�buff_up��
        if (send_report == true) {
            pthread_mutex_lock(&mx_stat_rep);
            report_ready = false; //��report_ready��Ϊfalse
            //�ٽ�"stat":...��ӵ�buff_up��
            j = snprintf((char*)(buff_up + buff_index), TX_BUFF_SIZE - buff_index, "%s", status_report);
            pthread_mutex_unlock(&mx_stat_rep);
            if (j > 0) { //snprintfִ�гɹ�
                buff_index += j;
            }
            else {
                MSG("ERROR: [up] snprintf failed line %u\n", (__LINE__ - 5));
                exit(EXIT_FAILURE);
            }
        }

        /* end of JSON datagram payload */ //datagram����
        buff_up[buff_index] = '}'; //������һ���}
        ++buff_index;
        buff_up[buff_index] = 0; /* add string terminator, for safety */

        printf("buff_index: %d\n", buff_index);

        printf("buff_up original format: "); //����datagrams������orinigal����json
        for (int count = 0; count < buff_index; count++) {
            printf("%02X", buff_up[count]);
        }
        printf("\n");

        printf("\nJSON up: %s\n", (char*)(buff_up + 12)); /* DEBUG: display JSON payload */

        /* send datagram to server */ //��������datagrams
        send(sock_up, (void*)buff_up, buff_index, 0); //socket send
        clock_gettime(CLOCK_MONOTONIC, &send_time); //�õ�����ʱ��
        pthread_mutex_lock(&mx_meas_up);
        meas_up_dgram_sent += 1; //PUSH_DATA datagrams sent
        meas_up_network_byte += buff_index; //PUSH_DATA datagrams sent size

        /* wait for acknowledge (in 2 times, to catch extra packets) */ //PUSH_ACK packet
        for (i = 0; i < 2; ++i) { //������
            j = recv(sock_up, (void*)buff_ack, sizeof buff_ack, 0);
            clock_gettime(CLOCK_MONOTONIC, &recv_time); //�õ�����ʱ��
            if (j == -1) { //û�յ��κλظ�
                if (errno == EAGAIN) { /* timeout */ //��ʱ�����ã��ȵڶ���
                    continue;
                }
                else { /* server connection error */ //������ֱ���˳�
                    break;
                }
            }
            else if ((j < 4) || (buff_ack[0] != PROTOCOL_VERSION) || (buff_ack[3] != PKT_PUSH_ACK)) { //����������PUSH_ACK�Ľṹ
             //MSG("WARNING: [up] ignored invalid non-ACL packet\n");
                continue;
            }
            else if ((buff_ack[1] != token_h) || (buff_ack[2] != token_l)) { //token����������Ҫ��PUSH_ACK
             //MSG("WARNING: [up] ignored out-of sync ACK packet\n");
                continue;
            }
            else {
                MSG("INFO: [up] PUSH_ACK received in %i ms\n", (int)(1000 * difftimespec(recv_time, send_time)));
                meas_up_ack_rcv += 1; //ȷ��
                break; //�����еڶ���
            }
        }
        pthread_mutex_unlock(&mx_meas_up);
    }
    MSG("\nINFO: End of upstream thread\n");
}