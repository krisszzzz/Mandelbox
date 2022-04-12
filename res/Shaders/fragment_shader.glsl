
#version 460 core
// This is a stripped down version of a shader I had used on my 2017 page:
// https://www.wothke.ch/modum
//
// The code is rather verbose and it might be useful to play around with
// here on shadertoy.
//
// It is heavily based on the logic from an old version of Fractal Lab 
// (see http://www.subblue.com). Copyright 2011, Tom Beddard
// 

//precision highp float;

#define HALFPI 1.570796
#define MIN_EPSILON 6e-7
#define MIN_NORM 1.5e-7

#define maxIterations 15                              // 1 to 30
#define stepLimit 105                                 // 10 to 300
#define aoIterations 3                                // 0 to 10

#define minRange 6e-5

out     vec4 fragColor;
uniform float x_pos;
uniform float y_pos;
uniform float z_pos;
uniform vec2  iResolution;
uniform vec2  iMouse;
uniform float scale;
			
// fractal params			
const float deFactor= 0.89;                             // 0 to 1
//const float scale= -2.81;                             // -10 to 10
const float surfaceDetail= 0.66;                      // 0.1 to 2
const float surfaceSmoothness= 0.79;                  // 0.01 to 1
const float boundingRadius= 114.02;                   // 0.1 to 150
const vec3 offset= vec3(0., 0., 0.);                  // -3 to 3

const float sphereScale= 1.;                          // 0.01 to 3
float boxScale= 0.5;                                  // 0.01 to 3
const float boxFold= 1.;                              // 0.01 to 3
const float fudgeFactor= 0.;                          // 0 to 100
const float gamma= 0.799;                             // gamma correction; 0.1 to 2

// lights
const vec3  light= vec3(48, 191, -198);               // light position
const vec2  ambientColor= vec2(0.41, 0);              // ambient intensity, ambient c


// ambient occlusion
const float aoIntensity= 0.21;                        // 0 to 1
const float aoSpread= 11.79;                          // 0 to 20

// camera
const float cameraFocalLength= 0.9;                   // 0.1 to 3
float fovfactor = 0.0;                                      // field of view
vec3 cameraPosition = vec3(0.0);
mat3 cameraRotation = mat3(0.0);

float aspectRatio   = 0.0;
float pixelScale    = 0.0;
float epsfactor     = 0.0;

vec2  size          = vec2(0.0);
vec2  halfSize      = vec2(0.0);

vec3  w = vec3(0, 0, 1);
vec3  v = vec3(0, 1, 0);
vec3  u = vec3(1, 0, 0);


// Pre-calculations
float mR2 = 0.0;                                           // Min radius
float fR2 = 0.0;                                           // Fixed radius
vec2  scaleFactor = vec2(0.0);
						

// Details about the Mandelbox DE algorithm:
// http://www.fractalforums.com/3d-fractal-generation/a-mandelbox-distance-estimate-formula/
float Mandelbox(vec3 w)
{

    float md = 1000.0;
    vec3 c = w;
				
	// distance estimate
    vec4 p = vec4(w.xyz, deFactor),
        p0 = vec4(w.xyz, 1.0);  // p.w is knighty's DEfactor
				
    for (int i = 0; i < int(maxIterations); i++) {
		// box fold:
        p.xyz = clamp(p.xyz, -boxFold, boxFold) * 2.0 * boxFold - p.xyz;  // box fold
					
		// sphere fold:
        float d = dot(p.xyz, p.xyz);
        p.xyzw *= clamp(max(fR2 / d, mR2), 0.0, 1.0);  // sphere fold
					
        p.xyzw = p * scaleFactor.xxxy + p0 + vec4(offset, 0.0);

        
    }			
    return (length(p.xyz) - fudgeFactor) / p.w;
}


// Define the ray direction from the pixel coordinates
vec3 rayDirection(vec2 pixel)
{
    vec2 p = (0.5 * halfSize - pixel) / vec2(halfSize.x, -halfSize.y);
    p.x   *= aspectRatio;
    vec3 d = (p.x * u + p.y * v - cameraFocalLength * w);
    return normalize(cameraRotation * d);
}


// Intersect bounding sphere
//
// If we intersect then set the tmin and tmax values to set the start and
// end distances the ray should traverse.
bool intersectBoundingSphere(vec3 origin,
                             vec3 direction,
                             out float tmin,
                             out float tmax)
{
    bool hit = false;
    float b = dot(origin, direction);
    float c = dot(origin, origin) - boundingRadius;
    float disc = b*b - c;           // discriminant
    tmin = tmax = 0.0;

    if (disc > 0.0) {
		// Real root of disc, so intersection
        float sdisc = sqrt(disc);
        float t0 = -b - sdisc;          // closest intersection distance
        float t1 = -b + sdisc;          // furthest intersection distance

        if (t0 >= 0.0) {
			// Ray intersects front of sphere
            tmin = t0;
            tmax = t0 + t1;
        } else {
			// Ray starts inside sphere
            tmax = t1;
        }
        
        hit = true;
    }

    return hit;
}

