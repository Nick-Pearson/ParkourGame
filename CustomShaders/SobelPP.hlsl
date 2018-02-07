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
int inputIdx = 24;

// Sobel Kernels
static const float kdy[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
static const float kdx[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};

float4 dy = float4(0,0,0,0);
float4 dx = float4(0,0,0,0);

for(int x = -1; x <=1; ++x)
{
  for(int y = -1; y <=1; ++y)
  {
    float4 val = SceneTextureLookup(UV + float2(x * InvSize.x, y * InvSize.y), inputIdx, false);
    int kidx = (x+1) + (3*(y+1));

    dy += kdy[kidx] * val;
    dx += kdx[kidx] * val;
  }
}

return sqrt(dy*dy + dx*dx);
//return SceneTextureLookup(UV, inputIdx, false);
