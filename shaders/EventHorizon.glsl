void main()
{
    // 1. Setup Coordinates (Flipped Y) and Center
    vec2 uv = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec2 centered = uv - 0.5;

    // 2. Global Audio Forces (0.375 active zoom)
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.12 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.42 * 0.375, 0.5)).g;

    // 3. Gravitational Warping
    // The "Singularity" size is driven by the bass
    float dist = length(centered);
    float singularity = 0.05 + (bass * 0.2);
    
    // Distort the UVs: Light bends more as it gets closer to the center
    // No noise, just a 1/d relationship for the lensing effect
    float lens = singularity / (dist + 0.01);
    vec2 warpedUv = centered * (1.0 + lens * 2.0);

    // 4. Mathematical Starfield (No Noise/Fract)
    // We use a high-frequency sine grid to create "stars"
    // The "twinkle" is driven by the high frequencies
    float stars = sin(warpedUv.x * 100.0) * sin(warpedUv.y * 100.0);
    stars = pow(stars, 20.0); // Sharpen the points into tiny dots
    stars *= (0.5 + high * 2.0); // Make them pop on high-end hits

    // 5. The Accretion Disk (FFT Spectrum)
    // A thin, glowing ring that samples the FFT based on the angle
    float angle = atan(centered.y, centered.x);
    float freq = texture(Audio, vec2(abs(sin(angle)) * 0.375, 0.5)).g;
    
    // The ring sits just outside the singularity
    float ringWidth = 0.005 + (mid * 0.02);
    float ringRadius = singularity + 0.05 + (freq * 0.1);
    float ring = smoothstep(ringWidth, 0.0, abs(dist - ringRadius));

    // 6. Deep Space Palette
    vec3 colVoid = vec3(0.0, 0.0, 0.01);
    vec3 colIndigo = vec3(0.1, 0.2, 0.5);
    vec3 colWhite = vec3(1.0, 1.0, 1.0);

    // 7. Final Composition
    // Background is the starfield warped by the lens
    vec3 finalColor = colWhite * stars * step(singularity, dist);
    
    // Add the Accretion Disk (The Audio Visualizer)
    finalColor += mix(colIndigo, colWhite, high) * ring;
    
    // The Singularity (Pure Black Hole)
    finalColor *= step(singularity, dist);
    
    // Add a subtle "Einstein Ring" glow on the very edge
    float glow = smoothstep(0.1, 0.0, abs(dist - singularity)) * mid;
    finalColor += colIndigo * glow;

    FragColor = vec4(finalColor, 1.0);
}