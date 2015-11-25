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

for i=0,10,1 do
	O:StaticEntity(_, {name='pod'..i, multiMesh='pod', position={x=0, y=0, z=i*10} } )
end





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