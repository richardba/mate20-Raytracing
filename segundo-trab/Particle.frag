#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 particleColor;

in vec3 varyingNormalDirection;
in vec3 varyingViewDirection;

// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	// Output color = color of the texture at the specified UV
	vec4 _Color = vec4(1, 1, 1, 0.5);
	float newOpacity = min(1.0, _Color.a / abs(dot(varyingNormalDirection, varyingViewDirection)));
	_Color = texture( myTextureSampler, UV ) * particleColor;
	_Color.a = _Color.a*newOpacity;
	if(_Color.a < 0.1)
    discard;

  float maxColor = length(_Color.rgb);

  color.r = abs(dFdx(maxColor))*_Color.r;
  color.b = abs(dFdy(maxColor))*_Color.b;
  color.g = abs(dFdy(maxColor))*_Color.g;
  color.a = _Color.a;

	color = _Color;
}
