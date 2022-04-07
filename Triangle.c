#include "Common.h"
#include "Defines.h"
#include "Structs.h"

/* Triangle plotting and drawing functions */

Span LeftEdge;
Span RightEdge;

extern uint8_t far screen_buf [];
extern uint8_t tile [];
extern uint8_t* g_Keyboard;

void plotTriangleLineColorBlendedInt(Vec2 p1, Vec2 p2, Vec2_int center, Span* edge, uint8_t start_color, uint8_t end_color)
{
	int offset;
	int counter;
    int x = (int)(p1.x + 0.5) + center.x;
	int y = (int)(p1.y + 0.5) + center.y;
    int x_diff = (int)(p2.x - p1.x + 0.5);
    int y_diff = (int)(p2.y - p1.y + 0.5);
    int x_sign = SIGN(x_diff);
	int y_sign = SIGN(y_diff);
	
    int color = start_color;
    int color_diff = end_color - start_color;
	int color_sign = SIGN(color_diff);
	int color_counter;
	
	x_diff = abs(x_diff);
	y_diff = abs(y_diff);
	color_diff = abs(color_diff);
	
    if (y_diff > x_diff)
    {
		color_counter = y_diff/2;
		// beginning at y_diff/2 instead of 0 fixes a minor inaccuracy
        for (offset = 0; offset < y_diff; offset++)
        {
            edge->offset[y] = x;
            edge->color[y] = (uint8_t)color;
			// move along the y coordinate
			y += y_sign;
			// check whether to move on the y coordinate
			counter += x_diff;
			while (counter > y_diff)
			{
				counter -= y_diff;
				x += x_sign;
			}
			// check whether to step the color value forwards
			color_counter += color_diff;
			while (color_counter > y_diff)
			{
				color_counter -= y_diff;
				color += color_sign;
			}
        }
    }
    else if (x_diff > y_diff)
    {
		color_counter = x_diff/2;
		// beginning at x_diff/2 instead of 0 fixes a minor inaccuracy	
        for (offset = 0; offset < x_diff; offset++)
        {
            edge->offset[y] = x;
            edge->color[y] = (uint8_t)color;
			// move along the x coordinate
			x += x_sign;
			// check whether to move on the y coordinate
			counter += y_diff;
			while (counter > x_diff)
			{
				counter -= x_diff;
				y += y_sign;
			}
			// check whether to step the color value forwards
			color_counter += color_diff;
			while (color_counter > x_diff)
			{
				color_counter -= x_diff;
				color += color_sign;
			}
        }
    }
}

void plotTriangleLine(Vec2 p1, Vec2 p2, Vec2_int center, Span* edge)
{
    uint8_t color;
    
    float offset_y;
    float offset_x;
    
    float x_diff = (p2.x - p1.x);
    float y_diff = (p2.y - p1.y);
   
    float slope = 0.0;
    
    int y_sign = SIGN(y_diff);
    int x, y;
    
    y_diff = fabs(y_diff);
    
    slope = x_diff / y_diff;
        
    for (offset_y = 0; offset_y < y_diff; offset_y++)
    {
        offset_x = offset_y * slope;
        x = (int)(center.x + p1.x + offset_x);
        y = (int)(center.y + p1.y + offset_y) * y_sign;
        edge->offset[y] = x;
        //SET_PIXEL(edge->offset[y], y, color);
    }
}

