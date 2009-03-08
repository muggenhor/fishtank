uniform vec3  wiggle_freq;
uniform vec3  wiggle_dir;
uniform float wiggle_phase;
uniform float turn;

void main(void)
{
	float a = 2.0 * length(wiggle_dir);
	float b = length(wiggle_freq) + 1E-20;
	float c = 1.0 / sqrt(a * a * b * b + 1.);
	float q = 0.5 * (1. - c);
	float p = 0.5 * (c + 1.);
	float s_a = 0.5 * q / b;

	float alpha = dot(vec3(gl_Vertex), wiggle_freq);

	vec4 new_wiggle_dir = vec4(wiggle_dir.x, wiggle_dir.y, wiggle_dir.z, 0.);

	vec4 wiggled_pos = gl_Vertex + new_wiggle_dir * sin(alpha + wiggle_phase);

	// Approximate the elliptic integral
	wiggled_pos.x += p * sin(2. * alpha + wiggle_phase) * s_a;

	// Approximate the elliptic integral, weirder but better looking.
	// wiggled_pos.xyz += wiggle_freq.xyz * (sin(2. * alpha + wiggle_phase) * s_a / b);
	// turns:
	if (abs(turn) > 1E-5)
	{
		float i_turn = 1. / turn;
		float turn_a = wiggled_pos.x * turn;

		if (turn > 0.)
		{
			wiggled_pos.x = sin(turn_a) * (i_turn + wiggled_pos.z);
			wiggled_pos.z = cos(turn_a) * (i_turn + wiggled_pos.z) - i_turn;
		}
		else
		{
			wiggled_pos.x = sin(-turn_a) * (-i_turn - wiggled_pos.z);
			wiggled_pos.z = -i_turn - cos(-turn_a) * (-i_turn - wiggled_pos.z);
		}
	}

	gl_Position = gl_ModelViewProjectionMatrix * wiggled_pos;
	gl_FrontColor = gl_Color;
	gl_BackColor = gl_Color;
	gl_FrontSecondaryColor = gl_SecondaryColor;
	gl_BackSecondaryColor = gl_SecondaryColor;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
