speed_constant = 0.0009

function start( this )
	-- body
end

function jump( this, delta)
	if getHit( this, 0) then
		addVelocity( this, 0, 0.06, 0)
		setHit( this, 0, false)
	end
end

function up( this, delta)
	if getHit( this, 0) then
		x, y, z = calcForceUp( delta, speed_constant)
		addVelocity( this, x, y, z)
	end
end

function down( this, delta)
	if getHit( this, 0) then
		x, y, z = calcForceUp( delta, speed_constant)
		addVelocity( this, -x, -y, -z)
	end
end

function right( this, delta)
	if getHit( this, 0) then
		x, y, z = calcForceSide( delta, speed_constant)
		addVelocity( this, -x, -y, -z)
	end
end

function left( this, delta)
	if getHit( this, 0) then
		x, y, z = calcForceSide( delta, speed_constant)
		addVelocity( this, x, y, z)
	end
end

function destory( ... )
	-- body
end

function timer( ... )
	-- body
end
