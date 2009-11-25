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

	local function loadFishModels(name, properties)
		local model = loadModel('Vissen/Modellen', name, model_matrix)
		return Fish(model, properties, aquarium.ground.maxHeight)
	end

	-- Load fishes
	fishes = {
		loadFishModels('vis2',             'vis2'           ),
		loadFishModels('vis2',             'vis2'           ),
		loadFishModels('BlueTang0',        'vis3'           ),
		loadFishModels('GuppyBlueGrass',   'guppieBT'       ),
		loadFishModels('RyugunoTukai0',    'neon'           ),
		loadFishModels('neonTeT',          'neon'           ),
		loadFishModels('neonTeT',          'neon'           ),
		loadFishModels('neonTeT',          'neon'           ),
		loadFishModels('Angel',            'Angel'          ),
		loadFishModels('kumanomi',         'kumanomi'       ),
		loadFishModels('kumanomi',         'kumanomi'       ),
		loadFishModels('Rantyu',           'rantyu'         ),
		loadFishModels('Rantyu',           'rantyu'         ),
		loadFishModels('Rantyu',           'rantyu'         ),
		loadFishModels('yellow_submarine', 'YellowSubmarine'),
		loadFishModels('haai',             'haai'           ),
	}
end

do
	local function v(x, z)
		x = -(aquarium.size.x / 2) + x
		z = -(aquarium.size.z / 2) + z

		local groundposx = (x + (aquarium.size.x / 2)) / aquarium.size.x * (aquarium.ground:width())
		local groundposy = (z + (aquarium.size.z / 2)) / aquarium.size.z * (aquarium.ground:depth())

		local y = aquarium.ground:HeightAt(groundposx, groundposy)

		return Eigen.Vector3f(x, y, z)
	end

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

	-- Add bubble particle gens
	for k,pos in ipairs({
			v(23,   67),
			v(580, 150),
		}) do
		aquarium:AddBubbleSpot(pos)
	end
end

cameraTransformations:push_back(Transformation(Transformation.FLIP_UP_DOWN))

objects.wall1 = Wall(aquarium, 'wall1.jpg')
objects.wall1.corners = {
	Eigen.Vector3d(-0.5, -0.5, -0.5),
	Eigen.Vector3d(-0.5,  0.5, -0.5),
	Eigen.Vector3d(-0.5,  0.5,  0.5),
	Eigen.Vector3d(-0.5, -0.5,  0.5)
}

objects.wall2 = Wall(aquarium, 'wall2.jpg')
objects.wall2.corners = {
	Eigen.Vector3d(-0.5, -0.5, -0.5),
	Eigen.Vector3d(-0.5,  0.5, -0.5),
	Eigen.Vector3d( 0.5,  0.5, -0.5),
	Eigen.Vector3d( 0.5, -0.5, -0.5)
}

objects.ceiling = Wall(aquarium, 'ceiling.jpg')
objects.ceiling.corners = {
	Eigen.Vector3d(-0.5,  0.5,  0.5),
	Eigen.Vector3d(-0.5,  0.5, -0.5),
	Eigen.Vector3d( 0.5,  0.5, -0.5),
	Eigen.Vector3d( 0.5,  0.5,  0.5)
}

for key,object in pairs(objects) do
	aquarium:addObject(object)
end

for key,fish in pairs(fishes) do
	aquarium:addFish(fish)
end
