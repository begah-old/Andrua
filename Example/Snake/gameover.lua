local gameover = {}
setmetatable(gameover, {__index = _G})
_ENV = gameover

score = 0
Play = 0
Back = 0

Text = ""

init = function()
	Play = button.new("Play", Screen_Width / 10 * 2, Screen_Height / 10 * 3, Screen_Width / 10 * 3,
		Screen_Height / 20)
	Back = button.new("Back", Screen_Width / 10 * 5, Screen_Height / 10 * 3, Screen_Width / 10 * 3,
		Screen_Height / 20)
end

setScore = function(scr)
	score = scr
	Text = "Game Over : score is " .. score
end

render = function()
	renderer.fixedText(Text, Screen_Width / 10 * 2.5, Screen_Height / 10 * 5, Screen_Width / 10 * 5,
		Screen_Height / 20, 1, 0, 0, 1)

	if button.render(Play) then
		changeScreen("snake")
	elseif button.render(Back) then
		changeScreen("menu")
	end
end

close = function()

end

return gameover