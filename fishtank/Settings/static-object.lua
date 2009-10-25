class 'StaticObject' (Object)

do
	local model_matrix = Eigen.Matrix4f.create({
		-1, 0, 0, 0,
		 0, 0, 1, 0,
		 0, 1, 0, 0,
		 0, 0, 0, 1
	})

	function StaticObject:__init(model_name, object_height, pos)
		local model = model_name
		if type(model) == 'string' then
			model = loadModel('Objecten/Modellen', model, model_matrix)
		end

		Object.__init(self, model, pos)
		self.scale = object_height / (model.bb_h.y - model.bb_l.y)
		self.pos = pos
	end
end
