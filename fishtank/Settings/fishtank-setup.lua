aquarium.eye_distance = 300
aquarium.size = Eigen.Vector3d(660,
                               185, -- 6250
                               330)
faceRange.x = .5
faceRange.y = .5
swimArea.x = 620
swimArea.y = 180
swimArea.z = 290
win_pos.x = 1675
win_pos.y = -25
win_size.x = 1920
win_size.y = 1080

win_size.x = 1280
win_size.y = 720
win_pos.x = 0
win_pos.y = 0

aquarium.ground.maxHeight = 30

dofile 'static-object.lua'
dofile 'wall.lua'

do
	local model_matrix = Eigen.Matrix4f.create({
		-1, 0, 0, 0,
		 0, 0, 1, 0,
		 0, 1, 0, 0,
		 0, 0, 0, 1
	})

	function loadFishModels(name, properties, count)
		local count = count or 1
		local model = loadModel('Vissen/Modellen', name, model_matrix)
		for i=1,count do
			aquarium:AddFish(model, properties)
		end
	end
end

-- Load fishes
for k,fish in ipairs({
		{'vis2',             'vis2'              },
		{'vis2',             'vis2'              },
		{'BlueTang0',        'vis3'              },
		{'GuppyBlueGrass',   'guppieBT'          },
		{'RyugunoTukai0',    'neon'              },
		{'neonTeT',          'neon',            3},
		{'Angel',            'Angel'             },
		{'kumanomi',         'kumanomi',        2},
		{'Rantyu',           'rantyu',          3},
		{'yellow_submarine', 'YellowSubmarine'   },
		{'haai',             'haai'              },
	}) do
	loadFishModels(unpack(fish))
end

function v(x, z)
        x = -(aquarium.size.x / 2) + x
        z = -(aquarium.size.z / 2) + z

	groundposx = (x + (aquarium.size.x / 2)) / aquarium.size.x * (aquarium.ground:width())
	groundposy = (z + (aquarium.size.z / 2)) / aquarium.size.z * (aquarium.ground:depth())

        y = aquarium.ground:HeightAt(groundposx, groundposy)

        return Eigen.Vector3f(x, y, z)
end

do
	local function zeester(x, z)
		return StaticObject('zeester', 3, v(x, z))
	end

	local function zw_gras(x, z)
		return StaticObject('zw_gras', 40, v(x, z))
	end

	local function zw_stroken(x, z)
		return StaticObject('zw_stroken', 140, v(x, z))
	end

	objects = {
		StaticObject('kumanomi_dood',   40,  v(550, 150)),
		StaticObject('schatkist_draai', 30,  v(100, 100)),
		zeester(                                20, 300 ),
		zeester(                               200, 200 ),
		zeester(                               375, 156 ),
		zeester(                               532, 297 ),
		StaticObject('zw_boom',         60,  v(557, 125)),
		zw_gras(                                33, 125 ),
		zw_gras(                               125, 256 ),
		zw_gras(                               352,  75 ),
		zw_stroken(                              0,   0 ),
		zw_stroken(                             56,   0 ),
		zw_stroken(                            630,   0 )
	}
	for key,object in pairs(objects) do
		aquarium:addObject(object)
	end
end

-- Add bubble particle gens
for k,pos in ipairs({
		v(23,   67),
		v(580, 150),
	}) do
	aquarium:AddBubbleSpot(pos)
end

cameraTransformations:push_back(Transformation(Transformation.FLIP_UP_DOWN))

wall1 = Wall(aquarium, 'wall1.jpg')
wall1.corners = {
	Eigen.Vector3d(-0.5, -0.5, -0.5),
	Eigen.Vector3d(-0.5,  0.5, -0.5),
	Eigen.Vector3d(-0.5,  0.5,  0.5),
	Eigen.Vector3d(-0.5, -0.5,  0.5)
}
aquarium:addObject(wall1)

wall2 = Wall(aquarium, 'wall2.jpg')
wall2.corners = {
	Eigen.Vector3d(-0.5, -0.5, -0.5),
	Eigen.Vector3d(-0.5,  0.5, -0.5),
	Eigen.Vector3d( 0.5,  0.5, -0.5),
	Eigen.Vector3d( 0.5, -0.5, -0.5)
}
aquarium:addObject(wall2)

ceiling = Wall(aquarium, 'ceiling.jpg')
ceiling.corners = {
	Eigen.Vector3d(-0.5,  0.5,  0.5),
	Eigen.Vector3d(-0.5,  0.5, -0.5),
	Eigen.Vector3d( 0.5,  0.5, -0.5),
	Eigen.Vector3d( 0.5,  0.5,  0.5)
}
aquarium:addObject(ceiling)
