#version 450

in vec3 vertex_vs;
in vec3 normal_vs;
in vec3 color_vs;

const vec3 la = vec3(0.3);
const vec3 ld = vec3(0.5);
const vec3 ls = vec3(0.1);


const vec3 ka = vec3(0.8);
const vec3 ks = vec3(0.8);
const float shininess = 1.0;

uniform mat4 model_matrix;
uniform mat4 view_matrix;

out vec4 frag_color;

void main() {
    //vec3 normal1 = normalize(normal_vs);
	vec3 X = dFdx(vertex_vs);
	vec3 Y = dFdy(vertex_vs);
	vec3 normal1 = normalize(cross(X, Y));


    vec3 pos_light = inverse(view_matrix)[3].xyz;
    vec3 dir_light = normalize(-vertex_vs + pos_light);
    vec3 v = normalize(vertex_vs);
    vec3 r = reflect(-dir_light, normal1);

    float sDotN = max(dot(dir_light, normal1), 0.0);
    vec3 ambient = la*ka;
    vec3 diffuse = ld*color_vs*sDotN;
    vec3 specular = vec3(0.0);
    if( sDotN > 0.0 )
        specular = ls*ks*pow(max(dot(r,v), 0.0), shininess);

    frag_color = vec4( diffuse + ambient + specular, 1 );
}

