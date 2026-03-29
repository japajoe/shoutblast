// 1. Full-Spectrum Palettizer (0.0 to 1.0)
vec3 GetFullSpectrum(float t)
{
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.0, 0.33, 0.67);
    return 0.5 + 0.5 * cos(6.28318 * (c * t + d));
}

// 2. High-Performance Pseudo-Random Hash
float Hash(vec2 p) 
{
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main()
{
    // 3. Setup Coordinates (Centered)
    vec2 uv = (TexCoords * 2.0 - 1.0);
    uv.y *= -1.0; 

    // 4. Multi-Band Global Sampling
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.15 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    vec3 finalColor = vec3(0.0);

    // 5. Kinetic Filament Loop (48 High-Density Strands)
    // Instead of dots, we draw 1-pixel wide paths
    for(float i = 0.0; i < 48.0; i++)
    {
        float t = i / 48.0;
        float freq = texture(Audio, vec2(t * 0.375, 0.5)).g;
        
        // 6. Vector Field Action
        // Every strand has a unique starting orbit and velocity
        float angle = t * 6.28318 + (Time * 0.2);
        vec2 dir = vec2(cos(angle), sin(angle));
        
        // The "Flow": Bass expands the field, Mid twists it
        float flowDist = 0.2 + (bass * 0.4) + (sin(Time + i) * mid * 0.1);
        vec2 p = dir * flowDist;
        
        // 7. Domain Warping
        // Add a secondary high-frequency "jitter" to the strand position
        p += vec2(Hash(vec2(i, 0.0)), Hash(vec2(0.0, i))) * (high * 0.05);

        // 8. 1-Pixel Needle Logic
        float d = length(uv - p);
        
        // Cubic falloff for maximum sharpness (No blur)
        float size = 0.0001 + (freq * 0.02);
        float brightness = size / (d * d * d);

        // 9. Full-Spectrum Color Variation
        // Each strand gets a unique color that cycles with Time and Pitch
        float hue = t + (Time * 0.1) + (freq * 0.5);
        vec3 col = GetFullSpectrum(hue);
        
        // 10. Incandescence
        // High frequencies cause the strand to turn pure white at the core
        col = mix(col, vec3(1.0), step(0.8, freq));
        
        finalColor += col * brightness * (freq + 0.1);
    }

    // 11. Screen-Space Glitch Detail
    // On massive bass hits, invert the color of a random 1-pixel scanline
    float scanline = step(0.98, fract(uv.y * 100.0 + Time * 10.0)) * step(0.9, bass);
    finalColor = mix(finalColor, 1.0 - finalColor, scanline * 0.5);

    // 12. Final Composition
    finalColor *= smoothstep(2.0, 0.2, length(uv));
    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}