run_counter = run_counter or 0

if g_camera_frame then
	return run()
end

function run()
	print("[Lua] run #" .. run_counter)
	run_counter = run_counter + 1

	local b = binarize(g_camera_frame)
	-- print_array(b)
	local ls, le = nil, nil
	for k,v in pairs(b) do
		if v == 1 then
			if not ls then
				ls = k
			end
		elseif ls then
			le = k
			break
		end
	end
	if not le then le = #b end
	print("[Lua] Line begins at " .. ls)
	print("[Lua] Line ends at " .. le)

	local m = (ls + le) / 2 - 64
	local a = (m / 128) * 45
	print("[Lua] Turn angle is " .. a)

	local signals = {}
	signals["angle"] = a
	signals["lspeed"] = 42 * run_counter
	signals["rspeed"] = 42 * run_counter
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
		if (v <= 30) then
			t[k] = 1
		else
			t[k] = 0
		end
	end
	return t
end
