// Sobel Filter Code for Post Process Materials
//  Inputs:
//        UV - Base texture coords
//        InvSize - Inverse size of the scene texture (output by the scene texture node)
//  Output:
//        Soble edge magnitude

#define NEEDS_SCENE_TEXTURES 1

// Post Process input Index
// 14 - PostProcessInput0
// ...
// 20 - Post ProcessInput6
// 24 - Custom Stencil
int SceneDepth = 1;
int CustomDepth = 13;
int CustomStencil = 24;

static const float kdy[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
static const float kdx[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};

float4 dy = float4(0,0,0,0);
float4 dx = float4(0,0,0,0);

for(int x = -1; x <=1; ++x)
{
  for(int y = -1; y <=1; ++y)
  {
    byte val = SceneTextureLookup(UV + float2(x * InvSize.x, y * InvSize.y), CustomStencil, false);
    int kidx = (x+1) + (3*(y+1));

    dy += kdy[kidx] * val;
    dx += kdx[kidx] * val;
  }
}

float4 sobelVal = sqrt(dy*dy + dx*dx);
return sobelVal * (SceneTextureLookup(UV, SceneDepth, false).x < SceneTextureLookup(UV, CustomDepth, false).x ? float4(1,1,1,1) : float4(0,0,0,1));

#define GetStencilValue(inUV) SceneTextureLookup(inUV, CustomStencil, false) * (SceneTextureLookup(inUV, SceneDepth, false).x < SceneTextureLookup(inUV, CustomDepth, false).x ? float4(1,1,1,1) : float4(0,0,0,1));

float4 val = float4(0,0,0,0);

for(int x = -Dist; x <= Dist; ++x)
{
  for(int y = -Dist; y <= Dist; ++y)
  {
    if(x == 0 && y == 0) continue;
    val += GetStencilValue(UV + float2(x * InvSize.x, y * InvSize.y));
  }
}


int numValues = (1 + (2 * Dist));
numValues = numValues * 2;

return (val / numValues);
#undef GetStencilValue
