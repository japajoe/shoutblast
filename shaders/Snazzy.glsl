void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec2 centered = uv * 2.0 - 1.0;

    // 2. Global "Mass" Samples (0.375 active zoom)
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    // 3. Grid-Based Extrusion
    // Divide the world into 12 chunky vertical pillars
    float pillars = 12.0;
    float pIndex = floor(uv.x * pillars) / pillars;
    
    // Sample FFT for the specific pillar
    float freq = texture(Audio, vec2(pIndex * 0.375, 0.5)).g;

    // 4. Fake 3D "Shadow" Logic
    // We create a height for the pillar based on the frequency
    float height = freq * 0.8 + (bass * 0.2);
    
    // Determine if we are "inside" the front face or the "side" shadow
    float distToCenter = abs(uv.y - 0.5);
    float frontFace = step(distToCenter, height * 0.5);
    
    // The "Shadow" is a slightly offset version of the face
    float shadow = step(distToCenter, height * 0.52) * (1.0 - frontFace);

    // 5. Brutalist "Industrial" Palette
    // Raw Concrete -> Electric Copper -> Cold Lead
    vec3 colConcrete = vec3(0.1, 0.1, 0.12); // Deep Background
    vec3 colCopper   = vec3(0.9, 0.4, 0.1);  // Glowing Core
    vec3 colLead     = vec3(0.4, 0.4, 0.45); // Pillar Face
    
    // 6. Dynamic Lighting
    // High-frequency hits (snares) make the pillar faces flash white
    vec3 faceColor = mix(colLead, vec3(1.0), high * freq);
    
    // 7. Final Composition
    // Layering: Front Face > Shadow > Background
    vec3 finalColor = colConcrete;
    
    // Apply the "Shadow" (darker version of the core color)
    finalColor = mix(finalColor, colCopper * 0.3, shadow);
    
    // Apply the "Front Face"
    finalColor = mix(finalColor, faceColor, frontFace);

    // 8. Add "Internal Glow"
    // Makes it look like the pillars are heated from the inside
    float glow = smoothstep(0.1, 0.0, distToCenter) * bass;
    finalColor += colCopper * glow;

    FragColor = vec4(finalColor, 1.0);
}