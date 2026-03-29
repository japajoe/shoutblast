void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = vec2(TexCoords.x, 1.0 - TexCoords.y);
    
    // 2. Sample 3 key frequency zones (0.375 active zoom)
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.15 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    // 3. The "Fault Line" logic
    // We sample the frequency at the current X to determine the "height" of the tear
    float freq = texture(Audio, vec2(uv.x * 0.375, 0.5)).g;
    
    // Create a jagged horizontal split in the middle of the screen
    // The bass pushes the whole line up/down, the freq creates the "teeth"
    float faultLine = 0.5 + (freq * 0.4) + (sin(Time + uv.x * 3.14) * bass * 0.1);
    
    // 4. Split the screen into two halves (Top and Bottom)
    float mask = step(faultLine, uv.y);
    
    // 5. High-Contrast "Magma" Palette
    // Deep Charcoal (Bottom) vs Burning Crimson/Gold (Top)
    vec3 colDeep   = vec3(0.04, 0.04, 0.06); // Dark Slate
    vec3 colMagma  = vec3(0.9, 0.1, 0.05);  // Blood Red
    vec3 colGold   = vec3(1.0, 0.8, 0.1);   // Bright Gold
    
    // Top plate color reacts to the mids and highs
    vec3 topColor = mix(colMagma, colGold, mid);
    topColor += colGold * high * 0.5; // Flash white/gold on high peaks

    // 6. Edge Glow (The "Friction" Line)
    // Create a bright highlight exactly where the two plates meet
    float edge = smoothstep(0.02, 0.0, abs(uv.y - faultLine));
    
    // 7. Final Composition
    vec3 finalColor = mix(colDeep, topColor, mask);
    finalColor += colGold * edge * (0.5 + bass); // The fault line glows with the bass

    FragColor = vec4(finalColor, 1.0);
}