local menu = {}
setmetatable(menu, {__index = _G})
_ENV = menu

Play = -1
Exit = -1
TB = -1

init = function()
	Play = button.new("Play", Screen_Width / 10 * 3.5, Screen_Height / 10 * 6, Screen_Width / 10 * 3, 
		Screen_Height / 20)
	Exit = button.new("Exit", Screen_Width / 10 * 3.5, Screen_Height / 10 * 4, Screen_Width / 10 * 3, 
		Screen_Height / 20)
	TB = textbox.new("Text : ", 0, 0, Screen_Width / 10 * 3, Screen_Height / 20)
	textbox.letters(TB, true)
	textbox.numbers(TB, true)
end

resize = function()
	button.resize(Play, Screen_Width / 10 * 3.5, Screen_Height / 10 * 6, Screen_Width / 10 * 3, 
		Screen_Height / 20)
	button.resize(Exit, Screen_Width / 10 * 3.5, Screen_Height / 10 * 4, Screen_Width / 10 * 3, 
		Screen_Height / 20)
	textbox.resize(TB, 0, 0, Screen_Width / 10 * 3, Screen_Height / 20)
end

render = function()
	if button.render(Play) then
		stateChange("pong")
	end
	
	if button.render(Exit) then
		engine.close()
	end

	textbox.render(TB)
end

close = function()
	button.free(Play)
	button.free(Exit)
end

return menu