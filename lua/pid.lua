local M = {}

local dT = 0.01
local Kp = 0.88
local Ti = 0.18
local Td = 0

local Ki = 0
local Kd = 0

local storedErrors = 3
local errors = {}

local last_ouput = 0
local last_input = 0

local initial_reset = false

local Omin = -45.0
local Omax = 45.0

function transform(x, in_min, in_max, out_min, out_max)
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
end

function M.reset()
    Ki = Ti * Kp * dT
    Kd = Td * Kp / dT

    for i = 1, storedErrors do
        errors[i] = 0
    end
    last_ouput = 0
end

function M.run(setpoint, input)
    local output

    if not initial_reset then M.reset() end

    if input < 0 then input = 0 end

    setpoint = setpoint - 63
    input = input - 63

    errors[3] = errors[2]
    errors[2] = errors[1]
    errors[1] = input - setpoint

    local P = Kp * (errors[1] - errors[2])
    local I = Ki * errors[1]
    local D = Kd * (errors[1] - 2.0 * errors[2] + errors[3])

    output = last_ouput + P + I + D
    last_ouput = output

    output = output * transform(math.abs(input), 0.0, 64.0, 0.0, 2.0)
    
    local inDerivative = input - last_input;
    last_input = input;

    output = output * transform(math.abs(inDerivative), 0, 127.0, 1.0, 2.0)
    output = transform(output, -63.0, 63.0, Omin, Omax)
    
    if (output > Omax) then output = Omax end
    if (output < Omin) then output = Omin end

    return output
end

return M
