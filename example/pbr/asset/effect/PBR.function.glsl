#ifndef _PBR_FUNCTION_GLSL_
#define _PBR_FUNCTION_GLSL_

#pragma include "BRDF.function.glsl"
#pragma include "TextureLod.function.glsl"

vec3 pbr_diffuse(vec3 diffuseColor, float roughness, float NoV, float NoL, float VoH)
{
    return brdf_diffuse(diffuseColor, roughness, NoV, NoL, VoH) * NoL;
}

vec3 pbr_specular(vec3 specularColor, float roughness, float NoL, float NoV, float NoH, float VoH)
{
    // f = D * F * G / (4 * (N.L) * (N.V));
    return brdf_distribution(roughness, NoH)
         * brdf_fresnel(specularColor, VoH)
         * brdf_geometricVisibility(roughness, NoV, NoL, VoH)//;
         * NoL;
        //  * (4.0 * NoL * NoV);
}

vec3 pbr_envDiffuse(sampler2D irradianceMap, vec3 normal)
{
    return texture2D(irradianceMap, normalToLatLongUV(normal)).rgb;
}

vec3 pbr_fresnelSchlickWithRoughness(vec3 specularColor, vec3 e, vec3 n, float gloss)
{
    return specularColor + (max(vec3(gloss), specularColor) - specularColor) * pow(1.0 - saturate(dot(e, n)), 5.0);
}

vec3 pbr_envSpecular(vec3       specularColor,
                     sampler2D  radianceMap,
                     sampler2D  integratedBRDF,
                     int        numMipmaps,
                     float      roughness,
                     vec3       worldSpaceReflectionVector,
                     float      NoV,
                     vec3       normal,
                     vec3       view)
{
    float mipMapIndex = roughness * float(numMipmaps);
    vec3 prefilteredColor = texturelod_texture(
        radianceMap,
        normalToLatLongUV(worldSpaceReflectionVector),
        mipMapIndex
    ).rgb;
    vec2 envBRDF = texture2D(integratedBRDF, vec2(roughness, NoV)).xy;

    return pbr_fresnelSchlickWithRoughness(specularColor, normal, view, 1.0 - roughness)
        * prefilteredColor;
        // * (specularColor * envBRDF.x + envBRDF.y);
}

#endif // _PBR_FUNCTION_GLSL_
