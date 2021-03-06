#version 440
/*
 * Lighting fragment shader for a deferred PBR pipeline. Computes the ambient
 * component.
 */

#define PI 3.141592654
#define MAX_MIP 4.0

struct Camera
{
  vec3 position;
  vec3 viewDir;
};

// Camera uniform.
uniform Camera camera;

// Uniforms for ambient lighting.
layout(binding = 0) uniform samplerCube irradianceMap;
layout(binding = 1) uniform samplerCube reflectanceMap;
layout(binding = 2) uniform sampler2D brdfLookUp;

// Uniforms for the geometry buffer.
uniform vec2 screenSize;
layout(binding = 3) uniform sampler2D gPosition;
layout(binding = 4) uniform sampler2D gNormal;
layout(binding = 5) uniform sampler2D gAlbedo;
layout(binding = 6) uniform sampler2D gMatProp;

uniform float intensity = 1.0;

// Output colour variable.
layout(location = 0) out vec4 fragColour;

// Trowbridge-Reitz distribution function.
float TRDistribution(vec3 N, vec3 H, float alpha);
// Smith-Schlick-Beckmann geometry function.
float SSBGeometry(vec3 N, vec3 L, vec3 V, float roughness);
// Schlick approximation to the Fresnel factor.
vec3 SFresnel(float cosTheta, vec3 F0);
// Schlick approximation to the Fresnel factor, with roughness!
vec3 SFresnelR(float cosTheta, vec3 F0, float roughness);

void main()
{
  vec2 fTexCoords = gl_FragCoord.xy / screenSize;

  vec3 position = texture(gPosition, fTexCoords).xyz;
  vec3 normal = normalize(texture(gNormal, fTexCoords).xyz);
  vec3 albedo = texture(gAlbedo, fTexCoords).rgb;
  float metallic = texture(gMatProp, fTexCoords).r;
  float roughness = texture(gMatProp, fTexCoords).g;
  float ao = texture(gMatProp, fTexCoords).b;

  vec3 F0 = mix(vec3(0.04), albedo, metallic);

  vec3 view = normalize(position - camera.position);
  vec3 reflection = reflect(view, normal);

  float nDotV = abs(dot(normal, -view));
  vec3 ks = SFresnelR(nDotV, F0, roughness);
  vec3 kd = (vec3(1.0) - ks) * (1.0 - roughness);

	vec3 radiosity = vec3(0.0);
	vec3 ambientDiff = kd * texture(irradianceMap, normal).rgb * albedo;
  vec3 ambientSpec = textureLod(reflectanceMap, reflection,
                                roughness * MAX_MIP).rgb;
  vec2 brdfInt = texture(brdfLookUp, vec2(nDotV, roughness)).rg;
  ambientSpec = ambientSpec * (brdfInt.r * ks + brdfInt.g);

	vec3 colour = intensity * (radiosity + (ambientDiff + ambientSpec) * ao);

  fragColour = vec4(colour, 1.0);
}

// Trowbridge-Reitz distribution function.
float TRDistribution(vec3 N, vec3 H, float roughness)
{
  float alpha = roughness*roughness;
  float a2 = alpha * alpha;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;

  float nom   = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}

// Schlick-Beckmann geometry function.
float Geometry(float NdotV, float roughness)
{
  float r = (roughness + 1.0);
  float k = (roughness * roughness) / 8.0;

  float nom   = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}

// Smith's modified geometry function.
float SSBGeometry(vec3 N, vec3 L, vec3 V, float roughness)
{
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float g2 = Geometry(NdotV, roughness);
  float g1 = Geometry(NdotL, roughness);

  return g1 * g2;
}

// Schlick approximation to the Fresnel factor.
vec3 SFresnel(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 SFresnelR(float cosTheta, vec3 F0, float roughness)
{
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
