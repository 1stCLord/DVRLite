var container = document.getElementById('visualization');
var options = {stack:false };
var videoElement = document.getElementById("video");
var playbackSpeedSlider = document.getElementById("playbackSpeed");
var playbackSpeedText = document.getElementById("playbackSpeedText");
var fromDatePicker = document.getElementById("fromDatePicker");
var toDatePicker = document.getElementById("toDatePicker");

fromDatePicker.onblur = function()
{
	newQuery(fromDatePicker, "startTime");
}

toDatePicker.onblur = function()
{
	newQuery(toDatePicker, "endTime");
}

function newQuery(valueSource, queryName)
{
	let searchParams = new URLSearchParams(window.location.search);
	let value = "\"" + valueSource.value + " 00:00:00\"";
	searchParams.set("startTime", value);
	let newurl = window.location.protocol + "//" + window.location.host + window.location.pathname + '?' + unescape(searchParams.toString());
	window.location.href = newurl;		
}

playbackSpeedSlider.oninput = function()
{
	playbackSpeedText.innerHTML = "Playback Speed:" + this.value;
	videoElement.playbackRate = playbackSpeedSlider.value;
}

videoElement.onended = function()
{
	var itemIds = timeline.getSelection();
	var id  = itemIds[0] + 1;
	timeline.setSelection(id);
	var item = items.get(id);
	videoElement.setAttribute("src", 'videos/' + item.source + '/' + item.video);
	videoElement.playbackRate = playbackSpeedSlider.value;
};


function action(properties)
{
	if(properties.items.length == 0)
	{
		videoElement.removeAttribute("src");
	}
	else
	{
		var item = items.get(properties.items[0]); 
		//window.open('videos/' + item.source + '/' + item.video);
		videoElement.setAttribute("src", 'videos/' + item.source + '/' + item.video);
		videoElement.playbackRate = playbackSpeedSlider.value;
	}
	
}