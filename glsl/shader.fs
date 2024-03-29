#version 450
// In
uniform float iTime;
uniform vec2 iResolution;
uniform vec2 iMove;
uniform float iZoom;
uniform vec4 iMouse;
uniform int iFps;
uniform sampler2D iChannel0;

// Out
out highp vec4 fragColor;


void drawMouseCursor(inout vec4 fragColor, in vec2 fragCoord){
    float distance  = length(vec2(0., 0.) - fragCoord);
    float innerRadius = 45;
    float outerRadius = 50;

    if (distance > innerRadius && distance < outerRadius){
        fragColor = vec4(1., 1., 1., 1.);
    }
}

void drawGrid(inout vec4 fragColor, in vec2 fragCoord){
    float gridSize = 100.;

    ivec2 pixelCoord = ivec2(fragCoord.xy);
    if (mod(pixelCoord.x, gridSize) <= .1 || mod(pixelCoord.y, gridSize) <= .1){
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}

void mainImage(out vec4 fragColor, in vec2 fragCoord){
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;

    // Time varying pixel color
    vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));

    // Output to screen
    fragColor = vec4(col, 1.0);

    // Draw grid
    drawGrid(fragColor, fragCoord);

    if(fragCoord.x < 0)
        fragColor -= vec4(.50);

    // Mouse cursor
    drawMouseCursor(fragColor, fragCoord - (iMouse.xy + iMove) / iZoom);
}

float PrintChars(vec2 p, int text[16]){
    float val = 0.;
    p.x += .25;
    
    int to = text[0] + 1;

    for(int x = 1; x < to; x ++){
        int c = text[x];
    
        vec2 dFdx = dFdx(p/16.), dFdy = dFdy(p/16.);
        if(p.x < .0|| p.x > 1. || p.y < 0. || p.y > 1.){
          //  return val;
        } else{
        
            float r = textureGrad( iChannel0, p/16. + fract( vec2(c, 15-c/16) / 16. ), dFdx, dFdy ).x;
            if(r > 0.5)
                val += r;
        }
        
        p.x -= .5;
    }
    
    return val;
}

float PrintChar(int c, vec2 p){
    p.x += .25;

    vec2 dFdx = dFdx(p/16.), dFdy = dFdy(p/16.);
    if (p.x<.0|| p.x>1. || p.y<0.|| p.y>1.) return 0.;//vec4(0,0,0,1e5);
    //if (p.x<.25|| p.x>.75 || p.y<0.|| p.y>1.) return vec4(0,0,0,1e5); // strange bug with an old driver
    return textureGrad( iChannel0, p/16. + fract( vec2(c, 15-c/16) / 16. ), dFdx, dFdy ).x;
}

float PrintFloatP2(vec2 p, float val){
    int vali = int(val), count = 0;
    int arr[16];

    // -
    if(val < 0){
        arr[count + 1] = 45;
        vali *= -1;
        count ++;
    }

    // 0
    if(vali == 0){
        arr[count + 1] = 48;
        count ++;
     }

    // Count
    while(vali > 0){
        vali /= 10;
        count ++;
    }

    if(count < 12){
        arr[0] = count + 3;
        arr[count + 1] = 46;
        arr[count + 2] = 48 + int(fract(val) * 10.);
        arr[count + 3] = 48 + int(fract(val) * 100.) % 10;
    } else
        arr[0] = count;

    // Write
    vali = abs(int(val));
    while(vali > 0){
        arr[count --] = 48 + vali % 10;
        vali /= 10;
    }

    return PrintChars(p, arr);
}

float PrintCharsCoord(vec2 fragCoord, vec2 pos, float size, int text[16]){
    return PrintChars((fragCoord - pos) / size, text);
}

vec2 PrintCharsCoord(vec2 fragCoord, vec2 pos, float size){
    return (fragCoord - pos) / size;
}

// Tool bar
// JavaScript text to int: var t = 'X coodrinate', ts = t.length; while(t.length < 15) t+= ' '; ts + ', ' + t.split('').map(x=>x.charCodeAt(0)).reduce((a,b)=>a + ', ' + b);
int text_zoom[] = int[](1, 90, 32, 99, 111, 111, 114, 100, 105, 110, 97, 116, 101, 32, 32, 32);
int text_fps[] = int[](3, 70, 80, 83, 111, 111, 114, 100, 105, 110, 97, 116, 101, 32, 32, 32);
int text_any[] = int[](1, 70, 80, 83, 111, 111, 114, 100, 105, 110, 97, 116, 101, 32, 32, 32);


void toolBar(inout vec4 fragColor, in vec2 fragCoord){
    vec2 uv = fragCoord/iResolution.xy;

    if(fragCoord.y < 35.){
        fragColor = vec4(.94, .94, .94, 1.);        

        //  Zoom
        fragColor.xyz -= PrintChars(PrintCharsCoord(fragCoord, vec2(0., -10.), 50.), text_zoom);
        fragColor.xyz -= PrintFloatP2(PrintCharsCoord(fragCoord, vec2(40., -10.), 50.), iZoom);

        // XY
        text_any[1] = 88;
        fragColor.xyz -= PrintChars(PrintCharsCoord(fragCoord, vec2(200., -10.), 50.), text_any);
        fragColor.xyz -= PrintFloatP2(PrintCharsCoord(fragCoord, vec2(240., -10.), 50.), (iMouse.x + iMove.x) / iZoom);

        text_any[1] = 89;
        fragColor.xyz -= PrintChars(PrintCharsCoord(fragCoord, vec2(500., -10.), 50.), text_any);
        fragColor.xyz -= PrintFloatP2(PrintCharsCoord(fragCoord, vec2(540., -10.), 50.), (iMouse.y + iMove.y) / iZoom);
    }

    // FPS
    if(iFps > 0.){
        fragColor.xyz += PrintChars(PrintCharsCoord(fragCoord, vec2(0., iResolution.y - 60.), 70.), text_fps);
        fragColor.xyz += PrintFloatP2(PrintCharsCoord(fragCoord, vec2(120., iResolution.y - 60.), 70.), iFps);        
    }
}

// Main function
void main(){
	// Main Image
	mainImage(fragColor, (gl_FragCoord.xy + iMove.xy) / iZoom);

    // ToolBar
    toolBar(fragColor, gl_FragCoord.xy);
}