#define DEBUG_WIFI(m) SERIAL_DBG.print(m)
#define IOTWEBCONF_DEBUG_TO_SERIAL

#include <Arduino.h>
#include <ArduinoOTA.h>

#include <WiFi.h>

#include "webhandling.h"
#include <IotWebConf.h>
#include <IotWebConfOptionalGroup.h>
#include <IotWebConfESP32HTTPUpdateServer.h>
#include <IotWebRoot.h>
#include <time.h>
#include <DNSServer.h>
#include <iostream>
#include <string.h>
#include "common.h"
#include "html.h"
#include "favicon.h"

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "kDecoder";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "123456789";


// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "002"

// -- Status indicator pin.
//      First it will light up (kept LOW), on Wifi connection it will blink,
//      when connected to the Wifi it will turn off (kept HIGH).
#define STATUS_PIN 47
#define ON_LEVEL HIGH

// -- Method declarations.
void handleRoot();

extern void kDecoderReset();

// -- Callback methods.
void convertSettings();
void configSaved();
void wifiConnected();
void handleConfigSavedPage(iotwebconf::WebRequestWrapper* webRequestWrapper);

DNSServer dnsServer;
WebServer server(80);
HTTPUpdateServer httpUpdater;

SoundGroup soundGroup("sound", "Sound Settings");

OutputGroup OutputGroup1 = OutputGroup("og1");
OutputGroup OutputGroup2 = OutputGroup("og2");
OutputGroup OutputGroup3 = OutputGroup("og3");
OutputGroup OutputGroup4 = OutputGroup("og4");
OutputGroup OutputGroup5 = OutputGroup("og5");
OutputGroup OutputGroup6 = OutputGroup("og6");
OutputGroup OutputGroup7 = OutputGroup("og7");
OutputGroup OutputGroup8 = OutputGroup("og8");

ServoGroup ServoGroup1 = ServoGroup("sg1");
ServoGroup ServoGroup2 = ServoGroup("sg2");
ServoGroup ServoGroup3 = ServoGroup("sg3");

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);

bool isFirstCharacterValid(const char* value) {
    if (value[0] != '/') {
        Serial.println("Fehler: Der Dateiname muss mit '/' beginnen.");
        return false;
    }
    return true;
}

bool areAllCharactersValid(const char* value) {
    const char* allowedChars_ = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-/";
    while (*value) {
        if (!strchr(allowedChars_, *value)) {
            Serial.println("Fehler: Ungültiges Zeichen im Dateinamen.");
            return false;
        }
        value++;
    }
    return true;
}

bool containsSpaces(const char* value) {
    while (*value) {
        if (*value == ' ') {
            Serial.println("Fehler: Der Dateiname darf keine Leerzeichen enthalten.");
            return true;
        }
        value++;
    }
    return false;
}

bool hasValidExtension(const char* filename) {
    // Liste der erlaubten Dateiendungen (immer mit Punkt)
    const char* validExtensions[] = {
        ".mp3", ".aac", ".aacp", ".wav", ".flac", ".vorbis", ".m4a"
    };
    const size_t extCount = sizeof(validExtensions) / sizeof(validExtensions[0]);

    size_t len = strlen(filename);
    if (len == 0) return false;

    for (size_t i = 0; i < extCount; ++i) {
        size_t extLen = strlen(validExtensions[i]);
        if (len >= extLen) {
            if (strcmp(filename + len - extLen, validExtensions[i]) == 0) {
                return true;
            }
        }
    }
    return false;
}


bool formValidator(iotwebconf::WebRequestWrapper* webRequestWrapper) {
    bool allValid = true;
    OutputGroup* group_ = &OutputGroup1;
    while (group_ != nullptr) {
        group_->getFilenameParam().errorMessage = nullptr;
        if (group_->isActive()) {
            char filenameID_[STRING_LEN];
            snprintf(filenameID_, STRING_LEN, "%s-filename", group_->getId());
            String filename_ = webRequestWrapper->arg(filenameID_);

            if (filename_.length() == 0) {
                // Leeres Feld ist erlaubt, keine weiteren Prüfungen
            }
            else {
                if (containsSpaces(filename_.c_str())) {
                    group_->getFilenameParam().errorMessage = "Fehler: Der Dateiname darf keine Leerzeichen enthalten.";
                    allValid = false;
                }
                else if (!isFirstCharacterValid(filename_.c_str())) {
                    group_->getFilenameParam().errorMessage = "Fehler: Der Dateiname muss mit '/' beginnen.";
                    allValid = false;
                }
                else if (!areAllCharactersValid(filename_.c_str())) {
                    group_->getFilenameParam().errorMessage = "Fehler: Ungültiges Zeichen im Dateinamen.";
                    allValid = false;
                }
                else if (!hasValidExtension(filename_.c_str())) {
                    group_->getFilenameParam().errorMessage = "Fehler: Ungültige Dateiendung. Erlaubt sind: .mp3, .aac, .aacp, .wav, .flac, .vorbis, .m4a";
                    allValid = false;
                }
            }
        }
        group_ = (OutputGroup*)group_->getNext();
    }
    return allValid;
}