// Calculate the gradient in each dimension from the intersection point
vec3 generateNormal(vec3 z, float d)
{
    float e = max(d * 0.5, MIN_NORM);
				
    float dx1 = Mandelbox(z + vec3(e, 0, 0));
    float dx2 = Mandelbox(z - vec3(e, 0, 0));
				
    float dy1 = Mandelbox(z + vec3(0, e, 0));
    float dy2 = Mandelbox(z - vec3(0, e, 0));
				
    float dz1 = Mandelbox(z + vec3(0, 0, e));
    float dz2 = Mandelbox(z - vec3(0, 0, e));
				
    return normalize(vec3(dx1 - dx2, dy1 - dy2, dz1 - dz2));
}

// Ambient occlusion approximation.
// Based upon boxplorer's implementation which is derived from:
// http://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf
float ambientOcclusion(vec3 p, vec3 n, float eps)
{
    float o = 1.0;                  // Start at full output colour intensity
    eps *= aoSpread;                // Spread diffuses the effect
    float k = aoIntensity / eps;    // Set intensity factor
    float d = 2.0 * eps;            // Start ray a little off the surface
				
    for (int i = 0; i < aoIterations; ++i) {
        o -= (d - Mandelbox(p + n * d)) * k;
        d += eps;
        k *= 0.5;                   // AO contribution drops as we move further from the surface 
    }
				
    return clamp(o, 0.0, 1.0);
}

// Calculate the output color for each input pixel
vec4 render(vec2 pixel)
{
    vec3  ray_direction = rayDirection(pixel);
	
    float ray_length = minRange;
    vec3  ray        = cameraPosition + ray_length * ray_direction;
    vec4  bg_color   = vec4(0.6, 0.6, .6, 1.);
    vec4  color      = bg_color;
				
    float dist   = 0.0;
    float eps    = MIN_EPSILON;
    vec3  normal = vec3(0);
    int   steps  = 0;
    
    bool  hit    = false;
    float tmin   = 0.0;	// 'out' params of intersectBoundingSphere()
    float tmax   = 0.0;
			
    if (intersectBoundingSphere(ray, ray_direction, tmin, tmax)) {
        ray_length = tmin;
        ray = cameraPosition + ray_length * ray_direction;
			
        for (int i = 0; i < stepLimit; i++) {            
            dist  = Mandelbox(ray);
            dist *= surfaceSmoothness;
            
			// If we hit the surface on the previous step check again to make sure it wasn't
			// just a thin filament
            if (hit && dist < eps || ray_length > tmax) {	// XXX ray_length < tmin impossible!
                steps--;
                break;
            }
						
            hit = false;
            ray_length += dist;	// XXX dist.x is always positive...
            ray = cameraPosition + ray_length * ray_direction;
            eps = ray_length * epsfactor;

			// add-on effect hack: "distant" stuff turns into water... 			
            if (dist < eps || ray_length < tmin) {
				hit = true;	// normal mode				
			}
    	}
    }
			
				
    if (hit) {
        float aof = 1.0;      
        normal = generateNormal(ray, eps);
        aof = ambientOcclusion(ray, normal, eps);  				                
        color.rgb *= aof;
       		
    } 
    
    return color; 
}

// Return rotation matrix for rotating around vector v by angle
mat3 rotationMatrixVector(vec3 v, float angle)
{
    float c = cos(radians(angle));
    float s = sin(radians(angle));
				
    return mat3(c + (1.0 - c) * v.x * v.x, (1.0 - c) * v.x * v.y - s * v.z, (1.0 - c) * v.x * v.z + s * v.y,
               (1.0 - c) * v.x * v.y + s * v.z, c + (1.0 - c) * v.y * v.y, (1.0 - c) * v.y * v.z - s * v.x,
               (1.0 - c) * v.x * v.z - s * v.y, (1.0 - c) * v.y * v.z + s * v.x, c + (1.0 - c) * v.z * v.z);
}


void main() 
{
	// size of generated texture   
	size = iResolution.xy;
	halfSize= size/2.0;
	   
	// setup camera
    vec3 dir= vec3(20. + 0.25*iMouse.y, 20. + 0.25*iMouse.x, -22.); 	// 180= full circle
    cameraPosition= vec3( x_pos, 
						  y_pos, 
						  z_pos); //sin(iTime*.05)*.2);	
                            
		
    cameraRotation= 	rotationMatrixVector(u, dir.x) *
          				rotationMatrixVector(v, dir.y) * 
        				rotationMatrixVector(w, dir.z);
	
    fovfactor = 1.0 / sqrt(1.0 + cameraFocalLength * cameraFocalLength);
	aspectRatio = size.x / size.y;
	pixelScale = 1.0 / min(size.x, size.y);
	epsfactor = 2.0 * fovfactor * pixelScale * surfaceDetail;

    
		// update derived fractal
	mR2 = boxScale * boxScale;
	fR2 = sphereScale * mR2;
	scaleFactor = vec2(scale, abs(scale)) / mR2;

						
    vec4 color = render(vec2(gl_FragCoord.x, gl_FragCoord.y));	// do without antialiasing to limit GPU load				
	fragColor = vec4(pow(abs(color.rgb), vec3(1.0 / gamma)), color.a);
}
