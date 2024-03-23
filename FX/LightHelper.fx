struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float pda;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Position;
    float Range;
    
    float3 Att;
    float pad;
};

struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;
    
    float3 Direction;
    float Spot;
    
    float3 Att;
    float pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Reflect;
};

void ComputeDirectionalLight(Material mat, DirectionalLight L, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec)
{
    // 출력 성분들을 초기화
    ambient = 0.0f;
    diffuse = 0.0f;
    spec = 0.0f;

    // 빛 벡터는 광선이 나아가는 방향의 반대 방향이다.
    float3 lightVec = -L.Direction;

    // 주변광 항을 더한다.
    ambient = mat.Ambient * L.Ambient;

    // 빛이 막히지 않고 표면에 도달한다는 가정 하에서
    // 분산광 항과 반영광 항을 더한다.
    float diffuseFactor = dot(lightVec, normal);

    // 동적 분기를 피하기 위해 조건문을 펼친다.
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }
}

void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec)
{
    // 출력 성분들을 초기화한다.
    ambient = 0.0f;
    diffuse = 0.0f;
    spec = 0.0f;
    
    // 빛 벡터(표면 점에서 광원으로의 벡터)
    float3 lightVec = L.Position - pos;
    
    // 표면 점과 광원 사이의 거리
    float d = length(lightVec);
    
    // 범위 판정
    if (d > L.Range)
        return;
    
    // 빛 벡터를 정규화한다.
    lightVec /= d;
    
    // 주변광 항
    ambient = mat.Ambient * L.Ambient;
    
    // 빛이 막히지 않고 표면에 도달한다는 가정 하에서
    // 분산광 항과 반영광 항을 더한다.
    float diffuseFactor = dot(lightVec, normal);
    
    // 동적 분기를 피하기 위해 조건문을 펼친다.
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }

    // 감쇠
    float att = 1.0f / dot(L.Att, float3(1.0f, d, d * d));
    
    diffuse *= att;
    spec *= att;
}

void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec)
{
    // 출력 성분들을 초기화한다.
    ambient = 0.0f;
    diffuse = 0.0f;
    spec = 0.0f;
    
    // 빛 벡터 (표면 점에서 광원으로의 벡터)
    float3 lightVec = L.Position - pos;
    
    // 표면 점과 광원 사이의 거리
    float d = length(lightVec);
    
    // 범위 판정
    if (d > L.Range)
        return;
    
    // 빛 벡터를 정규화한다.
    lightVec /= d;
    
    // 주변광 항
    ambient = mat.Ambient * L.Ambient;
    
    // 빛이 막히지 않고 표면에 도달한다는 가정 하에서
    // 분산광 항과 반영광 항을 더한다.
    float diffuseFactor = dot(lightVec, normal);
    
    // 동적 분기를 피하기 위해 조건문을 펼친다.
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }

    // 점적광 계수를 계산한다.
    float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);
    
    // 빛의 세기를 점적광 계수로 비례하고 거리에 따라 감쇠시킨다.
    float att = spot / dot(L.Att, float3(1.0f, d, d * d));

    ambient *= spot;
    diffuse *= att;
    spec *= att;
}
