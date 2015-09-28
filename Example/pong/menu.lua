local menu = {}
setmetatable(menu, {__index = _G})
_ENV = menu

local justPressed = false

Play = 0
Exit = 0

init = function()
	Play = button.new("Play", Screen_Width / 10 * 3, Screen_Height / 10 * 6, Screen_Width / 10
		* 4, Screen_Height / 20)
	Exit = button.new("Exit", Screen_Width / 10 * 3, Screen_Height / 10 * 4, Screen_Width / 10 
		* 4, Screen_Height / 20)
end

render = function()
	if button.render(Play) then
		stateChange("pong")
	end
	if button.render(Exit) then
		engine.close()
	end
	justPressed = false
end

close = function()

end

mousePressed = function()
	justPressed = true
end

return menu