#!/bin/env lua

AllSpecs = {}
TotalSpecs = 0
FailedSpecs = 0
PassedSpecs = 0
Failures = {}

-- Spec DSL Definitions
-------------------------------------------------------------------------------
function describe(desc)
    return function(specs)
        table.insert(AllSpecs, { type = "describe", desc = desc, specs = specs})
    end
end

function context(desc)
    return function(specs)
        return { type = "context", desc = desc, specs = specs }
    end
end

function it(desc)
    return function(spec)
        return { type = "it", desc = desc, spec = spec }
    end
end

-- Test Assertion Functions
-------------------------------------------------------------------------------
function check_nequal(expected, actual)
    if (expected == actual) then
        error(string.format("Expected %s and %s to differ", tostring(expected), tostring(actual)))
    end
end

function check_equal(expected, actual)
    if (expected ~= actual) then
        error(string.format("Expected %s, received %s instead", tostring(expected), tostring(actual)))
    end
end

-- Spec Driver Logic
-------------------------------------------------------------------------------
builtin_error = error
function error_wrapper(msg)
    builtin_error({msg = msg})
end

function parse_stack(errobj)
  local lastframe = nil
  local level = 1
  while true do
    local info = debug.getinfo(level, "nSl")
    if not info then break end
    if info.name and info.name == "xpcall" then
        break
    else
        lastframe = string.format("%s:%d", info.short_src, info.currentline)
    end
    level = level + 1
  end
  if (type(errobj) == "table") then
    return string.format("%s: %s", lastframe, errobj.msg)
  else
    local  fstoccr = errobj:find(":")
    local  secoccr = errobj:find(":",fstoccr+1)
    return string.format("%s: %s", lastframe, errobj:sub(secoccr+2, -1))
  end
end

function run_spec(spec, depth)
    local status, errobj = xpcall(spec.spec, parse_stack)
    --local status, errobj = pcall(spec.spec)
    if not status then
        FailedSpecs = FailedSpecs + 1
        table.insert(Failures, { spec = spec, err = errobj })
        io.write(" (Failed - ", FailedSpecs, ")")
    else
        PassedSpecs = PassedSpecs + 1
    end
    TotalSpecs = TotalSpecs + 1
end

function run_specs(specs, depth)
    error = error_wrapper
    for k,val in ipairs(specs) do
        -- Print the description
        for i=1, depth do
            io.write("  ")
        end
        io.write(val.desc)
        -- Run the test or recursively run the group
        if val.type == "it" then
            run_spec(val, depth)
            io.write("\n")
        else
            io.write("\n")
            run_specs(val.specs, depth+1)
        end
    end
    error = builtin_error
end

-- Spec Main Routine
-------------------------------------------------------------------------------
(function()
    -- Load up the spec files specified on the command line
    for k,val in ipairs(arg) do
        dofile(val)
    end
    -- Run all the specs
    run_specs(AllSpecs, 0)
    -- Print the results
    if table.getn(Failures) > 0 then
        io.write("\nFailures:\n")
    end
    for k,val in ipairs(Failures) do
        io.write("  ", val.err, "\n")
    end
    io.write("\nTest Results\n")
    io.write("------------\n")
    io.write("Total:  ", TotalSpecs, "\n")
    io.write("Passed: ", PassedSpecs, "\n")
    io.write("Failed: ", FailedSpecs, "\n")
end)()

