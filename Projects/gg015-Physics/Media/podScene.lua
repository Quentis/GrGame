O:FirstPersonCam(_, {name='default', position={x=20, y=20, z=20} } )

O:IndexedMesh(_, {name='giraffe', file='giraffe.obj'})
O:Material(_, {name='spotted', technique='textured', pass='textured'}, function(_)
	O:setSrv(_, {effectVariable='kdTexture', file='giraffe.jpg'})
end )
	
O:MultiMesh(_, {name='giraffe'}, function(_)
	O:FlipMesh(_, {}, function(_)
		O:ShadedMesh(_, {mien='basic', indexedMesh='giraffe', material='spotted'})
	end )
end )

O:StaticEntity(_, {name='giraffe0', multiMesh='giraffe', position = { x=2, y=0, z=0} } )



O:MultiMeshFromFile(_, {name='pod', file='geopod.x'})

O:PhysicsMaterial(_, {name = 'default'})

for i=0,10,1 do
	O:PhysicsEntity(_, {name='pod'..i, multiMesh='pod', position={x=0, y=20, z=i*10} }, function (_)
		O:Dynamics(_, {linearVelocity={y=10}, angularVelocity={y=2}} )
		O:Shape(_, {geometryType='eCAPSULE', material='default', orientationAngle=0, orientationAxis={x=0, y=1, z=0}, halfHeight=5, radius=3, position = { y=0 } } )
		O:Shape(_, {geometryType='eBOX',     material='default', orientationAngle=0.79, orientationAxis={x=0, y=1, z=0}, halfExtents = {x=4, y=1, z=4}, position = { x=4.5, y=-1, z=0 } } )
		O:Shape(_, {geometryType='eBOX',     material='default', orientationAngle=0.79, orientationAxis={x=0, y=1, z=0}, halfExtents = {x=4, y=1, z=4}, position = { x=-5, y=-1, z=-2 } } )
		O:Shape(_, {geometryType='eBOX',     material='default', orientationAngle=0.79, orientationAxis={x=0, y=1, z=0}, halfExtents = {x=4, y=1, z=4}, position = { x=-5, y=-1, z=2 } } )
		O:Shape(_, {geometryType='eBOX',     material='default', orientationAngle=1.098, orientationAxis={x=-0.146, y=-0.353, z=-0.353}, halfExtents = {x=4, y=1, z=4}, position = { x=-7, y=2, z=0 } } )
	end )
end


O:MultiMeshFromFile(_, {name='ground', file='ground.x'})
O:PhysicsEntity(_, {name='ground', multiMesh='ground', position={x=0, y=-20, z=0} }, function(_)
	O:Shape(_, {
			geometryType='eBOX',
			material='default',
			orientationAngle=0,
			orientationAxis={x=0, y=1, z=0},
			halfExtents = {x=1000, y=1, z=1000},
			position = { y=0 } } )
end )


O:IndexedMesh(_, {name='background', file='quad.x'})
O:Material(_, {name='skybackground', technique='envmapped', pass='background'}, function(_)
	O:setSrv(_, {effectVariable='envTexture', file='cloudynoon.dds'})
end )
	
O:MultiMesh(_, {name='skybackground'}, function(_)
	O:FlipMesh(_, {}, function(_)
		O:ShadedMesh(_, {mien='basic', indexedMesh='background', material='skybackground'})
	end )
end )

O:StaticEntity(_, {name='sky', multiMesh='skybackground'} )




O:Material(_, {name='window', technique='envmapped', pass='envmapped'}, function(_)
	O:setSrv(_, {effectVariable='kdTexture', file='giraffe.jpg'})
	O:setSrv(_, {effectVariable='envTexture', file='cloudynoon.dds'})
end )
	
O:MultiMesh(_, {name='multipod'}, function(_)
	O:FlipMesh(_, {}, function(_)
		O:ShadedMesh(_, {mien='basic', indexedMesh='pod[0]', material='spotted'})
	end )
	O:FlipMesh(_, {}, function(_)
		O:ShadedMesh(_, {mien='basic', indexedMesh='pod[1]', material='window'})
	end )
end )

O:StaticEntity(_, {name='multipod0', multiMesh='multipod', position = { x=20, y=20, z=20} } )