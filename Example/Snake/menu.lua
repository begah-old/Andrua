local menu = {}
setmetatable(menu, {__index = _G})
_ENV = menu

Play = 0
Exit = 0
Parts_TextBox = 0

init = function()
	Play = button.new("Play", Screen_Width / 10 * 2, Screen_Height / 10 * 3, Screen_Width / 10 * 3,
		Screen_Height / 20)
	Exit = button.new("Exit", Screen_Width / 10 * 5, Screen_Height / 10 * 3, Screen_Width / 10 * 3,
		Screen_Height / 20)
	Parts_TextBox = textbox.new("Start length : ", Screen_Width / 10 * 2.5, Screen_Height / 10 * 5,
		Screen_Width / 10 * 5, Screen_Height / 20)
	textbox.set(Parts_TextBox, snake.InitialBody_Length)
	textbox.numbers(Parts_TextBox, true)
end

resize = function()
	button.resize(Play, Screen_Width / 10 * 2, Screen_Height / 10 * 3, Screen_Width / 10 * 3,
		Screen_Height / 20)
	button.resize(Exit, Screen_Width / 10 * 5, Screen_Height / 10 * 3, Screen_Width / 10 * 3,
		Screen_Height / 20)
	textbox.resize(Parts_TextBox, Screen_Width / 10 * 2.5, Screen_Height / 10 * 5,
		Screen_Width / 10 * 5, Screen_Height / 20)
end

render = function()
	textbox.render(Parts_TextBox)

	if button.render(Play) then
		snake.InitialBody_Length = textbox.text(Parts_TextBox)
		changeScreen("snake")
	elseif button.render(Exit) then
		engine.close()
	end
end

close = function()
	engine.closeKeyboard()
	button.free(Play)
	button.free(Exit)
	textbox.free(Parts_TextBox)
end

return menu