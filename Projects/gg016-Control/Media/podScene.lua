-- Global array with keys used in the script. PhysicsApp::animate will update these, and only these, in every frame.
keysPressed={
	U=false,
	J=false,
	H=false,
	K=false,
	VK_SPACE=false,
	VK_NUMPAD0=false,
	VK_NUMPAD1=false,
	VK_NUMPAD2=false,
	VK_NUMPAD3=false,
	VK_NUMPAD4=false,
	VK_NUMPAD5=false,
	VK_NUMPAD6=false,
	VK_NUMPAD7=false,
	VK_NUMPAD8=false,
	VK_NUMPAD9=false
}
dt = 0.1

cloneInstanceId = 0
-- Function to perform a one level deep copy on a table. Useful to clone control state tables.
function clone(t)
  local t2 = {}
  for k,v in pairs(t) do
    t2[k] = v
  end
  t2.instanceId = cloneInstanceId
  cloneInstanceId = cloneInstanceId + 1
  return t2
end

-- create a giraffe MultiMesh manually
O:IndexedMesh(_, {name='giraffe', file='giraffe.obj'})
O:Material(_, {name='spotted', technique='textured', pass='textured'}, function(_)
	O:setSrv(_, {effectVariable='kdTexture', file='giraffe.jpg'})
end )
O:MultiMesh(_, {name='giraffe'}, function(_)
	O:FlipMesh(_, {}, function(_)
		O:ShadedMesh(_, {mien='basic', indexedMesh='giraffe', material='spotted'})
	end )
end )

-- another giraffe multimesh with different effect pass
O:Material(_, {name='shiny', technique='envmapped', pass='envmapped'}, function(_)
	O:setSrv(_, {effectVariable='kdTexture', file='giraffe.jpg'})
	O:setSrv(_, {effectVariable='envTexture', file='cloudyNoon.dds'})
end )
O:MultiMesh(_, {name='shinyGiraffe'}, function(_)
	O:FlipMesh(_, {}, function(_)
		O:ShadedMesh(_, {mien='basic', indexedMesh='giraffe', material='shiny'})
	end )
end )

-- sky background multimesh
O:IndexedMesh(_, {name='quad', file='quad.x'})
O:Material(_, {name='sky', technique='envmapped', pass='background'}, function(_)
	O:setSrv(_, {effectVariable='envTexture', file='cloudyNoon.dds'})
end )
O:MultiMesh(_, {name='background'}, function(_)
	O:FlipMesh(_, {}, function(_)
		O:ShadedMesh(_, {mien='basic', indexedMesh='quad', material='sky'})
	end )
end )

-- some entities
O:StaticEntity(_, {name='giraffe0', multiMesh='giraffe', position = { x=2, y=0, z=0} } )
O:StaticEntity(_, {name='giraffe1', multiMesh='shinyGiraffe', position = { x=0, y=0, z=0} } )
O:StaticEntity(_, {name='background', multiMesh='background' } )

-- from file
O:MultiMeshFromFile(_, {name='ground', file='ground.x'})

-- from file
O:MultiMeshFromFile(_, {name='pod', file='geopod.x'})

-- manually, based on geometries already loaded by MultiMeshFromFile
O:MultiMesh(_, {name='envmapWindowPod'}, function(_)
	O:FlipMesh(_, {}, function(_)
		O:ShadedMesh(_, {mien='basic', indexedMesh='pod[0]', material='spotted'})
	end )
	O:FlipMesh(_, {}, function(_)
		O:ShadedMesh(_, {mien='basic', indexedMesh='pod[1]', material='shiny'})
	end )
end )

-- material for physics simulation
O:PhysicsMaterial(_, {name='default'})

-- Function to set dynamic properties and add shapes to a PhysicsEntity. Predefined as a lua function so that it can be reused for multiple entities.
geopodShapes = function(_)
	O:Dynamics(_, {actorFlags={'eDISABLE_GRAVITY'}, linearDamping=2, angularDamping=2} )
	O:Shape(_, {geometryType='eCAPSULE', material='default', orientationAngle=0, orientationAxis={x=0, y=1, z=0}, halfHeight=5, radius=3, position = { y=0 } } )
	O:Shape(_, {geometryType='eBOX',     material='default', orientationAngle=0.79, orientationAxis={x=0, y=1, z=0}, halfExtents = {x=4, y=1, z=4}, position = { x=4.5, y=-1, z=0 } } )
	O:Shape(_, {geometryType='eBOX',     material='default', orientationAngle=0.79, orientationAxis={x=0, y=1, z=0}, halfExtents = {x=4, y=1, z=4}, position = { x=-5, y=-1, z=-2 } } )
	O:Shape(_, {geometryType='eBOX',     material='default', orientationAngle=0.79, orientationAxis={x=0, y=1, z=0}, halfExtents = {x=4, y=1, z=4}, position = { x=-5, y=-1, z=2 } } )
	O:Shape(_, {geometryType='eBOX',     material='default', orientationAngle=1.098, orientationAxis={x=-0.146, y=-0.353, z=-0.353}, halfExtents = {x=4, y=1, z=4}, position = { x=-7, y=2, z=0 } } )
