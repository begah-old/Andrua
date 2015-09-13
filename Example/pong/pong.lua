local pong = {} 
setmetatable(pong, {__index = _G}) 
_ENV = pong 
 
Paddle_goTo = 0  
  
Score_Player = 0  
Score_Computer = 0  
  
Ball = {  
	x = 0, y = 0,  
	width = 20, height = 20,  
	xspeed = 0, yspeed = 0  
}  
  
Paddle1 = {  
	x = 0, y = 20,  
	width = 20, height = 100,  
	yspeed = 0  
}  
  
Paddle2 = {  
	x = 0, y = 20,  
	width = 20, height = 100,  
	yspeed = 0  
}  
  
init = function()  
	Paddle2.x = Screen_Width - Paddle2.width  
  
	Ball.x = Screen_Width / 2 - Ball.width / 2  
	Ball.y = Screen_Height / 2 - Ball.height / 2  
	Ball.xspeed = -3  
end  
  
ballPhysic = function(Paddle)  
	if Ball.x > Paddle.x and Ball.x < Paddle.x + Paddle.width then   
		if Ball.y > Paddle.y and Ball.y < Paddle.y + Paddle.width then   
			Ball.x = Paddle.x + Paddle.width   
			Ball.xspeed = -Ball.xspeed   
			Ball.yspeed = Ball.yspeed + Paddle.yspeed  
		elseif Ball.y+Ball.height > Paddle.y and Ball.y + Ball.height < Paddle.y + Paddle.height then   
			Ball.x = Paddle.x + Paddle.width   
			Ball.xspeed = -Ball.xspeed  
			Ball.yspeed = Ball.yspeed + Paddle.yspeed  
		end  
	elseif Ball.x + Ball.width > Paddle.x and Ball.x + Ball.width < Paddle.x + Paddle.width then  
		if Ball.y > Paddle.y and Ball.y < Paddle.y + Paddle.width then   
			Ball.x = Paddle.x - Ball.width   
			Ball.xspeed = -Ball.xspeed   
			Ball.yspeed = Ball.yspeed + Paddle.yspeed  
		elseif Ball.y+Ball.height > Paddle.y and Ball.y + Ball.height < Paddle.y + Paddle.height then   
			Ball.x = Paddle.x - Ball.width  
			Ball.xspeed = -Ball.xspeed  
			Ball.yspeed = Ball.yspeed + Paddle.yspeed  
		end  
	end  
end  
  
update = function()  
-- Update Paddle 1

	Paddle1.y = Paddle1.y + Paddle1.yspeed
	if Paddle1.yspeed < 0 then   
		if Paddle1.y + Paddle1.height / 2 < Paddle_goTo then  
			Paddle1.yspeed = 0  
		end  
	elseif Paddle1.yspeed > 0 then 
		if Paddle1.y + Paddle1.height / 2 > Paddle_goTo then  
			Paddle1.yspeed = 0  
		end  
	end  
  
	if Paddle1.y < 0 then  
		Paddle1.y = 0  
		Paddle1.yspeed = 0  
	elseif Paddle1.y + Paddle1.height > Screen_Height then  
		Paddle1.y = Screen_Height - Paddle1.height  
		Paddle1.yspeed = 0  
	end  
  
-- Update Paddle2  
	if Ball.y < Paddle2.y then  
		Paddle2.yspeed = -3  
	elseif Ball.y + Ball.height > Paddle2.y + Paddle2.height then  
		Paddle2.yspeed = 3  
	else  
		Paddle2.yspeed = 0  
	end  
  
	Paddle2.y = Paddle2.y + Paddle2.yspeed  
  
	if Paddle2.y < 0 then  
		Paddle2.y = 0  
		Paddle2.yspeed = 0  
	elseif Paddle2.y + Paddle2.height > Screen_Height then  
		Paddle2.y = Screen_Height - Paddle2.height  
		Paddle2.yspeed = 0  
	end
  
-- Update Ball  
	Ball.x = Ball.x + Ball.xspeed  
	Ball.y = Ball.y + Ball.yspeed  
  
	ballPhysic(Paddle1)  
	ballPhysic(Paddle2)  
  
	if Ball.y < 0 then  
		Ball.y = 0  
		Ball.yspeed = -Ball.yspeed  
	elseif Ball.y + Ball.height > Screen_Height then  
		Ball.y = Screen_Height - Ball.height  
		Ball.yspeed = -Ball.yspeed  
	end  
  
	if Ball.x < 0 then  
		Ball.x = Screen_Width / 2 - Ball.width / 2  
		Ball.xspeed = -Ball.xspeed  
		Score_Computer = Score_Computer + 1  
	elseif Ball.x + Ball.width > Screen_Width then  
		Ball.x = Screen_Width / 2 - Ball.width / 2  
		Ball.xspeed = -Ball.xspeed  
		Score_Player = Score_Player + 1  
	end
end  
  
render = function()  
	update()  
	renderer.rectangle(0, 0, Screen_Width, Screen_Height, 1, 1, 1, 1)  
  
	renderer.fixedText("Player : " .. Score_Player .. "|Computer : " .. Score_Computer,   
		Screen_Width / 10 * 3, Screen_Height / 10 * 9, Screen_Width / 10 * 4, Screen_Height / 10, 
		0, 0, 0, 1) 
	 
	renderer.rectangle(Ball.x, Ball.y, Ball.width, Ball.height, 0.6, 0.6, 0, 1) 
	renderer.rectangle(Paddle1.x, Paddle1.y, Paddle1.width, Paddle1.height, 0.6, 0.6, 0, 1) 
	renderer.rectangle(Paddle2.x, Paddle2.y, Paddle2.width, Paddle2.height, 0.6, 0.6, 0, 1) 
end 
 
close = function() 
 
end 
 
mousePressed = function() 
	if mouse_y < Paddle1.y then 
		Paddle1.yspeed = -3
		Paddle_goTo = mouse_y 
	elseif mouse_y > Paddle1.y + Paddle1.height then 
		Paddle1.yspeed = 3
		Paddle_goTo = mouse_y 
	end 
end

return pong