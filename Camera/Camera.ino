#include "WiFi.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"           
#include "soc/rtc_cntl_reg.h" 
#include "driver/rtc_io.h"
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include <SPIFFS.h>
#include <FS.h>

// Substituir com nome e senha da rede a ser usada.
const char* ssid = "RedeTeste";
const char* password = "Testeapenas";

AsyncWebServer server(80);

boolean detected; 

#define FILE_PHOTO "/photo.jpg"

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

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { text-align:center; }
    .vert { margin-bottom: 10%; }
    .hori{ margin-bottom: 0%; }
  </style>
</head>
<body>
  <div><img src="http://192.168.43.122/saved-photo" id="photo" width="70%"></div>
</body>
</html>)rawliteral";

void setup() 
{
 Serial.begin(115200);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) 
 {
  delay(1000);
  Serial.println("Connecting to WiFi...");
 }
 if (!SPIFFS.begin(true)) 
 {
  Serial.println("An Error has occurred while mounting SPIFFS");
  ESP.restart();
 }
 else
 {
  delay(500);
  Serial.println("SPIFFS mounted successfully");
 }
 Serial.print("IP Address: http://");
 Serial.println(WiFi.localIP());
 WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
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
 if (psramFound())
 {
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;
 } 
 else 
 {
  config.frame_size = FRAMESIZE_SVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
 }
 esp_err_t err = esp_camera_init(&config);
 if (err != ESP_OK) 
 {
  Serial.printf("Camera init failed with error 0x%x", err);
  ESP.restart();
 } 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", index_html);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });
 server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) 
 {
  request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
 });
 server.begin();
 pinMode(13, INPUT);
}

bool checkPhoto( fs::FS &fs )
{
 File f_pic = fs.open( FILE_PHOTO );
 unsigned int pic_sz = f_pic.size();
 return ( pic_sz > 100 );
}

void capturePhotoSaveSpiffs() 
{
 camera_fb_t * fb = NULL; 
 bool ok = 0; 
 do 
 {
  Serial.println("Taking a photo...");
  fb = esp_camera_fb_get();
  if (!fb) 
  {
   Serial.println("Camera capture failed");
   return;
  }
  Serial.printf("Picture file name: %s\n", FILE_PHOTO);
  File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);
  if (!file) 
  {
   Serial.println("Failed to open file in writing mode");
  }
  else
  {
   file.write(fb->buf, fb->len); 
   Serial.print("The picture has been saved in ");
   Serial.print(FILE_PHOTO);
   Serial.print(" - Size: ");
   Serial.print(file.size());
   Serial.println(" bytes");
  }
  file.close();
  esp_camera_fb_return(fb);
  ok = checkPhoto(SPIFFS);
 } 
 while ( !ok );
}

void loop() 
{
 if(!digitalRead(13))
 {
  Serial.println("Detected.");
  detected = true; 
 }
 else
 {
  detected = false;
 }
 if (detected) 
 {
  capturePhotoSaveSpiffs();
 }
 delay(1);
}
