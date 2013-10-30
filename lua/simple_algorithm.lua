last_turn_angle = last_turn_angle or 0

-- g_camera_frame is not set when Lua does a compile run
if g_camera_frame then
	return run()
end

function calculate_threshold(frame)
        local sum = 0
        local lowest = 255
        for k, v in pairs(frame) do
               sum = sum + v
               if v < lowest then lowest = v end
        end
        sum = sum / 128 - lowest / 1
        return sum
end

function binarize(frame)
	local binarized = {}
	local threshold = calculate_threshold(frame)
        print_array(frame)
	for k,v in pairs(frame) do
		if (v <= threshold) then
			binarized[k] = true
		else
			binarized[k] = false
		end
	end
	return binarized
end

function run()
	local b = binarize(g_camera_frame)
        --print_array(b)
        local line_found = false

	local start_found, end_found = false, false
	local start_pos, end_pos = nil, nil

	for k, v in pairs(b) do
	        if v and not start_found then
                        start_found = true
	                start_pos = k
	        end
	        if not v and start_found and math.abs(start_pos - k) < 40 then
	                end_found = true
	                end_pos = k
                end
        end

        local angle
        if start_found and end_found then
                local m = (start_pos + end_pos) / 2 - 64
	        angle = (m / 128)
	        last_turn_angle = angle
        else
                angle = last_turn_angle
	end
                print("a = " .. angle)

	local signals = {}
	signals["angle"] = angle * 45;
	signals["lspeed"] = (1 - (math.abs(angle) / 45)) * 10
	signals["rspeed"] = signals["lspeed"]
	return signals
end

function print_array(a)
        local p
	for _,v in pairs(a) do
	p = v
        if type(v) == "boolean" then
                if p then
                        p = 1
                else
                        p = 0
                end
        end
                io.write(p .. " ")
	end
	print()
end
