function reloadImg(id, src)
{
	var d=new Date();
	loadImage(id, src+"&a="+d.getTime());
}

function loadImage(id, src) 
{
	var ts = Date.now(), img = new Image;

    /*img.onerror = function() 
	{
      if(Date.now() - ts < 2000) 
	  {
        setTimeout(function() { img.src = src; }, 500);
      }
    }*/
    img.onload = function() 
	{
      document.getElementById(id).src = src;
    }
    img.src = src;
}