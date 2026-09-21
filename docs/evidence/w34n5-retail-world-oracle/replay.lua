local ffi = require("ffi")
local mem = PCSX.getMemPtr()

local function off(addr) return bit.band(addr, 0x001fffff) end
local function ptr(addr) return mem + off(addr) end
local function s32(addr) return tonumber(ffi.cast("int32_t*", ptr(addr))[0]) end
local function reg(name) return tonumber(PCSX.getRegisters().GPR.n[name]) end

local worldFrame, branchCount = 0, 0
local loaded = false

PCSX.addBreakpoint(0x8007105c, "Exec", 4, "slot1 call", function()
    print(string.format("W34N5_REPLAY SLOT1_CALL target=%08x", reg("v0")))
end)
PCSX.addBreakpoint(0x80071064, "Exec", 4, "slot1 return", function()
    print(string.format("W34N5_REPLAY SLOT1_RETURN D7CC=%d", s32(0x8009d7cc)))
end)
PCSX.addBreakpoint(0x800712d0, "Exec", 4, "driver", function()
    print(string.format("W34N5_REPLAY DRIVER D554=%d D7CC=%d",
        s32(0x8009d554), s32(0x8009d7cc)))
end)
PCSX.addBreakpoint(0x8007130c, "Exec", 4, "frame head", function()
    worldFrame = worldFrame + 1
    print(string.format("W34N5_REPLAY FRAME_HEAD frame=%d D554=%d D7CC=%d",
        worldFrame, s32(0x8009d554), s32(0x8009d7cc)))
end)
PCSX.addBreakpoint(0x800719c8, "Exec", 4, "frame branch", function()
    branchCount = branchCount + 1
    print(string.format("W34N5_REPLAY FRAME_BRANCH frame=%d D554=%d taken=%d",
        worldFrame, s32(0x8009d554), s32(0x8009d554) ~= 0 and 1 or 0))
    if branchCount >= 5 then
        print(string.format("W34N5_REPLAY PASS frames=%d", worldFrame))
        PCSX.quit(0)
    end
end)

PCSX.Events.createEventListener("ExecutionFlow::SaveStateLoaded", function()
    loaded = true
    print("W34N5_REPLAY STATE_LOADED")
end)
PCSX.Events.createEventListener("ExecutionFlow::ShellReached", function()
    PCSX.pauseEmulator()
    PCSX.nextTick(function()
        local stateFile = Support.File.open(
            "/home/blizz/dev/xenogears-decomp/scratchpad/retail_world_session_3e10093a.state",
            "READ")
        PCSX.loadSaveState(stateFile)
        if not loaded then print("W34N5_REPLAY LOAD_EVENT_MISSING") end
        PCSX.resumeEmulator()
    end)
end)

print("W34N5_REPLAY READY")
