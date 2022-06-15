Texture2D txDiffuse : register(t5);
Texture2D txDiffuse2 : register(t6);
Texture2D colorTex : register(t7);
Texture2D shaderTexture : register(t8);
Texture2D depthMapTexture : register(t9);
SamplerComparisonState samShadow : register(s1);

//Texture2D fireTex : register(t5);

//Texture2D fireBase : register(t6);
//Texture2D fireDistort : register(t7);
//Texture2D fireOpa : register(t8);
//Texture2D renderTex : register(t9);
//--------------------------------------------------------------------------------------
Texture2D fireTex : register(t0);
SamplerState fireSamp : register(s0);
Texture2D renderTex : register(t1);
Texture2D fireBase : register(t2);
Texture2D fireDistort : register(t3);
Texture2D fireOpa : register(t4);
Texture2D tx;

SamplerState samLinear; //register(s0);
SamplerState SampleTypeClamp; /* : register(s1);*/
SamplerState SampleTypeWrap; /*: register(s2);*/

//SamplerState fireSamp : register(s3);




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
    float lightChoice;
    float3 mEye;
    matrix lightViewMatrix;

    matrix lightProjectionMatrix;
    
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
    //float4 WorldPos : POSITION1;
    //float3 Tang : TANGENT;
    //float3 Binorm : BINORMAL;
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
float4 BasicPS(PS_INPUT input):SV_Target
{
    return float4(0, 0, 0, 0);
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
    
    float3 triblend = saturate(pow(input.WorldNormal, 4));
    triblend /= max(dot(triblend, float3(1, 1, 1)), 0.0001);

    float3 axisSign = input.WorldNormal < 0 ? -1 : 1;

    float2 uvX = input.WorldPos.zy;
    float2 uvY = input.WorldPos.xz;
    float2 uvZ = input.WorldPos.xy;

    float4 colX = txDiffuse.Sample(samLinear, uvX);
    float4 colY = txDiffuse.Sample(samLinear, uvY);
    float4 colZ = txDiffuse.Sample(samLinear, uvZ);
    
    float toCam = (mEye - input.WorldPos.xyz);
    float lf = dot(toCam, normalize(input.WorldNormal));
    return saturate(lf * colX * triblend.x + colY * triblend.y + colZ * triblend.z);
}

float4 BumpMap(PS_INPUT input) : SV_TARGET
{
   //return fireTex.Sample(fireSamp, input.Tex);
    float3 triblend = saturate(pow(input.WorldNormal, 4));
    triblend /= max(dot(triblend, float3(1, 1, 1)), 0.0001);

    float3 axisSign = input.WorldNormal < 0 ? -1 : 1;

    float2 uvX = input.WorldPos.zy;
    float2 uvY = input.WorldPos.xz;
    float2 uvZ = input.WorldPos.xy;

    float4 colX = txDiffuse.Sample(samLinear, uvX);
    float4 colY = txDiffuse.Sample(samLinear, uvY);
    float4 colZ = txDiffuse.Sample(samLinear, uvZ);
    
    float3 tnormalX = normalize(2.0 * txDiffuse2.Sample(samLinear, uvX).xyz - 1.0);
    float3 tnormalY = normalize(2.0 * txDiffuse2.Sample(samLinear, uvY).xyz - 1.0);
    float3 tnormalZ = normalize(2.0 * txDiffuse2.Sample(samLinear, uvZ).xyz - 1.0);


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

    //float3 lightPos1 = float3( sin(lightPos[3].x ), 10, cos(lightPos[3].x));
    float3 lightDir = normalize(lightPos[3] - input.WorldPos.xyz);
    float lf = dot(worldNormal, lightDir);

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
    output.Tex = input.Tex;
    output.Col = float4(0.5, 0.5, 0.5, 1.0);
    float4 col = float4(0, 0, 0, 0);
    //if (lightChoice == 0)
    //{
    //    float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
    //    float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
    //    float4 lightCol = float4(1.0, 0.6, 0.8, 1.0);
    //    float3 lightDir = normalize(lightPos[3].xyz - input.Pos.xyz);
    //    float3 normal = normalize(input.Norm);
    //    float diff = max(0.0, dot(lightDir, normal));
    //    output.Col += (materialAmb + diff * materialDiff) * lightCol;
    //}
    //if (lightChoice == 1)
    //{
    float inner = 0.9999;
    float outer = inner * 0.999;
    float epsilon = inner - outer;
    for (int i = 0; i < 4; i++)
    {
        float4 a = lightPos[i];
        a.y -= 100;
        float3 toPos = normalize(output.WorldPos.xyz - a.xyz);
        float3 spotDir = normalize(-a.xyz);
        float d = dot(spotDir, toPos);
        float spotIntensity = clamp((d - outer) / epsilon, 0, 1);
        col += float4(1, 1, 1, 1) * dot(input.Norm, -toPos) * spotIntensity;
    }
    output.Col = saturate(col);
    //}
   
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
    output.Col = float4(0.3, 0.3, 0.3, 1.0);
    return output;
}

