

Texture2D txDiffuse : register(t0);
Texture2D renderTex : register(t1);
Texture2D txDiffuse2 : register(t2);
Texture2D txDiffuse3 : register(t3);
Texture2D txDiffuse4 : register(t4);

SamplerState samp1 : register(s0);
SamplerComparisonState samShadow : register(s1);
SamplerState samp2 : register(s2);

cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
};
struct Light
{
    float3 pos;
    float range;
    float3 dir;
    float cone;
    float3 att;
    float4 ambient;
    float4 diffuse;
};

cbuffer cbChangesEveryFrame : register(b2)
{
    matrix View;
    matrix World;
    float4 lightPos[4];
    float time;
    float l;
    float3 mEye;
    float model;
    
};
cbuffer cbLight : register(b0)
{
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
}



//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;

    uint vertexID : SV_VertexID;
};
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float4 Col : COLOR;
    float2 Tex : TEXCOORD0;
    float3 lDir : TEXCOORD1;
    float4 lightViewPosition : TEXCOORD2;
    float3 lightPos : TEXCOORD3;
    float4 WorldPos : TEXCOORD4;
    float3 WorldNormal : TEXCOORD5;
    float3 viewDir : TEXCOORD6;
    float4 LightSpacePos : TEXCOORD7;
};

float localLight(PS_INPUT input)
{
    float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
    float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
    float4 lightCol = float4(1.0, 0.6, 0.8, 1.0);
    float3 lightDir = normalize(lightPos[0].xyz - input.WorldPos.xyz);
    float3 normal = normalize(input.WorldNormal);
    float diff = max(0.0, dot(lightDir, normal));
    input.Col += (materialAmb + diff * materialDiff) * lightCol;
    return input.Col;

}
float4 spotlight(PS_INPUT input)
{
    float4 col = float4(1, 0.5, 0.3, 1);
    float inner = 0.9999;
    float outer = inner * 0.99;
    float epsilon = inner - outer;
    for (int i = 0; i < 4; i++)
    {
        float4 a = lightPos[i];
        float3 toPos = normalize(input.WorldPos.xyz - a.xyz);
        float3 spotDir = normalize(-a.xyz);
        float d = dot(spotDir, toPos);
        float spotIntensity = clamp((d - outer) / epsilon, 0, 1);
        col += (dot(input.WorldNormal, -toPos) * spotIntensity);
    }
   
    return col/2;
}


float4 BasicPS(PS_INPUT input) : SV_Target
{
   
    if(l!=2)
    {
       
        float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
        float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
        float4 lightCol = float4(1.0, 1, 1, 1.0);
        float3 lightDir = normalize(lightPos[0].xyz - input.WorldPos.xyz);
        float3 normal = normalize(input.WorldNormal);
        float diff = max(0.0, dot(lightDir, normal));
        input.Col += (materialAmb + diff * materialDiff) * lightCol;
    }
    else
    {
        input.Col = spotlight(input);
    }
  
    input.Col.a = 0.2;
    
    
    return saturate(input.Col);
}

PS_INPUT BasicVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.WorldPos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Norm = input.Norm;
    output.WorldNormal = mul(input.Norm, (float3x3) World);
    return output;
}
PS_INPUT ScreenVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    output.Pos = float4(sign(input.Pos.xy), 0.0, 1.0); // -1 to 1
    output.Tex = (output.Pos.xy + 1) / 2;
    
    return output;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.WorldPos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Norm = input.Norm;
    output.WorldNormal = mul(input.Norm, World);
    return output;
}
float4 PS(PS_INPUT input) : SV_Target
{
    //return txDiffuse.Sample(samp1, input.Tex);
    float3 triblend = saturate(pow(input.WorldNormal, 4));
    triblend /= max(dot(triblend, float3(1, 1, 1)), 0.0001);

    float3 axisSign = input.WorldNormal < 0 ? -1 : 1;

    float2 uvX = input.WorldPos.zy;
    float2 uvY = input.WorldPos.xz;
    float2 uvZ = input.WorldPos.xy;

    float4 colX = txDiffuse.Sample(samp1, uvX);
    float4 colY = txDiffuse.Sample(samp1, uvY);
    float4 colZ = txDiffuse.Sample(samp1, uvZ);
    
    float toCam = (mEye - input.WorldPos.xyz);
    float lf = dot(toCam, normalize(input.WorldNormal));
    return saturate(lf * colX * triblend.x + colY * triblend.y + colZ * triblend.z);
}

