void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = (TexCoords * 2.0 - 1.0);
    uv.y *= -1.0; 

    // 2. Multi-Band Sampling
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.15 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    vec3 finalColor = vec3(0.0);

    // 3. The "Elastic" Loop (128 particles)
    for(float i = 0.0; i < 128.0; i++)
    {
        float t = i / 128.0;
        float freq = texture(Audio, vec2(t * 0.375, 0.5)).g;
        
        // 4. Angular Elasticity
        // Particles wobble on their orbits based on high-frequency "chatter"
        float wobble = sin(Time * 10.0 + i) * (high * 0.1);
        float rotation = Time * (0.3 + t * 0.5);
        float angle = (i * 0.5) + rotation + wobble;
        
        // 5. Centrifugal Tension
        // Bass expands the core, but 'mid' energy creates "Surface Tension"
        // This makes the rings look like they are vibrating under pressure
        float tension = sin(Time * 5.0 + i * 2.0) * (mid * 0.05);
        float radius = 0.2 + (t * 0.4) + (bass * 0.2) + tension;
        
        vec2 p = vec2(cos(angle), sin(angle)) * radius;
        
        // 6. Hyper-Sharp Falloff
        float d = length(uv - p);
        float size = 0.0003 + (freq * 0.01);
        float brightness = size / (d * d * d);

        // 7. Thermal Palette: Indigo -> Emerald -> Gold -> White
        // We use a non-linear mix to make the "hot" particles feel dangerous
        vec3 pCol = mix(vec3(0.1, 0.0, 0.4), vec3(0.0, 1.0, 0.4), t); // Indigo to Emerald
        pCol = mix(pCol, vec3(1.0, 0.8, 0.0), freq);               // Emerald to Gold
        pCol = mix(pCol, vec3(1.0, 1.0, 1.0), step(0.9, freq));    // White-hot peak
        
        // 8. Luminance Weighting
        finalColor += pCol * brightness * (freq + 0.1);
    }

    // 9. Chromatic Glitch (Branchless)
    // On massive bass hits, we slightly shift the Red/Blue channels
    float shift = step(0.85, bass) * 0.01;
    finalColor.r += texture(Audio, vec2(uv.x * 0.1, 0.5)).g * shift;
    finalColor.b += texture(Audio, vec2(uv.y * 0.1, 0.5)).g * shift;

    // 10. Final Compression
    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}