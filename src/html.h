// html.h

#ifndef _HTML_h
#define _HTML_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

const char html_button_response[] PROGMEM = R"=====(
function submitForm(event) { 
    event.preventDefault(); // Prevents the default behavior of the form 
    var form = event.target; 
    var formData = new FormData(form);
    fetch(form.action, { 
        method: form.method, 
        body: formData 
    }).then(response => { 
        if (response.ok) { 
            console.log('Form successfully submitted'); 
            requestData(); // Call requestData function
        } else { 
            console.error('Error submitting the form'); 
        } 
    }).catch(error => { 
        console.error('Network error:', error); 
    }); 
}
)=====";

const char html_button_toggle[] PROGMEM = R"=====(
<form id="form-[id]" action="/post" method="post" onsubmit="submitForm(event)">
    <input type="hidden" name="group" value="[value]">
    <button id="[id]" class="btn-toggle" type="submit">[name]</button>
</form>
)=====";

const char html_button_blue[] PROGMEM = R"=====(
<form id="form-[id]" action="/post" method="post" onsubmit="submitForm(event)">
    <input type="hidden" name="group" value="[value]">
    <button id="[id]" class="btn-blue" type="submit">[name]</button>
</form>
)=====";

const char html_css_code[] PROGMEM = R"=====(
<style>
.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  inset: 0;
  background-color: #ccc;
  transition: background-color 0.4s;
}

.slider::before {
  content: "";
  position: absolute;
  width: 26px;
  height: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  transition: transform 0.4s;
}

input:checked + .slider {
  background-color: #2196F3;
}

input:checked + .slider::before {
  transform: translateX(26px);
}

input:focus + .slider {
  box-shadow: 0 0 1px #2196F3;
}

/* Runde Ecken */
.slider.round {
  border-radius: 34px;
}

.slider.round::before {
  border-radius: 50%;
}

.btn {
    color: white;
    border: 2px solid black;
    padding: 0.5em 1em;
    border-radius: 4px;
    cursor: pointer;
}

.btn-toggle {
    background-color: red;
}

.btn-blue {
    background-color: #2196F3;
}


</style>
)=====";

const char html_js_hideclass[] PROGMEM = R"=====(
function hideClass(id) {
    var dropdown = document.getElementById(id + '-mode');
    var selectedValue = dropdown.options[dropdown.selectedIndex].value;

    // Definition aller Felder mit zugehörigen Sichtbarkeits-Arrays
    var fields = [
        { className: '-number', show: ["52", "53", "54", "55", "60", "61", "62"] },
        { className: '-timeon', show: ["1", "50", "51", "52", "53", "54", "55", "60", "81", "83", "201", "202", "203", "251", "253", "40"] },
        { className: '-timeoff', show: ["50", "51", "52", "53", "54", "55", "60", "83", "253"] },
        { className: '-multiplier', show: ["1", "50", "51", "52", "53", "54", "55", "60", "61", "62", "81", "83", "201", "202", "203", "251", "252", "253", "40"] },
        { className: '-onfade', show: ["50", "51", "62", "83", "102", "103", "104", "105", "106", "110", "70"] },
        { className: '-offfade', show: ["50", "51", "62", "83", "102", "103", "104", "105", "106", "110", "70"] }
    ];

    // brightness ist eine Ausnahme: "nicht sichtbar" bei diesen Werten
    var brightnessClass = document.getElementsByClassName(id + '-brightness')[0];
    var brightnessHide = ["0", "201", "202", "1"];
    if (brightnessClass)
        brightnessClass.style.display = brightnessHide.includes(selectedValue) ? "none" : "block";

    // Sichtbarkeit für alle anderen Felder setzen
    fields.forEach(function(field) {
        var el = document.getElementsByClassName(id + field.className)[0];
        if (el)
            el.style.display = field.show.includes(selectedValue) ? "block" : "none";
    });

    // Label-Anpassungen
    var labelMap = [
        { sel: 'label[for="' + id + '-timeon"]', def: 'Time on (ms)' },
        { sel: 'label[for="' + id + '-timeoff"]', def: 'Time off (ms)' },
        { sel: 'label[for="' + id + '-onfade"]', def: 'Fader on (ms)' },
        { sel: 'label[for="' + id + '-offfade"]', def: 'Fader off (ms)' },
        { sel: 'label[for="' + id + '-multiplier"]', def: 'Multiplier' },
        { sel: 'label[for="' + id + '-brightness"]', def: 'Brightness' }
    ];
    labelMap.forEach(function(l) {
        var label = document.querySelector(l.sel);
        if (label) label.innerHTML = l.def;
    });

    // Spezialfälle für Labels
    if (["83"].includes(selectedValue)) {
        var l;
        l = document.querySelector('label[for="' + id + '-timeon"]');
        if (l) l.innerHTML = 'Minimum flash time (ms)';
        l = document.querySelector('label[for="' + id + '-timeoff"]');
        if (l) l.innerHTML = 'Maximal flash time (ms)';
        l = document.querySelector('label[for="' + id + '-onfade"]');
        if (l) l.innerHTML = 'Minimum pause time (s)';
        l = document.querySelector('label[for="' + id + '-offfade"]');
        if (l) l.innerHTML = 'Maximal pause time (s)';
    }
    if (["61", "62"].includes(selectedValue)) {
        var l = document.querySelector('label[for="' + id + '-multiplier"]');
        if (l) l.innerHTML = 'Chance';
    }
    if (["62"].includes(selectedValue)) {
        var l = document.querySelector('label[for="' + id + '-onfade"]');
        if (l) l.innerHTML = 'minimal glowing time (ms)';
        l = document.querySelector('label[for="' + id + '-offfade"]');
        if (l) l.innerHTML = 'maximal glowing time (ms)';
    }
    if (["203", "40"].includes(selectedValue)) {
        var l = document.querySelector('label[for="' + id + '-timeon"]');
        if (l) l.innerHTML = 'Start delay (ms)';
    }
    if (["203"].includes(selectedValue)) {
        l = document.querySelector('label[for="' + id + '-brightness"]');
        if (l) l.innerHTML = 'Motor Speed';
    }

    // Debug-Ausgabe
    console.log('selectedValue:', selectedValue);
}
)=====";

const char html_form_end[] PROGMEM = R"=====(
</br><a href='/'>Home</a>
</br><a href='#' onclick="postReset()">Reset</a>
<script>
function postReset() {
    fetch('/post', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'reset=true'
    })
    .then(response => {
        if (response.ok) {
            window.location.href = '/';
        }
    })
    .catch(error => {
        console.error('Reset fehlgeschlagen:', error);
    });
}
</script>
)=====";

const char html_js_updatedata[] PROGMEM = R"=====(
function updateData(jsonData) {
    const rssiElem = document.getElementById('RSSIValue');
    if (rssiElem && jsonData.rssi !== undefined) {
        rssiElem.innerHTML = jsonData.rssi + "dBm";
    }
    Object.keys(jsonData).forEach(key => {
        if (key.startsWith('output') || key.startsWith('servo')) {
            const button = document.getElementById(key);
            if (button) {
                // Lockeren Vergleich verwenden, damit '1' und 1 funktionieren
                button.style.backgroundColor = jsonData[key] == 1 ? 'green' : 'red';
            }
        }
    });
}
)=====";

#endif