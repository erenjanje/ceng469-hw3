#version 460 core

in vec4 vertex_position;
out vec4 frag_color;

uniform samplerCube cubemap;

float smoother(float t) {
    return ( t * t * t * ( t * ( t * 6.0f - 15.0f ) + 10.0f ) );
}

int perm[] = {151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
  151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

uniform struct CameraInformation {
    vec3 position;
    vec3 direction;
    vec3 up;
} camera;

float grad(int hash, vec3 vec) {
    int h = hash & 15;
    float u = (h < 8) ? vec.x : vec.y;
    float v = (h < 4) ? vec.y : ((h == 12 || h == 14) ? vec.x : vec.z);
    return (((h & 1) == 1) ? -u : u)
        + (((h & 2) == 2) ? -v : v);
}

#define LERP(t, x, y) mix(x, y, t)
#define FADE(t) smoother(t)
#define FASTFLOOR(x) floor(x)

#define PERLIN_MULTIPLIER 3.0f
#define CLOUD_MAX 10.0f
#define CLOUD_MIN 0.0f
#define MAX_CLOUD_Y CLOUD_MAX*PERLIN_MULTIPLIER
#define MIN_CLOUD_Y CLOUD_MIN*PERLIN_MULTIPLIER

float perlin(vec3 v) {
    float x = v.x;
    float y = v.y;
    float z = v.z;

    int ix0, iy0, ix1, iy1, iz0, iz1;
    float fx0, fy0, fz0, fx1, fy1, fz1;
    float s, t, r;
    float nxy0, nxy1, nx0, nx1, n0, n1;

    ix0 = int(FASTFLOOR( x )); // Integer part of x
    iy0 = int(FASTFLOOR( y )); // Integer part of y
    iz0 = int(FASTFLOOR( z )); // Integer part of z
    fx0 = x - ix0;        // Fractional part of x
    fy0 = y - iy0;        // Fractional part of y
    fz0 = z - iz0;        // Fractional part of z
    fx1 = fx0 - 1.0f;
    fy1 = fy0 - 1.0f;
    fz1 = fz0 - 1.0f;
    ix1 = ( ix0 + 1 ) & 0xff; // Wrap to 0..255
    iy1 = ( iy0 + 1 ) & 0xff;
    iz1 = ( iz0 + 1 ) & 0xff;
    ix0 = ix0 & 0xff;
    iy0 = iy0 & 0xff;
    iz0 = iz0 & 0xff;
    
    r = FADE( fz0 );
    t = FADE( fy0 );
    s = FADE( fx0 );

    nxy0 = grad(perm[ix0 + perm[iy0 + perm[iz0]]], vec3(fx0, fy0, fz0));
    nxy1 = grad(perm[ix0 + perm[iy0 + perm[iz1]]], vec3(fx0, fy0, fz1));
    nx0 = LERP( r, nxy0, nxy1 );

    nxy0 = grad(perm[ix0 + perm[iy1 + perm[iz0]]], vec3(fx0, fy1, fz0));
    nxy1 = grad(perm[ix0 + perm[iy1 + perm[iz1]]], vec3(fx0, fy1, fz1));
    nx1 = LERP( r, nxy0, nxy1 );

    n0 = LERP( t, nx0, nx1 );

    nxy0 = grad(perm[ix1 + perm[iy0 + perm[iz0]]], vec3(fx1, fy0, fz0));
    nxy1 = grad(perm[ix1 + perm[iy0 + perm[iz1]]], vec3(fx1, fy0, fz1));
    nx0 = LERP( r, nxy0, nxy1 );

    nxy0 = grad(perm[ix1 + perm[iy1 + perm[iz0]]], vec3(fx1, fy1, fz0));
    nxy1 = grad(perm[ix1 + perm[iy1 + perm[iz1]]], vec3(fx1, fy1, fz1));
    nx1 = LERP( r, nxy0, nxy1 );

    n1 = LERP( t, nx0, nx1 );
    
    float ret = 0.936f * ( LERP( s, n0, n1 ) );

    if(v.y >= MAX_CLOUD_Y+1.0f) {
        ret *= 1.0f/(v.y-MAX_CLOUD_Y);
    } else if(v.y <= MIN_CLOUD_Y-1.0f) {
        ret *= 1.0f/(MIN_CLOUD_Y-v.y);
    }
    return ret;
}

vec3 random3(vec3 v) {
    return fract(sin(vec3(dot(v, vec3(127.1f, 311.7f, 296.2f)), dot(v, vec3(269.5f, 183.3f, 561.4f)), dot(v, vec3(742.4f, 253.7f, 510.3f)))) * 43758.5453f);
}

float worley(vec3 v) {
    vec3 i_st = floor(v);
    vec3 f_st = fract(v);
    float min_dist = 1.0f;

    for(int z = -1; z <= 1; z++) {
        for(int y = -1; y <= 1; y++) {
            for(int x = -1; x <= 1; x++) {
                vec3 neighbor = vec3(float(x), float(y), float(z));
                vec3 point = random3(i_st + neighbor);
                vec3 diff = neighbor + point - f_st;
                float dist = length(diff);
                min_dist = min(min_dist, dist);
            }
        }
    }
    float ret = min_dist;
    if(v.y >= MAX_CLOUD_Y+1.0f) {
        ret *= 1.0f/(v.y-MAX_CLOUD_Y);
    } else if(v.y <= MIN_CLOUD_Y-1.0f) {
        ret *= 1.0f/(MIN_CLOUD_Y-v.y);
    }
    return ret;
}

#define MOON_DIRECTION vec3(-1.0f, 0.1875f, 0.0f)

#define STEP_COUNT 3
#define SCATTER_STEP_COUNT 10
#define CLOUD_COLOR vec3(0.9f, 0.9f, 0.9f)
#define MOONLIGHT vec3(0.309803922f, 0.411764706f, 0.533333333f)

#define WORLEY_ENABLED 0.0f

float scatter_march_ray(vec3 position, vec3 direction, float t) {
    vec3 point = (position + direction*t)*PERLIN_MULTIPLIER;
    return perlin(point)*(1.0f - WORLEY_ENABLED*worley(point));
}

vec3 march_ray(vec3 position, vec3 direction, float t) {
    float moonlight = 0.0f;
    vec3 point = (position + direction*t)*PERLIN_MULTIPLIER;
    for(int i = 0; i < SCATTER_STEP_COUNT; i++) {
        moonlight += scatter_march_ray(point, MOON_DIRECTION, 1.0f/float(SCATTER_STEP_COUNT) * float(i)) / float(SCATTER_STEP_COUNT);
    }
    float cloud_presence = perlin(point)*(1.0f - WORLEY_ENABLED*worley(point));
    return (CLOUD_COLOR + (1.0f - moonlight)*MOONLIGHT)*cloud_presence;
}

void raymarch(inout vec3 color, vec3 position, vec3 direction) {
    for(int i = 0; i < STEP_COUNT; i++) {
        color += march_ray(position, direction*STEP_COUNT/2.0f, 1.0f/float(STEP_COUNT) * float(i))/float(STEP_COUNT);
    }
}

const vec3 cloud_color = vec3(1.0f, 1.0f, 1.0f);

void main() {
    vec3 ray = vertex_position.xyz;
    vec3 abs_ray = abs(ray);
    vec3 uvr = ray / max(abs_ray.x, max(abs_ray.y, abs_ray.z));
    vec3 color = texture(cubemap, vec3(-uvr.x, uvr.yz)).rgb;
    raymarch(color, camera.position, normalize(ray));
    frag_color = vec4(color, 1.0f);
}
