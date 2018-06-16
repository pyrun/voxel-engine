function start( this )
	-- body
	print( this, "hello world!")
	print( this, "i have control about you! RISE AND SHINE")
end

function destory( ... )
	-- body
end

side_x = true
ground_flag = false

function timer( this, time )
	x, y, z = getPosition( this)
	velx, vely, velz = getVelocity( this)
	ground = getHit( this, 0)

	if getHit( this, 2) and side_x == false then
		side_x = true
		setRotation( this, 0, -3.14/2, 0)
	elseif getHit( this, 3) and side_x == true then
		side_x = false
		setRotation( this, 0, 3.14/2, 0)
	end

	if ground then
		if side_x == false then 
			if velx < 0.05 then
				addVelocity( this, 0.01, 0, 0)
			end
		else
			if velx > -0.05 then
				addVelocity( this, -0.01, 0, 0)
			end
		end
	end
end