void plotTriangleLineColorBlended(Vec2 p1, Vec2 p2, Vec2_int center, Span* edge, uint8_t color_1, uint8_t color_2)
{
    float color = color_1;
    int color_diff = color_2 - color_1;
    uint8_t test_color;
    
    float offset_y;
    float offset_x;
    
    float x_diff = (p2.x - p1.x);
    float y_diff = (p2.y - p1.y);
    
    float color_ratio;
   
    float slope = 0.0;
    
    int y_sign = SIGN(y_diff);
    int x, y;
    
    if (edge == &LeftEdge)
        test_color = 32;
    
    else if (edge == & RightEdge)
        test_color = 40;
    
    if (y_diff > x_diff)
    {
        y_diff = fabs(y_diff);
        color_ratio = color_diff / y_diff;
        slope = x_diff / y_diff;
            
        for (offset_y = 0; offset_y < y_diff; offset_y++)
        {
            offset_x = offset_y * slope;
            x = (int)(center.x + p1.x + offset_x);
            y = (int)(center.y + p1.y + offset_y) * y_sign;
            edge->offset[y] = x;
            edge->color[y] = (uint8_t)color;
            color += color_ratio;
            if (KEY_IS_PRESSED (KEY_Q))
                SET_PIXEL(edge->offset[y], y, test_color);
            else if (KEY_IS_PRESSED (KEY_R))
                SET_PIXEL(edge->offset[y], y, edge->color[y]);
        }
    }
    
    else if (x_diff > y_diff)
    {
        x_diff = fabs(x_diff);
        color_ratio = color_diff / x_diff; //Potential divide by zero
        slope = y_diff / x_diff;//Potential divide by zero
            
        for (offset_x = 0; offset_x < x_diff; offset_x++)
        {
            offset_y = offset_x * slope;
            x = (int)(center.x + p1.x + offset_x);
            y = (int)(center.y + p1.y + offset_y) * y_sign;
            edge->offset[y] = x;
            edge->color[y] = (uint8_t)color;
            color += color_ratio;
            if (KEY_IS_PRESSED (KEY_Q))
                SET_PIXEL(edge->offset[y], y, test_color);
            else if (KEY_IS_PRESSED (KEY_R))
                SET_PIXEL(edge->offset[y], y, edge->color[y]);
        }
    }
}

void plotTriangleLineTexture(Vec2 p1, Vec2 p2, Vec2_int center, Span* edge, Vec2 tex_start, Vec2 tex_end)
{
    Vec2 uv;
    
    float offset_y;
    float offset_x;
    
    float x_diff = (p2.x - p1.x);
    float y_diff = (p2.y - p1.y);
	
    float u_diff = (tex_end.x - tex_start.x);
    float v_diff = (tex_end.y - tex_start.y);
    
    float u_ratio;
    float v_ratio;
    
    float slope = 0.0;
    
    int y_sign = SIGN(y_diff);
	int x_sign = SIGN(x_diff);
	
    int x, y;
	
	uint8_t test_color;
    
	uv.x = tex_start.x;
	uv.y = tex_start.y;
    y_diff = fabs(y_diff)+1;
	x_diff = fabs(x_diff)+1;
	
    if (y_diff > x_diff)
    {
	    slope = (x_diff / y_diff) * x_sign;
	    u_ratio = u_diff / y_diff;
	    v_ratio = v_diff / y_diff;

        for (offset_y = 0; offset_y < y_diff; offset_y++)
        {
            offset_x = offset_y * slope;
            x = (int)(center.x + p1.x + offset_x);
            y = (int)(center.y + p1.y + (offset_y * y_sign));
            edge->offset[y] = x;
            edge->texture[y].x = uv.x;
            edge->texture[y].y = uv.y;
			uv.x += u_ratio;
            uv.y += v_ratio;
        }
    }
    
    else if (x_diff > y_diff)
    {  
        slope = (y_diff / x_diff) * y_sign;
	    u_ratio = u_diff / x_diff;
	    v_ratio = v_diff / x_diff;
            
        for (offset_x = 0; offset_x < x_diff; offset_x++)
        {
            offset_y = offset_x * slope;
            x = (int)(center.x + p1.x + (offset_x * x_sign));
            y = (int)(center.y + p1.y + offset_y);
            edge->offset[y] = x;
            edge->texture[y].x = uv.x;
            edge->texture[y].y = uv.y;
			uv.x += u_ratio;
            uv.y += v_ratio;
        }
    }
	
	// for debugging purposes
	if (KEY_IS_PRESSED(KEY_W))
	{
	    if (edge == &LeftEdge)
	        test_color = 32;
	    
	    else if (edge == & RightEdge)
	        test_color = 40;
			
		p1.x += center.x;
		p1.y += center.y;
		p2.x += center.x;
		p2.y += center.y;
		drawLine(p1, p2, test_color);
	}
}

void fillSpansColorBlendedInt(int top, int bottom)
{
    int line, start_x, end_x;
    uint8_t start_color, end_color;
    
    for (line = top; line < bottom; line++)
    {
        start_x 	= (int)LeftEdge.offset[line];
        end_x 		= (int)RightEdge.offset[line];
        start_color = LeftEdge.color[line];
        end_color 	= RightEdge.color[line];
        
        drawLineHorzColorBlendedInt(start_x, end_x, line,
									start_color, end_color);
    }
}

