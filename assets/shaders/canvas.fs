#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D silhouette_texture;
uniform sampler2D structure_texture;

layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

void main() {
    vec3 pixcol = texture(silhouette_texture, TexCoords).rgb;
    if (pixcol != vec3(0.0f)) {
        vec2 size = 2.0f / textureSize(silhouette_texture, 0);

        const int sz = 1;
        for (int i = -sz; i <= sz; ++i) {
            for (int j = -sz; j <= sz; ++j) {
                if (i == 0 && j == 0) continue;

                vec2 offset = vec2(i, j) * size;

                if (texture(silhouette_texture, TexCoords + offset).rgb != pixcol) {
                    FragColor = vec4(0.2627451f, 0.9686275f, 0.7098039f, 1.0f);
                    return;
                }
            }
        }
    }

    FragColor = texture(structure_texture, TexCoords);
}
