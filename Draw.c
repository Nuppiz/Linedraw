#include "Common.h"
#include "Defines.h"
#include "Structs.h"
#include "Video.h"

/* Graphics drawing functions */

extern uint8_t far screen_buf [];
extern Polygon poly_array [];
extern Line line_array [];
extern Line_int intline_array [];
extern Mesh2D CarMesh;
extern Texture CarTex;
extern Mesh3D Cube;
extern uint8_t* g_Keyboard;

void drawSprite(int x, int y, Texture* texture)
{
    int pix_x = x;
    int pix_y = y;
    int index_x = 0;
    int index_y = 0;
    int i = 0;

    if (texture->transparent == TRUE)
    {
        for (index_y=0;index_y<texture->height;index_y++)
        {
            for (index_x=0;index_x<texture->width;index_x++)
            {
                if (texture->pixels[i] != TRANSPARENT_COLOR)
                {
                    SET_PIXEL(pix_x, pix_y, texture->pixels[i]);
                    i++;
                    pix_x++;
                }
                else
                {
                    i++;
                    pix_x++;
                }
            }
            index_x = 0;
            pix_x = x;
            pix_y++;
        }
    }
    else
    {
        for (index_y=0;index_y<texture->height;index_y++)
        {
            for (index_x=0;index_x<texture->width;index_x++)
            {
                SET_PIXEL(pix_x, pix_y, texture->pixels[i]);
                i++;
                pix_x++;
            }
            index_x = 0;
            pix_x = x;
            pix_y++;
        }
    }
}

void drawPolygon(Polygon* poly, int center_x, int center_y)
{
    int i = 0;
    Vec2 start_loc;
    Vec2 end_loc;
    
    while (i < poly->numPoints - 1)
    {
        start_loc.x = center_x + poly->transformedV[i].x;
        start_loc.y = center_y + poly->transformedV[i].y;
        
        end_loc.x = center_x + poly->transformedV[i + 1].x;
        end_loc.y = center_y + poly->transformedV[i + 1].y;

        drawLine(start_loc, end_loc, poly->color);
        
        i++;
    }
    
    start_loc.x = center_x + poly->transformedV[i].x;
    start_loc.y = center_y + poly->transformedV[i].y;
    
    end_loc.x = center_x + poly->transformedV[0].x;
    end_loc.y = center_y + poly->transformedV[0].y;
    
    drawLine(start_loc, end_loc, poly->color);
}

void drawPolygonInt(Polygon* poly, int center_x, int center_y)
{
    int i = 0;
    Vec2_int start_loc;
    Vec2_int end_loc;
    
    while (i < poly->numPoints - 1)
    {
        start_loc.x = center_x + (int)poly->transformedV[i].x;
        start_loc.y = center_y + (int)poly->transformedV[i].y;
        
        end_loc.x = center_x + (int)poly->transformedV[i + 1].x;
        end_loc.y = center_y + (int)poly->transformedV[i + 1].y;

        drawLineInt(start_loc, end_loc, poly->color);
        
        i++;
    }
    
    start_loc.x = center_x + (int)poly->transformedV[i].x;
    start_loc.y = center_y + (int)poly->transformedV[i].y;
    
    end_loc.x = center_x + (int)poly->transformedV[0].x;
    end_loc.y = center_y + (int)poly->transformedV[0].y;
    
    drawLineInt(start_loc, end_loc, poly->color);
}

void drawLines()
{
    int i = 0;
    
    while (i < 5)
    {
        drawLine(line_array[i].startpos, line_array[i].endpos, line_array[i].color);
        i++;
    }
    drawLineInt(intline_array[0].startpos, intline_array[0].endpos, line_array[0].color);
    drawLineInt(intline_array[1].startpos, intline_array[1].endpos, line_array[1].color);
}

void drawPolygons()
{
    int i = 0;
    int x = 100;
    int y = 100;
    
    while (i < 3)
    {
        drawPolygon(&poly_array[i], x, y);
        x += 45;
        i++;
    }
    drawPolygonInt(&poly_array[i], 60, 150);
}

void drawStuff()
{
    //drawLines();
    /*if (KEY_IS_PRESSED (KEY_1))
        drawPolygons();
    //test_draw();
    //drawFilledTriangle(&poly_array[5]);
    else if (KEY_IS_PRESSED (KEY_2))
        drawShadedTriangle(&poly_array[5]);
    //drawTexturedTriangle(&poly_array[6]);
    //drawTexturedTriangle(&poly_array[7]);
    //drawSprite(144, 68, &CarTex);
    else if (KEY_IS_PRESSED (KEY_3))
        draw2DMesh(&CarMesh);    
    else if (KEY_IS_PRESSED (KEY_4))*/
    //drawFilled3DCube(&Cube);
    drawTextured3DCube(&Cube);
    //draw3DCube(&Cube);
}