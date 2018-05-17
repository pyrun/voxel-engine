local l_grass = getByName( "grass")
local l_earth = getByName( "earth")
local l_stone = getByName( "stone")

local l_glowcrystal = getByName( "glowcrystal")
local l_type = 0;

local l_dstance_island = 0
local l_noise_top = 0
local l_noise_down = 0
local l_size_inv = 30

function rowX( real_pos_x, real_pos_y, real_pos_z, seedX, seedY, seedZ)

end

function rowZ( real_pos_x, real_pos_y, real_pos_z, seedX, seedY, seedZ)
	l_dstance_island = distanceVec2( 0, 0, real_pos_x, real_pos_z)

	l_noise_top = -perlinVec2( (real_pos_x + seedX)/l_size_inv, (real_pos_z + seedZ)/l_size_inv )
	l_noise_top = l_noise_top*2

	l_noise_down = perlinVec2( (real_pos_x + seedX)/l_size_inv, (real_pos_z + seedZ)/l_size_inv )
	l_noise_down = l_noise_down * 40 - 20

	if  l_dstance_island > 20 then
	    l_noise_down = l_noise_down + ((l_dstance_island-20)/1.1)
	end
end

function block( chunk_id, position_chunk_x, position_chunk_y, position_chunk_z, real_pos_x, real_pos_y, real_pos_z, seedX, seedY, seedZ)
	local l_light = false

	l_type = l_stone

	if not (l_noise_top > real_pos_y+1) then
	    l_type = l_grass
	elseif not (l_noise_top > real_pos_y+3) then
		l_type = l_earth
	end

	if rand( 60) == 1 and l_noise_top > real_pos_y-1 and l_noise_top < real_pos_y and l_noise_down < real_pos_y-1 then
		l_type = l_glowcrystal
		l_light = true
		return l_light, l_type
	end

	if l_noise_top < real_pos_y then
		return false, 0;
	elseif l_noise_down > real_pos_y then
		return false, 0;
	end

	return l_light, l_type
end