#version 460 core

struct Ray {
	vec3 origin;
	vec3 direction;
	vec3 invDirection;
};

struct AABB {
	vec3 min;
	vec3 max;
};

struct Triangle {
	vec3 p0;
	int material;
	vec3 p1;
	float area;
	vec3 p2;
	float d;
	//vec3 edge0, edge1, edge2;
	//vec3 normal;
	//vec2 uv0, uv1, uv2;
	//float area;
	//float d;
	//float samplePDF;
};

struct Intersection {
	vec3 position;
	bool intersected;
	vec3 normal;
	float distance;
	int material;
};

struct Material {
	vec3 albedo;
	vec3 emission;
};

const int MaxRayBounces = 128;
const float Infinity = 1.0f / 0.0f;
const float fovy = 0.6806784f;
const float Pi = 3.14159265359;

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;

layout(std430, binding = 1) buffer trianglesBuffer {
    Triangle triangles[];
};

layout(std430, binding = 2) buffer materialsBuffer {
    Material materials[];
};

uniform mat4 uViewMatrix;
uniform int uNumTriangles;
uniform uint uTime;
uniform int uReset;

uint state;

void InitState(uint seed) {
    state = seed * 747796405 + 2891336453;
}

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

float NextRandomValue() {
    const uint mantissaMask = 0x007FFFFFu;
    const uint one          = 0x3F800000u;
   
    state = hash(state);
    state &= mantissaMask;
    state |= one;
    
    float r2 = uintBitsToFloat(state);
    return r2 - 1.0;
}

float RandomFloat(float min, float max) {
	return min + (max - min) * NextRandomValue();
}

vec3 RandomSphereDirection() {
	vec3 result = normalize(vec3(RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f)));
	return result;
}

vec3 RandomCosineDirection() {
	float r1 = NextRandomValue();
	float r2 = NextRandomValue();
	float z = sqrt(1 - r2);
	float phi = 2 * Pi * r1;
	float x = cos(phi) * sqrt(r2);
	float y = sin(phi) * sqrt(r2);
	return vec3(x, y, z);
}

float AABBIntersectionDistance(Ray ray, AABB box) {
	vec3 tMin = (box.min - ray.origin) * ray.invDirection;
	vec3 tMax = (box.max - ray.origin) * ray.invDirection;
	vec3 t0 = min(tMin, tMax);
	vec3 t1 = max(tMin, tMax);
	float near = max(max(t0.x, t0.y), t0.z);
	float far = min(min(t1.x, t1.y), t1.z);
	bool hit = far >= near && far > 0;
	return hit ? near : Infinity;
}

Intersection IntersectTriangle(Ray ray, Triangle triangle) {
	//vec3 edgeAB = triangle.p1 - triangle.p0;
	//vec3 edgeAC = triangle.p2 - triangle.p0;
	//vec3 n = cross(edgeAB, edgeAC);
	//vec3 ao = ray.origin - triangle.p0;
	//vec3 dao = cross(ao, ray.direction);
	//
	//float determinant = -dot(ray.direction, n);
	//float invDet = 1 / determinant;
	//
	//float dst = dot(ao, n) * invDet;	
	//float u = dot(edgeAC, dao) * invDet;
	//float v = -dot(edgeAB, dao) * invDet;
	//float w = 1 - u - v;
	//
	//Intersection intersection;
	//intersection.intersected = false;
	//intersection.intersected = determinant != 0.0f && dst >= 0 && u >= 0 && v >= 0 && w >= 0;
	//intersection.normal = normalize(n);
	//intersection.position = ray.origin + ray.direction * dst;
	//intersection.distance = dst;
	//intersection.material = triangle.material;
	//
	//return intersection;

	const float epsilon = 0.0001f;
	
	Intersection intersection;
	intersection.intersected = false;
	
	vec3 edge1 = triangle.p1 - triangle.p0;
	vec3 edge2 = triangle.p2 - triangle.p0;
	vec3 ray_cross_e2 = cross(ray.direction, edge2);
	float det = dot(edge1, ray_cross_e2);
	
	if (det > -epsilon && det < epsilon) {
		return intersection;
	}
	
	float inv_det = 1.0 / det;
	vec3 s = ray.origin - triangle.p0;
	float u = inv_det * dot(s, ray_cross_e2);
	
	if (u < 0 || u > 1) {
        return intersection;
	}
	
	vec3 s_cross_e1 = cross(s, edge1);
	float v = inv_det * dot(ray.direction, s_cross_e1);	
	
	if (v < 0 || u + v > 1) {
        return intersection;
	}
	
	float t = inv_det * dot(edge2, s_cross_e1);
	
	if (t < epsilon) {
		return intersection;
	}
	
	intersection.intersected = true;
	intersection.distance = t;
	intersection.position = ray.origin + ray.direction * t;
	intersection.normal = normalize(cross(edge1, edge2));
	intersection.material = triangle.material;
	
	return intersection;
}

Intersection Intersect(Ray ray) {
	Intersection intersection;
	intersection.intersected = false;
	float maxDistance = Infinity;
	for (int i = 0; i < uNumTriangles; i++) {
		Triangle triangle = triangles[i];
		Intersection triangleIntersection = IntersectTriangle(ray, triangle);
		if (triangleIntersection.intersected && triangleIntersection.distance < maxDistance) {
			intersection = triangleIntersection;
			maxDistance = intersection.distance;
		}
	}
	return intersection;
}

vec3 Trace(Ray originalRay) {
	vec3 L = vec3(0.0f, 0.0f, 0.0f);
	vec3 beta = vec3(1.0f, 1.0f, 1.0f);

	Ray ray = originalRay;
	for (int depth = 0; ; depth++) {
		Intersection intersection = Intersect(ray);	
		if (!intersection.intersected) {
			break;
		}
		
		L += materials[intersection.material].emission * beta;

		if (depth >= MaxRayBounces) {
			break;
		}

		vec3 reflectedDirection = RandomSphereDirection();

		beta *= (materials[intersection.material].albedo / Pi) * abs(dot(reflectedDirection, intersection.normal)) / (1.0f / (Pi * 4));

		if(beta == vec3(0.0f)) {
			break;
		}

		ray.origin = intersection.position;
		ray.direction = reflectedDirection;
		ray.invDirection = 1.0f / ray.direction;
	}

	return L;
}

Ray GetRay(vec2 uv, float aspect) {
	float theta = mix(-(fovy * aspect) / 2, (fovy * aspect) / 2, uv.x);
	float phi = mix(-fovy / 2, fovy / 2, uv.y);
	Ray ray;
	ray.direction = normalize(mat3(uViewMatrix) * vec3(tan(theta), tan(phi), -1.0f));
	ray.invDirection = 1.0f / ray.direction;
	vec4 origin = uViewMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
	ray.origin = origin.xyz / origin.w;
	return ray;
}

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = imageSize(screen);
	vec2 uv = vec2(pixel_coords) / dims;
	float aspect = float(dims.x) / dims.y;

	InitState((gl_GlobalInvocationID.y * dims.x + gl_GlobalInvocationID.x) * uTime);

	vec4 sumPixel = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	if (!bool(uReset)) {
		sumPixel = imageLoad(screen, pixel_coords);
	}
	for (int i = 0; i < 1; i++) {
		Ray ray = GetRay(uv, aspect);
		sumPixel += vec4(Trace(ray), 1.0);
	}
	imageStore(screen, pixel_coords, sumPixel);
}