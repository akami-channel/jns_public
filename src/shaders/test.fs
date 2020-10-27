#version 330 core
in vec2 st_FragCoord;
out vec4 st_FragColor;
 #ifdef GL_ES
precision mediump float;
#endif

#define M_PI 3.14159265358979323846

//#extension GL_OES_standard_derivatives : enabl
uniform float time;
uniform vec2 resolution;
uniform vec2 mouse;

uniform float my_var2;

vec2 rotate( in vec2 v, in float a) {
    return vec2(cos(a) * v.x + sin(a) * v.y, -sin(a) * v.x + cos(a) * v.y);
}

float torus( in vec3 p, in vec2 t) {
    vec2 q = abs(vec2(max(abs(p.x), abs(p.z)) - t.x, p.y));
    return max(q.x, q.y) - t.y;
}

float trap( in vec3 p) {
    //return abs(max(abs(p.z)-0.1, abs(p.x)-0.1))-0.01;
    //return length(max(abs(p.xy) - 0.05, -0.010));
    return length(max(abs(p) - 0.35, 0.0));
    //return abs(length(p.xz)-0.2)-0.01;
    return min(length(p.xz), max(length(p.yz), length(p.xy))) - 0.05;
    return abs(min(torus(vec3(p.x, mod(p.y, 0.4) - 0.2, p.z), vec2(0.1, 0.05)), max(abs(p.z) - 0.05, abs(p.x) - 0.05))) - 0.005;
    //return abs(min(torus(p, vec2(0.3, 0.05)), max(abs(p.z)-0.05, abs(p.x)-0.05)))-0.005;
}

float map( in vec3 p) {
    float time2 = time + 60.0;
    float cutout = dot(abs(p.yz), vec2(0.5)) - 0.035;
    // float road = max(abs(p.y - 0.025), abs(p.z) - 0.035);
    float road = abs(p.y - 0.015);

    vec3 z = abs(1.0 - mod(p, 2.0));
    z.yz = rotate(z.yz, time2 * 0.05);

    float d = 999.0;
    float s = 1.0;
    for (float i = 0.0; i < 3.0; i++) {
        z.xz = rotate(z.xz, radians(i * 10.0 + time2));
        z.zy = rotate(z.yz, radians((i + 1.0) * 20.0 + time2 * 1.1234));
        z = abs(1.0 - mod(z + i / 3.0, 2.0));

        z = z * 2.0 - 0.3;
        s *= 0.5;
        d = min(d, trap(z) * s);
    }
    return min(max(d, -cutout), road);
}

vec3 hsv( in float h, in float s, in float v) {
    return mix(vec3(1.0), clamp((abs(fract(h + vec3(3, 2, 1) / 3.0) * 6.0 - 3.0) - 1.0), 0.0, 1.0), s) * v;
}

vec3 intersect( in vec3 rayOrigin, in vec3 rayDir) {
    float time2 = time + 60.0;
    float total_dist = 0.0;
    vec3 p = rayOrigin;
    float d = 1.0;
    float iter = 0.0;
    float mind = 3.14159 + sin(time2 * 0.0) * 0.0; // Move road from side to side slowly

    for (int i = 0; i < 59; i++) {
        if (d < 0.001) continue;

        d = map(p);
        // This rotation causes the occasional distortion - like you would see from heat waves
        p += d * vec3(rayDir.x, rotate(rayDir.yz, sin(mind)));
        mind = min(mind, d);
        total_dist += d;
        iter++;
    }

    vec3 color = vec3(0.0);
    if (d < 0.001) {
        float x = (iter / 59.0);
        float y = (d - 0.01) / 0.01 / (59.0);
        // float z = (0.01 - d) / 0.001 / 59.0 / mouse.x;
        float z = (0.01 - d) / 0.001 / 59.0 / 2.;
        if (max(abs(p.y - 0.025), abs(p.z) - 10.035) < 0.002) { // Road
            float w = smoothstep(mod(p.x * 50.0, 4.0), 2.0, 2.01);
            w -= 1.0 - smoothstep(mod(p.x * 50.0 + 2.0, 4.0), 2.0, 1.99);
            w = fract(w + 0.0001);
            float a = fract(smoothstep(abs(p.z), 0.0025, 0.0026));
            color = vec3((.20 - x - y * 2.) * mix(vec3(0.8, 0.8, 0), vec3(0.1), 1.0 - (1.0 - w) * (1.0 - a)));
        } else {
            float q = 1.0 - x - y * 2. + z;
            color = hsv(q * 0.2 + 0.85, 1.0 - q * 0.2, q);
        }
    } else
        color = hsv(d, 1.0, 1.0) * mind * 45.0; // Background
    return color;
}

    // Return 4x4 rotation X matrix
    // angle in radians
    // ========================================
    mat4 Rot4X(float a ) {
        float c = cos( a );
        float s = sin( a );
        return mat4( 1, 0, 0, 0,
                     0, c,-s, 0,
                     0, s, c, 0,
                     0, 0, 0, 1 );
    }

    // Return 4x4 rotation Y matrix
    // angle in radians
    // ========================================
    mat4 Rot4Y(float a ) {
        float c = cos( a );
        float s = sin( a );
        return mat4( c, 0, s, 0,
                     0, 1, 0, 0,
                    -s, 0, c, 0,
                     0, 0, 0, 1 );
    }

    // Return 4x4 rotation Z matrix
    // angle in radians
    // ========================================
    mat4 Rot4Z(float a ) {
        float c = cos( a );
        float s = sin( a );
        return mat4(
            c,-s, 0, 0,
            s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
         );
    }

