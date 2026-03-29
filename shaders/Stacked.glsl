void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = vec2(TexCoords.x, 1.0 - TexCoords.y);
    
    // 2. Global "Impact" Samples (0.375 active zoom)
    float lowEnd = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float highEnd = texture(Audio, vec2(0.45 * 0.375, 0.5)).g;

    // 3. Raster Strip Logic
    // Divide the screen into 64 horizontal "strips"
    float strips = 64.0;
    float stripIndex = floor(uv.y * strips) / strips;
    
    // 4. Per-Strip Displacement
    // Each strip's X-offset is driven by the frequency at its Y-height
    float freq = texture(Audio, vec2(stripIndex * 0.375, 0.5)).g;
    
    // The "Friction" offset: Strips slide based on frequency intensity
    float xOffset = freq * 0.4 * sign(sin(stripIndex * 100.0)); 
    float displacedX = uv.x + xOffset;

    // 5. Shape Logic: The "Center Channel"
    // Instead of bars, we create a vertical "column" that gets torn apart
    float columnWidth = 0.2 + (lowEnd * 0.4);
    float mask = step(abs(displacedX - 0.5), columnWidth * 0.5);

    // 6. Brutalist Palette: High-Contrast Primary Tones
    // Deep Slate -> Blood Orange -> Cold White
    vec3 colBase = vec3(0.05, 0.05, 0.06);
    vec3 colAlt  = vec3(0.9, 0.2, 0.1);
    vec3 colPeak = vec3(1.0, 1.0, 1.0);
    
    // Mix color based on the frequency intensity of the specific strip
    vec3 color = mix(colBase, colAlt, freq);
    color = mix(color, colPeak, step(0.8, freq));

    // 7. Edge Sharpening
    // Adds a 1-pixel bright edge to the strips that are moving the most
    float edge = step(0.98, fract(displacedX * 10.0)) * highEnd;
    
    vec3 finalColor = (mask * color) + (edge * colPeak * mask);

    FragColor = vec4(finalColor, 1.0);
}