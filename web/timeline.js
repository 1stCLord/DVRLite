var container = document.getElementById('visualization');
var options = {stack:false };
videoElement = document.getElementById("video");

videoElement.onended = function()
{
	var itemIds = timeline.getSelection();
	var id  = itemIds[0];
	timeline.setSelection(id+1);
	var item = items.get(id);
	videoElement.setAttribute("src", 'videos/' + item.source + '/' + item.video);
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
	}
	
}