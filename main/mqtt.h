#define	PUBLISH		100
#define	SUBSCRIBE	200

// Store subscribed data
typedef struct {
	int sequence;
	int total_data_len;
	int current_data_offset;
	int topic_type;
	int topic_len;
	char topic[64];
	int data_len;
	char data[1024];
} MQTT_t;

// Store json object as a string
typedef struct {
	char host[32];
	char port[32];
	char clientId[32];
	char username[32];
	char password[32];
	char topicSub[32];
	char topicPub[32];
	char qosSub[32];
	char qosPub[32];
	char payload[32];
} TEXT_t;