MySelectParameter::MySelectParameter(
    const char* label,
    const char* id,
    char* valueBuffer,
    int length,
    const char* optionValues,
    const char* optionNames,
    size_t optionCount,
    size_t nameLength,
    const char* defaultValue,
    const char* customHtml
) : iotwebconf::SelectParameter(label, id, valueBuffer, length, optionValues, optionNames, optionCount, nameLength, defaultValue) {
    this->customHtml = customHtml;
};


class CustomHtmlFormatProvider : public iotwebconf::OptionalGroupHtmlFormatProvider {
protected:
    String getScriptInner() override {
        return
            HtmlFormatProvider::getScriptInner() +
            String(FPSTR(IOTWEBCONF_HTML_FORM_OPTIONAL_GROUP_JAVASCRIPT)) +
            String(FPSTR(html_js_hideclass));
    }
    virtual String getFormEnd() {
        return
            OptionalGroupHtmlFormatProvider::getFormEnd() +
            String(FPSTR(html_form_end));
    }
};
CustomHtmlFormatProvider customHtmlFormatProvider;

class MyHtmlRootFormatProvider : public HtmlRootFormatProvider {
protected:
    virtual String getStyleInner() {
        return
            HtmlRootFormatProvider::getStyleInner() +
            String(FPSTR(html_css_code));
    }

    virtual String getScriptInner() {
        String s_ =
            HtmlRootFormatProvider::getScriptInner() +
            String(FPSTR(html_button_response)) +
            String(FPSTR(html_js_updatedata));
        s_.replace("{millisecond}", "500");
        return s_;
    }
};

