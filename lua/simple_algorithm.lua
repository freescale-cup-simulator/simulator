last_turn_angle = last_turn_angle or 0
last_lCenter = last_lCenter or 999
last_lWidth = last_lWidth or 999
lCenter = lCenter or 0


-- g_camera_frame is not set when Lua does a compile run
if g_camera_frame then
	return run()
end

function process_camera(camera_array)
    local CAM_ERROR = 15.0

    local edge = 0
    local minimal = camera_array[1]
    local min_center = 999
    local was_min = false
    local min_width = 0
    local error = camera_array[1] / 100.0 * CAM_ERROR
    local lineFlag = 0
    local finishFlag = 0

    for i = 1, 128 do
        edge = edge + camera_array[i]
        if camera_array[i] < minimal then
            minimal = camera_array[i]
        end
    end
    edge = edge / 128

    local flag = 0
    local err = edge * 0.4
    local lWidth = 0
    local lCenter = 0
    local centerDeriv = 0
    local widthDeriv = 0

    local i_ = 0

    for i = 6, 128 do
        if flag > 0 then
            if (camera_array[i] - camera_array[i-2]) > err then
                lWidth = i - flag
                local centerNow = flag + lWidth / 2
                centerDeriv = math.abs(last_lCenter - centerNow)
                widthDeriv = math.abs(last_lWidth - lWidth)

                if (widthDeriv < 10 and centerDeriv < 50 and centerDeriv < min_center
                    and lWidth > 4 and lWidth < 36) or last_lWidth == 999 then

                    last_lWidth = lWidth
                    min_width = widthDeriv
                    min_center = centerDeriv
                    lCenter = centerNow
                    lineFlag = lineFlag + 1
                    finishFlag = finishFlag - 1
                end

                if lWidth > 5 and lWidth < 36 then
                    finishFlag = finishFlag + 1
                end

                was_min = 1
                flag = 0
            end
        end

        if flag == 0 then
            if (camera_array[i] - camera_array[i-2]) < -err then
                flag = i
                was_min = 1
            end
        end
        i_ = i
    end

    if lCenter > 0 then
        finishCount = 0
        last_lCenter = lCenter
        return lCenter
    else
        return -1
    end
end

function run()
    -- print_array(g_camera_frame)
    local center = process_camera(g_camera_frame)

    -- print(center)

    if center ~= -1 then
        center = center - 64
        last_turn_angle = center
    else
        center = last_turn_angle
    end

	local signals = {}
	signals["angle"] = center
	signals["lspeed"] = (1 - (math.abs(center) / 45)) * 10
	signals["rspeed"] = signals["lspeed"]
	signals["linepos"] = center
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
