class 'Wall' (Object)
Wall.texCoords = {
	Eigen.Vector2i(1, 0),
	Eigen.Vector2i(1, 1),
	Eigen.Vector2i(0, 1),
	Eigen.Vector2i(0, 0)
}

function Wall:__init(aquarium, texture)
	Object.__init(self)
	self.aquarium = aquarium
	self.texture = loadTexture('', texture)
end

function Wall:draw()
	gl.color(1, 1, 1)
	self.texture:bind()

	gl.matrix.push()
	gl.matrix.scale(self.aquarium.size)

	gl.Begin(gl.QUADS)
		for index = 1,4 do
			gl.texCoord(self.texCoords[index])
			gl.vertex(self.corners[index])
		end
	gl.End()

	gl.matrix.pop()
end
