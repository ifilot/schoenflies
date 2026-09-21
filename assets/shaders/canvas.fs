#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D silhouette_texture;
uniform sampler2D labels_texture;
uniform sampler2D structure_texture;

layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

// Compact FXAA pass for the single-sample textures used by the molecule
// compositor. Hardware MSAA still handles the final widget framebuffer.
vec4 fxaa(sampler2D image, vec2 uv) {
    vec2 texel = 1.0 / vec2(textureSize(image, 0));
    vec3 rgbNW = texture(image, uv + vec2(-1.0, -1.0) * texel).rgb;
    vec3 rgbNE = texture(image, uv + vec2( 1.0, -1.0) * texel).rgb;
    vec3 rgbSW = texture(image, uv + vec2(-1.0,  1.0) * texel).rgb;
    vec3 rgbSE = texture(image, uv + vec2( 1.0,  1.0) * texel).rgb;
    vec4 center = texture(image, uv);
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM = dot(center.rgb, luma);
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    vec2 direction;
    direction.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    direction.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    float reduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.03125, 0.0078125);
    float reciprocal = 1.0 / (min(abs(direction.x), abs(direction.y)) + reduce);
    direction = clamp(direction * reciprocal, vec2(-8.0), vec2(8.0)) * texel;
    vec3 rgbA = 0.5 * (
        texture(image, uv + direction * (1.0 / 3.0 - 0.5)).rgb +
        texture(image, uv + direction * (2.0 / 3.0 - 0.5)).rgb);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(image, uv + direction * -0.5).rgb +
        texture(image, uv + direction * 0.5).rgb);
    float lumaB = dot(rgbB, luma);
    return vec4((lumaB < lumaMin || lumaB > lumaMax) ? rgbA : rgbB, center.a);
}

void main() {
    vec3 silhouette_col = texture(silhouette_texture, TexCoords).rgb;
    if (silhouette_col != vec3(0.0f)) {
        vec2 size = 2.0f / textureSize(silhouette_texture, 0);

        const int sz = 1;
        for (int i = -sz; i <= sz; ++i) {
            for (int j = -sz; j <= sz; ++j) {
                if (i == 0 && j == 0) continue;

                vec2 offset = vec2(i, j) * size;

                if (texture(silhouette_texture, TexCoords + offset).rgb != silhouette_col &&
                    texture(labels_texture, TexCoords).rgb == vec3(0.0f)) {
                    FragColor = vec4(0.2627451f, 0.9686275f, 0.7098039f, 1.0f);
                    return;
                }
            }
        }
    }

    // based on (but not exactly) the Sobel operator
    float n[8];
    float w = 1.0f / textureSize(labels_texture, 0).x;
    float h = 1.0f / textureSize(labels_texture, 0).y;
    n[0] = texture(labels_texture, TexCoords + vec2(-w, -h)).r;
    n[1] = texture(labels_texture, TexCoords + vec2( 0, -h)).r;
    n[2] = texture(labels_texture, TexCoords + vec2( w, -h)).r;
    n[3] = texture(labels_texture, TexCoords + vec2(-w,  0)).r;
    n[4] = texture(labels_texture, TexCoords + vec2( w,  0)).r;
    n[5] = texture(labels_texture, TexCoords + vec2(-w,  h)).r;
    n[6] = texture(labels_texture, TexCoords + vec2( 0,  h)).r;
    n[7] = texture(labels_texture, TexCoords + vec2( w,  h)).r;

    float sobel_edge_h = (n[2] + 2 * n[4] + n[7]) - (n[0] + 2 * n[3] + n[5]);
    float sobel_edge_v = (n[0] + 2 * n[1] + n[2]) - (n[5] + 2 * n[6] + n[7]);
    float sobel = clamp(1.5 - min(sobel_edge_h * sobel_edge_h + sobel_edge_v * sobel_edge_v, 1.0), 0.0, 1.0);

    FragColor = max(fxaa(structure_texture, TexCoords) * sobel, texture(labels_texture, TexCoords));
}
