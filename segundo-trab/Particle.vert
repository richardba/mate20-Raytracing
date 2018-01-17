#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; // Position of the center of the particule and size of the square

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 particleColor;
out vec3 varyingNormalDirection;
out vec3 varyingViewDirection;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)

void main()
{
	float particleSize = xyzs.w; // because we encoded it this way.
	vec3 particleCenter_wordspace = xyzs.xyz;

	vec3 vertexPosition_worldspace =
		particleCenter_wordspace
		+ CameraRight_worldspace * squareVertices.x * particleSize
		+ CameraUp_worldspace * squareVertices.y * particleSize;

	// Posição espacial da particula :D
	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

	// Coordenadas UV do vertice
	UV = squareVertices.xy + vec2(0.5, 0.5);
	particleColor = color;

	// Ajuste para correção de silhueta
	varyingNormalDirection = normalize(vec3(vec4(squareVertices, 1) * inverse(VP)));
  varyingViewDirection = normalize(CameraUp_worldspace - vec3(VP * vec4(particleCenter_wordspace, 1)));


}