float4 PhongPS(PS_INPUT input) : SV_Target
{
      
    float4 col = float4(0, 0, 0, 0);
    //if (lightChoice == 0)
    //{
    float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
    float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
    float4 lightCol = float4(1.0, 0.6, 0.8, 1.0);
    float3 lightDir = normalize(lightPos[3].xyz - input.Pos.xyz);
    float3 normal = normalize(input.Norm);
    float diff = max(0.0, dot(lightDir, normal));
    input.Col += (materialAmb + diff * materialDiff) * lightCol;
    //}
    //if (lightChoice == 1)
    //{
    //    float inner = 0.9999;
    //    float outer = inner * 0.999;
    //    float epsilon = inner - outer;
    //    for (int i = 0; i < 4; i++)
    //    {
    //        float4 a = lightPos[i];
    //        a.y += 50;
    //        float3 toPos = normalize(input.WorldPos.xyz - a.xyz);
    //        float3 spotDir = normalize(-a.xyz);
    //        float d = dot(spotDir, toPos);
    //        float spotIntensity = clamp((d - outer) / epsilon, 0, 1);
    //        col += float4(1, 1, 1, 1) * dot(input.Norm, -toPos) * spotIntensity;
    //    }
    //    input.Col = saturate(col);
    //}
  
    return input.Col;
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
    input.LightSpacePos.xyz /= input.LightSpacePos.w;
    input.LightSpacePos.x = input.LightSpacePos.x / 2 + 0.5;
    input.LightSpacePos.y = input.LightSpacePos.y / -2 + 0.5;
    
    float sf = 0;
    
    //if (input.LightSpacePos.x < -1.0f || input.LightSpacePos.x > 1.0f ||
    //    input.LightSpacePos.y < -1.0f || input.LightSpacePos.y > 1.0f ||
    //    input.LightSpacePos.z < 0.0f || input.LightSpacePos.z > 1.0f)
    //{​​​​​​​
    //    sf =  1;
    //}​​​​​​​
    //else
    //{
    //    sf = renderTex.SampleCmpLevelZero(samShadow, input.LightSpacePos.xy, input.LightSpacePos.z);
    //}
    
    float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
    float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
    float4 lightCol = float4(1.0, 0.6, 0.8, 1.0);
    
    float3 lightDir = normalize(lightPos[0].xyz- input.WorldPos.xyz);
    float3 normal = normalize(input.WorldNormal);
    float diff = saturate(dot(lightDir, normal)); 

    return materialAmb + diff * materialDiff * sf;
}



PS_INPUT ToonVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.WorldPos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Norm = normalize(input.Norm);
    output.WorldNormal = mul(input.Norm, World);
    return output;

}
float4 ToonPS(PS_INPUT input) : SV_Target
{
// The color of the diffuse light
    float4 DiffuseColor = float4(1, 1, 1, 1);
 
// The intensity of the diffuse light
    float DiffuseIntensity = 10.0;
 
    float4 l = float4(0, 10, -10, 0);
    float4 LineColor = float4(0.3, 0.2, 0, 1);
    float LineThickness = .03;
    //input.Norm = normalize(-input.Norm);
    float3 lightDir = normalize(lightPos[3].xyz - input.Pos.xyz);
    lightDir = normalize(-lightDir);
    float intensity = dot(lightDir, input.Norm);
 
    // Calculate what would normally be the final color, including texturing and diffuse lighting
    float4 color = float4(0.2f, 0.7f, 0.3f, 0.5f);
    color.a = 1;
 
    // Discretize the intensity, based on a few cutoff points
    if (intensity > 0.95)
        color = float4(1.0, 1, 1, 1.0) * color;
    else if (intensity > 0.5)
        color = float4(0.7, 0.7, 0.7, 1.0) * color;
    else if (intensity > 0.05)
        color = float4(0.35, 0.35, 0.35, 1.0) * color;
    else
        color = float4(0.1, 0.1, 0.1, 1.0) * color;
 
    return saturate(color);
}
float4 LineColor = float4(1, 1, 0, 0);
PS_INPUT OutlineVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    float4 original = mul(mul(mul(input.Pos, World), View), Projection);
    float4 normal = mul(mul(mul(input.Norm, World), View), Projection);
    
    output.Pos = original + (mul(0.01, normal));
	
    return output;
}
float4 OutlinePS(PS_INPUT input) :SV_Target
{
    return LineColor;
}

PS_INPUT tranparentVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    float4 inPos = input.Pos;
    inPos.xyz *= 100.0;
    output.viewDir = inPos.xyz;
    float4 skyPos = inPos;
    //skyPos.xyz += eyePos.xyz;
