#include "Common.h"
#include "Defines.h"
#include "Structs.h"

/* Garbage functions not currently used anywhere, saved for posterity */

Vec2 change_vec_scale(Vec2 vector, float scale)
{
    Vec2 newVector;
    
    newVector.x = vector.x * scale;
    newVector.y = vector.y * scale;
    
    return newVector;
}

void change_poly_angles(Polygon* poly, float angle)
{
    char i;
    
    for (i = 0; i < poly->numPoints; i++)
    {
        poly->transformedV[i] = change_vec_angle(poly->transformedV[i], angle);
    }
    poly->angle = angle;
}

void change_poly_size(Polygon* poly, float scale)
{
    char i;
    
    for (i = 0; i < poly->numPoints; i++)
    {
        poly->transformedV[i] = change_vec_scale(poly->transformedV[i], scale);
    }
}

void drawLineColorBlended(Vec2 p0, Vec2 p1, uint8_t st_col, uint8_t end_col)
{
    int offset_x = 0;
    int offset_y = 0;
    
    int x_diff = (p1.x - p0.x);
    int y_diff = (p1.y - p0.y);
    
    int length = sqrt((abs(x_diff) * abs(x_diff)) + (abs(y_diff) * abs(y_diff)));
	// why the fuck do you take square root here lmao wtffbbq XDXDSDDD
    uint8_t col_add = 0;
    uint8_t col_step = length / abs(end_col - st_col);
    uint8_t col_counter = 0;
    
    int counter;
    
    int x_diff_true = x_diff;
    int y_diff_true = y_diff;
    
    int x_sign = SIGN(x_diff);
    int y_sign = SIGN(y_diff);
    
    x_diff = abs(x_diff);
    y_diff = abs(y_diff);
    
    if (y_diff == 0)
    {
        if (x_diff_true < 0)
        {
            p0.x += x_diff_true;
        }
        
        for (offset_x = 0; offset_x < x_diff; offset_x++)
        {
            SET_PIXEL(p0.x + offset_x, p0.y, st_col + col_add);
            col_counter++;
            if (col_counter == col_step && (st_col + col_add) < end_col)
            {
                col_add++;
                col_counter = 0;
            }
        }
    }
        
    else if (x_diff == 0)
    {
        if (y_diff_true < 0)
        {
            p0.y += y_diff_true;
        }
        
        for (offset_y = 0; offset_y < y_diff; offset_y++)
        {
            SET_PIXEL(p0.x, p0.y + offset_y, st_col + col_add);
            col_counter++;
            if (col_counter == col_step && (st_col + col_add) < end_col)
            {
                col_add++;
                col_counter = 0;
            }
        }
    }
    
    else if (y_diff < x_diff)
    {
        counter = 0;
        while (offset_x < x_diff)
        {
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, st_col + col_add);
            offset_x++;
            col_counter++;
            if (col_counter == col_step && (st_col + col_add) < end_col)
            {
                col_add++;
                col_counter = 0;
            }
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
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, st_col + col_add);
            offset_y++;
            col_counter++;
            if (col_counter == col_step && (st_col + col_add) < end_col)
            {
                col_add++;
                col_counter = 0;
            }
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
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, st_col + col_add);
            offset_y++;
            offset_x++;
            col_counter++;
            if (col_counter == col_step && (st_col + col_add) < end_col)
            {
                col_add++;
                col_counter = 0;
            }
        }
    }
}

void test_draw()
{
    char i = 0;
    float angle = 12;
    int radius = 90;
    Vec2 v1;
    Vec2 v2;
    Vec2 v3;
    
    while (i < 12)
    {
        v1.x = cos(angle) * radius + 160.0;
        v1.y = sin(angle) * radius + 100.0;
        v2.x = cos(angle + RAD_30) * radius + 160.0;
        v2.y = sin(angle + RAD_30) * radius + 100.0;
        v3.x = 160.0;
        v3.y = 100.0;
        drawLineColorBlended(v1, v2, 64, 79);
        drawLineColorBlended(v3, v1, 64, 79);
        //SET_PIXEL(v1.x, v1.y, 14);
        //SET_PIXEL(v2.x, v2.y, 14);
        angle += RAD_30;
        i++;
    }
}

void makeQuadrilateral()
{
    Vec2 point_a = {24.5, 0.0};
    Vec2 point_b = {0.0, 99.0};
    Vec2 point_c = {49.0, 99.0};
    Vec2_int center = {100, 68};
    
    poly_array[6] = makeTexturedTriangle(0.0, 3, 35.0, 1.0, point_a, point_b, point_c, center);
    point_a.x = 74.5;
    point_a.y = 99.0;
    point_b.x = 50.0;
    point_b.y = 0.0;
    point_c.x = 99.0;
    point_c.y = 0.0;
    center.x = 130;
    center.y = 85;
    poly_array[7] = makeTexturedTriangle(0.0, 3, 35.0, 1.0, point_a, point_b, point_c, center);
    poly_array[6].angle = 5.76;
    poly_array[7].angle = 2.618;
    updatePoly(&poly_array[6]);
    updatePoly(&poly_array[7]);
}