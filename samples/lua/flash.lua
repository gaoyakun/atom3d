app_event_trigger = nil
flash_event_trigger = nil
app = nil
guirenderer = nil
flashctrl = nil

function onInit (event)
	ATOM_KernelConfig.initializeWithPath(ATOM_GetCurrentDirectory())
	registerPluginsImage ()
	registerPluginsRender ()
	registerPluginsEngine ()
	ATOM_InitializeGfx (ATOM_RenderWindowDesc(300, 200, false, false, true, 0, "atom3d lua example"), true, false)

	guirenderer:setViewport(ATOM_Rect2Di(0, 0, 300, 200))

	flash_event_trigger = ATOM_ScriptEventTrigger(__SCRIPT__)
	flash_event_trigger.onFlashCall = "onFlashCall"
	flashctrl = ATOM_FlashCtrl(guirenderer:getDesktop(), ATOM_Rect2Di(0, 0, 300, 200), 0, 0, ATOM_Widget.ShowNormal)
	flashctrl:setEventTrigger (flash_event_trigger)
	
	flashctrl:loadSWF("/interactionTest.swf")
end

function onFlashCall (event)
	local flashCallEvent = ATOM_FlashCallEvent._CastFrom(event)
	ATOM_LOGGER.log("flash called: "..flashCallEvent.request)
end

function onExit (event)
	ATOM_DoneGfx ()
end

function onIdle (event)
	guirenderer:handleEvent(event)
	local device = ATOM_GetRenderDevice()
	device:setClearColor (nil, 0, 0, 0.5, 1)
	device:beginFrame ()
	device:clear (true, true, true)
	guirenderer:render()
	device:endFrame ()
	device:present ()
end

function onWindowClose (event)
	app:postQuitEvent (0)
end

function onUnhandled (event)
	guirenderer:handleEvent(event)
end

function main ()	
	guirenderer = ATOM_GUIRenderer()

	app_event_trigger = ATOM_ScriptEventTrigger(__SCRIPT__)
	app_event_trigger.onAppInit = "onInit"
	app_event_trigger.onAppIdle = "onIdle"
	app_event_trigger.onAppExit = "onExit"
	app_event_trigger.onWindowClose = "onWindowClose"
	app_event_trigger.onUnhandled = "onUnhandled"
	app = ATOM_Application()
	app:setEventTrigger(app_event_trigger)

	app:run ()
end
