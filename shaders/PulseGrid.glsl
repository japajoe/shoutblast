void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec2 centeredUV = uv * 2.0 - 1.0;

    // 2. Sample 3 key frequency zones (0.375 active zoom)
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.12 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    // 3. Grid Deformation
    // The Bass "sucks" the grid into the center
    float zoom = 1.0 - (bass * 0.3);
    centeredUV *= zoom;

    // 4. Create a "Sharp" Tiled Grid
    // 20x20 cells
    vec2 gridUV = fract(centeredUV * 10.0);
    float gridLine = step(0.05 + (mid * 0.2), length(gridUV - 0.5));

    // 5. Reactive Mask
    // Only show the grid where the frequency for that X-position is high
    float freq = texture(Audio, vec2(abs(centeredUV.x) * 0.375, 0.5)).g;
    float mask = step(abs(centeredUV.y), freq);

    // 6. High-Saturation Electric Palette
    // Deep Charcoal -> Electric Blue -> Burning White
    vec3 colBase = vec3(0.02, 0.02, 0.05);
    vec3 colMid  = vec3(0.0, 0.5, 1.0);
    vec3 colHigh = vec3(1.0, 1.0, 1.0);

    // Color based on height and high-frequency "spikes"
    vec3 color = mix(colMid, colHigh, high);
    
    // 7. Final Composition
    // The "gridLine" creates the holes, the "mask" limits it to the spectrum
    vec3 finalColor = mix(colBase, color, (1.0 - gridLine) * mask);

    // Add a central "Core Glow" that pulses with the bass
    float glow = (0.05 / length(centeredUV)) * bass;
    finalColor += colMid * glow;

    FragColor = vec4(finalColor, 1.0);
}