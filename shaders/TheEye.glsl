void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = (TexCoords * 2.0 - 1.0);
    uv.y *= -1.0; 

    // 2. High-Fidelity Sampling
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.15 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    vec3 finalColor = vec3(0.0);

    // 3. The "Shell" Loop (128 high-density points)
    for(float i = 0.0; i < 128.0; i++)
    {
        float t = i / 128.0;
        float freq = texture(Audio, vec2(t * 0.375, 0.5)).g;
        
        // 4. Multi-Shell Rotation
        // Divide particles into 4 rings with opposing directions
        float shellId = floor(i / 32.0);
        float direction = (mod(shellId, 2.0) == 0.0) ? 1.0 : -1.0;
        
        // Rotation speed influenced by frequency and mid-range "heat"
        float rotation = Time * (0.5 + shellId * 0.2) * direction;
        float whip = sin(Time * 2.0 + mid * 4.0) * 0.5;
        float angle = (i * 0.2) + rotation + whip;
        
        // 5. Orbital Displacement
        // Bass pushes the shells outward; Freq adds local "jitter"
        float baseRadius = 0.1 + (shellId * 0.15);
        float radius = baseRadius + (bass * 0.25) + (freq * 0.1);
        
        vec2 p = vec2(cos(angle), sin(angle)) * radius;
        
        // 6. Sharp-Edge Luminance
        float d = length(uv - p);
        
        // Extremely tight falloff for 1-pixel "needle" points
        float size = 0.0004 + (freq * 0.015);
        float brightness = size / (d * d * d); // Cubic falloff for maximum sharpness

        // 7. Electric Spectrum (Deep Violet -> Cyan -> White)
        // Shift hue based on the shell ID for structured color depth
        vec3 pCol = mix(vec3(0.5, 0.0, 1.0), vec3(0.0, 0.9, 1.0), t);
        
        // Add "Incandescence" on high-frequency hits
        pCol = mix(pCol, vec3(1.0, 1.0, 1.0), step(0.8, freq));
        
        // 8. Kinetic Accumulation
        // Particles only contribute if they are active
        finalColor += pCol * brightness * (freq + 0.05);
    }

    // 9. Centripetal Glow
    // A very faint, high-frequency "haze" in the core to tie it together
    float coreGlow = smoothstep(0.5, 0.0, length(uv)) * high * 0.1;
    finalColor += vec3(0.0, 0.5, 1.0) * coreGlow;

    // 10. Frame Clipping
    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}