function reloadImg(id, src)
{
	var d=new Date();
	loadImage(id, src+"&a="+d.getTime());
}

function loadImage(id, src) 
{
	var ts = Date.now(), img = new Image;

    img.onload = function() 
	{
      document.getElementById(id).src = src;
    }
    img.src = src;
}