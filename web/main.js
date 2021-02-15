window.onload = function() 
{
	var settingsDropdown = document.getElementById('headerdropdown');
	settingsDropdown.onchange = function()
	{
		var selectedOption = settingsDropdown.options[settingsDropdown.selectedIndex];
		window.location.href = selectedOption.getAttribute("href");
	};
};