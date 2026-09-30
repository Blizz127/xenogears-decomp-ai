R"GLSL(#version 330 core
uniform sampler2D sceneColor, sceneDepth, protection;
uniform vec2 origin;
uniform float amount[8];
uniform vec4 focus; // positive linear depth, range, tilt, unused
uniform vec2 fogRange;
uniform int hasDepth, hasProtection, lightCount, shadowCount, particleCount;
uniform vec4 lights[16], shadows[16], particles[16];
out vec4 fragColor;
ivec2 bounded(ivec2 p) { return clamp(p, ivec2(0), textureSize(sceneColor,0)-1); }
vec4 colorAt(ivec2 p) { return texelFetch(sceneColor,bounded(p),0); }
float protectedAt(ivec2 p) {
    return hasProtection != 0 ? texelFetch(protection,bounded(p),0).r : 0.0;
}
void main() {
    ivec2 pixel = ivec2(gl_FragCoord.xy-origin);
    vec2 size = vec2(textureSize(sceneColor,0));
    vec2 uv = (vec2(pixel)+0.5)/size;
    vec4 original = colorAt(pixel);
    // Do not filter, relight, grade, or overlay protected sprite/UI pixels.
    if (protectedAt(pixel) > 0.0) { fragColor=original; return; }
    vec3 c=original.rgb;
    float depth = hasDepth != 0 ? texelFetch(sceneDepth,pixel,0).r : 0.0;
    float coc = depth > 0.0 ? smoothstep(0.0,focus.y,
        abs(depth-(focus.x+(uv.y-0.5)*focus.z))) * amount[0] : 0.0;
    if (amount[0] > 0.0 || amount[1] > 0.0) {
        vec3 blurred=c, glow=max(c-vec3(0.72),vec3(0)); float weight=1.0;
        // Local restrained blur/glow; every base/sample lookup is nearest.
        for (int i=0;i<12;i++) {
            float angle=float(i)*2.39996323;
            float radius=(1.0+float(i%3))*max(size.y/240.0,1.0);
            ivec2 q=bounded(pixel+ivec2(round(vec2(cos(angle),sin(angle))*radius)));
            if (protectedAt(q)>0.0) continue;
            float z=hasDepth != 0 ? texelFetch(sceneDepth,q,0).r : 0.0;
            // Avoid bleeding across foreground/background depth edges.
            if (amount[0]>0.0 && (depth<=0.0 || z<=0.0 || abs(z-depth)>focus.y)) continue;
            vec3 sampleColor=colorAt(q).rgb;
            blurred+=sampleColor; glow+=max(sampleColor-vec3(0.72),vec3(0)); weight+=1.0;
        }
        c=mix(c,blurred/weight,coc)+glow/weight*amount[1]*0.7;
    }
    if (amount[2]>0.0) {
        // Ambient and projected radial light accents; not world-space PBR.
        c*=mix(vec3(1),vec3(1.035,1.0,0.955),amount[2]);
        for(int i=0;i<lightCount;i++) {
            float r=length((uv-lights[i].xy)*vec2(size.x/size.y,1))/lights[i].z;
            float falloff=pow(max(1.0-r,0.0),2.0)*lights[i].w;
            c+=vec3(1.0,0.66,0.31)*falloff*amount[2]*0.22;
        }
    }
    if (amount[3]>0.0) for(int i=0;i<shadowCount;i++) {
        vec2 d=(uv-shadows[i].xy)/shadows[i].zw;
        c*=1.0-exp(-dot(d,d)*2.5)*amount[3]*0.28;
    }
    if (amount[5]>0.0 && depth>0.0)
        c=mix(c,vec3(0.62,0.65,0.67),smoothstep(fogRange.x,fogRange.y,depth)*amount[5]*0.45);
    if (amount[6]>0.0 && depth>0.0) for(int i=0;i<particleCount;i++) {
        if (particles[i].z>depth) continue;
        float d=length((uv-particles[i].xy)*vec2(size.x/size.y,1));
        float a=1.0-smoothstep(0.0,particles[i].w,d);
        c+=vec3(0.95,0.80,0.52)*a*amount[6]*0.25;
    }
    if (amount[7]>0.0) {
        float l=dot(c,vec3(0.2126,0.7152,0.0722));
        vec3 graded=mix(vec3(l),c,0.92)*vec3(1.035,1.005,0.97);
        c=mix(c,graded,amount[7]);
    }
    if (amount[4]>0.0) {
        vec2 q=(uv-0.5)*2.0;
        c*=1.0-smoothstep(0.35,1.6,dot(q,q))*amount[4]*0.25;
    }
    fragColor=vec4(clamp(c,0.0,1.0),original.a);
}
)GLSL"