float4 BumpMap(PS_INPUT input) : SV_TARGET
{
   //return fireTex.Sample(fireSamp, input.Tex);
    float3 triblend = saturate(pow(input.WorldNormal, 4));
    triblend /= max(dot(triblend, float3(1, 1, 1)), 0.01);

    float3 axisSign = input.WorldNormal < 0 ? -1 : 1;

    float2 uvX = input.WorldPos.zy;
    float2 uvY = input.WorldPos.xz;
    float2 uvZ = input.WorldPos.xy;

    float4 colX = txDiffuse.Sample(samp1, uvX);
    float4 colY = txDiffuse.Sample(samp1, uvY);
    float4 colZ = txDiffuse.Sample(samp1, uvZ);
    
    float3 tnormalX = normalize(4.0 * txDiffuse2.Sample(samp1, uvX).xyz - 1.0);
    float3 tnormalY = normalize(4.0 * txDiffuse2.Sample(samp1, uvY).xyz - 1.0);
    float3 tnormalZ = normalize(4.0 * txDiffuse2.Sample(samp1, uvZ).xyz - 1.0);


    float3 tangentX = normalize(cross(input.WorldNormal, float3(0, axisSign.x, 0)));
    float3 bitangentX = normalize(cross(tangentX, input.WorldNormal)) * axisSign.x;
    float3x3 tbnX = float3x3(tangentX, bitangentX, input.WorldNormal);

    float3 tangentY = normalize(cross(input.WorldNormal, float3(0, axisSign.y, 0)));
    float3 bitangentY = normalize(cross(tangentY, input.WorldNormal)) * axisSign.y;
    float3x3 tbnY = float3x3(tangentY, bitangentY, input.WorldNormal);

    float3 tangentZ = normalize(cross(input.WorldNormal, float3(0, axisSign.z, 0)));
    float3 bitangentZ = normalize(cross(tangentZ, input.WorldNormal)) * axisSign.z;
    float3x3 tbnZ = float3x3(tangentZ, bitangentZ, input.WorldNormal);

    float3 worldNormal = normalize(
        clamp(mul(tnormalX, tbnX), -1, 1) * triblend.x +
        clamp(mul(tnormalY, tbnY), -1, 1) * triblend.y +
        clamp(mul(tnormalZ, tbnZ), -1, 1) * triblend.z
    );
    float3 lightDir;

    float lf;
    if (l==2)
    {
        for (int i = 0; i < 4; i++)
        {
            lightDir = normalize(lightPos[i].xyz - input.WorldPos.xyz);
            lf += dot(worldNormal, -lightDir);
        }
    }
    else
    {
        lightDir = normalize(lightPos[0].xyz - input.WorldPos.xyz);
        lf = dot(worldNormal, -lightDir);
    }

    return saturate(lf * (colX * triblend.x + colY * triblend.y + colZ * triblend.z));
}

PS_INPUT GouroudVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.WorldPos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Norm = input.Norm;
    output.WorldNormal = mul(input.Norm, World);
    output.Tex = input.Tex;
    if (l != 2)
    {
        float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
        float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
        float4 lightCol = float4(1.0, 0.6, 0.8, 1.0);
        float3 lightDir = normalize(lightPos[0].xyz - input.Pos.xyz);
        float3 normal = normalize(input.Norm);
        float diff = max(0.0, dot(lightDir, normal));
        output.Col += (materialAmb + diff * materialDiff) * lightCol;
    }
    else
    {
        output.Col = spotlight(output);

    }
   
    return output;
}
////Gouroud