void handleRoot() {
    if (iotWebConf.handleCaptivePortal()) {
        return;
    }

    std::string content_;
    MyHtmlRootFormatProvider fp_;

    content_ += fp_.getHtmlHead(iotWebConf.getThingName()).c_str();
    content_ += fp_.getHtmlStyle().c_str();
    content_ += fp_.getHtmlHeadEnd().c_str();
    content_ += fp_.getHtmlScript().c_str();

    content_ += fp_.getHtmlTable().c_str();
    content_ += fp_.getHtmlTableRow().c_str();
    content_ += fp_.getHtmlTableCol().c_str();

    content_ += String(F("<fieldset align=left style=\"border: 1px solid\">\n")).c_str();
    content_ += String(F("<table border=\"0\" align=\"center\" width=\"100%\">\n")).c_str();
    content_ += String(F("<tr><td align=\"left\"> </td></td><td align=\"right\"><span id=\"RSSIValue\">no data</span></td></tr>\n")).c_str();

    content_ += fp_.getHtmlTableEnd().c_str();
    content_ += fp_.getHtmlFieldsetEnd().c_str();

    content_ += String(F("<fieldset align=left style=\"border: 1px solid\">\n")).c_str();
    content_ += String(F("<table border=\"0\" align=\"center\" width=\"100%\">\n")).c_str();

    uint8_t i_ = 1;

    OutputGroup* outputgroup_ = &OutputGroup1;
    while (outputgroup_ != nullptr) {
        if ((outputgroup_->isActive()) && (outputgroup_->getMode()) >= 10) {
            String b_ = html_button_toggle;
            b_.replace("[value]", String(i_));
            b_.replace("[name]", String(outputgroup_->getDesignation()) + " (" + String(outputgroup_->getMode()) + ")");
            b_.replace("[id]", "output" + String(i_));
            if (DecoderGroupIsEnabled(i_ - 1)) {
                b_.replace("red", "green");
            }

            content_ += b_.c_str();
            content_ += fp_.addNewLine(2).c_str();
            i_ += 1;
        }
        outputgroup_ = (OutputGroup*)outputgroup_->getNext();
    }

    ServoGroup* servogroup_ = &ServoGroup1;
    while (servogroup_ != nullptr) {
        if (servogroup_->isActive()) {
            String b_ = html_button_toggle;
            b_.replace("[value]", String(i_));
            b_.replace("[name]", String(servogroup_->getDesignation()));
            b_.replace("[id]", "servo" + String(i_));
            if (DecoderGroupIsEnabled(i_ - 1)) {
                b_.replace("red", "green");
            }

            content_ += b_.c_str();
            content_ += fp_.addNewLine(2).c_str();
            i_ += 1;
        }
        servogroup_ = (ServoGroup*)servogroup_->getNext();
    }

    content_ += fp_.getHtmlTableEnd().c_str();
    content_ += fp_.getHtmlFieldsetEnd().c_str();

    content_ += String(F("<fieldset align=left style=\"border: 1px solid\">\n")).c_str();
    content_ += String(F("<table border=\"0\" align=\"center\" width=\"100%\">\n")).c_str();
    content_ += fp_.getHtmlTableRow().c_str();
    content_ += fp_.getHtmlTableCol().c_str();

    String on_ = html_button_blue;
    on_.replace("group", "all");
    on_.replace("[value]", "on");
    on_.replace("[name]", "All on");
    on_.replace("[id]", "allon");
    content_ += on_.c_str();

    content_ += fp_.addNewLine(1).c_str();

    String off_ = html_button_blue;
    off_.replace("group", "all");
    off_.replace("[value]", "off");
    off_.replace("[name]", "All off");
    off_.replace("[id]", "alloff");
    content_ += off_.c_str();

    content_ += fp_.getHtmlTableColEnd().c_str();
    content_ += fp_.getHtmlTableRowEnd().c_str();
    content_ += fp_.getHtmlTableEnd().c_str();
    content_ += fp_.getHtmlFieldsetEnd().c_str();

    content_ += fp_.addNewLine(2).c_str();

    content_ += fp_.getHtmlTable().c_str();
    content_ += fp_.getHtmlTableRowText("<a href = 'config'>Configuration</a>").c_str();
    content_ += fp_.getHtmlTableRowText("<a href = 'settings'>Overview</a>").c_str();
    content_ += fp_.getHtmlTableRowText(fp_.getHtmlVersion(Version)).c_str();
    content_ += fp_.getHtmlTableEnd().c_str();

    content_ += fp_.getHtmlTableColEnd().c_str();
    content_ += fp_.getHtmlTableRowEnd().c_str();
    content_ += fp_.getHtmlTableEnd().c_str();
    content_ += fp_.getHtmlEnd().c_str();

    server.sendHeader("Content-Length", String(content_.length()));
    server.send(200, "text/html", content_.c_str());
}

void handleSettings() {
    std::string content_;
    MyHtmlRootFormatProvider fp_;
    uint8_t i_ = 1;
    uint8_t count_ = 0;

    content_ += fp_.getHtmlHead(iotWebConf.getThingName()).c_str();
    content_ += fp_.getHtmlStyle().c_str();
    content_ += fp_.getHtmlHeadEnd().c_str();
    content_ += fp_.getHtmlScript().c_str();

    content_ += fp_.getHtmlTable().c_str();
    content_ += fp_.getHtmlTableRow().c_str();
    content_ += fp_.getHtmlTableCol().c_str();

    // content_ += "<h2>Overview for " + String(iotWebConf.getThingName()).c_str() + "</h2>";

    OutputGroup* outputgroup_ = &OutputGroup1;
    while (outputgroup_ != nullptr) {
        if (outputgroup_->isActive()) {
            content_ += String("<div>Output group " + String(outputgroup_->getDesignation()) + "</div>").c_str();
            content_ += "<ul>";
            content_ += String("<li>Mode: " + String(outputgroup_->getMode()) + "</li>").c_str();
            content_ += String("<li>Outputs used: " + String(outputgroup_->getNumber()) + "</li>").c_str();
            content_ += String("<li>DCC Address: " + String(outputgroup_->getAddress()) + +"</li>").c_str();
            content_ += "</ul>";
        }
        else {
            outputgroup_->applyDefaultValues();
        }
        outputgroup_ = (OutputGroup*)outputgroup_->getNext();
    }

    content_ += fp_.getHtmlTableEnd().c_str();
    content_ += fp_.getHtmlFieldsetEnd().c_str();

    content_ += fp_.addNewLine(2).c_str();

    content_ += fp_.getHtmlTable().c_str();
    content_ += fp_.getHtmlTableRowText("<a href = '/'>Home</a>").c_str();
    content_ += fp_.getHtmlTableRowText("<a href = 'config'>Configuration</a>").c_str();
    content_ += fp_.getHtmlTableRowText(fp_.getHtmlVersion(Version)).c_str();
    content_ += fp_.getHtmlTableEnd().c_str();

    content_ += fp_.getHtmlTableColEnd().c_str();
    content_ += fp_.getHtmlTableRowEnd().c_str();
    content_ += fp_.getHtmlTableEnd().c_str();
    content_ += fp_.getHtmlEnd().c_str();

    server.sendHeader("Content-Length", String(content_.length()));
    server.send(200, "text/html", content_.c_str());
}