void fillSpans(int top, int bottom, uint8_t color)
{
    int line;
    Vec2 p0, p1;
    
    for (line = top; line < bottom; line++)
    {
        drawLineHor(LeftEdge.offset[line], line, (RightEdge.offset[line] - LeftEdge.offset[line]), color);
    }
}

void fillSpansColorBlended(int top, int bottom)
{
    int line;
    Vec2 p0, p1;
    uint8_t col_1, col_2;
    
    for (line = top; line < bottom; line++)
    {
        p0.y = line;
        p1.y = line;
        p0.x = LeftEdge.offset[line];
        p1.x = RightEdge.offset[line];
        col_1 = LeftEdge.color[line];
        col_2 = RightEdge.color[line];
        
        //drawLineColorBlended(p0, p1, col_1, col_2);
        drawLineHorzColorBlended(p0.x, p1.x, line, col_1, col_2);
    }
}

void fillSpansTextured(int top, int bottom, uint8_t* sprite)
{
    int line;
    Vec2 p0, p1;
    Vec2 tex_start;
    Vec2 tex_end;

    for (line = top; line < bottom; line++)
    {
        p0.x = LeftEdge.offset[line];
        p1.x = RightEdge.offset[line];
        
        tex_start.x = LeftEdge.texture[line].x;
        tex_start.y = LeftEdge.texture[line].y;
        tex_end.x = RightEdge.texture[line].x;
        tex_end.y = RightEdge.texture[line].y;
        
        drawLineHorzTextured(p0.x, p1.x, line, tex_start, tex_end, sprite);
    }
}

void drawLineHorzMeshTextured(int start_x, int end_x, int start_y, Vec2 tex_start, Vec2 tex_end, Texture* texture)
{
    Vec2 uv;
    int offset_x;
    float u_ratio;
    float v_ratio;
    float x_diff = end_x - start_x;
    float u_diff = tex_end.x - tex_start.x;
    float v_diff = tex_end.y - tex_start.y;
	int tex_x_mask = texture->width-1; // Caution: This assumes texture dimensions
	int tex_y_mask = texture->height-1; // are powers of 2! Shit breaks otherwise
    int x, y, pixel;
    uint8_t color;
    uv.x = tex_start.x;
    uv.y = tex_start.y;
	start_x++;
    
    x_diff = fabs(x_diff);
    
	if (x_diff != 0)
	{
	    u_ratio = u_diff / x_diff;
	    v_ratio = v_diff / x_diff;
	
	    
	    if (texture->transparent == TRUE)
	    {
	        for (offset_x = 0; offset_x < x_diff; offset_x++)
	        {
	            //pixel = (((int)uv.y) % texture->height) * texture->width
				//		+ (((int)uv.x) % texture->width);
	            pixel = (((int)uv.y) & tex_y_mask) * texture->width
						+ (((int)uv.x) & tex_x_mask);
	            color = texture->pixels[pixel];
	            if (color != TRANSPARENT_COLOR)
	                SET_PIXEL(start_x + offset_x, start_y, color);
	            uv.x += u_ratio;
	            uv.y += v_ratio;
	        }
	    }
	    else
	    {
	        for (offset_x = 0; offset_x < x_diff; offset_x++)
	        {
	            //pixel = (((int)uv.y) % texture->height) * texture->width
				//		+ (((int)uv.x) % texture->width);
	            pixel = (((int)uv.y) & tex_y_mask) * texture->width
						+ (((int)uv.x) & tex_x_mask);
	            color = texture->pixels[pixel];
	            SET_PIXEL(start_x + offset_x, start_y, color);
	            uv.x += u_ratio;
	            uv.y += v_ratio;
	        }
	    }
	}
}

void fillSpansMeshTextured(int top, int bottom, Texture* texture)
{
    int line;
    Vec2 p0, p1;
    Vec2 tex_start;
    Vec2 tex_end;

    for (line = top; line < bottom; line++)
    {
        p0.x = LeftEdge.offset[line];
        p1.x = RightEdge.offset[line];
        
        tex_start.x = LeftEdge.texture[line].x;
        tex_start.y = LeftEdge.texture[line].y;
        tex_end.x = RightEdge.texture[line].x;
        tex_end.y = RightEdge.texture[line].y;
		
		//printf("Line %d OK!\n", line);
        
        drawLineHorzMeshTextured(p0.x, p1.x, line, tex_start, tex_end, texture);
    }
}

