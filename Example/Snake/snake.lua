local snake = {}
setmetatable(snake, {__index = _G})
_ENV = snake

Game_Height = 20
Game_Width = 40

InitialBody_Length = 1

Snake = {}
Coin = {}

init = function()
	Snake.head = { x = Game_Width / 2, y = Game_Height / 2, direction = 1, lastX = 1, lastY = 1 }
	Snake.body = {}
	
	for i = 1, InitialBody_Length do
		Snake.body[i] = { x = Game_Width / 2, y = Game_Height / 2, lastX = 0, lastY = 0 }
	end

	Coin.image = image.load("coin.png")
	Coin.x = math.random(Game_Width) - 1
	Coin.y = math.random(Game_Height) - 1
end

updatePart = function(part)
	if part.direction == 1 then
		part.x = part.x - 1
	elseif part.direction == 2 then
		part.y = part.y - 1
	elseif part.direction == 3 then
		part.x = part.x + 1
	elseif part.direction == 4 then
		part.y = part.y + 1
	end
end

update = function()
	Snake.head.lastX = Snake.head.x
	Snake.head.lastY = Snake.head.y
	updatePart(Snake.head)

	if Snake.head.x == Coin.x and Snake.head.y == Coin.y then
		Coin.x = math.random(Game_Width) - 1
		Coin.y = math.random(Game_Height) - 1
		print(Coin.x, Coin.y)
		Snake.body[#Snake.body + 1] = { x = Snake.body[#Snake.body].x, y = Snake.body[#Snake.body].y,
			lastX = 0, lastY = 0 }
	--	Snake.body[#Snake.body].direction = 0
	end

	local last = Snake.head
	for i,v in ipairs(Snake.body) do
		if v.x == Snake.head.x and v.y == Snake.head.y then
			gameover.setScore(#Snake.body)
			changeScreen("gameover")
		end

		v.lastX = v.x
		v.lastY = v.y

		v.x = last.lastX
		v.y = last.lastY

		last = v
	end
end

Counter = 0

render = function()
	Counter = Counter + 1
	if Counter >= engine.getFPS() / 4 then
		update()
		Counter = 0
	end

	width = Screen_Width / Game_Width
	height = Screen_Height / Game_Height

	for i,v in ipairs(Snake.body) do
		renderer.rectangle(width * v.x, height * v.y, width, height, 1, 0, 1, 1)
	end

	renderer.rectangle(width * Snake.head.x, height * Snake.head.y, width, height, 1, 0, 0, 1)

	image.drawRectangle(width * Coin.x, height * Coin.y, width, height, Coin.image)
end

close = function()
	image.free(Coin.image)
end

mousePressed = function()
	if mouse_x < Screen_Width / 4 then
		Snake.head.direction = 1
	elseif mouse_x > Screen_Width / 4 * 3 then
		Snake.head.direction = 3
	elseif mouse_y < Screen_Height / 4 then
		Snake.head.direction = 2
	elseif mouse_y > Screen_Height / 4 * 3 then
		Snake.head.direction = 4
	end
end

return snake