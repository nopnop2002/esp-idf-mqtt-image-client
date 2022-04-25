//document.getElementById("datetime").innerHTML = "WebSocket is not connected";

var websocket = new WebSocket('ws://'+location.hostname+'/');
var imagedata;
var topic;
var imageSize;
var imageType;
var imageName;

function getTextValueByName(name) {
	var textbox = document.getElementsByName(name)
	//console.log('name=', name);
	//console.log('textbox=', textbox);
	//console.log('textbox.length=', textbox.length);
	ret = new Array();
	for (var i=0;i<textbox.length;i++) {
		//console.log('textbox[%d].value=%s', i, textbox[i].value);
		ret[i] = textbox[i].value;
	}
	//console.log('typeof(ret)=', typeof(ret));
	//console.log('ret=', ret);
	return ret;
}

function sendText(name) {
	console.log('sendText');
/*
	var array = ["11", "22", "33"];
	var data = {};
	//data["foo"] = "abc";
	data["foo"] = array;
	var array = ["aa"];
	data["bar"] = array;
	data["hoge"] = 100;
	json_data = JSON.stringify(data);
	console.log(json_data);
*/

	var data = {};
	data["id"] = name;
	data["host"] = getTextValueByName("host");
	console.log('data=', data);
	data["port"] = getTextValueByName("port");
	data["clientId"] = getTextValueByName("clientId");
	data["username"] = getTextValueByName("username");
	data["password"] = getTextValueByName("password");
	data["topic"] = getTextValueByName("topic");
	data["qos"] = getTextValueByName("qos");
	data["payload"] = getTextValueByName("payload");
	console.log('data=', data);
	json_data = JSON.stringify(data);
	console.log('json_data=' + json_data);
	websocket.send(json_data);

/*
	var data = {};
	data["id"] = "button";
	data["name"] = name;
	console.log('name=', name);
	console.log('data=', data);
	json_data = JSON.stringify(data);
	console.log('json_data=' + json_data);
	websocket.send(json_data);
*/
}

function clearImage() {
	console.log('clearImage');
	while(true) {
		var article = document.getElementById('article');
  	if (article.hasChildNodes()) {
			console.log("removeChild");
      article.removeChild(article.firstChild);
  	} else {
			break;
		}
	}
}


websocket.onopen = function(evt) {
	console.log('WebSocket connection opened');
	var data = {};
	data["id"] = "init";
	console.log('data=', data);
	json_data = JSON.stringify(data);
	console.log('json_data=' + json_data);
	websocket.send(json_data);
	//document.getElementById("datetime").innerHTML = "WebSocket is connected!";
}

websocket.onmessage = function(evt) {
	var msg = evt.data;
	//console.log("msg=" + msg);
	var values = msg.split('\4'); // \4 is EOT
	//console.log("values=" + values);
	switch(values[0]) {
		case 'ID':
			//console.log("ID values[1]=" + values[1]);
			//console.log("ID values[2]=" + values[2]);
			//console.log("ID values[3]=" + values[3]);
			if (values[2] == "value") document.getElementById(values[1]).innerHTML = values[3];
			if (values[2] == "bcolor") document.getElementById(values[1]).style.backgroundColor = values[3];
			break;

		// Display Base64 format image data in img tag
		// https://javascript.programmer-reference.com/js-base64-img/
		// https://gray-code.com/javascript/display-image-in-base64-format/
		// Data URI Scheme
		// <img src="data:image/png;base64,iVBORw0KGgoAAAANSU ... 5ErkJggg==" alt="Bear" width="200">
		// Normal Scheme
		// <img src="gazou1.jpg" width="200">
		case 'TOPIC':
			console.log("TOPIC values[1]=" + values[1]);
			console.log("TOPIC values[2]=" + values[2]);
			console.log("TOPIC values[3]=" + values[3]);
			topic = values[1];
			imageSize = values[2];
			imageType = values[3];
			imagedata = "data:" + values[3] + ";base64,"; //Base64データ
			console.log("TOPIC imagedata=" + imagedata);
			break;

		case 'IMAGE':
			console.log("IMAGE values[1]=" + values[1]);
			imagedata = imagedata + values[2]; //Base64データ
			console.log("IMAGE imagedata.length=" + imagedata.length);
			//const msg = document.createElement('div')
			//msg.className = 'message-body';
			//msg.innerText = values[2] + '\nOn topic: ' + values[1];
			//document.getElementById('article').appendChild(msg);
			break;

		case 'SHOW':
			console.log("SHOW imagedata.length=" + imagedata.length);
			console.log("SHOW topic=" + topic);
			console.log("SHOW imageSize=" + imageSize);
			console.log("SHOW imageType=" + imageType);
			//document.getElementById("images").src = imagedata;

			// Create anchor element
			// https://hakuhin.jp/js/anchor.html
/*
			var anchor = document.createElement("a");
			anchor.href = "http://www.yahoo.co.jp/";
			anchor.target = "_blank";
			document.getElementById('article').appendChild(anchor);
			var img_element = document.createElement('img')
			img_element.src = imagedata;
			img_element.width = 300;
			img_element.height = 300;
			anchor.appendChild(img_element);
*/

			// Create img element
			// https://gray-code.com/javascript/create-new-img-element/
			var img_element = document.createElement('img')
			img_element.src = imagedata;
			img_element.width = 300;
			img_element.height = 300;
			document.getElementById('article').appendChild(img_element);
			document.getElementById("topic").value = topic;
			document.getElementById("size").value = imageSize;
			document.getElementById("type").value = imageType;

			break;

		default:
			break;
	}
}

websocket.onclose = function(evt) {
	console.log('Websocket connection closed');
	//document.getElementById("datetime").innerHTML = "WebSocket closed";
}

websocket.onerror = function(evt) {
	console.log('Websocket error: ' + evt);
	//document.getElementById("datetime").innerHTML = "WebSocket error????!!!1!!";
}
