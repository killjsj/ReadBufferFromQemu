shader_type canvas_item;

void fragment() {
    vec4 tex = texture(TEXTURE, UV);
    // 交换 R 和 B 通道
    COLOR = vec4(tex.b, tex.g, tex.r, 1);
}