void sortPair(Vec2* v0, Vec2* v1)
{
    Vec2 temp;
    
    if (v0->y > v1->y)
    {
        temp = *v0;
        *v0 = *v1;
        *v1 = temp;
    }
}

void sortPairWithColor(Vec2* v0, Vec2* v1, uint8_t* color1, uint8_t* color2)
{
    Vec2 temp;
    uint8_t Coloremp;
    
    if (v0->y > v1->y)
    {
        temp = *v0;
        Coloremp = *color1;
        *v0 = *v1;
        *color1 = *color2;
        *v1 = temp;
        *color2 = Coloremp;
    }
}

void sortPairWithPoint(Vec2* v0, Vec2* v1, Vec2* p0, Vec2* p1)
{
    Vec2 temp;
    Vec2 point_temp;
    
    if (v0->y > v1->y)
    {
        temp = *v0;
        point_temp = *p0;
        *v0 = *v1;
        *p0 = *p1;
        *v1 = temp;
        *p1 = point_temp;
    }
}

void drawFilledTriangle(Polygon* triangle)
{    
    Vec2 A, B, C;
    Vec2 AB, AC;
    Vec2_int center = {159, 99};
    float cross_product;
    
    A = triangle->transformedV[0];
    B = triangle->transformedV[1];
    C = triangle->transformedV[2];
    
    sortPair(&A, &B);
    sortPair(&A, &C);
    sortPair(&B, &C);
    
    AB.x = B.x - A.x;
    AB.y = B.y - A.y;
    AC.x = C.x - A.x;
    AC.y = C.y - A.y;
    
    cross_product = (AB.x * AC.y) - (AB.y * AC.x);
    
    if (cross_product > 0)
    {
        plotTriangleLine(A, B, center, &RightEdge);
        plotTriangleLine(B, C, center, &RightEdge);
        plotTriangleLine(A, C, center, &LeftEdge);
    }
    else
    {
        plotTriangleLine(A, B, center, &LeftEdge);
        plotTriangleLine(B, C, center, &LeftEdge);
        plotTriangleLine(A, C, center, &RightEdge);
    }
    if (KEY_IS_PRESSED (KEY_E))
        fillSpans(center.y + A.y, center.y + C.y, 85);
}

void drawShadedTriangle(Polygon* triangle)
{    
    Vec2 A, B, C;
    Vec2 AB, AC;
    Vec2_int center = {99, 99};
    float cross_product;
    uint8_t A_color, B_color, C_color;
    
    A = triangle->transformedV[0];
    B = triangle->transformedV[1];
    C = triangle->transformedV[2];
    
    if (KEY_IS_PRESSED (KEY_1))
        triangle->vertexColors[0]++;
    
    else if (KEY_IS_PRESSED (KEY_2))
        triangle->vertexColors[1]++;
    
    else if (KEY_IS_PRESSED (KEY_3))
        triangle->vertexColors[2]++;
    
    else if (KEY_IS_PRESSED (KEY_4))
        triangle->vertexColors[0]--;
    
    else if (KEY_IS_PRESSED (KEY_5))
        triangle->vertexColors[1]--;
    
    else if (KEY_IS_PRESSED (KEY_6))
        triangle->vertexColors[2]--;
    
    A_color = triangle->vertexColors[0];
    B_color = triangle->vertexColors[1];
    C_color = triangle->vertexColors[2];
    
    sortPairWithColor(&A, &B, &A_color, &B_color);
    sortPairWithColor(&A, &C, &A_color, &C_color);
    sortPairWithColor(&B, &C, &B_color, &C_color);
    
    AB.x = B.x - A.x;
    AB.y = B.y - A.y;
    AC.x = C.x - A.x;
    AC.y = C.y - A.y;
    
    cross_product = (AB.x * AC.y) - (AB.y * AC.x);
    
    if (cross_product > 0)
    {
        plotTriangleLineColorBlendedInt(A, B, center, &RightEdge, A_color, B_color);
        plotTriangleLineColorBlendedInt(B, C, center, &RightEdge, B_color, C_color);
        plotTriangleLineColorBlendedInt(A, C, center, &LeftEdge, A_color, C_color);
    }
    else
    {
        plotTriangleLineColorBlendedInt(A, B, center, &LeftEdge, A_color, B_color);
        plotTriangleLineColorBlendedInt(B, C, center, &LeftEdge, B_color, C_color);
        plotTriangleLineColorBlendedInt(A, C, center, &RightEdge, A_color, C_color);
    }
    //if (KEY_IS_PRESSED (KEY_W))
    fillSpansColorBlendedInt(center.y + A.y, center.y + C.y);
}

