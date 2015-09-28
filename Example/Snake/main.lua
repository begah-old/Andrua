menu = require("menu")
snake = require("snake")
gameover = require("gameover")

Screen_Width = 0
Screen_Height = 0

mouse_x = 0
mouse_y = 0

useDisplay = true
useFullScreen = true

State = "menu"

closeScreen = nil

init = function()
	menu.init()
end

resize = function()
	if State == "menu" then
		menu.resize()
	end
end

changeScreen = function(nState)
	if State == "snake" then
		closeScreen = snake.close		
	elseif State == "gameover" then
		closeScreen = gameover.close
	elseif State == "menu" then
		closeScreen = menu.close
	end

	State = nState
	if State == "snake" then
		snake.init()
	elseif State == "gameover" then
		gameover.init()
	elseif State == "menu" then
		menu.init()
	end
end

render = function()
	if closeScreen then
		closeScreen()
		closeScreen = nil
	end

	if State == "snake" then
		snake.render()
	elseif State == "gameover" then
		gameover.render()
	elseif State == "menu" then
		menu.render()
	end

	renderer.fixedText(engine.getFPS(), 0, Screen_Height - 50, 
		200, 50, 1, 1, 0, 1)
end

close = function()
	if State == "snake" then
		snake.close()
	elseif State == "gameover" then
		gameover.close()
	elseif State == "menu" then
		menu.close()
	end
end

mouse_action = function(action)
	if action == "pressed" then
		if State == "snake" then
			snake.mousePressed()
		end
	end
end