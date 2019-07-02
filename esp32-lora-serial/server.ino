//----------------------------------------//
//  server.ino
//
//  created 23/06/2019
//  by Luiz Henrique Cassettari
//----------------------------------------//

const char* _serverIndex =
  R"(<html><body><form method='POST' action='/edit' enctype='multipart/form-data'>
                  <input type='file' name='update' multiple>
                  <input type='submit' value='Upload File'>
               </form>
         </body></html>)";

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include "index.h"
#include "mini.h"

WebServer server(80);
File fsUploadFile;
static int wifi_status;
static boolean restart;

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (server.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}

bool exists(String path) {
  bool yes = false;
  File file = SPIFFS.open(path, "r");
  if (!file.isDirectory()) {
    yes = true;
  }
  file.close();
  return yes;
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.htm";
  }
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (exists(pathWithGz) || exists(path)) {
    if (exists(pathWithGz)) {
      path += ".gz";
    }
    Serial.println("handleFileRead: " + path);
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() {
  if (server.uri() != "/edit") {
    return;
  }
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //Serial.print("handleFileUpload Data: "); Serial.println(upload.currentSize);
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}

void handleFileDelete() {
  if (server.args() == 0) {
    return server.send(500, "text/plain", "BAD ARGS");
  }
  String path = server.arg(0);
  Serial.println("handleFileDelete: " + path);
  if (path == "/") {
    return server.send(500, "text/plain", "BAD PATH");
  }
  if (!exists(path)) {
    return server.send(404, "text/plain", "FileNotFound");
  }
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate() {
  if (server.args() == 0) {
    return server.send(500, "text/plain", "BAD ARGS");
  }
  String path = server.arg(0);
  Serial.println("handleFileCreate: " + path);
  if (path == "/") {
    return server.send(500, "text/plain", "BAD PATH");
  }
  if (exists(path)) {
    return server.send(500, "text/plain", "FILE EXISTS");
  }
  File file = SPIFFS.open(path, "w");
  if (file) {
    file.close();
  } else {
    return server.send(500, "text/plain", "CREATE FAILED");
  }
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileList() {
  if (!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  Serial.println("handleFileList: " + path);

  File root = SPIFFS.open(path);
  path = String();

  String output = "[";
  if (root.isDirectory()) {
    File file = root.openNextFile();
    while (file) {
      if (output != "[") {
        output += ',';
      }
      output += "{\"type\":\"";
      output += (file.isDirectory()) ? "dir" : "file";
      output += "\",\"name\":\"";
      output += String(file.name()).substring(1);
      output += "\"}";
      file = root.openNextFile();
    }
  }
  output += "]";
  server.send(200, "text/json", output);
}

void server_setup()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("FS error!");
  }

  server_on();

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", index_html::content, index_html::length);
  });
  
  server.on("/min.css", HTTP_GET, []() {
    server.send_P(200, milligram_min_css::type, milligram_min_css::content, milligram_min_css::length);
  });
  

  server.on("/list", HTTP_GET, handleFileList);

  //load editor
  server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) {
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", _serverIndex);
    }
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);


  server.on("/remove", HTTP_GET, []() {
    remove_config();
    server.send(200, "text/plain", "remove_config");
  });

  server.on("/all", HTTP_GET, []() {

    if (server.hasArg("name"))
    {
      send_name = String(server.arg("name"));
    }

    if (server.hasArg("mode"))
    {
      String value = server.arg("mode");
      send_mode = value.toInt();
    }

    if (server.hasArg("frequency"))
    {
      String value = server.arg("frequency");
      long f = value.toInt();
      if (f != 0 && f > 100E6)
        frequency = f;
      
    }

    if (server.hasArg("bw"))
    {
      String value = server.arg("bw");
      signalBandwidth = value.toInt();
    }

    if (server.hasArg("sf"))
    {
      String value = server.arg("sf");
      spreadingFactor = value.toInt();
    }

    save_config();

    String json = "{";
    json += "\"name\":\"" + String(send_name) + "\" ";
    json += ", \"mode\":" + String(send_mode);

    json += ", \"frequency\":" + String(frequency);
    json += ", \"bw\":" + String(signalBandwidth);
    json += ", \"sf\":" + String(spreadingFactor);

    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });


  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
}

void server_connected()
{
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void server_loop()
{
  if (restart) ESP.restart();
  if (wifi_status == 255) return;

  server.handleClient();

  if (WiFi.status() == WL_CONNECTED)
  {
    if (WiFi.status() != wifi_status) {
      wifi_status = WiFi.status();
      server_connected();
      String str = "";
      str += WiFi.localIP().toString();
      oled_string(str);
    }
  }
}

void server_off()
{
  WiFi.mode(WIFI_OFF);
  wifi_status = 255;
  Serial.printf("Connection status: %d\n", WiFi.status());
}

void server_on()
{
  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(APssid, APpassword);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());

  if (String(ssid) != "") {
    if (String(WiFi.SSID()) != String(ssid)) {
      WiFi.begin(ssid, password);
    }
  }
  else
  {
    WiFi.begin();
  }

  Serial.printf("Connection status: %d\n", WiFi.status());
  wifi_status = WL_DISCONNECTED;
}

void load_config()
{
  String filename = "/config.txt";
  if (SPIFFS.exists(filename)) {
    Serial.println("load_config");
    File file = SPIFFS.open(filename, "r");
    for (int i = 0; i < 5; i++)
    {
      String value = file.readStringUntil('\n');
      Serial.println(value);
      if (i == 0) send_name = value;
      if (i == 1) send_mode = value.toInt();
      if (i == 2)  frequency = value.toInt();
      if (i == 3) signalBandwidth = value.toInt();
      if (i == 4) spreadingFactor = value.toInt();

    }
    file.close();
  }
  else
  {
    save_config();
  }
}
void save_config()
{
  Serial.println("save_config");
  String filename = "/config.txt";
  String config = "";
  config += send_name;
  config += "\n";
  config += send_mode;
  config += "\n";
  config += frequency;
  config += "\n";
  config += signalBandwidth;
  config += "\n";
  config += spreadingFactor;
  config += "\n";

  File file = SPIFFS.open(filename, "w+");
  if (file) {
    Serial.println(config);
    file.print(config);
    file.close();
  }
}

void remove_config()
{
  String filename = "/config.txt";
  SPIFFS.remove(filename);
  restart = true;
}