void handleData() {
    String json_ = "{";
    json_ += "\"rssi\":\"" + String(WiFi.RSSI()) + "\"";
    uint8_t i_ = 0;
    OutputGroup* outputgroup_ = &OutputGroup1;
    while (outputgroup_ != nullptr) {
        if (outputgroup_->isActive()) {
            json_ += ",\"output" + String(i_ + 1) + "\":" + DecoderGroupIsEnabled(i_);
            i_ += 1;
        }
        outputgroup_ = (OutputGroup*)outputgroup_->getNext();
        
    }

    ServoGroup* servogroup_ = &ServoGroup1;
    while (servogroup_ != nullptr) {
		//Serial.print("[DEBUG] Checking servo group "); Serial.print(i_); Serial.print(" - Active: "); Serial.println(servogroup_->isActive());
        if (servogroup_->isActive()) {
            json_ += ",\"servo" + String(i_ + 1) + "\":" + DecoderGroupIsEnabled(i_);
            i_ += 1;
        }
        servogroup_ = (ServoGroup*)servogroup_->getNext();
    }

    json_ += "}";
    server.sendHeader("Content-Length", String(json_.length()));
    server.send(200, "application/json", json_);

}

void SendServer(int code, const char* content_type, const char* content) {
    server.setContentLength(strlen(content));
    server.send(code, content_type, content);
    Serial.println("    " + String(code) + " " + content);
}

void handlePost() {
    Serial.println("POST request");
    if (server.hasArg("group")) {
        String value_ = server.arg("group");
        uint8_t group_ = value_.toInt() - 1;
        Serial.println("    Toggel group: " + String(group_));
        if (group_ < 10) {
            handleDecoderGroup(group_);

        }
        else {
            SendServer(400, "text/plain", "Invalid group");
            return;
        }

        server.sendHeader("Location", "/", true);
        SendServer(302, "text/plain", "redirection");
        return;
    }

    if (server.hasArg("reset")) {
        Serial.println("    Resetting decoder");
        kDecoderReset();
        server.sendHeader("Location", "/", true);
        SendServer(200, "text/plain", "Reset successful");
        return;
    }

    if (server.hasArg("all")) {
        String value_ = server.arg("all");
        if (value_ == "on") {
            for (int i_ = 0; i_ < 10; i_++) {
                if (!DecoderGroupIsEnabled(i_)) {
                    handleDecoderGroup(i_);
                }
            }
        }
        else if (value_ == "off") {
            for (int i_ = 0; i_ < 10; i_++) {
                if (DecoderGroupIsEnabled(i_)) {
                    handleDecoderGroup(i_);
                }
            }
        }
        else {
            SendServer(400, "text/plain", "Invalid value");
            return;
        }
        server.sendHeader("Location", "/", true);
        SendServer(302, "text/plain", "redirection");
        return;
    }
    SendServer(400, "text/plain", "Invalid request");
}

void handleAPPasswordMissingPage(iotwebconf::WebRequestWrapper* webRequestWrapper) {
    std::string content_;
    MyHtmlRootFormatProvider fp_;

    content_ += fp_.getHtmlHead(iotWebConf.getThingName()).c_str();
    content_ += fp_.getHtmlStyle().c_str();
    content_ += fp_.getHtmlHeadEnd().c_str();
    content_ += "<body>";
    content_ += "You should change the default AP password to continue.";
    content_ += fp_.addNewLine(2).c_str();
    content_ += "Return to <a href = ''>configuration page</a>.";
    content_ += fp_.addNewLine(1).c_str();
    content_ += "Return to <a href='/'>home page</a>.";
    content_ += "</body>";
    content_ += fp_.getHtmlEnd().c_str();

    webRequestWrapper->sendHeader("Content-Length", String(content_.length()));
    webRequestWrapper->send(200, "text/html", content_.c_str());
}

