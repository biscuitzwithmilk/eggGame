#version 330

// Input vertex attributes from Raylib (automatically passed)
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

// Custom uniform input: The color we want to subtract
uniform vec4 subtractColor;

uniform float spread;

// Output color
out vec4 finalColor;

void main()
{
    vec4 texColor = texture(texture0, fragTexCoord);
    
    vec3 rgb = abs(texColor.rgb - subtractColor.rgb);

    rgb *= spread;

    finalColor = vec4(rgb, texColor.a);
}