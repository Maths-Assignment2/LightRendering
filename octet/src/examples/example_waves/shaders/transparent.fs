uniform vec3 object_colour_;
uniform vec3 light_colour_;

uniform float shineDamper;
uniform float reflectivity;

varying vec3 rel_normal_;
varying vec3 vert_to_light_;
varying vec3 vert_to_camera_;
varying vec3 world_pos;

void main()
{
	float brightness = max(dot(normalize(rel_normal_), normalize(vert_to_light_)), 0.1);
	vec3 diffuse = brightness * light_colour_;

	// Specular
	vec3 norm_vert_to_camera = normalize(vert_to_camera_);
	vec3 light_to_vert = -vert_to_light_;
	vec3 reflected_light = reflect(light_to_vert, normalize(rel_normal_));

	float specular = max( dot( reflected_light, norm_vert_to_camera ), 0.0);
	float damping = pow(specular, shineDamper);
	vec3 specular_output = damping * reflectivity * light_colour_;
    //refraction attempt 1
    float ratio =1.00/1.52;
    vec3 I = normalize(light_to_vert-vert_to_camera_);
    vec3 R = refract(I,normalize(rel_normal_),ratio);
	
	vec3 result = (diffuse + specular_output) * object_colour_;
    gl_FragColor = vec4(result, R);
}