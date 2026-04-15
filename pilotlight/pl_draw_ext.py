from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal
from pilotlight.pl_core import * # color constants


class plDrawI:

    def add_triangle_filled(layer, p0, p1, p2, *, color=PL_COLOR_32_WHITE, **kwargs):
        return internal.plDrawI_add_triangle_filled(layer, p0, p1, p2, color=color, **kwargs)

    def add_triangle(layer, p0, p1, p2, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
        return internal.plDrawI_add_triangle(layer, p0, p1, p2, color=color, thickness=thickness, **kwargs)

    def add_line(layer, p0, p1, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
        return internal.plDrawI_add_line(layer, p0, p1, color=color, thickness=thickness, **kwargs)
    
    def add_rect(layer, pMin, pMax, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
        return internal.plDrawI_add_rect(layer, pMin, pMax, color=color, thickness=thickness, **kwargs)
    
    def add_rect_rounded(layer, pMin, pMax, radius, segments, flags, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
        return internal.plDrawI_add_rect_rounded(layer, pMin, pMax, radius, segments, flags, color=color, thickness=thickness, **kwargs)
    
    def add_quad(layer, p0, p1, p2, p3, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
        return internal.plDrawI_add_quad(layer, p0, p1, p2, p3, color=color, thickness=thickness, **kwargs)
    
    def add_circle(layer, p, radius, segments, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
        return internal.plDrawI_add_circle(layer, p, radius, segments, color=color, thickness=thickness, **kwargs)
    
    def add_polygon(layer, points, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
        return internal.plDrawI_add_polygon(layer, points, color=color, thickness=thickness, **kwargs)

    def add_bezier_quad(layer, p0, p1, p2, segments, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
        return internal.plDrawI_add_bezier_quad(layer, p0, p1, p2, segments, color=color, thickness=thickness, **kwargs)

    def add_bezier_cubic(layer, p0, p1, p2, p3, segments, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
        return internal.plDrawI_add_bezier_cubic(layer, p0, p1, p2, p3, segments, color=color, thickness=thickness, **kwargs)
    

########################################################################################################################
# [SECTION] enums
########################################################################################################################

# PL_DRAW_FLAG_ CONSTANTS
PL_DRAW_FLAG_NONE=internal.PL_DRAW_FLAG_NONE
PL_DRAW_FLAG_DEPTH_TEST=internal.PL_DRAW_FLAG_DEPTH_TEST
PL_DRAW_FLAG_DEPTH_WRITE=internal.PL_DRAW_FLAG_DEPTH_WRITE
PL_DRAW_FLAG_CULL_FRONT=internal.PL_DRAW_FLAG_CULL_FRONT
PL_DRAW_FLAG_CULL_BACK=internal.PL_DRAW_FLAG_CULL_BACK
PL_DRAW_FLAG_FRONT_FACE_CW=internal.PL_DRAW_FLAG_FRONT_FACE_CW
PL_DRAW_FLAG_REVERSE_Z_DEPTH=internal.PL_DRAW_FLAG_REVERSE_Z_DEPTH

# PL_DRAW_RECT_FLAG_ CONSTANTS
PL_DRAW_RECT_FLAG_NONE=internal.PL_DRAW_RECT_FLAG_NONE
PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP_LEFT=internal.PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP_LEFT
PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP_RIGHT=internal.PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP_RIGHT
PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM_LEFT=internal.PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM_LEFT
PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM_RIGHT=internal.PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM_RIGHT
PL_DRAW_RECT_FLAG_ROUND_CORNERS_NONE=internal.PL_DRAW_RECT_FLAG_ROUND_CORNERS_NONE
PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP=internal.PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP
PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM=internal.PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM
PL_DRAW_RECT_FLAG_ROUND_CORNERS_LEFT=internal.PL_DRAW_RECT_FLAG_ROUND_CORNERS_LEFT
PL_DRAW_RECT_FLAG_ROUND_CORNERS_RIGHT=internal.PL_DRAW_RECT_FLAG_ROUND_CORNERS_RIGHT
PL_DRAW_RECT_FLAG_ROUND_CORNERS_All=internal.PL_DRAW_RECT_FLAG_ROUND_CORNERS_All