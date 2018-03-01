#version 410

in vec3 normal;
in vec2 color;

layout( location = 0 ) out vec4 fragColor;

struct SHC{
    vec3 L00, L1m1, L10, L11, L2m2, L2m1, L20, L21, L22;
};

SHC groove = SHC(
    vec3( 0.3783264,  0.4260425,  0.4504587),
    vec3( 0.2887813,  0.3586803,  0.4147053),
    vec3( 0.0379030,  0.0295216,  0.0098567),
    vec3(-0.1033028, -0.1031690, -0.0884924),
    vec3(-0.0621750, -0.0554432, -0.0396779),
    vec3( 0.0077820, -0.0148312, -0.0471301),
    vec3(-0.0935561, -0.1254260, -0.1525629),
    vec3(-0.0572703, -0.0502192, -0.0363410),
    vec3( 0.0203348, -0.0044201, -0.0452180)
);

vec3 sh_light(vec3 normal2, SHC l){
    float x = normal2.x;
    float y = normal2.y;
    float z = normal2.z;

    const float C1 = 0.429043;
    const float C2 = 0.511664;
    const float C3 = 0.743125;
    const float C4 = 0.886227;
    const float C5 = 0.247708;

    return (
        C1 * l.L22 * (x * x - y * y) +
        C3 * l.L20 * z * z +
        C4 * l.L00 -
        C5 * l.L20 +
        2.0 * C1 * l.L2m2 * x * y +
        2.0 * C1 * l.L21  * x * z +
        2.0 * C1 * l.L2m1 * y * z +
        2.0 * C2 * l.L11  * x +
        2.0 * C2 * l.L1m1 * y +
        2.0 * C2 * l.L10  * z
    );
}

uniform sampler2D sampler;

void main() {
    fragColor = texture2D( sampler, color); // * vec4 ( sh_light( vec3(normal), groove) , 1); //vec4( UV.x, 1, 1, 1); //texture( sampler, UV ).rgb; //vec4 ( 0.1, 0.1, 0.1, 1);
}
