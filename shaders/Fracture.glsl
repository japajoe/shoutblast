void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec2 centered = uv - 0.5;

    // 2. Sample 3 High-Energy Zones (0.375 active zoom)
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.12 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.45 * 0.375, 0.5)).g;

    // 3. The "Stress" Field
    // We create a jagged radial distance field that warps with the audio
    float angle = atan(centered.y, centered.x);
    float dist = length(centered);
    
    // Sample FFT based on the angle to create a "starburst" of cracks
    float freqAtAngle = texture(Audio, vec2(abs(sin(angle)) * 0.375, 0.5)).g;
    
    // 4. Crack Geometry
    // We use a high-frequency sine wave multiplied by the FFT to create "veins"
    float veins = sin(dist * 50.0 - Time * 2.0 + freqAtAngle * 10.0);
    float crackPattern = smoothstep(0.95 - (bass * 0.2), 1.0, veins);

    // 5. The "Void" Mask
    // A central circular "dead zone" that expands with the kick drum
    float coreSize = 0.1 + (bass * 0.4);
    float coreMask = step(coreSize, dist);

    // 6. Brutalist Monochrome Palette
    // Matte Black -> Zinc Grey -> Incandescent White
    vec3 colVoid  = vec3(0.01, 0.01, 0.015);
    vec3 colZinc  = vec3(0.3, 0.3, 0.35);
    vec3 colWhite = vec3(1.0, 1.0, 1.0);

    // 7. Light Dispersion
    // Everything glows white when a high peak (snare/clap) occurs
    vec3 accentColor = mix(colZinc, colWhite, high);
    
    // 8. Final Composition
    // The cracks only exist outside the "Core"
    vec3 finalColor = colVoid;
    
    // Add the "Fracture" lines
    finalColor += accentColor * crackPattern * coreMask;
    
    // Add a sharp "Rim" around the core that pulses with the bass
    float rim = smoothstep(0.02, 0.0, abs(dist - coreSize));
    finalColor += colWhite * rim * (0.5 + mid);

    // Add a subtle "Dust" or grain that moves with the high frequencies
    float grain = fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
    finalColor += colZinc * grain * 0.05 * high;

    FragColor = vec4(finalColor, 1.0);
}