//sky position to clipping space
    output.Pos = mul(skyPos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    return output;

    
}
float4 transPS(PS_INPUT input) : SV_Target
{
    float3 viewDir = mEye.xyz - input.WorldPos;
    viewDir = reflect(-viewDir, input.Norm);
    float3 refrDir = refract(-viewDir, input.Norm, -1.0);
    float4 sky = fireTex.Sample(fireSamp, viewDir);
    float4 refrCol = fireTex.Sample(samLinear, refrDir);
    return 0.5 * sky + 0.5 * refrCol;
    
    //float4 col = sky * (1.0 - smoothstep(0.0, 0.5, length(input.Tex - 0.5)));
    //float4 col = lerp(sky, col2, col2.a);
    //return sky;
}


float4 GlowingPS(PS_INPUT input) : SV_Target
{
   //return float4(1, 0, 0, 0);
    float factor = abs(sin(time)) * 10.0;
    float pixWidth = 1.0 / 800;
    float pixHeight = 1.0 / 600;
    float4 ColAtST = float4(0.0, 0.0, 0.0, 0.0);
    float2 XY;
    int loops = 4;
    int count = 0;
    for (int i = -loops; i <= loops; i++)
    {
        for (int j = -loops; j <= loops; j++)
        {
            XY = input.Tex + float2(i * pixWidth, j * pixHeight) * factor;
            ColAtST.rgb += renderTex.Sample(fireSamp, XY);
            count++;
        }
    }
    ColAtST.rgb /= 3.5;
    ColAtST *= 1.5;
    float4 texCol = renderTex.Sample(fireSamp, input.Tex);
    if (texCol.r <= 0.2)
        //texCol = float4(10* ColAtST.xy, 0.5* ColAtST.z, 1.0);
        return ColAtST;
    else
        return float4(0.7, 0.8, 0.2, 1.0);
    
    //return float4(0.4, 0.3, 0.2, 1.0);
}
PS_INPUT FireVS1(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    output.WorldPos = mul(input.Pos, World);
    output.Norm = mul(float4(input.Norm, 1), World).xyz;
    output.WorldNormal = mul(input.Norm, World);
    return output;
}
float4 FirePS1(PS_INPUT input) : SV_Target
{
    //return float4(1, 0, 0, 0);
    float3 triblend = saturate(pow(input.WorldNormal, 4));
    triblend /= max(dot(triblend, float3(1, 1, 1)), 0.0001);

    float3 axisSign = input.WorldNormal < 0 ? -1 : 1;

    float2 uvX = input.WorldPos.zy;
    float2 uvY = input.WorldPos.xz;
    float2 uvZ = input.WorldPos.xy;

    float colX = txDiffuse.Sample(fireSamp, uvX);
    float4 colY = txDiffuse.Sample(fireSamp, uvY);
    float4 colZ = txDiffuse.Sample(fireSamp, uvZ);
    
    float3 tnormalX = normalize(2.0 * txDiffuse2.Sample(samLinear, uvX).xyz - 1.0);
    float3 tnormalY = normalize(2.0 * txDiffuse2.Sample(samLinear, uvY).xyz - 1.0);
    float3 tnormalZ = normalize(2.0 * txDiffuse2.Sample(samLinear, uvZ).xyz - 1.0);


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

    //float3 lightPos1 = float3( sin(lightPos[3].x ), 10, cos(lightPos[3].x));
    float3 lightDir = normalize(lightPos[1] - input.WorldPos.xyz);
    float lf = dot(input.WorldNormal, lightDir);

    return saturate(lf * (colX * triblend.x + colY * triblend.y + colZ * triblend.z));
}

PS_INPUT FireVS2(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    //output.Tex = input.Tex;
    output.Pos = float4(sign(input.Pos.xy), 0.0, 1.0);

    output.Tex = output.Pos.xy * 0.5 + 0.5; //texCoord: [0, 1]x[0, 1]
	
    return output;
}

