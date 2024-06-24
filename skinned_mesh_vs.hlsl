#include "skinned_mesh.hlsli"


// WATER_SIMULATION
struct wave_parameter
{
    float A; // Amplitude 
    float L; // Wavelength 
    float S; // Speed
    float Q; // Steepness
    float2 D; // Direction
    float2 padding;
};
cbuffer wave_simulation : register(b3)
{
    float time;
    float time_delta;
    float2 padding;
    wave_parameter wave_parameters[3];
};

void simulate(inout float3 P, out float3 N)
{
	//Convert left-handed Y-up to right-handed Z-up
	P.xyz = P.xzy;

	const float x = P.x;
	const float y = P.y;
	const float z = P.z;

	const float PI = 3.14159265358979;
	const float g = 9.8;

	const float t = time;

	float A[3]; // Amplitude 
	float L[3]; // Wavelength 
	float S[3]; // Speed
	float2 D[3]; // Direction
	float Q[3]; // Steepness
	[unroll]
	for (int i = 0; i < 3; ++i)
	{
		A[i] = wave_parameters[i].A;
		L[i] = wave_parameters[i].L;
		S[i] = wave_parameters[i].S;
		Q[i] = wave_parameters[i].Q;
#if 1
		D[i] = normalize(wave_parameters[i].D); // Directional
#else
		D[i] = normalize(float2(x, y) - wave_parameters[i].D); //  Circular
#endif
	}

	// 1.2.1 Selecting the Waves
	float H = 0;
	[unroll]
	for (int i = 0; i < 3; ++i)
	{
#if 0
		const float w = 2 / L[i];
#else
		// Equation 13
		const float w = sqrt(g * 2 * PI / L[i]);
#endif
		const float p = S[i] * w;
		const float W = A[i] * sin(dot(D[i], float2(x, y)) * w + t * p); // Equation 1
		H += W; // Equation 2

#if 0
		P.z += W; // Equation 3
#else
		// Gerstner Waves
		// Equation 9
		const float Qi = A[i] > 0 ? Q[i] / (w * A[i] * 3) : 0;
		P.x += Qi * A[i] * D[i].x * cos(dot(w * D[i], float2(x, y)) + p * t);
		P.y += Qi * A[i] * D[i].y * cos(dot(w * D[i], float2(x, y)) + p * t);
		P.z += A[i] * sin(dot(w * D[i], float2(x, y)) + p * t);
#endif
	}

	float3 B = float3(1, 0, 0);
	float3 T = float3(0, 1, 0);
	N = float3(0, 0, 1);
	[unroll]
	for (int i = 0; i < 3; ++i)
	{
#if 0
		const float w = 2 / L[i];
#else
		// Equation 13
		const float w = sqrt(g * 2 * PI / L[i]);
#endif
		const float p = S[i] * w;
#if 0
		// Equation 7
		const float C = cos(dot(D[i], P.xy) * w + t * p);
		N.x -= w * D[i].x * A[i] * C;
		N.y -= w * D[i].y * A[i] * C;
#else
		// Equation 10
		const float WA = w * A[i];
		const float S = sin(w * dot(D[i], P.xy) + p * t);
		const float C = cos(w * dot(D[i], P.xy) + p * t);
		const float Qi = A[i] > 0 ? Q[i] / (w * A[i] * 3) : 0;

		B.x -= Qi * (D[i].x * D[i].x) * WA * S;
		B.y -= Qi * (D[i].x * D[i].y) * WA * S;
		B.z += D[i].x * WA * C;

		T.x -= Qi * (D[i].x * D[i].y) * WA * S;
		T.y -= Qi * (D[i].y * D[i].y) * WA * S;
		T.z += D[i].y * WA * C;

		N.x -= D[i].x * WA * C;
		N.y -= D[i].y * WA * C;
		N.z -= Qi * WA * S;
#endif

	}

	//Convert right-handed Z-up to left-handed Y-up
	P.xyz = P.xzy;
	N.xyz = normalize(N.xzy);
}



VS_OUT main(VS_IN vin)
{

     vin.normal.w = 0;
     float sigma = vin.tangent.w;
     vin.tangent.w = 0;
     float4 blended_position = { 0, 0, 0, 1 };
     float4 blended_normal = { 0, 0, 0, 0 };
     float4 blended_tangent = { 0, 0, 0, 0 };
     for (int bone_index = 0; bone_index < 4; ++bone_index)
         {
         blended_position += vin.bone_weights[bone_index]
             *mul(vin.position, bone_transforms[vin.bone_indices[bone_index]]);
         blended_normal += vin.bone_weights[bone_index]
            *mul(vin.normal, bone_transforms[vin.bone_indices[bone_index]]);
     blended_tangent += vin.bone_weights[bone_index]
         * mul(vin.tangent, bone_transforms[vin.bone_indices[bone_index]]);
         }
     vin.position = float4(blended_position.xyz, 1.0f);
     vin.normal = float4(blended_normal.xyz, 0.0f);
     vin.tangent = float4(blended_tangent.xyz, 0.0f);
	 // WATER_SIMULATION
	 float3 P = mul(vin.position, world).xyz;
	 float3 N = normalize(mul(vin.normal, world)).xyz;
	 simulate(P, N);
    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, view_projection));
    vout.world_position = mul(vin.position, world);
    //vin.normal.w = 0;

    vout.world_normal = normalize(mul(vin.normal, world));
    vout.world_tangent = normalize(mul(vin.tangent, world));
    vout.world_tangent.w = sigma;
    vout.texcoord = vin.texcoord;
    //vout.color = material_color; 
     #if 1
         vout.color = material_color;
     #else
         vout.color = 0;
     const float4 bone_colors[4] = {
      {1, 0, 0, 1},
      {0, 1, 0, 1},
      {0, 0, 1, 1},
      {1, 1, 1, 1},
      };
     for (int bone_index = 0; bone_index < 4; ++bone_index)
       {
      vout.color += bone_colors[vin.bone_indices[bone_index] % 4]
        *vin.bone_weights[bone_index];
      }
      #endif

	

    return vout;
}