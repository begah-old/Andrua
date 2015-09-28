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

	Square_Width = Screen_Width / Game_Width
	Square_Height = Screen_Height / Game_Height

	Coin.image = animation.load("coin.png")

	for i in pairs(animation) do
		print(i)
	end

	animation.setSize(Coin.image, Square_Width, Square_Height)

	Coin.x = math.random(Game_Width) - 1
	Coin.y = math.random(Game_Height) - 1

	animation.setPos(Coin.image, Square_Width * Coin.x, Coin.y * Square_Height)
	animation.reverseOnFinish(Coin.image)
end

createExplosion = function()
	local Explosion = particle.new( Square_Width * Snake.head.x + Square_Width / 2,
		Square_Height * Snake.head.y + Square_Height / 2, 500)

	particle.setGravity(Explosion, 0, 0)
	particle.setLife(Explosion, 500, 1000)
	particle.setSpeed(Explosion, 0.05, 4)
	particle.setColor(Explosion, 1, 0.9, 0.1, 1, 1, 0, 0, 0)

	particle.emit(Explosion, 500)
	particle.toggleEmitting(Explosion)

	particle.deleteOnFinish(Explosion, true)
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

	if part.x >= Game_Width then
		part.x = 0
	elseif part.x < 0 then
		part.x = Game_Width - 1
	elseif part.y < 0 then
		part.y = Game_Height - 1
	elseif part.y >= Game_Height then
		part.y = 0
	end
end

update = function()
	Snake.head.lastX = Snake.head.x
	Snake.head.lastY = Snake.head.y
	updatePart(Snake.head)

	if Snake.head.x == Coin.x and Snake.head.y == Coin.y then
		Coin.x = math.random(Game_Width) - 1
		Coin.y = math.random(Game_Height) - 1

		animation.setPos(Coin.image, Square_Width * Coin.x, Square_Height * Coin.y)

		createExplosion()
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
	print(engine.getFPS())
	Counter = Counter + 1
	if Counter >= engine.getFPS() / 10 then
		update()
		Counter = 0
	end

	Square_Width = Screen_Width / Game_Width
	Square_Height = Screen_Height / Game_Height

	for i,v in ipairs(Snake.body) do
		renderer.rectangle(Square_Width * v.x, Square_Height * v.y, Square_Width, 
			Square_Height, 1, 0, 1, 1)
	end

	renderer.rectangle(Square_Width * Snake.head.x, Square_Height * Snake.head.y, 
		Square_Width, Square_Height, 1, 0.2, 0.7, 1)

	animation.render(Coin.image)
end

close = function()
	image.free(Coin.image)
end

mousePressed = function()
	if mouse_x < Screen_Width / 4 and Snake.head.direction % 2 == 0 then
		Snake.head.direction = 1
	elseif mouse_x > Screen_Width / 4 * 3 and Snake.head.direction % 2 == 0 then
		Snake.head.direction = 3
	elseif mouse_y < Screen_Height / 4 and Snake.head.direction % 2 ~= 0 then
		Snake.head.direction = 2
	elseif mouse_y > Screen_Height / 4 * 3 and Snake.head.direction % 2 ~= 0 then
		Snake.head.direction = 4
	end
end

return snake