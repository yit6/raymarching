#version 330 core
layout(location = 0) out vec4 color;
in vec2 vertex;
uniform float t;
uniform float ratio;
#define M_PI 3.1415926535897932384626433832795
float smin( float a, float b, float k )
{
    a = pow( a, k ); b = pow( b, k );
    return pow( (a*b)/(a+b), 1.0/k );
}

float sdCone( in vec3 p, in vec2 c, float h )
{
  // c is the sin/cos of the angle, h is height
  // Alternatively pass q instead of (c,h),
  // which is the point at the base in 2D
  vec2 q = h*vec2(c.x/c.y,-1.0);
    
  vec2 w = vec2( length(p.xz), p.y );
  vec2 a = w - q*clamp( dot(w,q)/dot(q,q), 0.0, 1.0 );
  vec2 b = w - q*vec2( clamp( w.x/q.x, 0.0, 1.0 ), 1.0 );
  float k = sign( q.y );
  float d = min(dot( a, a ),dot(b, b));
  float s = max( k*(w.x*q.y-w.y*q.x),k*(w.y-q.y)  );
  return sqrt(d)*sign(s);
}

float sdCappedCylinderX( vec3 p, float h, float r )
{
  vec2 d = abs(vec2(length(p.yz),p.x)) - vec2(h,r);
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float sdCappedCylinderY( vec3 p, float h, float r )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - vec2(h,r);
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float sdCappedCylinderZ( vec3 p, float h, float r )
{
  vec2 d = abs(vec2(length(p.xy),p.z)) - vec2(h,r);
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float sdOctahedron( vec3 p, float s) {
  p = abs(p);
  float m = p.x+p.y+p.z-s;
  vec3 q;
       if( 3.0*p.x < m ) q = p.xyz;
  else if( 3.0*p.y < m ) q = p.yzx;
  else if( 3.0*p.z < m ) q = p.zxy;
  else return m*0.57735027;
    
  float k = clamp(0.5*(q.z-q.y+s),0.0,s); 
  return length(vec3(q.x,q.y-s+k,q.z-k)); 
}

float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sphere(vec3 v, vec3 loc, float r) {
	return length(v-loc) - r;
}

float sdf(vec3 v) {
	//return sdBox(v-vec3(0,10,0),vec3(1))-sin(t)-1;
	//return (1-(sin(t/2)+1)/2)*(sdBox(v,vec3(0.6))-0.4)+(sin(t/2)+1)*sdTorus(v,vec2(1,0.5))/2;
	//return (1-(sin(t)+1)/2)*(sdBox(v-vec3(0,10,0),vec3(0.9))-0.1)+(sin(t)+1)*sphere(v,vec3(0,10,0),1)/2;
	//return smin(sphere(v,vec3(sin(t*10),0,0),0.5),sphere(v,vec3(-sin(t*10)*1.5,0,0),1),0.19);
	//return max(sphere(v,vec3(0,0,0),1.5),-sphere(v,vec3(sin(t),1.5,0),0.5));
	//return sdOctahedron(v,1)-0.3;
	//return max(max(sdCappedCylinderX(v,1,1),sdCappedCylinderY(v,1,1)),sdCappedCylinderZ(v,1,1));
	return sdCone(v,vec2(1),1)-0.1;
	//return max(abs(sphere(v,vec3(0),1))-0.1,sdBox(v+vec3(0,0,1),vec3(2,2,1)));
}

vec3 march(vec3 v,vec3 dir) {
	return v+dir*sdf(v);
}

vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sdf(vec3(p.x + 0.00001, p.y, p.z)) - sdf(vec3(p.x - 0.00001, p.y, p.z)),
        sdf(vec3(p.x, p.y + 0.00001, p.z)) - sdf(vec3(p.x, p.y - 0.00001, p.z)),
        sdf(vec3(p.x, p.y, p.z  + 0.00001)) - sdf(vec3(p.x, p.y, p.z - 0.00001))
    ));
}

void main() {
	float hit;
	vec3 origin = vec3(10*cos(-t),10*sin(-t),0);
	vec2 v = vec2(vertex.x-0.5,1);
	vec3 dir = vec3(cos(-t*1+M_PI/2)*v.x-sin(-t*1+M_PI/2)*v.y,sin(-t*1+M_PI/2)*v.x+cos(-t*1+M_PI/2)*v.y,(vertex.y-0.5)/ratio);
	int count = 0;
	while(sdf(origin)>0.01 && count < 1000 && sdf(origin)<1000) {
		origin=march(origin,dir);
		count++;
	}
	vec3 normal = estimateNormal(origin);
	vec3 toLight = normalize(vec3(10,-5,0) - origin);
	vec3 diffuseFactor = vec3(1,0.7,0.7)*max(0.0, dot(normal, toLight))+vec3(0.1,0.1,0.1);
	toLight = normalize(vec3(-5,-6,5) - origin);
	diffuseFactor += vec3(0.7,1,0.7)*max(0.0, dot(normal, toLight));
	toLight = normalize(vec3(-5,6,5) - origin);
	diffuseFactor += vec3(0.7,0.7,1)*max(0.0, dot(normal, toLight));
	
	color = vec4(diffuseFactor.x,diffuseFactor.y,diffuseFactor.z,1);
	//color = vec4(vec3(count/10), 1);
	//color = vec4(0,origin.y,0,1);
}
