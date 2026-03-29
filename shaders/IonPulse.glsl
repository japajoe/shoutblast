void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = (TexCoords * 2.0 - 1.0);
    uv.y *= -1.0; 

    // 2. Global Bass for pulsing the overall scale
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;

    // 3. Clear Background
    vec3 finalColor = vec3(0.0);

    // 4. Particle Loop (Purely points of light)
    for(float i = 0.0; i < 32.0; i++)
    {
        float t = i / 32.0;
        
        // Sample frequency for this specific particle
        float freq = texture(Audio, vec2(t * 0.375, 0.5)).g;
        
        // Position particles in a circular burst that pulses with bass
        float angle = i * 0.8 + Time * 0.1;
        float radius = 0.2 + (i * 0.02) + (bass * 0.15);
        vec2 p = vec2(cos(angle), sin(angle)) * radius;
        
        // Calculate the glow of the particle
        float d = length(uv - p);
        
        // High-contrast brightness: sharp falloff, no long "lines"
        float size = 0.001 + (freq * 0.04);
        float brightness = size / (d * d); // Square falloff for sharper dots

        // 5. Better Colors: Electric Spectrum
        // Deep Violet -> Bright Cyan -> Pure White
        vec3 pCol = mix(vec3(0.4, 0.0, 1.0), vec3(0.0, 0.8, 1.0), t);
        pCol = mix(pCol, vec3(1.0, 1.0, 1.0), freq * 0.5);
        
        finalColor += pCol * brightness * freq;
    }

    // 6. Final clamp to prevent over-exposure
    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}