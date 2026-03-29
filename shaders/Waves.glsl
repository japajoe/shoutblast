void main()
{
    // 1. Setup Coordinates (Flipped Y) and center Y
    vec2 uv = vec2(TexCoords.x, 1.0 - TexCoords.y);
    float centerY = uv.y - 0.5;

    // 2. Global Audio Samples
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.45 * 0.375, 0.5)).g;

    // 3. Sample Frequency at the current X position
    // We use the 0.375 active zoom
    float freq = texture(Audio, vec2(uv.x * 0.375, 0.5)).g;

    // 4. Create the "Waveguide"
    // The line height is driven by the frequency data
    // We add a 'sin' movement to make it feel like a traveling wave
    float wave = sin(uv.x * 10.0 - Time * 5.0) * (bass * 0.1);
    float linePos = freq * 0.4 + wave;

    // 5. Define the Line Thickness
    // High frequencies make the line "glow" or get "thicker/noisier"
    float thickness = 0.01 + (high * 0.05);
    float dist = abs(centerY - linePos);
    float lineMask = smoothstep(thickness, 0.0, dist);

    // 6. Electric Gradient Palette
    // Deep Blue -> Neon Cyan -> White
    vec3 colLow  = vec3(0.1, 0.0, 1.0); // Deep Blue
    vec3 colMid  = vec3(0.0, 1.0, 0.9); // Electric Cyan
    vec3 colHigh = vec3(1.0, 1.0, 1.0); // Pure White

    // Color shifts based on X position and current frequency intensity
    vec3 color = mix(colLow, colMid, uv.x);
    color = mix(color, colHigh, freq);

    // 7. Add a "Digital Shadow"
    // A faint version of the line mirrored on the bottom
    float shadowDist = abs(uv.y - 0.2 + (linePos * 0.5));
    float shadow = smoothstep(0.1, 0.0, shadowDist) * 0.2;

    // Final Composite
    // Pure black background with a glowing, frequency-reactive laser
    vec3 finalColor = (color * lineMask) + (colLow * shadow);

    FragColor = vec4(finalColor, 1.0);
}