// const float offset = 1.0 / 300.0;  

// void main()
// {
//     vec2 offsets[9] = vec2[](
//         vec2(-offset,  offset), // top-left
//         vec2( 0.0f,    offset), // top-center
//         vec2( offset,  offset), // top-right
//         vec2(-offset,  0.0f),   // center-left
//         vec2( 0.0f,    0.0f),   // center-center
//         vec2( offset,  0.0f),   // center-right
//         vec2(-offset, -offset), // bottom-left
//         vec2( 0.0f,   -offset), // bottom-center
//         vec2( offset, -offset)  // bottom-right    
//     );

//     float kernel[9] = float[](
//         -1, -1, -1,
//         -1,  9, -1,
//         -1, -1, -1
//     );
    
//     vec3 sampleTex[9];
//     for(int i = 0; i < 9; i++)
//     {
//         sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
//     }
//     vec3 col = vec3(0.0);
//     for(int i = 0; i < 9; i++)
//         col += sampleTex[i] * kernel[i];
    
//     st_FragColor = vec4(col, 1.0);
// }  

void main(void) {
    float time2 = time + 60.0;
    vec3 upDirection = vec3(0, -1, 0);
    vec3 cameraDir = normalize(vec3(4., -0.3 - 1.5, 0.));
    vec3 cameraOrigin = vec3(time2 * 0.1, 0, 0);

    vec3 u = normalize(cross(upDirection, cameraOrigin));
    vec3 v = normalize(cross(cameraDir, u));
    vec2 screenPos = -1.0 + 2.0 * st_FragCoord.xy / resolution.xy;
    screenPos.x *= resolution.x / resolution.y;
    vec3 rayDir = normalize(u * screenPos.x + v * screenPos.y + cameraDir * (1.0 - length(screenPos) * 0.5));
    vec3 col = vec3(intersect(cameraOrigin, rayDir));
    // st_FragColor = vec4(col, 1.0);

    float angle = 90.0;
    float radian_angle = angle * M_PI / 180.0;

    radian_angle = sin(time / 10.0) * 2 * M_PI;
    float radian_angle2 = cos(time / 12.0) * 2 * M_PI;
    float radian_angle3 = sin(time / 17.0) * 2 * M_PI;

    mat4 rotMat1 = Rot4Y(radian_angle);
    mat4 rotMat2 = Rot4X(radian_angle2);
    mat4 rotMat3 = Rot4Z(radian_angle3);

    vec4 colVec4 = vec4(col, 1.0);

    colVec4 = colVec4 - vec4 (0.5, 0.5, 0.5, 0.0);

    // Do stuff

    colVec4 = colVec4 * rotMat1 * rotMat2 * rotMat3;

    colVec4 = colVec4 + vec4 (0.5, 0.5, 0.5, 0.0);

    vec4 temp_frag_color = colVec4;



    // float blue_gamma = 1.7;
    // temp_frag_color.b = pow(temp_frag_color.b, 1.0/blue_gamma);

    // colVec4.b = temp_frag_color.b;

    st_FragColor = colVec4 - vec4(0.0, 0.0, 0.0, 0.0);
    //st_FragColor = vec4(1.0, 0.5, 0.0, 1.0);
    // st_FragColor = colVec4 - vec4(0.0, colVec4.y, colVec4.z, 0.0);
}