end

O:MultiMeshFromFile(_, {name='rocket', file='rocket.obj'})

rocketControlState = { 
	script = function(entity, state)
	end,
	onContact = function(entity, state, otherState)
		state.killed = true
	end 	
}

rocketShapes = function(_)
	O:Dynamics(_, {actorFlags={'eDISABLE_GRAVITY'}, linearVelocity={x=100} } )
	O:Shape(_, {geometryType='eCAPSULE', material='default', orientationAngle=0, orientationAxis={x=0, y=1, z=0}, halfHeight=2, radius=1, position = { y=0 } } )
end

-- A lua table that can be set as the controlState object of a PhysicsEntity. It must have a 'script' key, with a function value. PhysicsEntity::animate will call it.
playerControlState = { 
	rocketIndex = 0,
	cooldown = 0.5,
	cooldownRemaining = 0,
	script = function(entity, state)
		state.cooldownRemaining = state.cooldownRemaining - dt
		if keysPressed.VK_NUMPAD5 == true then O:addForceAndTorque(entity, { force={x= 100000} } ) end
		if keysPressed.VK_NUMPAD0 == true then O:addForceAndTorque(entity, { force={x=-100000}} ) end
		if keysPressed.VK_NUMPAD6 == true then O:addForceAndTorque(entity, { torque={y= 100000} } ) end
		if keysPressed.VK_NUMPAD4 == true then O:addForceAndTorque(entity, { torque={y=-100000}} ) end
		if keysPressed.VK_NUMPAD8 == true then O:addForceAndTorque(entity, { torque={z= 100000} } ) end
		if keysPressed.VK_NUMPAD2 == true then O:addForceAndTorque(entity, { torque={z=-100000}} ) end
		if keysPressed.VK_NUMPAD7 == true then O:addForceAndTorque(entity, { torque={x= 100000} } ) end
		if keysPressed.VK_NUMPAD9 == true then O:addForceAndTorque(entity, { torque={x=-100000}} ) end
		if (keysPressed.VK_SPACE  == true) and (state.cooldownRemaining <= 0) then 
			state.cooldownRemaining = state.cooldown
			state.rocketIndex = state.rocketIndex+1
			O:spawn(entity, {name='rocket'..cloneInstanceId..'p'..state.rocketIndex, controlState=clone(rocketControlState), multiMesh='rocket', position={ x=15, y=-2 } }, rocketShapes ) 
		end
	end 
}

-- A lua table that can be set as the controlState object of a PhysicsEntity. It must have a 'script' key, with a function value. PhysicsEntity::animate will call it.
-- This controlState has additional data. The use of additional state data is at the script-writer's discretion.
-- Note that it is not a good idea to pass the same controlState to multiple PhysicsEntities. Use the clone function to get copies.
cruiserControlState = { 
	targetIndex = 1,
	targets = { 
		{position={x=100, y=100}, proximityRadius=10, maxForce = 30000, maxTorque = 30000},
		{position={z=100, y=100}, proximityRadius=10, maxForce = 30000, maxTorque = 30000},		 
		{position={x=-100, y=100}, proximityRadius=10, maxForce = 30000, maxTorque = 30000},		 
		{position={z=-100, y=100}, proximityRadius=10, maxForce = 30000, maxTorque = 30000} },
	script = function(entity, state)
		if O:setForceAndTorqueForTarget(entity, state.targets[state.targetIndex] ) then
			state.targetIndex = state.targetIndex + 1
			if state.targetIndex > #(state.targets) then
				state.targetIndex = 1
			end
		end
	end
}

-- A controlState with different targets. The script part is reused.
interceptorControlState = { 
	targetIndex = 1,
	targets = { 
		{mark='pod0', position={}, proximityRadius=0, maxForce = 30000, maxTorque = 30000} },
	script = cruiserControlState.script
}

-- entity definitions
O:ControlledEntity(_, {name='pod0', controlState=playerControlState, multiMesh='pod', position={ y=90 }}, geopodShapes )
O:ControlledEntity(_, {name='pod1', controlState=clone(cruiserControlState), multiMesh='envmapWindowPod', position={ z=0.05, y=70 }}, geopodShapes )
O:ControlledEntity(_, {name='pod2', controlState=clone(interceptorControlState), multiMesh='envmapWindowPod', position={ z=100.05, y=70 }}, geopodShapes )
-- an entity without Dynamics will be static.
O:ControlledEntity(_, {name='ground', multiMesh='ground'}, function(_)
	O:Shape(_, {geometryType='eBOX', material='default', halfExtents = {x=400, y=2, z=400}, position = { y=2 } } )
end )

-- camera
O:FirstPersonCam(_, {name='default', position={x=50, y=50, z=50} } )