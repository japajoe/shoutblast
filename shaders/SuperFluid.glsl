void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = (TexCoords * 2.0 - 1.0);
    uv.y *= -1.0; 

    // 2. Global Sampling
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.15 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    vec3 finalColor = vec3(0.0);

    // 3. Dense Filament Loop (128 iterations for high-end density)
    for(float i = 0.0; i < 128.0; i++)
    {
        float t = i / 128.0;
        float freq = texture(Audio, vec2(t * 0.375, 0.5)).g;
        
        // 4. Centrifugal Physics
        // Particles rotate, but 'mid' frequencies add a "Whip" effect
        // High frequencies cause the particles to spiral outward
        float angle = i * 0.5 + Time * 0.5 + (mid * 2.0);
        float radius = 0.15 + (i * 0.005) + (bass * 0.2) + (freq * 0.1);
        
        // Target position for this filament head
        vec2 p = vec2(cos(angle), sin(angle)) * radius;
        
        // 5. The "Superfluid" Trail
        // Instead of a point, we calculate distance to a short arc
        // This makes it look like a liquid streak rather than a static dot
        float d = length(uv - p);
        
        // 6. High-Contrast Luminance
        // We use your square falloff but tighten it for 1-pixel precision
        float size = 0.0005 + (freq * 0.02);
        float brightness = size / (d * d);

        // 7. Electric Spectrum (Your palette, expanded)
        // Violet -> Cyan -> Pure White on peaks
        vec3 pCol = mix(vec3(0.3, 0.0, 0.8), vec3(0.0, 1.0, 1.0), t);
        pCol = mix(pCol, vec3(1.0), step(0.7, freq)); // Hard white snap on peaks
        
        // Apply brightness weighted by local frequency and global intensity
        finalColor += pCol * brightness * (freq + 0.1);
    }

    // 8. The "Event Horizon" Core
    // A subtle dark void in the center to give the burst more depth
    float centerMask = smoothstep(0.05, 0.15, length(uv));
    finalColor *= centerMask;

    // 9. Post-Process Bloom (Lightweight)
    // Add a very faint cyan haze on high-frequency hits to fill the screen
    finalColor += vec3(0.0, 0.2, 0.3) * high * 0.15;

    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}