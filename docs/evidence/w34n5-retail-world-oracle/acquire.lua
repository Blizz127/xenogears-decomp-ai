local ffi = require("ffi")
local mem = PCSX.getMemPtr()

local function off(addr) return bit.band(addr, 0x001fffff) end
local function ptr(addr) return mem + off(addr) end
local function s32(addr) return tonumber(ffi.cast("int32_t*", ptr(addr))[0]) end
local function u32(addr) return tonumber(ffi.cast("uint32_t*", ptr(addr))[0]) end
local function w32(addr, value) ffi.cast("uint32_t*", ptr(addr))[0] = value end
local function reg(name) return tonumber(PCSX.getRegisters().GPR.n[name]) end

local fieldFrame, worldFrame, session, branchCount = 0, 0, 0, 0
local stateSaved, exitSeeded, loaded = false, false, false

local function saveWorldState()
    local state = PCSX.createSaveState()
    local path = "/home/blizz/dev/xenogears-decomp/scratchpad/retail_world_session_3e10093a.state"
    local out = Support.File.open(path, "TRUNCATE")
    out:writeMoveSlice(state)
    out:close()
    stateSaved = true
    print(string.format("W34N5_TRACE STATE_SAVED path=%s mode=%d D7CC=%d",
        path, u32(0x8009c5a8), s32(0x8009d7cc)))
end

PCSX.addBreakpoint(0x8007554c, "Exec", 4, "field frame", function()
    fieldFrame = fieldFrame + 1
end)
PCSX.addBreakpoint(0x8007856c, "Exec", 4, "retail exit test", function()
    if not exitSeeded then
        exitSeeded = true
        w32(0x800adbe4, 0)
        print(string.format("W34N5_EXIT SEED pc=8007856c frame=%d D_ADBE4=%d",
            fieldFrame, s32(0x800adbe4)))
    end
end)
PCSX.addBreakpoint(0x80070cfc, "Exec", 4, "WorldMapMain", function()
    print(string.format("W34N5_TRACE WORLD_ENTRY fieldFrame=%d", fieldFrame))
end)
PCSX.addBreakpoint(0x80071000, "Exec", 4, "slot0", function()
    print(string.format("W34N5_TRACE SLOT0_SELECT mode=%d D7CC=%d",
        u32(0x8009c5a8), s32(0x8009d7cc)))
end)
PCSX.addBreakpoint(0x80071034, "Exec", 4, "session head", function()
    session = session + 1
    print(string.format("W34N5_TRACE SESSION_HEAD n=%d mode=%d D7CC=%d",
        session, u32(0x8009c5a8), s32(0x8009d7cc)))
    if not stateSaved then saveWorldState() end
end)
PCSX.addBreakpoint(0x8007105c, "Exec", 4, "slot1", function()
    print(string.format("W34N5_TRACE SLOT1_CALL session=%d target=%08x", session, reg("v0")))
end)
PCSX.addBreakpoint(0x80071064, "Exec", 4, "slot1 return", function()
    print(string.format("W34N5_TRACE SLOT1_RETURN session=%d D7CC=%d",
        session, s32(0x8009d7cc)))
end)
PCSX.addBreakpoint(0x800712d0, "Exec", 4, "driver", function()
    print(string.format("W34N5_TRACE DRIVER session=%d D554=%d D7CC=%d",
        session, s32(0x8009d554), s32(0x8009d7cc)))
end)
PCSX.addBreakpoint(0x8007130c, "Exec", 4, "frame head", function()
    worldFrame = worldFrame + 1
    print(string.format("W34N5_TRACE FRAME_HEAD frame=%d D554=%d D7CC=%d",
        worldFrame, s32(0x8009d554), s32(0x8009d7cc)))
end)
PCSX.addBreakpoint(0x800719c8, "Exec", 4, "frame branch", function()
    branchCount = branchCount + 1
    print(string.format("W34N5_TRACE FRAME_BRANCH frame=%d D554=%d taken=%d",
        worldFrame, s32(0x8009d554), s32(0x8009d554) ~= 0 and 1 or 0))
    if branchCount >= 5 then
        print(string.format("W34N5_TRACE PASS frames=%d sessions=%d stateSaved=%d",
            worldFrame, session, stateSaved and 1 or 0))
        PCSX.quit(0)
    end
end)

PCSX.Events.createEventListener("ExecutionFlow::SaveStateLoaded", function()
    loaded = true
    print("W34N5_EXIT FIELD_STATE_LOADED")
end)
PCSX.Events.createEventListener("ExecutionFlow::ShellReached", function()
    PCSX.pauseEmulator()
    PCSX.nextTick(function()
        local stateFile = Support.File.open(
            "/home/blizz/dev/xenogears-decomp/scratchpad/retail_field_entry_3e10093a_scph5500_f180.state",
            "READ")
        PCSX.loadSaveState(stateFile)
        if not loaded then print("W34N5_EXIT LOAD_EVENT_MISSING") end
        PCSX.resumeEmulator()
    end)
end)

print("W34N5_ACQUIRE READY")
