local l_steel_plate = getByName( "steel_plate")
local l_steel = getByName( "steel")
local l_earth = getByName( "earth")

local l_glowcrystal = getByName( "glowcrystal")
local l_type = 0;

local l_dstance_island = 0
local l_noise_top = 0
local l_noise_down = 0
local l_size_inv = 30

--function rowX( real_pos_x, real_pos_y, real_pos_z, seedX, seedY, seedZ)

--end

function rowZ( real_pos_x, real_pos_y, real_pos_z, seedX, seedY, seedZ)

end

local hit = false

function block( chunk_id, position_chunk_x, position_chunk_y, position_chunk_z, real_pos_x, real_pos_y, real_pos_z, seedX, seedY, seedZ)
	local l_light = false

	l_type = l_earth


	if real_pos_x%16 == 0 or real_pos_y%16 == 0 or real_pos_z%16 == 0 then
		l_type = l_steel
	end

	if rand( 22) == 1 and l_type == l_steel then
		l_type = l_glowcrystal
		l_light = true
	end

	if real_pos_x == 0 and real_pos_y == 0 and real_pos_z == 0 then
		hit = true
		setPortal( chunk_id, real_pos_x-position_chunk_x, real_pos_y-position_chunk_y+1, real_pos_z-position_chunk_z)
	end

	if l_earth == l_type then
		return false, 0;
	end

	return l_light, l_type
end