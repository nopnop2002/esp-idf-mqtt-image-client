/* Convert from jpeg/png image to bsee64

	 This example code is in the Public Domain (or CC0 licensed, at your option.)

	 Unless required by applicable law or agreed to in writing, this
	 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	 CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "mbedtls/base64.h"
#include "cJSON.h"

#include "mqtt.h"

static const char *TAG = "CONV";

extern MessageBufferHandle_t xMessageBufferMain;
extern QueueHandle_t xQueueSubscribe;
extern char *MOUNT_POINT;

static void listSPIFFS(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent *pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(TAG, "d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}

static void clearSPIFFS(char * path) {
	ESP_LOGI(TAG, "clearSPIFFS path=[%s]", path);
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent *pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(TAG, "d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
		char fullPath[64];
		//sprintf(fullPath, "%s/%s", path, pe->d_name);
		strcpy(fullPath, path);
		strcat(fullPath, "/");
		strcat(fullPath, pe->d_name);
		ESP_LOGI(TAG, "clearSPIFFS fullPath=[%s]", fullPath);
		unlink(fullPath);
	}
	closedir(dir);
}


int32_t calcBase64EncodedSize(int origDataSize)
{
	// 6bit単位のブロック数（6bit単位で切り上げ）
	// Number of blocks in 6-bit units (rounded up in 6-bit units)
	int32_t numBlocks6 = ((origDataSize * 8) + 5) / 6;
	// 4文字単位のブロック数（4文字単位で切り上げ）
	// Number of blocks in units of 4 characters (rounded up in units of 4 characters)
	int32_t numBlocks4 = (numBlocks6 + 3) / 4;
	// 改行を含まない文字数
	// Number of characters without line breaks
	int32_t numNetChars = numBlocks4 * 4;
	// 76文字ごとの改行（改行は "\r\n" とする）を考慮したサイズ
	// Size considering line breaks every 76 characters (line breaks are "\ r \ n")
	//return numNetChars + ((numNetChars / 76) * 2);
	return numNetChars;
}

esp_err_t Image2Base64(char * filename, size_t fsize, unsigned char * base64_buffer, size_t base64_buffer_len)
{
	unsigned char* image_buffer = NULL;
	image_buffer = malloc(fsize);
	if (image_buffer == NULL) {
		ESP_LOGE(TAG, "malloc fail. image_buffer %d", fsize);
		return ESP_FAIL;
	}

	FILE * fp;
	if((fp=fopen(filename,"rb"))==NULL){
		ESP_LOGE(TAG, "fopen fail. [%s]", filename);
		free(image_buffer);
		return ESP_FAIL;
	} else {
		for (int i=0;i<fsize;i++) {
			fread(&image_buffer[i],sizeof(char),1,fp);
		}
		fclose(fp);
	}

	size_t encord_len;
	esp_err_t ret = mbedtls_base64_encode(base64_buffer, base64_buffer_len, &encord_len, image_buffer, fsize);
	ESP_LOGI(TAG, "mbedtls_base64_encode=%d encord_len=%d", ret, encord_len);
	free(image_buffer);
	return ret;
}


void converte(void *pvParameters)
{
	ESP_LOGI(TAG, "Start");

	MQTT_t mqttBuf;
	FILE* file = NULL;
	char markerJPEG[] = {0xFF, 0xD8};
	char markerPNG[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
	char imageFileName[64];
	char base64FileName[64];
	char topic[128];
	char imageType[32];
	int total_data_len = 0;

	while (1) {
		xQueueReceive(xQueueSubscribe, &mqttBuf, portMAX_DELAY);
		ESP_LOGD(TAG, "type=%d", mqttBuf.topic_type);

		if (mqttBuf.topic_type == SUBSCRIBE) {
			ESP_LOGD(TAG, "DATA=%.*s\r", mqttBuf.data_len, mqttBuf.data);
			if (mqttBuf.current_data_offset == 0) {
				ESP_LOGI(TAG, "TOPIC=[%.*s]", mqttBuf.topic_len, mqttBuf.topic);
				ESP_LOG_BUFFER_HEXDUMP(TAG, mqttBuf.data, 10, ESP_LOG_INFO);
				clearSPIFFS(MOUNT_POINT);
				memset(topic, 0, sizeof(topic));
				strncpy(topic, mqttBuf.topic, mqttBuf.topic_len);
				if (strncmp(mqttBuf.data, markerJPEG, sizeof(markerJPEG)) == 0) {
					ESP_LOGI(TAG, "JPEG file");
					strcpy(imageType, "image/jpeg");
					//strcpy(imageFileName, "/spiffs/image.jpeg");
					//sprintf(imageFileName, "/root/image%d.jpeg",xTaskGetTickCount());
					sprintf(imageFileName, "%s/image%"PRIu32".jpeg", MOUNT_POINT, xTaskGetTickCount());
					//file = fopen("/spiffs/image.jpeg", "wb");
					file = fopen(imageFileName, "wb");
					if (file == NULL) {
						ESP_LOGE(TAG, "Failed to open file for jpeg file");
						while(1) { vTaskDelay(1); }
					} else {
						total_data_len = mqttBuf.total_data_len;
						fwrite(mqttBuf.data, mqttBuf.data_len, 1, file);
					}
				} else if (strncmp(mqttBuf.data, markerPNG, sizeof(markerPNG)) == 0) {
					ESP_LOGI(TAG, "PNG file");
					strcpy(imageType, "image/png");
					//strcpy(imageFileName, "/spiffs/image.png");
					//sprintf(imageFileName, "/spiffs/image%d.png",xTaskGetTickCount());
					sprintf(imageFileName, "%s/image%"PRIu32".png", MOUNT_POINT, xTaskGetTickCount());
					//file = fopen("/spiffs/image.png", "wb");
					file = fopen(imageFileName, "wb");
					if (file == NULL) {
						ESP_LOGE(TAG, "Failed to open file for png file");
					} else {
						total_data_len = mqttBuf.total_data_len;
						fwrite(mqttBuf.data, mqttBuf.data_len, 1, file);
					}
				} else {
					ESP_LOGW(TAG, "Unknown Image file type");
					ESP_LOGW(TAG, "This project only supports png and jpeg");
				}
			} else {
				if (file == NULL) continue;
				fwrite(mqttBuf.data, mqttBuf.data_len, 1, file);
				size_t received_data_size = mqttBuf.current_data_offset + mqttBuf.data_len;
				ESP_LOGI(TAG, "sequence=%d received_data_size=%d total_data_len=%d", mqttBuf.sequence, received_data_size, mqttBuf.total_data_len);
				if (mqttBuf.current_data_offset + mqttBuf.data_len == mqttBuf.total_data_len) {
					fclose(file);
					ESP_LOGI(TAG, "image file create");

					// verify file size
					file = fopen(imageFileName, "rb");
					struct stat st;
					fstat(fileno(file), &st);
					ESP_LOGI(TAG, "%s st.st_size=%ld", imageFileName, st.st_size);
					fclose(file);
					file = NULL;

					if (st.st_size != total_data_len) {
						ESP_LOGE(TAG, "total_data_len miss match");
						unlink(imageFileName);
					} else {
						ESP_LOGI(TAG, "total_data_len match. start convert to base64");
						listSPIFFS(MOUNT_POINT);
						int32_t base64Size = calcBase64EncodedSize(st.st_size);
						ESP_LOGI(TAG, "base64Size=%"PRIi32, base64Size);

						// Convert from JPEG to BASE64
						unsigned char*	img_src_buffer = NULL;
						size_t img_src_buffer_len = base64Size + 1;
						img_src_buffer = malloc(img_src_buffer_len);
						if (img_src_buffer == NULL) {
							ESP_LOGE(TAG, "malloc fail. img_src_buffer_len %d", img_src_buffer_len);
							while(1) { vTaskDelay(1); }
						}
						esp_err_t ret = Image2Base64(imageFileName, st.st_size, img_src_buffer, img_src_buffer_len);
						ESP_LOGI(TAG, "Image2Base64=%d", ret);
						if (ret != 0) {
							ESP_LOGE(TAG, "Error in mbedtls encode! ret = -0x%x", -ret);
							while(1) { vTaskDelay(1); }
						}
						ESP_LOGI(TAG, "base64 convert finish");
		
						// Save base64 file
						// Sometimes file creation fails. So i'll retry.
						sprintf(base64FileName, "%s/base64%"PRIu32".txt", MOUNT_POINT, xTaskGetTickCount());
						while(1) {
							file = fopen(base64FileName, "wb");
							if (file == NULL) {
								ESP_LOGE(TAG, "Failed to open file for base64File");
								while(1) { vTaskDelay(1); }
							}
							fwrite(img_src_buffer, img_src_buffer_len, 1, file);
							fclose(file);

							// confirm for file size
							file = fopen(base64FileName, "rb");
							fstat(fileno(file), &st);
							fclose(file);
							ESP_LOGI(TAG, "%s st.st_size=%ld img_src_buffer_len=%d", base64FileName, st.st_size, img_src_buffer_len);
							if (st.st_size == img_src_buffer_len) break;
							vTaskDelay(10);
						}

						ESP_LOGI(TAG, "base64 file create");
						free(img_src_buffer);
						unlink(imageFileName);
						ESP_LOGI(TAG, "image file unlink");
						listSPIFFS(MOUNT_POINT);

						// Send request to main
						cJSON *response;
						response = cJSON_CreateObject();
						cJSON_AddStringToObject(response, "id", "subscribe-data");
						cJSON_AddStringToObject(response, "topic", topic);
						cJSON_AddStringToObject(response, "imagetype", imageType);
						cJSON_AddStringToObject(response, "imagefile", base64FileName);
						char *my_json_string = cJSON_Print(response);
						ESP_LOGI(TAG, "my_json_string=[%s]",my_json_string);
						size_t xBytesSent = xMessageBufferSend(xMessageBufferMain, my_json_string, strlen(my_json_string), portMAX_DELAY);
						if (xBytesSent != strlen(my_json_string)) {
							ESP_LOGE(TAG, "xMessageBufferSend fail");
						}
						cJSON_Delete(response);
						cJSON_free(my_json_string);
					}
				}

			} // end if
		} // end if
	} // end while

	// Never reach here
	vTaskDelete(NULL);

}
