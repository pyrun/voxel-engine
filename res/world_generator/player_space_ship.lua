local l_steel_plate = getByName( "steel_plate")
local l_steel = getByName( "steel")
local l_earth = getByName( "earth")

local l_glowcrystal = getByName( "glowcrystal")
local l_type;

local l_dstance_island = 0
local l_noise_top = 0
local l_noise_down = 0
local l_size_inv = 30

local one_chunk = false

-- not working
function start()
	for x=-2,2 do
		for y=1,-2 do
			for z=-2,2 do
				addChunk( x, y, z, true)
			end
		end
	end
end

function rowX( real_pos_x, real_pos_y, real_pos_z, seedX, seedY, seedZ)
	one_chunk = false
	if real_pos_x < 32 and real_pos_x >= 0 then
		one_chunk = true
	end
end

function rowZ( real_pos_x, real_pos_y, real_pos_z, seedX, seedY, seedZ)
	if real_pos_z < 32 and real_pos_z >= 0 then
	else
		one_chunk = false
	end
end

local hit = false

function block( chunk_id, position_chunk_x, position_chunk_y, position_chunk_z, real_pos_x, real_pos_y, real_pos_z, seedX, seedY, seedZ)
	local l_light = false

	l_type = l_earth

	if real_pos_y < 32 and real_pos_y >= 0 then
	else
		one_chunk = false
	end

	if one_chunk == false then
		return false, 0;
	end

	if real_pos_y%32 == 0 or real_pos_y%32 == 31 then
		l_type = l_steel_plate
	end

	if real_pos_x%32 == 0 or real_pos_x%32 == 31 or
		real_pos_z%32 == 0 or real_pos_z%32 == 31 then
		l_type = l_steel_plate
	end

	local cross = false
	local cross2 = false

	if real_pos_y%31 == 6 and real_pos_x%(seedX%6+3) == 0 and not(l_type == l_steel_plate) then
		l_type = l_steel
		cross = true
	end

	if real_pos_y%31 == 6 and real_pos_z%(seedZ%6+3) == 0 then
		l_type = l_steel
		if cross == true then
			cross2 = true
		end
	end

	if cross2 then
		l_type = l_glowcrystal
		l_light = true
	end

	if real_pos_x == 15 and real_pos_y == 0 and real_pos_z == 15  then
		hit = true
		setPortal( chunk_id, real_pos_x-position_chunk_x, real_pos_y-position_chunk_y+1, real_pos_z-position_chunk_z)
	end

	if l_earth == l_type then
		return false, 0;
	end

	return l_light, l_type
end