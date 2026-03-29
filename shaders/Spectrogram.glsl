vec3 get_heatmap(float intensity)
{
    vec3 black = vec3(0.0, 0.0, 0.0);
    vec3 blue = vec3(0.05, 0.05, 0.4);
    vec3 purple = vec3(0.5, 0.0, 0.5);
    vec3 red = vec3(0.9, 0.1, 0.1);
    vec3 orange = vec3(1.0, 0.5, 0.0);
    vec3 yellow = vec3(1.0, 1.0, 0.7);
    
    vec3 color = mix(black, blue, smoothstep(0.0, 0.1, intensity));
    color = mix(color, purple, smoothstep(0.1, 0.3, intensity));
    color = mix(color, red, smoothstep(0.3, 0.6, intensity));
    color = mix(color, orange, smoothstep(0.6, 0.8, intensity));
    color = mix(color, yellow, smoothstep(0.8, 1.0, intensity));
    
    return color;
}

void main()
{
    // Use the exact width of one pixel to prevent sub-pixel sampling drift
    float speed = (1.0 / textureSize(Texture, 0).x) * 2.0;
    
    if (TexCoords.x > 1.0 - speed)
    {
        // Maps the vertical screen space (0.0 to 1.0) to 
        // the first half of the audio texture (0.0 to 0.5).
        float yCoord = (1.0 - TexCoords.y) * 0.5;
        
        float fft = texture(Audio, vec2(yCoord, 0.0)).g;
        float fftUp = texture(Audio, vec2(yCoord + 0.005, 0.0)).g;
        float fftDown = texture(Audio, vec2(yCoord - 0.005, 0.0)).g;
        
        float visualIntensity = (fft * 0.6) + (fftUp * 0.2) + (fftDown * 0.2);
        visualIntensity = pow(visualIntensity, 0.8);
        
        vec3 finalColor = get_heatmap(visualIntensity);
        
        FragColor = vec4(finalColor, 1.0);
    }
    else
    {
        // Direct 1:1 shift to the left
        vec2 uv = vec2(TexCoords.x + speed, TexCoords.y);
        
        FragColor = texture(Texture, uv);
    }
}