void main()
{
    // 1. Setup Coordinates (Flipped Y) and Center [-1, 1]
    vec2 uv = (TexCoords * 2.0 - 1.0);
    uv.y *= -1.0;

    // 2. Global Audio Samples (0.375 active zoom)
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.15 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    // 3. Coordinate Rotation (Mechanical Spin)
    // The mids drive the rotation speed of the iris "blades"
    float angle = atan(uv.y, uv.x) + (mid * 3.14);
    float dist = length(uv);
    
    // 4. Blade Logic (The Geometric "Cut")
    // We create a hexagonal "opening" that scales with the bass
    float sides = 6.0;
    float sharp = cos(floor(0.5 + angle * sides / 6.28) * 6.28 / sides - angle) * dist;
    
    // The "Opening" size is driven by the kick drum
    float innerRadius = 0.2 + (bass * 0.4);
    float outerRadius = innerRadius + 0.05 + (high * 0.1);
    
    // 5. Masking
    float irisMask = step(innerRadius, sharp) * step(sharp, outerRadius);
    float centerGlow = smoothstep(innerRadius, 0.0, sharp) * bass;

    // 6. High-Contrast "Electric Lead" Palette
    // Deep Charcoal -> Electric Blue -> Pure White
    vec3 colVoid  = vec3(0.02, 0.02, 0.03);
    vec3 colEdge  = vec3(0.0, 0.7, 1.0);
    vec3 colPeak  = vec3(1.0, 1.0, 1.0);
    
    // Color shifts based on high-frequency hits
    vec3 color = mix(colEdge, colPeak, high);

    // 7. Final Composition
    // The "Blades" of the aperture are the main visual
    vec3 finalColor = mix(colVoid, color, irisMask);
    
    // Add the "Internal Pressure" glow in the center
    finalColor += colEdge * centerGlow * 0.5;

    FragColor = vec4(finalColor, 1.0);
}