void handleSSIDNotConfiguredPage(iotwebconf::WebRequestWrapper* webRequestWrapper) {
    webRequestWrapper->sendHeader("Location", "/", true);
    webRequestWrapper->send(302, "text/plain", "SSID not configured");
}

void handleConfigSavedPage(iotwebconf::WebRequestWrapper* webRequestWrapper) {
    webRequestWrapper->sendHeader("Location", "/", true);
    webRequestWrapper->send(302, "text/plain", "Config saved");
}

void handleFavicon() {
    server.send_P(200, "image/x-icon", (const char*)favicon_ico, sizeof(favicon_ico));
}

void setupWeb() {

    OutputGroup1.setNext(&OutputGroup2);
    OutputGroup2.setNext(&OutputGroup3);
    OutputGroup3.setNext(&OutputGroup4);
    OutputGroup4.setNext(&OutputGroup5);
    OutputGroup5.setNext(&OutputGroup6);
    OutputGroup6.setNext(&OutputGroup7);
    OutputGroup7.setNext(&OutputGroup8);


    ServoGroup1.setNext(&ServoGroup2);
    ServoGroup2.setNext(&ServoGroup3);

    iotWebConf.setHtmlFormatProvider(&customHtmlFormatProvider);

	iotWebConf.addParameterGroup(&soundGroup);

    iotWebConf.addParameterGroup(&OutputGroup1);
    iotWebConf.addParameterGroup(&OutputGroup2);
    iotWebConf.addParameterGroup(&OutputGroup3);
    iotWebConf.addParameterGroup(&OutputGroup4);
    iotWebConf.addParameterGroup(&OutputGroup5);
    iotWebConf.addParameterGroup(&OutputGroup6);
    iotWebConf.addParameterGroup(&OutputGroup7);
    iotWebConf.addParameterGroup(&OutputGroup8);

    iotWebConf.addParameterGroup(&ServoGroup1);
    iotWebConf.addParameterGroup(&ServoGroup2);
    iotWebConf.addParameterGroup(&ServoGroup3);

    // -- Define how to handle updateServer calls.
    iotWebConf.setupUpdateServer(
        [](const char* updatePath) { httpUpdater.setup(&server, updatePath); },
        [](const char* userName, char* password) { httpUpdater.updateCredentials(userName, password); });

    iotWebConf.setConfigSavedCallback(&configSaved);
    iotWebConf.setWifiConnectionCallback(&wifiConnected);
    iotWebConf.setConfigSavedPage(&handleConfigSavedPage);
    iotWebConf.setConfigAPPasswordMissingPage(&handleAPPasswordMissingPage);
    iotWebConf.setConfigSSIDNotConfiguredPage(&handleSSIDNotConfiguredPage);

    iotWebConf.setStatusPin(STATUS_PIN, ON_LEVEL);
    iotWebConf.setConfigPin(CONFIG_PIN);
    iotWebConf.getApTimeoutParameter()->visible = true;

    iotWebConf.setFormValidator(&formValidator);

    // -- Initializing the configuration.
    iotWebConf.init();

    // -- Set up required URL handlers on the web server.
    server.on("/", handleRoot);
    server.on("/config", [] { iotWebConf.handleConfig(); });
    server.on("/settings", handleSettings);
    server.on("/data", HTTP_GET, handleData);
    server.on("/post", HTTP_POST, handlePost);
    server.on("/favicon.ico", HTTP_GET, handleFavicon);
    server.on("/favicon", HTTP_GET, handleFavicon);
    server.onNotFound([]() { iotWebConf.handleNotFound(); });

    convertSettings();
}

void loopWeb() {
    iotWebConf.doLoop();
    ArduinoOTA.handle();
}

void wifiConnected() {
    ArduinoOTA.begin();

}

void convertSettings() {
    ArduinoOTA.setHostname(iotWebConf.getThingName());
}

void configSaved() {
    ResetDCCDecoder = true;
    convertSettings();
    Serial.println(F("Configuration was updated."));
}
