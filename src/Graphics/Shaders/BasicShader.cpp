#include "BasicShader.hpp"

namespace ShoutBlast
{
    static std::string gVertexSource = R"(#version 330 core
out vec2 TexCoords;

void main() {
    // Generates a triangle that covers the [-1, 1] range
    // Vertex 0: (-1, -1), UV (0, 0)
    // Vertex 1: ( 3, -1), UV (2, 0)
    // Vertex 2: (-1,  3), UV (0, 2)
    
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    
    TexCoords.x = (x + 1.0) * 0.5;
    TexCoords.y = (y + 1.0) * 0.5;
    
    gl_Position = vec4(x, y, 0.0, 1.0);
})";
    static std::string gFragmentSource = R"(#version 330 core

#define MODE_TIME_DOMAIN 0
#define MODE_FREQUENCY_DOMAIN 1

uniform float Time;
uniform sampler2D Audio;
uniform float SampleRate;
uniform int FrameCount;
uniform vec2 Resolution;
uniform int Mode;

in vec2 TexCoords;
out vec4 FragColor;

vec3 get_gradient(vec2 uv, float frequency)
{
    // Define 3-stop colors
    vec3 colorLow  = vec3(0.0, 0.5, 1.0); // Cyan/Blue
    vec3 colorMid  = vec3(0.9, 0.1, 0.8); // Hot Pink
    vec3 colorHigh = vec3(1.0, 0.9, 0.5); // Golden White for peaks

    // Calculate relative height (0.0 to 1.0) 
    // Max check to prevent division by zero
    float h = clamp(uv.y / max(frequency, 0.001), 0.0, 1.0);

    // Two-step mix for a 3-color transition
    vec3 barColor = mix(
        mix(colorLow, colorMid, h), 
        colorHigh, 
        pow(h, 3.0) // Only show the 'White' at the very top peaks
    );

    return barColor;
}

void render_frequency_domain()
{
    vec2 uv = TexCoords;
    uv.y = 1.0 - uv.y;

    // 1. Setup background
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);

    // 2. Bar Configuration
    float numBars = 64.0;
    float slotWidth = Resolution.x / numBars;
    
    // Replace hardcoded gapPixels = 4.0 with a ratio
    // 0.2 means 20% of the slot is gap, 80% is bar.
    float gapRatio = 0.2;
    float gapPixels = slotWidth * gapRatio;

    // 3. Calculate Bar Position
    float pixelX = uv.x * Resolution.x;
    float barIndex = floor(pixelX / slotWidth);
    float slotX = mod(pixelX, slotWidth);

    // Create the gap between bars
    if (slotX >= slotWidth - gapPixels)
    {
        return;
    }

    // 4. Audio Texture Mapping
    float audioStart = 0.0;
    float audioEnd = 0.5 * 0.75; 
    float audioX = audioStart + (barIndex / (numBars - 1.0)) * (audioEnd - audioStart);
    
    // Sampling the Green channel (.g) for frequency data
    float frequency = texture(Audio, vec2(audioX, 0.5)).g;

    // 5. Render the Bar
    if (uv.y < frequency)
    {
        vec3 barColor = get_gradient(uv, frequency);
        
        FragColor = vec4(barColor, 1.0);
    }
}

void render_time_domain() {
    // 1. Zoom and Center
    // We remap TexCoords.y from [0, 1] to [-1, 1] to match audio amplitude
    float uv_y = TexCoords.y * 2.0 - 1.0;
    
    // 2. Sample the Audio
    // We add a tiny bit of horizontal scroll based on time for a "live" look
    float wave = texture(Audio, vec2(TexCoords.x, 0.5)).r;

    // 3. The "Distance Field"
    // Calculate how far this pixel is from the actual wave value
    float dist = abs(uv_y - wave);

    // 4. Create the Glow
    // A thin, sharp core plus a wide, soft bloom
    float core = 0.005 / dist;
    float glow = 0.02 / pow(dist, 0.8);
    
    // 5. Coloring
    // Cyberpunk Cyan/Green mix
    vec3 color = vec3(0.1, 0.8, 0.4);
    
    // 6. Add "Scanlines" and Fuzz
    // This gives it that old CRT / medical monitor vibe
    float scanline = sin(TexCoords.y * 200.0 + Time * 5.0) * 0.1 + 0.9;
    float noise = fract(sin(dot(TexCoords.xy ,vec2(12.9898,78.233))) * 43758.5453);
    
    vec3 finalColor = color * (core + glow);
    finalColor *= scanline;
    finalColor += noise * 0.02; // Subtle digital grain

    // 7. Vignette (Darken the edges)
    float vignette = 1.0 - length(TexCoords - 0.5) * 1.2;
    
    FragColor = vec4(finalColor * vignette, 1.0);
}
    
void main() {
    if(Mode == MODE_FREQUENCY_DOMAIN)
        render_frequency_domain();
    else
        render_time_domain();
})";

    std::string BasicShader::GetVertexSource()
    {
        return gVertexSource;
    }

    std::string BasicShader::GetFragmentSource()
    {
        return gFragmentSource;
    }
}