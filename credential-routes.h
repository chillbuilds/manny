#include "icons.h"

void handleRoot() {

  String rootHTML =
  "<!DOCTYPE html>"
    "<html>"

    "<style>"
    "body {overflow:hidden; margin-top:10%; background:#8093F1; color:white; font-family:Arial, Helvetica, sans-serif;}"
    "input {background:rgba(0,0,0,0.4); color:white; border:none; border-bottom:4px solid rgba(0,0,0,0.2);}"
    "button {padding:10px 40px; background:rgba(0,0,0,0.4); color:white; border:none; border-bottom:4px solid rgba(0,0,0,0.2); font-size:64px; border-radius:8px;}"
    ".textInput {padding:6px; font-size:64px; width:70%; margin-left:15%; margin-bottom:5%;}"
    "#save {width:30%; margin-left:35%; font-size:64px; border-radius:8px;}"
    "#credentials {"
      "overflow: hidden; max-height: 0; opacity: 0;"
      "transition: max-height 0.3s ease, opacity 0.3s ease;"
    "}"
    "#credentials.open {max-height: 500px; opacity:1;}"

    "</style>"

    "<head><title>manny update</title></head>"
    "<body>"

    "<div id='settings' style='position:absolute; top:20px; right:20px; opacity:0.4; z-index:100;'>"
      "<img style='width:96px;' src='" + settingsIcon + "'>"
    "</div>"

    "<div id='credentials'>"
      "<form action='/save' method='POST'>"
      "<input class='textInput' placeholder='SSID' name='ssid'><br>"
      "<input class='textInput' placeholder='password' name='pass'><br>"
      "<input id='save' type='submit' value='update'>"
      "</form>"
    "</div>"

    "<div style='position: absolute; width:80%; margin-left:10%; top:40%;'>"
      "<input style='width:100%;' type='range' id='brightnessSlider' name='brightnessSlider' min='2' max='255' value='" + brightness + "'>"
      "<div style='margin-top:15%; display: flex; justify-content: center; gap: 200px;'>"
        "<button id='on' value='light on'>on</button>"
        "<button id='off' value='light on'>off</button>"
      "</div>"
    "</div>"

    "<script>"
      "document.getElementById('settings').addEventListener('click', () => {"
      "  document.getElementById('credentials').classList.toggle('open');"
      "});"

      "let slider = document.getElementById('brightnessSlider');"

      "slider.addEventListener('change', () => {"
        "let value = slider.value;"
        "fetch('/update_brightness', {"
        "method: 'POST',"
        "headers: {'Content-Type': 'text/plain'},"
        "body: value"
      "})"
      ".then(response => response.text())"
      ".then(data => { console.log('server response:', data); })"
      ".catch(error => { console.error('Error sending slider value:', error); });"
      "});"

      "document.getElementById('on').addEventListener('click', () => {"
        "fetch('/light_on')"
        ".then(response => response.text())"
        ".then(data => console.log('Light On response:', data))"
        ".catch(error => console.error('Error turning light on:', error))"
      "});"
      "document.getElementById('off').addEventListener('click', () => {"
        "fetch('/light_off')"
        ".then(response => response.text())"
        ".then(data => console.log('Light On response:', data))"
        ".catch(error => console.error('Error turning light on:', error))"
      "});"
    "</script>"


    "</body>"
    "</html>";

  server.send(200, "text/html", rootHTML);
}

void handleSave() {
  String ssidNew = server.arg("ssid");
  String passNew = server.arg("pass");

  Serial.print("ssid: ");
  Serial.println(ssidNew);
  Serial.print("password: ");
  Serial.println(passNew);

  preferences.begin("wifi", false);
  preferences.putString("ssid", ssidNew);
  preferences.putString("pass", passNew);
  preferences.end();
  server.send(200, "text/html", 
  "<!DOCTYPE   html>"
    "<html>"
    "<style>"
    "body {overflow:hidden; margin-top:10%; background:#8093F1; color:white; font-family: Arial, Helvetica, sans-serif;}"
    "</style>"
    "<head><title>manny update</title></head>"
    "<body>"
    "<p style='text-align:center; width:60%; margin-left:20%; font-size:64px;'>network credentials updated<br><br>rebooting manny..</p>"
    "</body>"
    "</html>"
  );
  delay(2000);
  ESP.restart();
}