#include "Common.h"
#include "Defines.h"
#include "Structs.h"
#include "Video.h"

/* Line drawing functions */

extern uint8_t far screen_buf [];

void drawLineHor(int start_x, int start_y, int x_diff, uint8_t color)
{
    int offset_x;
    
    if (x_diff < 0)
    {
        start_x += x_diff;
        x_diff = abs(x_diff);
    }
    
    for (offset_x = 0; offset_x < x_diff; offset_x++)
    {
        SET_PIXEL(start_x + offset_x, start_y, color);
    }
}

void drawLineVer(int start_x, int start_y, int y_diff, uint8_t color)
{
    int offset_y;
    
    if (y_diff < 0)
    {
        start_y += y_diff;
        y_diff = abs(y_diff);
    }
    
    for (offset_y = 0; offset_y < y_diff + 1; offset_y++)
    {
        SET_PIXEL(start_x, start_y + offset_y, color);
    }
}

void drawLine(Vec2 v1, Vec2 v2, uint8_t color)
{
    float offset_x;
    float offset_y;
    
    float x_diff = (v2.x - v1.x);
    float y_diff = (v2.y - v1.y);
    
    int x_sign = SIGN(x_diff);
    int y_sign = SIGN(y_diff);
    
    int x_pixel;
    int y_pixel;
    
    float slope = 0.0;
    
    if (fabs(y_diff) < 1.5)
        drawLineHor(v1.x, v1.y, x_diff, color);
    
    else if (fabs(x_diff) < 1.5)
        drawLineVer(v1.x, v1.y, y_diff, color);
    
    else if (y_diff < 0 && x_diff < 0)
    {
        if (y_diff < x_diff)
        {
            slope = y_diff / x_diff;
            
            for (offset_x = 0; offset_x > x_diff; offset_x += x_sign)
            {
                offset_y = offset_x * slope;
                offset_x += 0.75;
                offset_y += 0.75;
                SET_PIXEL((int)v2.x - (int)offset_x, (int)v2.y - (int)offset_y, color);
            }
        }
        else
        {
            slope = x_diff / y_diff;
            
            for (offset_y = 0; offset_y > y_diff; offset_y += y_sign)
            {
                offset_x = offset_y * slope;
                offset_x -= 0.75;
                offset_y += 0.75;
                SET_PIXEL((int)v2.x - (int)offset_x, (int)v2.y - (int)offset_y, color);
            }
        }
    }
    
    else if (y_diff < 0 || x_diff < 0)
    {
        if (y_diff < x_diff)
        {
            slope = y_diff / x_diff;
            
            for (offset_x = 0; offset_x < x_diff; offset_x += x_sign)
            {
                offset_y = offset_x * slope;
                offset_x -= 0.6;
                offset_y -= 0.6;
                SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, color);
            }
        }

        else
        {
            slope = x_diff / y_diff;
            
            for (offset_y = 0; offset_y < y_diff; offset_y += y_sign)
            {
                offset_x = offset_y * slope;
                offset_x -= 0.6;
                offset_y -= 0.6;
                SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, color);
            }
        }
    }
    
    else if (y_diff < x_diff)
    {
        slope = y_diff / x_diff;
        
        for (offset_x = 0; offset_x < x_diff; offset_x += x_sign)
        {
            offset_y = offset_x * slope;
            SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, color);
        }
    }
    
    else
    {
        slope = x_diff / y_diff;
        
        for (offset_y = 0; offset_y < y_diff; offset_y += y_sign)
        {
            offset_x = offset_y * slope;
            SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, color);
        }
    }
}

void drawLineInt(Vec2_int p0, Vec2_int p1, uint8_t color)
{
    int offset_x = 0;
    int offset_y = 0;
    
    int x_diff = (p1.x - p0.x);
    int y_diff = (p1.y - p0.y);
    int counter;
    
    int x_diff_true = x_diff;
    int y_diff_true = y_diff;
    
    int x_sign = SIGN(x_diff);
    int y_sign = SIGN(y_diff);
    
    x_diff = abs(x_diff);
    y_diff = abs(y_diff);
    
    if (y_diff == 0)
        drawLineHor(p0.x, p0.y, x_diff_true, color);
    
    else if (x_diff == 0)
        drawLineVer(p0.x, p0.y, y_diff_true, color);
    
    else if (y_diff < x_diff)
    {
        counter = 0;
        while (offset_x < x_diff)
        {
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, color);
            offset_x++;
            counter += y_diff;
            if (counter >= x_diff)
            {
                offset_y++;
                counter -= x_diff;
            }
        }
    }
    
    else if (x_diff < y_diff)
    {
        counter = 0;
        while (offset_y < y_diff)
        {
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, color);
            offset_y++;
            counter += x_diff;
            if (counter >= y_diff)
            {
                offset_x++;
                counter -= y_diff;
            }
        }
    }
    
    else
    {        
        while (offset_y <= y_diff && offset_x <= x_diff)
        {
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, color);
            offset_y++;
            offset_x++;
        }
    }
}

void drawLineHorzColorBlendedInt(int start_x, int end_x, int y, uint8_t start_color, uint8_t end_color)
{    
	int x = start_x;
	int offset_x;
    int x_diff = end_x - start_x;
    int x_sign = SIGN(x_diff);

    int color = start_color;
    int color_diff = end_color - start_color;
	int color_sign = SIGN(color_diff);
	int color_counter;
	// beginning at x_diff/2 instead of 0 fixes a minor inaccuracy
	
	x_diff = abs(x_diff);
	color_diff = abs(color_diff);
	color_counter = x_diff/2;
	
    for (offset_x = 0; offset_x < x_diff; offset_x++)
    {
        SET_PIXEL(x, y, (uint8_t)color);
		x += x_sign;
		// check whether to step the color value forwards
        color_counter += color_diff;
		while (color_counter > x_diff)
		{
			color_counter -= x_diff;
			color += color_sign;
		}
    }
}

void drawLineHorzColorBlended(int start_x, int end_x, int start_y, uint8_t start_color, uint8_t end_color)
{
    float color = start_color;
    int color_diff = end_color - start_color;
    int offset_x;
    float x_diff = end_x - start_x;
    float color_ratio = color_diff / fabs(x_diff);
    
    for (offset_x = 0; offset_x < x_diff; offset_x++)
    {
        SET_PIXEL(start_x + offset_x, start_y, (uint8_t)color);
        color += color_ratio;
    }
}

void drawLineHorzTextured(int start_x, int end_x, int start_y, Vec2 sprite_start, Vec2 sprite_end, uint8_t* sprite)
{
    Vec2 uv;
    int offset_x;
    float u_ratio;
    float v_ratio;
    float x_diff = end_x - start_x;
    float u_diff = sprite_end.x - sprite_start.x;
    float v_diff = sprite_end.y - sprite_start.y;
    int x, y, pixel;
    uv.x = sprite_start.x;
    uv.y = sprite_start.y;
    
    x_diff = fabs(x_diff);
    
	if (x_diff != 0.0)
	{
		u_ratio = u_diff / x_diff;
		v_ratio = v_diff / x_diff;

		for (offset_x = 0; offset_x < x_diff; offset_x++)
		{
			pixel = (int)uv.y * 100 + (int)uv.x;
			SET_PIXEL(start_x + offset_x, start_y, sprite[pixel]);
			uv.x += u_ratio;
			uv.y += v_ratio;
		}
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