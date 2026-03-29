void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = vec2(TexCoords.x, 1.0 - TexCoords.y);
    float centerY = uv.y - 0.5;

    // 2. Sample 3 Core Frequency Zones (0.375 active zoom)
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.15 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    // 3. The "Seismic" Trace
    // Sample the FFT at the current X to get the line's height
    float freq = texture(Audio, vec2(uv.x * 0.375, 0.5)).g;
    
    // We mix the raw FFT with a bit of "noise" from the high-end
    float linePos = (freq * 0.6 - 0.3) + (sin(uv.x * 50.0 + Time * 10.0) * high * 0.05);

    // 4. Split-Plane Logic
    // Everything above the line is one color, everything below is another
    float split = step(linePos, centerY);

    // 5. Brutalist "Hazard" Palette
    // Deep Slate (Background) -> Safety Orange (Mid) -> Pure White (Peak)
    vec3 colDark   = vec3(0.04, 0.04, 0.05); // Deep Charcoal
    vec3 colHazard = vec3(1.0, 0.4, 0.0);   // Safety Orange
    vec3 colPeak   = vec3(1.0, 1.0, 1.0);   // White

    // Top color reacts to mids, bottom stays dark
    vec3 topColor = mix(colHazard, colPeak, high);
    
    // 6. The "Needle" Edge
    // A bright, 1-pixel line exactly at the split point
    float edge = smoothstep(0.01, 0.0, abs(centerY - linePos));

    // 7. Final Composition
    vec3 finalColor = mix(colDark, topColor, split);
    
    // Add the glowing trace line on top of the split
    finalColor += colPeak * edge * (0.8 + bass * 0.5);

    FragColor = vec4(finalColor, 1.0);
}