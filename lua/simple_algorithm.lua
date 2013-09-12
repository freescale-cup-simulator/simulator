run_counter = run_counter or 0
last_turn_angle = last_turn_angle or 0

if g_camera_frame then
	return run()
end

function run()
	-- print("[Lua] run #" .. run_counter)
	run_counter = run_counter + 1

	-- print_array(g_camera_frame)
	local b = binarize(g_camera_frame)
	-- print_array(b)
	local ls, le = nil, nil
	local sls, sle = false, false
	for i = 63, 0, -1 do
		if not sls and b[i] == 1 then
			sls = true
		end
		if sls and b[i] == 0 then
			ls = i
			break
		end
	end
	for i = 64, 127, 1 do
		if not sle and b[i] == 1 then
			sle = true
		end
		if sle and b[i] == 0 then
			le = i
			break
		end
	end
	if not le then le = #b end
	if not ls then ls = 0 end
	-- print("[Lua] Line begins at " .. ls)
	-- print("[Lua] Line ends at " .. le)

	local m = (ls + le) / 2 - 64
	local a = (m / 128)
	-- if (math.abs(a) <= 0.05) then
	-- 	a = last_turn_angle
	-- else
	-- 	last_turn_angle = a
	-- end
	-- print("[Lua] Turn angle is " .. a)

	local signals = {}
	signals["angle"] = a
	signals["lspeed"] = 1 - (a / 90)
	signals["rspeed"] = 1 - (a / 90)
	-- print(signals)
	return signals
end

function print_array(a)
	for _,v in pairs(a) do
		io.write(v .. " ")
	end
	print()
end

function binarize(a)
	local t = {}
	for k,v in pairs(a) do
		if (v <= 210) then
			t[k] = 1
		else
			t[k] = 0
		end
	end
	return t
end
