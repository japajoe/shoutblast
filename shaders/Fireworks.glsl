// 1. High-Performance Hash for spark distribution
vec2 hash22(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * vec3(443.897, 441.423, 437.195));
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.xx + p3.yz) * p3.zy);
}

void main()
{
    // 1. Setup Coordinates (Centered)
    vec2 uv = (TexCoords * 2.0 - 1.0);
    uv.y *= -1.0; 

    // 2. Multi-Band Sampling
    float bass = texture(Audio, vec2(0.01 * 0.375, 0.5)).g;
    float mid  = texture(Audio, vec2(0.15 * 0.375, 0.5)).g;
    float high = texture(Audio, vec2(0.40 * 0.375, 0.5)).g;

    vec3 finalColor = vec3(0.0);

    // 3. Explosion Configuration
    for (float i = 0.0; i < 8.0; i++)
    {
        // 4. Kinetic Launch Logic
        vec2 id = hash22(vec2(i, 123.45));
        float launchTime = Time * (0.5 + id.x) + id.y;
        
        // Modified: Slowed down from 0.5 to 0.3 to compensate for the longer distance
        float cycle = fract(launchTime * 0.3); 
        
        // Origin starts at -1.0 (bottom) and rises toward 1.0 (top)
        vec2 origin = vec2(id.x * 1.6 - 0.8, -1.0 + cycle * 2.0 + (bass * 0.2));
        
        // 5. Burst Physics
        vec2 p = uv - origin;
        float dist = length(p);
        float angle = atan(p.y, p.x);
        
        // 6. 1-Pixel Spark Distribution
        float rays = 32.0;
        float rayAngle = 6.2831 / rays;
        float rayId = floor(angle / rayAngle);
        float rayNoise = hash22(vec2(rayId, i)).x;
        
        float burstSpeed = 0.5 + (mid * 0.5);
        float sparkPos = cycle * burstSpeed * (0.8 + rayNoise * 0.4);
        
        // 7. Spark Geometry (Blobs)
        // Find the center point of the blob on this specific ray
        float centerAngle = (rayId + 0.5) * rayAngle;
        vec2 blobCenter = vec2(cos(centerAngle), sin(centerAngle)) * sparkPos;
        float blobDist = length(p - blobCenter);
        
        // 8. Kinetic Decay & Scintillation
        float life = 1.0 - cycle; 
        float thickness = 0.0005 + (high * 0.005 * life);
        
        // Use blobDist instead of sparkDist to create circular glows
        float brightness = thickness / (blobDist * blobDist);
        
        // 9. Curated Spectral Shift
        vec3 colBase = vec3(id.x, id.y, 1.0 - id.x);
        vec3 colFade = vec3(1.0, 0.5, 0.2); 
        vec3 color = mix(colBase, colFade, cycle);
        
        color = mix(color, vec3(1.0), smoothstep(0.1, 0.0, cycle));
        
        // 10. Accumulate
        finalColor += color * brightness * life * step(0.01, cycle);
    }

    // 11. Final Polish
    finalColor *= smoothstep(2.0, 0.5, length(uv));
    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}