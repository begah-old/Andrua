pong = require("pong")
menu = require("menu")

Screen_Width = 0 
Screen_Height = 0 

mousePressed = false 
 
mouse_x = 0 
mouse_y = 0 

useDisplay = true
useFullScreen = true

State = "menu"

init = function()
	menu.init()
end

stateChange = function(state)
	if State == "menu" then
		menu.close()
	elseif State == "pong" then
		pong.close()
	end

	if state == "menu" then
		menu.init()
	elseif state == "pong" then
		pong.init()
	end
	State = state
end

render = function()
	if State == "menu" then
		menu.render()
	elseif State == "pong" then
		pong.render()
	end	
end

close = function()
	if State == "menu" then
		menu.close()
	elseif State == "pong" then
		pong.close()
	end
end

resize = function()
	if State == "menu" then
		menu.resize()
	end
end	

mouse_action = function(action) 
	if action == 'pressed' then  
		mousePressed = true 

		if State == "pong" then
			pong.mousePressed()
		end
	elseif action == 'released' then 
		mousePressed = false 
	end 
end 