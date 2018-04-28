#include "landscape_generator.h"

int perm[512] = {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180, 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};

float dot(float x, float y, float z, float* g){
    return x*g[0] + y*g[1] + z*g[2];
}

static const int grad3[12][3] = {
    {1,1,0}, {-1,1,0}, {1,-1,0}, {-1,-1,0},
    {1,0,1}, {-1,0,1}, {1,0,-1}, {-1,0,-1},
    {0,1,1}, {0,-1,1}, {0,1,-1}, {0,-1,-1}
};
int fastfloor( const float x ) { return x > 0 ? (int) x : (int) x - 1; }
float dot( const int* g, const float x, const float y ) { return g[0]*x + g[1]*y; }
float raw_noise_2d( const float x, const float y ) {
    // Noise contributions from the three corners
    float n0, n1, n2;

    // Skew the input space to determine which simplex cell we're in
    float F2 = 0.5 * (sqrtf(3.0) - 1.0);
    // Hairy factor for 2D
    float s = (x + y) * F2;
    int i = fastfloor( x + s );
    int j = fastfloor( y + s );

    float G2 = (3.0 - sqrtf(3.0)) / 6.0;
    float t = (i + j) * G2;
    // Unskew the cell origin back to (x,y) space
    float X0 = i-t;
    float Y0 = j-t;
    // The x,y distances from the cell origin
    float x0 = x-X0;
    float y0 = y-Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if(x0>y0) {i1=1; j1=0;} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    else {i1=0; j1=1;} // upper triangle, YX order: (0,0)->(0,1)->(1,1)

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6
    float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
    float y2 = y0 - 1.0 + 2.0 * G2;

    // Work out the hashed gradient indices of the three simplex corners
    int ii = i & 255;
    int jj = j & 255;
    int gi0 = perm[ii+perm[jj]] % 12;
    int gi1 = perm[ii+i1+perm[jj+j1]] % 12;
    int gi2 = perm[ii+1+perm[jj+1]] % 12;

    // Calculate the contribution from the three corners
    float t0 = 0.5 - x0*x0-y0*y0;
    if(t0<0) n0 = 0.0;
    else {
        t0 *= t0;
        n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
    }

    float t1 = 0.5 - x1*x1-y1*y1;
    if(t1<0) n1 = 0.0;
    else {
        t1 *= t1;
        n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
    }

    float t2 = 0.5 - x2*x2-y2*y2;
    if(t2<0) n2 = 0.0;
    else {
        t2 *= t2;
        n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 70.0 * (n0 + n1 + n2);
}

float scaled_raw_noise_2d( const float loBound, const float hiBound, const float x, const float y ) {
    return raw_noise_2d(x, y) * (hiBound - loBound) / 2 + (hiBound + loBound) / 2;
}

void Landscape_Generator( Chunk* c_chunk, block_list* b_list) {
    int i;
    int cx, cy, cz; // for schleife
    int posx, posy, posz; // ausgerechnete

    float Island_high = 10.0f;
    float interval_range = 150.f;
    float interval_range_form = 15.f;
    float border = 10;

    // List
    int l_stone = b_list->getByName( "stone")->getID();
    int l_earth = b_list->getByName( "earth")->getID();
    int l_grass = b_list->getByName( "grass")->getID();

    for (int cz = 0; cz < CHUNK_SIZE; cz++) {
        for (int cx = 0; cx < CHUNK_SIZE; cx++) {
            // Position errechen
            posx = cx + ( c_chunk->getPos().x*CHUNK_SIZE);
            posz = cz + ( c_chunk->getPos().z*CHUNK_SIZE);
            // Noise generieren
            float high = scaled_raw_noise_2d( 0.5, 0.25, (float)posx/interval_range, (float)posz/interval_range);
            high += scaled_raw_noise_2d( 0, -0.02, (float)posx/interval_range_form, (float)posz/interval_range_form);
            int t = scaled_raw_noise_2d( 0, 15, (float)posx/interval_range_form, (float)posz/interval_range_form);

            // Höhe errechnen
            for(int cy = 0; cy < CHUNK_SIZE; cy++) {
                i++;
                // Position errechen
                posy = cy + (c_chunk->getPos().y*CHUNK_SIZE);
                int type = EMPTY_BLOCK_ID;
                //if( interval*Island_high > posy+high*Island_high )
                if( posy < Island_high*high + border) {
                    type = l_stone;
                    if( posy+3 > Island_high*high+ border)
                        type =  l_earth;
                    if( posy+1 > Island_high*high+ border)
                        type = l_grass;
                }
                if( type == l_grass )
                    c_chunk->setSunlight( cx, cy, cz, t%15);
                c_chunk->set( cx, cy, cz, type, false);
            }
        }
    }
}
