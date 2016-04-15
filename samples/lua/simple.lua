local app_event_trigger = nil
local app = nil

function onInit (event)
	ATOM_KernelConfig.initializeWithPath(ATOM_GetCurrentDirectory())
	registerPluginsImage ()
	registerPluginsRender ()
	registerPluginsEngine ()
	ATOM_InitializeGfx (ATOM_RenderWindowDesc(400, 300, false, false, true, 0, "atom3d lua example"), true, false)
end

function onExit (event)
	ATOM_DoneGfx ()
end

function onIdle (event)
	local device = ATOM_GetRenderDevice()
	device:setClearColor (nil, 0, 0, 0.5, 1)
	device:beginFrame ()
	device:clear (true, true, true)
	device:endFrame ()
	device:present ()
end

function onWindowClose (event)
	app:postQuitEvent (0)
end

function main ()	
	app_event_trigger = ATOM_ScriptEventTrigger(__SCRIPT__)
	app_event_trigger.onAppInit = "onInit"
	app_event_trigger.onAppIdle = "onIdle"
	app_event_trigger.onAppExit = "onExit"
	app_event_trigger.onWindowClose = "onWindowClose"
	app = ATOM_Application()
	app:setEventTrigger(app_event_trigger)
	app:run ()
end
