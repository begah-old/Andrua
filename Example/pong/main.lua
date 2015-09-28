pong = require("pong")
menu = require("menu")

Screen_Width = 0
Screen_Height = 0

mouse_x = 0
mouse_y = 0

State = "menu"

mousePressed = false

useDisplay = true
useFullScreen = true

init = function()
	menu.init()
end

render = function()
	if State == "menu" then
		menu.render()
	else
		pong.render()
	end
end

stateChange = function(name)
	if State == "menu" then
		menu.close()
	elseif State == "pong" then
		pong.close()
	end

	if name == "menu" then
		menu.init()
	elseif name == "pong" then
		pong.init()
	end

	State = name	
end

mouse_action = function(action)
    if action == 'pressed' then 
        mousePressed = true
      	if State == "menu" then
				  menu.mousePressed()
				else
					pong.mousePressed()
				end

    elseif action == 'released' then
        mousePressed = false
    end
end