float4 GouroudPS(PS_INPUT input) : SV_Target
{
    return input.Col;
}
//Phong
PS_INPUT PhongVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.WorldPos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Norm = input.Norm;
    output.WorldNormal = mul(input.Norm, World);
    output.Col = float4(0.3, 0.3, 0.3, 1.0);
    return output;
}

float4 PhongPS(PS_INPUT input) : SV_Target
{
    float4 col = float4(0, 0, 0, 0);
    if (l != 2)
    {
        float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
        float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
        float4 lightCol = float4(1.0, 0.6, 0.8, 1.0);
        float3 lightDir = normalize(lightPos[0].xyz - input.WorldPos.xyz);
        float3 normal = normalize(input.WorldNormal);
        float diff = max(0.0, dot(lightDir, normal));
        input.Col += (materialAmb + diff * materialDiff) * lightCol;
    }
    else
    {
       
        float4 col = float4(0, 0, 0.3, 1);
        float inner = 0.9999;
        float outer = inner * 0.999;
        float epsilon = inner - outer;
        for (int i = 0; i < 4; i++)
        {
            float4 a = lightPos[i];
            float3 toPos = normalize(input.WorldPos.xyz - a.xyz);
            float3 spotDir = normalize(-a.xyz);
            float d = dot(spotDir, toPos);
            float spotIntensity = clamp((d - outer) / epsilon, 0, 1);
            col += (dot(input.WorldNormal, -toPos) * spotIntensity);
        }
       input.Col = col; 
    }
    
    return saturate(input.Col);
}
PS_INPUT ShadowVS1(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, lightViewMatrix);
    output.Pos = mul(output.Pos, lightProjectionMatrix);
    return output;
}
PS_INPUT ShadowVS2(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    float4 worldPos = mul(input.Pos, World);
    output.WorldNormal = mul(input.Norm, (float3x3) World);
    output.Norm = input.Norm;
    output.Pos = mul(worldPos, View);
    output.Pos = mul(output.Pos, Projection);
    output.LightSpacePos = mul(worldPos, lightViewMatrix);
    output.LightSpacePos = mul(output.LightSpacePos, lightProjectionMatrix);
 
    return output;
}

float4 ShadowPS1(PS_INPUT input) : SV_Target
{
    return float4(1, 0, 0, 0);
}

float4 ShadowPS2(PS_INPUT input) : SV_Target
{
   

    //input.LightSpacePos.w = 1;
    input.LightSpacePos.xyz /= input.LightSpacePos.w;
    input.LightSpacePos.x = 0.5 + input.LightSpacePos.x * 0.5;
    input.LightSpacePos.y = 0.5 - input.LightSpacePos.y * 0.5;
   
    float sf = 0;
    
    if (input.LightSpacePos.x < -1.0f || input.LightSpacePos.x > 1.0f ||
        input.LightSpacePos.y < -1.0f || input.LightSpacePos.y > 1.0f ||
        input.LightSpacePos.z < 0.0f || input.LightSpacePos.z > 1.0f)
    {
        sf = 1;
    }
    else
    {
        sf = renderTex.SampleCmpLevelZero(samShadow, input.LightSpacePos.xy, input.LightSpacePos.z);
    }
   

    float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
    float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
    float4 lightCol = float4(1.0, 0.6, 0.8, 1.0);
    
    float3 lightDir = normalize(lightPos[3].xyz - input.Pos.xyz);
    float3 normal = normalize(input.WorldNormal);
    float diff = saturate(dot(lightDir, normal));
    return materialAmb + diff * materialDiff * sf;
}



