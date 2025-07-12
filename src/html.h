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

const char html_button_code[] PROGMEM = R"=====(
<form id="form-[id]" action="/post" method="post" onsubmit="submitForm(event)">
    <input type="hidden" name="group" value="[value]">
    <button id="[id]" style="background-color: red;" type="submit">[name]</button>
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
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  -webkit-transition: .4s;
  transition: .4s;
}

input:checked + .slider {
  background-color: #2196F3;
}

input:focus + .slider {
  box-shadow: 0 0 1px #2196F3;
}

input:checked + .slider:before {
  -webkit-transform: translateX(26px);
  -ms-transform: translateX(26px);
  transform: translateX(26px);
}

/* Rounded sliders */
.slider.round {
  border-radius: 34px;
}

.slider.round:before {
  border-radius: 50%;
}
</style>
)=====";

const char html_js_hideclass[] PROGMEM = R"=====(
function hideClass(id) {
    var dropdown = document.getElementById(id + '-mode');
    var selectedValue = dropdown.options[dropdown.selectedIndex].value;
    var numberClass = document.getElementsByClassName(id + '-number')[0];
    var timeonClass = document.getElementsByClassName(id + '-timeon')[0];
    var timeoffClass = document.getElementsByClassName(id + '-timeoff')[0];
    var multiplierClass = document.getElementsByClassName(id + '-multiplier')[0];
    var onfadeClass = document.getElementsByClassName(id + '-onfade')[0];
    var offfadeClass = document.getElementsByClassName(id + '-offfade')[0];
    var brightnessClass = document.getElementsByClassName(id + '-brightness')[0];

    // none is not visible, block is visible

    // The array parameters contains a list of items for which numberclass should not be visible
    var parameters = ["0", "201", "202", "1"];
    brightnessClass.style.display = parameters.includes(selectedValue) ? "none" : "block";

    // The array parameters contains a list of items for which numberclass should be visible
    var parameters = ["52", "53", "54", "55", "60", "61", "62"];
    numberClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    // The array parameters contains a list of items for which timeonClass should be visible
    parameters = ["1", "50", "51", "52", "53", "54", "55", "60", "81", "83", "201", "202"];
    timeonClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    // The array parameters contains a list of items for which timeoffClass should be visible
    parameters = ["50", "51", "52", "53", "54", "55", "60", "83"];
    timeoffClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    // The array parameters contains a list of items for which multiplierClass should be visible
    parameters = ["1", "50", "51", "52", "53", "54", "55", "60", "61", "62", "81", "83", "201", "202"];
    multiplierClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    // The array parameters contains a list of items for which onfadeClass should be visible
    parameters = ["50", "51", "62", "83", "102", "103", "104", "105", "106", "110", "70"];
    onfadeClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    // The array parameters contains a list of items for which offfadeClass should be visible
    parameters = ["50", "51", "62", "83", "102", "103", "104", "105", "106", "110", "70"];
    offfadeClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    document.querySelector('label[for="' + id + '-timeon"]').innerHTML = 'Time on (ms)';
    document.querySelector('label[for="' + id + '-timeoff"]').innerHTML = 'Time off (ms)';
    document.querySelector('label[for="' + id + '-onfade"]').innerHTML = 'Fader on (ms)';
    document.querySelector('label[for="' + id + '-offfade"]').innerHTML = 'Fader off (ms)';
    document.querySelector('label[for="' + id + '-multiplier"]').innerHTML = 'Multiplier';

    parameters = ["83"];
    if (parameters.includes(selectedValue)) {
        document.querySelector('label[for="' + id + '-timeon"]').innerHTML = 'Minimum flash time (ms)';
        document.querySelector('label[for="' + id + '-timeoff"]').innerHTML = 'Maximal flash time (ms)';
        document.querySelector('label[for="' + id + '-onfade"]').innerHTML = 'Minimum pause time (s)';
        document.querySelector('label[for="' + id + '-offfade"]').innerHTML = 'Maximal pause time (s)';
    }

    parameters = ["61", "62"];
    if (parameters.includes(selectedValue)) {
        document.querySelector('label[for="' + id + '-multiplier"]').innerHTML = 'Chance';
    }

    parameters = ["62"];
    if (parameters.includes(selectedValue)) {
        document.querySelector('label[for="' + id + '-onfade"]').innerHTML = 'minimal glowing time (ms)';
        document.querySelector('label[for="' + id + '-offfade"]').innerHTML = 'maximal glowing time (ms)';
    }
}
)=====";

const char html_form_end[] PROGMEM = R"=====(
</br><a href='/'>Home</a>
</br><a href='#' onclick="postReset()">Reset</a>
<script>
function postReset() {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', '/post', true);
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    xhr.onload = function() {
        if (xhr.status >= 200 && xhr.status < 400) {
            window.location.href = '/';
        }
    };
    xhr.send('reset=true');
}
</script>
)=====";

const char html_js_updatedata[] PROGMEM = R"=====(
function updateData(jsonData) {
   document.getElementById('RSSIValue').innerHTML = jsonData.rssi + "dBm" 
   for (var key in jsonData) {
       if (jsonData.hasOwnProperty(key) && (key.startsWith('output') || key.startsWith('servo'))) {
           var button = document.getElementById(key);
           if (button) {
               button.style.backgroundColor = jsonData[key] == '1' ? 'green' : 'red';
           }
       }
   }
}
)=====";

#endif