float4 shape(float2 texCoor, float2 freq)
{
    float2 newTexCoord = texCoor;
    newTexCoord.x += 0.1 * texCoor.y * sin(freq.x * (texCoor.y + time));
    newTexCoord.y += 0.1 * texCoor.y * sin(freq.x * (texCoor.y + time));
    newTexCoord.y = -newTexCoord.y;
    newTexCoord += float2(0, 0.5);
    return fireOpa.Sample(fireSamp, newTexCoord);
}
float4 FirePS2(PS_INPUT input) : SV_Target
{
    return renderTex.Sample(fireSamp, input.Tex);
    float factor = 10;
    float pixWidth = 1.0 / 800;
    float pixHeight = 1.0 / 600;
    float4 ColAtST = float4(0.0, 0.0, 0.0, 0.0);
    float2 XY;
    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            XY = input.Tex + float2(i * pixWidth, j * pixHeight) * factor;
            ColAtST.rgb += renderTex.Sample(fireSamp, XY);
        }
    }
    ColAtST.rgb /= (2 * 2 + 1) * (2 * 2 + 1);
    factor = 20;
    float4 ColAtST2 = float4(0.0, 0.0, 0.0, 0.0);
    for (int i = -3; i <= 3; i++)
    {
        for (int j = -3; j <= 3; j++)
        {
            XY = input.Tex + float2(i * pixWidth, j * pixHeight * 2) * factor;
            ColAtST2.rgb += renderTex.Sample(fireSamp, XY);
        }
    }
    ColAtST2.rgb /= (3 * 3 + 1) * (3 * 3 + 1);
    float fade = pow(1.0 - input.Tex.y, 0.5);
    float4 texCol = renderTex.Sample(fireSamp, input.Tex);
    float4 fireD = fireDistort.Sample(fireSamp, input.Tex - float2(0, time));
    float4 fireB = fireBase.Sample(fireSamp, input.Tex);
    float4 fireOpe = fireOpa.Sample(fireSamp, input.Tex);
    float4 all;
    if (texCol.r + texCol.b + texCol.g < 0.01)
    {
        all = (ColAtST2 * fireD * fireB);
        if (input.Tex.y < 0.8)
        {
            all += shape(input.Tex, float2(2, 8)) * ColAtST2 * fireB * fireD * 100;
        }
        else
        {
            all /= shape(input.Tex, float2(8, 10)) * ColAtST2 * fireB;
        }
        return all;
    }
    else
        return ColAtST * float4(0.7, 0.1, 0.1, 1.0);
}


PS_INPUT Deffered(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.Pos.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.Pos = mul(input.Pos, World);
    output.WorldPos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    // Store the texture coordinates for the pixel shader.
    output.Tex = input.Tex;
    
    // Calculate the normal vector against the world matrix only.
    output.Norm = mul(input.Norm, (float3x3) World);
	
    // Normalize the normal vector.
    output.Norm = normalize(output.Norm);

    return output;
}
struct PixelOutputType
{
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
};
PS_INPUT DeferredPixelShader(PS_INPUT input) : SV_TARGET
{
    PS_INPUT output = (PS_INPUT) 0;


    // Sample the color from the texture and store it for output to the render target.
    output.Col = shaderTexture.Sample(SampleTypeWrap, input.Tex);
	
    // Store the normal for output to the render target.
    output.Norm = float4(input.Norm, 1.0f);

    return output;
}
PS_INPUT LightVertexShader(PS_INPUT input) : SV_TARGET
{
    PS_INPUT output = (PS_INPUT) 0;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.Pos.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.Pos = mul(input.Pos, World);
    output.WorldPos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    // Store the texture coordinates for the pixel shader.
    output.Tex = input.Tex;


    return output;
}
Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypePoint : register(s0);


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
    float3 lightDirection;
    float padding;
};
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 colors;
    float4 normals;
    float3 lightDir;
    float lightIntensity;
    float4 outputColor;
 

    // Sample the colors from the color render texture using the point sampler at this texture coordinate location.
    colors = colorTexture.Sample(SampleTypePoint, input.tex);

    // Sample the normals from the normal render texture using the point sampler at this texture coordinate location.
    normals = normalTexture.Sample(SampleTypePoint, input.tex);
    
    // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(normals.xyz, lightDir));

    // Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity.
    outputColor = saturate(colors * lightIntensity);

    return outputColor;
}


PS_INPUT DustVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    float3 viewLeft = View._11_21_31;
    float3 viewUp = View._12_22_32;
    float4 inPos = input.Pos+5;
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
  

    float3 partPos = { 2.0f * t * xDirect * vertParc / (100 * 0.15), -10+0.5 * t * vertParc, 2.0 * t * vertParc / ((100 - 50) * 0.5) };
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
   float4 a = txDiffuse.Sample(fireSamp, uvX);
    a.a = 0.1;
    return a;
    float4 colX = txDiffuse.Sample(samLinear, uvX);
    float4 colY = txDiffuse.Sample(samLinear, uvY);
    float4 colZ = txDiffuse.Sample(samLinear, uvZ);
    float4 output = txDiffuse.Sample(samLinear, input.viewDir);
   
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
    DustPixel output = (DustPixel)0;
    

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
  
    // Store the particle color for the pixel shader. 
    output.color = input.color;

    return output;
}

float4 ParticlePixelShader(DustPixel input) : SV_TARGET
{
    //return float4(0, 0, 1, 0);
    float4 textureColor;
    float4 finalColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = txDiffuse.Sample(fireSamp, input.tex);
 
    // Combine the texture color and the particle color to get the final color result.
    finalColor = textureColor * input.color;
    finalColor.a *= 0.1;
    return finalColor;
}

