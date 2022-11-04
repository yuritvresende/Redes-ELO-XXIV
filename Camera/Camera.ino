#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h" 
#include <WiFiClientSecure.h>
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "Base64.h"

const char* ssid = "RedeTeste";
const char* password = "Testeapenas";
const char* host = "script.google.com";
String archivename = "filename=ESP32-CAM.jpg";
String mimeType = "&mimetype=image/jpeg";
String image = "&data=";
String Script = "/macros/s/AKfycbyxmsH0HFBdZpevsqqv4eRW26yKm7ir_xoTcsCqGPqkjAxOyG2RiEXQxRN90htMDR12FA/exec";
int wait = 10000;

#define PART_BOUNDARY "123456789000000000000987654321"

  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
  #define flash 4
 
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";
 
httpd_handle_t stream_httpd = NULL;
 
static esp_err_t stream_handler(httpd_req_t *req)
{
 camera_fb_t * fb = NULL;
 esp_err_t res = ESP_OK;
 size_t _jpg_buf_len = 0;
 uint8_t * _jpg_buf = NULL;
 char * part_buf[64];
 res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
 if(res != ESP_OK)
 {
  return res;
 }
 while(true)
 {   
  fb = esp_camera_fb_get();
  if (!fb) 
  {
   Serial.println("Camera capture failed");
   res = ESP_FAIL;
  } 
  else 
  {
   if(fb->width > 400)
   {
    if(fb->format != PIXFORMAT_JPEG)
    {
     bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
     esp_camera_fb_return(fb);
     fb = NULL;
     if(!jpeg_converted)
     {
      Serial.println("JPEG compression failed");
      res = ESP_FAIL;
     }
    } 
    else 
    {
     _jpg_buf_len = fb->len;
     _jpg_buf = fb->buf;
    }
   }
  }
  if(res == ESP_OK)
  {
   size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
   res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
  }
  if(res == ESP_OK)
  {
   res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
  }
  if(res == ESP_OK)
  {
   res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
  }
  if(fb)
  {
   esp_camera_fb_return(fb);
   fb = NULL;
   _jpg_buf = NULL;
  } 
  else if(_jpg_buf)
  {
   free(_jpg_buf);
   _jpg_buf = NULL;
  }
  if(res != ESP_OK)
  {
   break;
  }
  //Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
 }
 return res;
}
 
void startCameraServer()
{
 httpd_config_t config = HTTPD_DEFAULT_CONFIG();
 config.server_port = 80;
 httpd_uri_t index_uri = 
 {
  .uri       = "/",
  .method    = HTTP_GET,
  .handler   = stream_handler,
  .user_ctx  = NULL
 };
 //Serial.printf("Starting web server on port: '%d'\n", config.server_port);
 if (httpd_start(&stream_httpd, &config) == ESP_OK) 
 {
  httpd_register_uri_handler(stream_httpd, &index_uri);
 }
}

void Capture() 
{
 Serial.println("Connecting to " + String(host));
 WiFiClientSecure client;
 if (client.connect(host, 443)) 
 {
  Serial.println("Success!");
  camera_fb_t * fb = NULL;
  digitalWrite(flash, HIGH);
  delay(100);
  fb = esp_camera_fb_get();
  digitalWrite(flash, LOW);
  delay(100);    
  if(!fb) 
  {    
   Serial.println("Failure to capture image!");
   delay(1000);
   ESP.restart();
   return;   
  }
  char *input = (char *)fb->buf;
  char output[base64_enc_len(3)];
  String imageFile = ""; 
  for (int i=0; i<fb->len; i++) 
  {
   base64_encode(output, (input++), 3);
   if (i%3==0) imageFile += urlencode(String(output));
  }  
  String Data = archivename + mimeType + image;
  esp_camera_fb_return(fb); 
  Serial.println("Sending to Google Drive.");
  client.println("POST " + Script + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Content-Length: " + String(Data.length() + imageFile.length()));
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println();
  client.print(Data);
  int Index;  
  for (Index = 0; Index < imageFile.length(); Index = Index + 1000) 
  {
   client.print(imageFile.substring(Index, Index + 1000));
  }
  Serial.println("Waiting."); 
  long int starttime = millis();
  while (!client.available())
  {
   Serial.print(".");
   delay(100);
   if ((starttime + wait) < millis()) 
   {
    Serial.println();
    Serial.println("No answer.");
    break;
   }
  }
  Serial.println();   
  while (client.available()) 
  {
   Serial.print(char(client.read())); 
  }  
 } 
 else 
 {         
  Serial.println("Connection " + String(host) + " failed.");
 }
 client.stop();
}
 
 
String urlencode(String str) 
{
 String encodedString = "";
 char c;
 char code0;
 char code1;
 char code2;
 for (int i = 0; i < str.length(); i++)
 {
  c = str.charAt(i);
  if (c == ' ')
  {
   encodedString += '+';
  } 
  else if (isalnum(c))
  {
   encodedString += c;
  } 
  else
  {
   code1 = (c & 0xf) + '0';
   if ((c & 0xf) > 9)
   {
    code1 = (c & 0xf) - 10 + 'A';
   }
   c = (c>>4)&0xf;
   code0 = c + '0';
   if (c > 9)
   {
    code0=c - 10 + 'A';
   }
   code2 = '\0';
   encodedString += '%';
   encodedString += code0;
   encodedString += code1;
  }
  yield();
 }
 return encodedString;
}
 
void setup() 
{
 WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
 Serial.begin(115200);
 Serial.setDebugOutput(false);
 pinMode(flash, OUTPUT);
 camera_config_t config;
 config.ledc_channel = LEDC_CHANNEL_0;
 config.ledc_timer = LEDC_TIMER_0;
 config.pin_d0 = Y2_GPIO_NUM;
 config.pin_d1 = Y3_GPIO_NUM;
 config.pin_d2 = Y4_GPIO_NUM;
 config.pin_d3 = Y5_GPIO_NUM;
 config.pin_d4 = Y6_GPIO_NUM;
 config.pin_d5 = Y7_GPIO_NUM;
 config.pin_d6 = Y8_GPIO_NUM;
 config.pin_d7 = Y9_GPIO_NUM;
 config.pin_xclk = XCLK_GPIO_NUM;
 config.pin_pclk = PCLK_GPIO_NUM;
 config.pin_vsync = VSYNC_GPIO_NUM;
 config.pin_href = HREF_GPIO_NUM;
 config.pin_sscb_sda = SIOD_GPIO_NUM;
 config.pin_sscb_scl = SIOC_GPIO_NUM;
 config.pin_pwdn = PWDN_GPIO_NUM;
 config.pin_reset = RESET_GPIO_NUM;
 config.xclk_freq_hz = 20000000;
 config.pixel_format = PIXFORMAT_JPEG; 
 config.frame_size = FRAMESIZE_UXGA;
 config.jpeg_quality = 10;
 config.fb_count = 2;
   
 esp_err_t err = esp_camera_init(&config);
 if (err != ESP_OK) 
 {
  Serial.printf("Camera init failed with error 0x%x", err);
  return;
 }
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) 
 {
  delay(500);
  Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");
 startCameraServer();
 Serial.print("Camera Stream Ready! Go to: http://");
 Serial.print(WiFi.localIP());
}
 
void loop() 
{
 Capture();
 delay(1000);
}
