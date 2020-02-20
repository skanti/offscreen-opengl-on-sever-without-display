#version 450

in vec3 vertex_vs;
in vec3 color_vs;

const vec3 la = vec3(0.3);
const vec3 ld = vec3(0.5);
const vec3 ls = vec3(0.2);


const vec3 ka = vec3(1.0);
const vec3 ks = vec3(1.0);
const float shininess = 1.0;

uniform mat4 view_matrix;

out vec4 frag_color;

void main() {
	vec3 X = dFdx(vertex_vs);
	vec3 Y = dFdy(vertex_vs);
	vec3 normal = normalize(cross(X, Y));

    vec3 pos_light = inverse(view_matrix)[3].xyz;
    vec3 s = normalize(pos_light - vertex_vs);
    vec3 v = normalize(vertex_vs);
    vec3 r = reflect(-s, normal);

    float sDotN = max(dot(s, normal), 0.0);
    vec3 ambient = la*ka;
    vec3 diffuse = color_vs*sDotN;
    vec3 specular = vec3(0.0);
    if( sDotN > 0.0 )
        specular = ls*ks*pow(max(dot(r,v), 0.0), shininess);

    frag_color = vec4( diffuse + ambient + specular, 1 );
}