void drawTexturedTriangle(Polygon* triangle)
{    
    Vec2 A, B, C, point_a, point_b, point_c;
    Vec2 AB, AC;
    Vec2_int center = triangle->center;
    float cross_product;
    
    A = triangle->transformedV[0];
    B = triangle->transformedV[1];
    C = triangle->transformedV[2];
    
    point_a = triangle->UVCoords[0];
    point_b = triangle->UVCoords[1];
    point_c = triangle->UVCoords[2];
    
    sortPairWithPoint(&A, &B, &point_a, &point_b);
    sortPairWithPoint(&A, &C, &point_a, &point_c);
    sortPairWithPoint(&B, &C, &point_b, &point_c);
    
    AB.x = B.x - A.x;
    AB.y = B.y - A.y;
    AC.x = C.x - A.x;
    AC.y = C.y - A.y;
    
    cross_product = (AB.x * AC.y) - (AB.y * AC.x);
    
    if (cross_product > 0)
    {
        plotTriangleLineTexture(A, B, center, &RightEdge, point_a, point_b);
        plotTriangleLineTexture(B, C, center, &RightEdge, point_b, point_c);
        plotTriangleLineTexture(A, C, center, &LeftEdge, point_a, point_c);
    }
    else
    {
        plotTriangleLineTexture(A, B, center, &LeftEdge, point_a, point_b);
        plotTriangleLineTexture(B, C, center, &LeftEdge, point_b, point_c);
        plotTriangleLineTexture(A, C, center, &RightEdge, point_a, point_c);
    }
    fillSpansTextured(center.y + A.y, center.y + C.y, tile);
}

void draw2DMeshTriangle(Mesh2D* mesh, int triangle_ID)
{    
    Vec2 A, B, C, point_a, point_b, point_c;
    Vec2 AB, AC;
    Vec2_int center = mesh->center;
    float cross_product;
    
    A = mesh->transformedP[mesh->triangleVertices[0 + triangle_ID * 3].pointID];
    B = mesh->transformedP[mesh->triangleVertices[1 + triangle_ID * 3].pointID];
    C = mesh->transformedP[mesh->triangleVertices[2 + triangle_ID * 3].pointID];
    
    point_a = mesh->triangleVertices[0 + triangle_ID * 3].UV;
    point_b = mesh->triangleVertices[1 + triangle_ID * 3].UV;
    point_c = mesh->triangleVertices[2 + triangle_ID * 3].UV;
    
    sortPairWithPoint(&A, &B, &point_a, &point_b);
    sortPairWithPoint(&A, &C, &point_a, &point_c);
    sortPairWithPoint(&B, &C, &point_b, &point_c);
    
    AB.x = B.x - A.x;
    AB.y = B.y - A.y;
    AC.x = C.x - A.x;
    AC.y = C.y - A.y;
    
    cross_product = (AB.x * AC.y) - (AB.y * AC.x);
    
    if (cross_product > 0)
    {
        plotTriangleLineTexture(A, B, center, &RightEdge, point_a, point_b);
        plotTriangleLineTexture(B, C, center, &RightEdge, point_b, point_c);
        plotTriangleLineTexture(A, C, center, &LeftEdge, point_a, point_c);
    }
    else
    {
        plotTriangleLineTexture(A, B, center, &LeftEdge, point_a, point_b);
        plotTriangleLineTexture(B, C, center, &LeftEdge, point_b, point_c);
        plotTriangleLineTexture(A, C, center, &RightEdge, point_a, point_c);
    }
    fillSpansMeshTextured(center.y + A.y, center.y + C.y, mesh->texture);
}