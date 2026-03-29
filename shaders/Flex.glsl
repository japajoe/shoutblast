void main()
{
    // 1. Setup Coordinates (Flipped Y)
    vec2 uv = vec2(TexCoords.x, 1.0 - TexCoords.y);
    
    // 2. Sample 3 key frequency zones (0.375 active zoom)
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.15 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    // 3. Ribbon Distortion
    // We displace the X-coordinate based on the Y-position and the Bass
    float wave = sin(uv.y * 5.0 + Time * 2.0) * (bass * 0.2);
    float distortedX = uv.x + wave;

    // 4. Slice Logic
    // Divide the screen into 32 vertical "Slices"
    float numSlices = 32.0;
    float sliceIndex = floor(distortedX * numSlices) / numSlices;
    
    // Sample the FFT for each specific slice
    float freq = texture(Audio, vec2(sliceIndex * 0.375, 0.5)).g;

    // 5. Define the "Prism" Shape
    // Each slice is a vertical bar, but we use 'abs' to center them
    float barWidth = 0.8; // 80% of the slice width
    float horizontalMask = step(abs(fract(distortedX * numSlices) - 0.5), barWidth * 0.5);
    
    // Vertical Mask: Bars grow from the middle out
    float verticalMask = step(abs(uv.y - 0.5), freq * 0.5);

    // 6. High-Saturation Electric Palette
    // Indigo -> Magenta -> Hot Pink
    vec3 colA = vec3(0.2, 0.0, 1.0); // Indigo
    vec3 colB = vec3(1.0, 0.0, 0.6); // Magenta
    vec3 colC = vec3(1.0, 0.4, 0.9); // Hot Pink
    
    // Mix colors based on both height and frequency intensity
    vec3 color = mix(colA, colB, uv.y);
    color = mix(color, colC, freq);

    // 7. Final Output
    // Brighten the "active" parts of the ribbon based on high-frequency hits
    float brightness = 0.8 + (high * 0.5);
    vec3 finalColor = color * horizontalMask * verticalMask * brightness;

    // Clean black background (No grids or weird lines)
    FragColor = vec4(finalColor, 1.0);
}