float4 ToonPS(PS_INPUT input) : SV_Target
{
// The color of the diffuse light

    float3 lightDir = normalize(lightPos[0].xyz - input.WorldPos.xyz);
    float intensity = dot(normalize(lightDir), input.WorldNormal);
    if (l == 2)
    {
        for (int i = 0; i < 4; i++)
        {
        
            lightDir = normalize(lightPos[i].xyz - input.WorldPos.xyz);
            intensity += dot(normalize(lightDir), input.WorldNormal);
        }
    }
    if (intensity < 0)
        intensity = 0;
    // Calculate what would normally be the final color, including texturing and diffuse lighting
    float4 color = float4(1.f, 0.7f, 0.3f, 0.5f);
    color.a = 1;
 
    // Discretize the intensity, based on a few cutoff points
    if (intensity > 0.95)
        return color = float4(1.0, 1, 1, 1.0) * color;
    else if (intensity > 0.5)
        return color = float4(0.7, 0.7, 0.7, 1.0) * color;
    else if (intensity > 0.05)
        return color = float4(0.35, 0.35, 0.35, 1.0) * color;
    else
        return color = float4(0.1, 0.1, 0.1, 1.0) * color;
 
    return color;
}
float4 LineColor = float4(1, 0, 0, 0);
PS_INPUT OutlineVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    float4 original = mul(mul(mul(input.Pos, World), View), Projection);
    float4 normal = mul(mul(mul(input.Norm, World), View), Projection);
    float thickness;
    if (model == 0)
    {
        thickness = 3;
    }
    else if(model>0)
    {
        thickness = 0.003;

    }
    output.Pos = original + (mul(thickness, normal));
	
    return output;
}
float4 OutlinePS(PS_INPUT input) : SV_Target
{
    return LineColor;
}


float4 transPS(PS_INPUT input) : SV_Target
{
    if (l!=2)
    {
        float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
        float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
        float4 lightCol = float4(1.0, 1, 1, 1.0);
        float3 lightDir = normalize(lightPos[0].xyz - input.WorldPos.xyz);
        float3 normal = normalize(input.WorldNormal);
        float diff = max(0.0, dot(lightDir, normal));
        input.Col += (materialAmb + diff * materialDiff) * lightCol;
    }
    else
    {
        input.Col = spotlight(input);
    }
    input.Col.a = 0.6;
    return saturate(input.Col);
}



float4 FirePS1(PS_INPUT input) : SV_Target
{
    return float4(1, 0, 0, 0);
    float3 triblend = saturate(pow(input.WorldNormal, 4));
    triblend /= max(dot(triblend, float3(1, 1, 1)), 0.0001);

    float3 axisSign = input.WorldNormal < 0 ? -1 : 1;

    float2 uvX = input.WorldPos.zy;
    float2 uvY = input.WorldPos.xz;
    float2 uvZ = input.WorldPos.xy;

    float colX = txDiffuse.Sample(samp1, uvX);
    float4 colY = txDiffuse.Sample(samp1, uvY);
    float4 colZ = txDiffuse.Sample(samp1, uvZ);
    
    float3 tnormalX = normalize(2.0 * txDiffuse2.Sample(samp1, uvX).xyz - 1.0);
    float3 tnormalY = normalize(2.0 * txDiffuse2.Sample(samp1, uvY).xyz - 1.0);
    float3 tnormalZ = normalize(2.0 * txDiffuse2.Sample(samp1, uvZ).xyz - 1.0);

    float3 tangentX = normalize(cross(input.WorldNormal, float3(0, axisSign.x, 0)));
    float3 bitangentX = normalize(cross(tangentX, input.WorldNormal)) * axisSign.x;
    float3x3 tbnX = float3x3(tangentX, bitangentX, input.WorldNormal);

    float3 tangentY = normalize(cross(input.WorldNormal, float3(0, axisSign.y, 0)));
    float3 bitangentY = normalize(cross(tangentY, input.WorldNormal)) * axisSign.y;
    float3x3 tbnY = float3x3(tangentY, bitangentY, input.WorldNormal);

    float3 tangentZ = normalize(cross(input.WorldNormal, float3(0, axisSign.z, 0)));
    float3 bitangentZ = normalize(cross(tangentZ, input.WorldNormal)) * axisSign.z;
    float3x3 tbnZ = float3x3(tangentZ, bitangentZ, input.WorldNormal);

    float3 worldNormal = normalize(
        clamp(mul(tnormalX, tbnX), -1, 1) * triblend.x +
        clamp(mul(tnormalY, tbnY), -1, 1) * triblend.y +
        clamp(mul(tnormalZ, tbnZ), -1, 1) * triblend.z
    );

    if (l != 0)
    {
    
        float3 lightDir = normalize(lightPos[3].xyz - input.WorldPos.xyz);
        float lf = dot(input.Norm, lightDir);
        return saturate(lf * (colX * triblend.x + colY * triblend.y + colZ * triblend.z));
    }
}


