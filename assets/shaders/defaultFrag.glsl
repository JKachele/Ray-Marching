#version 460 core
#include hg_sdf.glsl
layout (location = 0) out vec4 fragColor;

uniform vec2 uResolution;
uniform float uTime;

const float FOV = 1.0;
const int MAX_STEPS = 256;
const float MAX_DIST = 500;
const float EPSILON = 0.001;

vec2 fOpUnionID(vec2 obj1, vec2 obj2) {
    return (obj1.x < obj2.x) ? obj1 : obj2;
}

vec2 map(vec3 ray) {
    // Plane
    float planeDist = fPlane(ray, vec3(0, 1, 0), 1.0);
    float planeID = 2.0;
    vec2 plane = vec2(planeDist, planeID);
    // Sphere
    float sphereDist = fSphere(ray, 1.0);
    float sphereID = 1.0;
    vec2 sphere = vec2(sphereDist, sphereID);
    // Result
    vec2 result = fOpUnionID(sphere, plane);
    return result;
}

vec2 rayMarch(vec3 rayOrigin, vec3 rayDirection) {
    vec2 hit;
    vec2 object;
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 ray = rayOrigin + object.x * rayDirection;
        hit = map(ray);
        object.x += hit.x;
        object.y = hit.y;
        if (abs(hit.x) < EPSILON || object.x > MAX_DIST) break;
    }
    return object;
}

vec3 getNormal(vec3 ray) {
    vec2 e = vec2(EPSILON, 0.0);
    vec3 n = vec3(map(ray).x) - vec3(map(ray - e.xyy).x, map(ray - e.yxy).x, map(ray - e.yyx).x);
    return normalize(n);
}

vec3 getLight(vec3 ray, vec3 rayDist, vec3 color) {
    vec3 lightPos = vec3(20.0, 40.0, -30.0);
    vec3 L = normalize(lightPos - ray);
    vec3 N = getNormal(ray);

    vec3 diffuse = color * clamp(dot(L, N), 0.0, 1.0);

    // Shadows
    float distToObj = rayMarch(ray + N * 0.02, normalize(lightPos)).x;
    if (distToObj < length(lightPos - ray)) return vec3(0);
    return diffuse;
}

vec3 getMaterial(vec3 ray, float id) {
    vec3 material = vec3(1);
    switch (int(id)) {
        case 1:
        material = vec3(0.9, 0.9, 0.0);break;
        case 2:
        material = vec3(0.2 + 0.4 * mod(floor(ray.x) + floor(ray.z), 2.0));break;
    }
    return material;
}

void render(inout vec3 col, in vec2 uv) {
    vec3 rayOrigin = vec3(0, 0, -3.0);
    vec3 rayDir = normalize(vec3(uv, FOV));

    vec2 object = rayMarch(rayOrigin, rayDir);

    if (object.x < MAX_DIST) {
        vec3 ray = rayOrigin + object.x * rayDir;
        vec3 material = getMaterial(ray, object.y);
        col += getLight(ray, rayDir, material);
    }
}

void main() {
    vec2 uv = (2.0 * gl_FragCoord.xy - uResolution.xy) / uResolution.y;

    vec3 col = vec3(0.0, 0.0, 0.0);
    render(col, uv);

    // Gamma correction
    col = pow(col, vec3(0.4545));
    fragColor = vec4(col, 1.0);
}