package src

var (
	Optimization     = true //选择ADR或设计的优化
	OptimizationName string
	SOTA             = false //whether to use OPR work
)

const (
	QOS           = 0
	MQTTPort      = "1883"
	SERVERADDRESS = "106.14.134.224" //Aliyun
	MQTTServer    = "tcp://" + SERVERADDRESS + ":" + MQTTPort
	WRITETOLOG    = true  // If true then received messages will be written to the console
	WRITETODISK   = false // If true then received messages will be written to the file below

	OUTPUTFILE = "/binds/receivedMessages.txt"

	USERNAME = "admin"
	PASSWORD = "admin"

	HISTORYCOUNT = 5               //Recent SNR history num
	N            = 6               //Real number of GW
	M            = 12              //Maximal number of ED (Do not change unless add more device)
	RealMNum     = 4               //Real number of ED
	Tinterval    = 10              //Transmission interval
	Lambda       = 1.0 / Tinterval // Arrival rate (packet/s)

	MAXRuntime = 1800000 //Total runtime of algorithm
)