PS_INPUT DustVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    float3 viewLeft = View._11_21_31;
    float3 viewUp = View._12_22_32;
    float4 inPos = input.Pos + 5;
    inPos.xyz = inPos.x * viewLeft + inPos.z * viewUp;
    float lifeSpan = 0.5f;
    float t = 2.0f * fmod(time, lifeSpan);
    float fadingRate = (1 - t / lifeSpan);
    float vertParc = (float) input.vertexID / 4 - frac((float) input.vertexID / 4);
    float xDirect = 0;
    if (vertParc % 2)
    {
        xDirect = 3;
    }
    else
    {
        xDirect = -3;
    }

    float3 partPos = { 2.0f * t * xDirect * vertParc / (100 * 0.15), -10 + 0.5 * t * vertParc, 2.0 * t * vertParc / ((100 - 50) * 0.5) };
    input.Pos -= float4(partPos, 0);
    input.Pos.y -= frac(t * 0.5f) * partPos.y;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;
	//output.PosWorld = mul(input.Pos, World);
    output.Norm = mul(float4(input.Norm, 1), World).xyz;
	
    return output;
}

float lifespan = 0.5f;
float4 DustPS(PS_INPUT input) : SV_Target
{
    float t = fmod(time, 0.1);
    float2 uvX = input.WorldPos.zy;
    float2 uvY = input.WorldPos.xz;
    float2 uvZ = input.WorldPos.xy;
    float4 a = txDiffuse.Sample(samp1, uvX);
    a.a = 0.1;
    return a;
    float4 colX = txDiffuse.Sample(samp1, uvX);
    float4 colY = txDiffuse.Sample(samp1, uvY);
    float4 colZ = txDiffuse.Sample(samp1, uvZ);
    float4 output = txDiffuse.Sample(samp1, input.viewDir);
   
    float4 initCol = { 1, 1, 0, 1 };
    float4 finalCol = { 1, 0, 0, 0 };
    float fadingRate = 1 - t / 0.1;
    float4 texCol = lerp(initCol, finalCol, t);
    texCol = texCol * (1.0 - smoothstep(0.0, 0.5, length(output - 0.5)));
    return texCol;
}
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};

struct DustPixel
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};

DustPixel ParticleVertexShader(VertexInputType input)
{
    DustPixel output = (DustPixel) 0;
    // Change the position vector to be 4 units for proper matrix calculations.
    //input.position.w = 1.0f;
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
  
    // Store the particle color for the pixel shader. 
    output.color = float4(1, 1, 1, 1);
    return output;
}

float4 ParticlePixelShader(DustPixel input) : SV_TARGET
{
    return input.color;
    float4 textureColor;
    float4 finalColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = txDiffuse.Sample(samp1, input.tex);
 
    // Combine the texture color and the particle color to get the final color result.
    finalColor = textureColor * input.color;
    return finalColor;
}

//PS_INPUT HDRVS(VS_INPUT input)
//{
//    PS_INPUT ouput = (PS_INPUT) 0;
    
    
    
    
//    return ouput;
//}

float4 HDRPS(PS_INPUT input) : SV_Target
{
    float4 materialAmb = float4(0.5, 1.0, 1, 1.0);
    float4 materialDiff = float4(1, 0.2, 0.2, 1.0);
    float4 lightCol = float4(0.3, 1, 1, 1.0);
    float gamme = 2.2;
    
    
    float3 mapped = materialDiff / (materialDiff + float(1));
    mapped = pow(mapped, float(1 / gamme));
    float3 lightDir = normalize(lightPos[0].xyz - input.WorldPos.xyz);
    float3 normal = normalize(input.Norm);
    float diff = max(0.0, dot(lightDir, normal));
    mapped += (diff + materialAmb) * lightCol;
 
    return float4(mapped, 1);
}