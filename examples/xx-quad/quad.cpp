/*
 * Copyright 2011-2016 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "common.h"
#include "bgfx_utils.h"

static float g_texelHalf = 0.0f;
static bool  g_originBottomLeft = false;

struct PosColorTexCoord0Vertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_rgba;
    float m_u;
    float m_v;

    static void init()
    {
        ms_decl.begin()
               .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
               .add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
               .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
               .end();
    }

    static bgfx::VertexDecl ms_decl;
};
bgfx::VertexDecl PosColorTexCoord0Vertex::ms_decl;

void screenQuad(float _x
              , float _y
              , float _width
              , float _height
              , bool _originBottomLeft = g_originBottomLeft
              , bool _texelHalf = g_texelHalf
              )
{
    if (bgfx::checkAvailTransientVertexBuffer(6, PosColorTexCoord0Vertex::ms_decl) )
    {
        bgfx::TransientVertexBuffer vb;
        bgfx::allocTransientVertexBuffer(&vb, 6, PosColorTexCoord0Vertex::ms_decl);
        PosColorTexCoord0Vertex* vertex = (PosColorTexCoord0Vertex*)vb.data;

        const float minx = _x;
        const float miny = _y;
        const float maxx = minx+_width;
        const float maxy = miny+_height;
        const float zz = 0.0f;

        const float texelHalfW = _texelHalf/_width;
        const float texelHalfH = _texelHalf/_height;
        const float minu = texelHalfW;
        const float maxu = 1.0f - texelHalfW;
        const float minv = _originBottomLeft ? texelHalfH+1.0f : texelHalfH     ;
        const float maxv = _originBottomLeft ? texelHalfH      : texelHalfH+1.0f;

        vertex[0].m_x = minx;
        vertex[0].m_y = miny;
        vertex[0].m_z = zz;
        vertex[0].m_rgba = 0xffffffff;
        vertex[0].m_u = minu;
        vertex[0].m_v = minv;

        vertex[1].m_x = maxx;
        vertex[1].m_y = miny;
        vertex[1].m_z = zz;
        vertex[1].m_rgba = 0xffffffff;
        vertex[1].m_u = maxu;
        vertex[1].m_v = minv;

        vertex[2].m_x = maxx;
        vertex[2].m_y = maxy;
        vertex[2].m_z = zz;
        vertex[2].m_rgba = 0xffffffff;
        vertex[2].m_u = maxu;
        vertex[2].m_v = maxv;

        vertex[3].m_x = maxx;
        vertex[3].m_y = maxy;
        vertex[3].m_z = zz;
        vertex[3].m_rgba = 0xffffffff;
        vertex[3].m_u = maxu;
        vertex[3].m_v = maxv;

        vertex[4].m_x = minx;
        vertex[4].m_y = maxy;
        vertex[4].m_z = zz;
        vertex[4].m_rgba = 0xffffffff;
        vertex[4].m_u = minu;
        vertex[4].m_v = maxv;

        vertex[5].m_x = minx;
        vertex[5].m_y = miny;
        vertex[5].m_z = zz;
        vertex[5].m_rgba = 0xffffffff;
        vertex[5].m_u = minu;
        vertex[5].m_v = minv;
        bgfx::setVertexBuffer(&vb);
    }
}

struct PosColorVertex
{
    float m_x;
    float m_y;
    uint32_t m_abgr;

    static void init()
    {
        ms_decl
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .end();
    }

    static bgfx::VertexDecl ms_decl;
};
bgfx::VertexDecl PosColorVertex::ms_decl;

void drawPolygon(const float* _coords, uint32_t _numCoords, float _r, uint32_t _abgr)
{
    #define MAX_TEMP_COORDS 100

    float tempCoords[MAX_TEMP_COORDS * 2];
    float tempNormals[MAX_TEMP_COORDS * 2];

    _numCoords = _numCoords < MAX_TEMP_COORDS ? _numCoords : MAX_TEMP_COORDS;

    for (uint32_t ii = 0, jj = _numCoords - 1; ii < _numCoords; jj = ii++)
    {
        const float* v0 = &_coords[jj * 2];
        const float* v1 = &_coords[ii * 2];
        float dx = v1[0] - v0[0];
        float dy = v1[1] - v0[1];
        float d = sqrtf(dx * dx + dy * dy);
        if (d > 0)
        {
            d = 1.0f / d;
            dx *= d;
            dy *= d;
        }

        tempNormals[jj * 2 + 0] = dy;
        tempNormals[jj * 2 + 1] = -dx;
    }

    for (uint32_t ii = 0, jj = _numCoords - 1; ii < _numCoords; jj = ii++)
    {
        float dlx0 = tempNormals[jj * 2 + 0];
        float dly0 = tempNormals[jj * 2 + 1];
        float dlx1 = tempNormals[ii * 2 + 0];
        float dly1 = tempNormals[ii * 2 + 1];
        float dmx = (dlx0 + dlx1) * 0.5f;
        float dmy = (dly0 + dly1) * 0.5f;
        float dmr2 = dmx * dmx + dmy * dmy;
        if (dmr2 > 0.000001f)
        {
            float scale = 1.0f / dmr2;
            if (scale > 10.0f)
            {
                scale = 10.0f;
            }

            dmx *= scale;
            dmy *= scale;
        }

        tempCoords[ii * 2 + 0] = _coords[ii * 2 + 0] + dmx * _r;
        tempCoords[ii * 2 + 1] = _coords[ii * 2 + 1] + dmy * _r;
    }

    uint32_t numVertices = _numCoords*6 + (_numCoords-2)*3;
    if (bgfx::checkAvailTransientVertexBuffer(numVertices, PosColorVertex::ms_decl) )
    {
        bgfx::TransientVertexBuffer tvb;
        bgfx::allocTransientVertexBuffer(&tvb, numVertices, PosColorVertex::ms_decl);
        uint32_t trans = _abgr&0xffffff;

        PosColorVertex* vertex = (PosColorVertex*)tvb.data;
        for (uint32_t ii = 0, jj = _numCoords-1; ii < _numCoords; jj = ii++)
        {
            vertex->m_x = _coords[ii*2+0];
            vertex->m_y = _coords[ii*2+1];
            vertex->m_abgr = _abgr;
            ++vertex;

            vertex->m_x = _coords[jj*2+0];
            vertex->m_y = _coords[jj*2+1];
            vertex->m_abgr = _abgr;
            ++vertex;

            vertex->m_x = tempCoords[jj*2+0];
            vertex->m_y = tempCoords[jj*2+1];
            vertex->m_abgr = trans;
            ++vertex;

            vertex->m_x = tempCoords[jj*2+0];
            vertex->m_y = tempCoords[jj*2+1];
            vertex->m_abgr = trans;
            ++vertex;

            vertex->m_x = tempCoords[ii*2+0];
            vertex->m_y = tempCoords[ii*2+1];
            vertex->m_abgr = trans;
            ++vertex;

            vertex->m_x = _coords[ii*2+0];
            vertex->m_y = _coords[ii*2+1];
            vertex->m_abgr = _abgr;
            ++vertex;
        }

        for (uint32_t ii = 2; ii < _numCoords; ++ii)
        {
            vertex->m_x = _coords[0];
            vertex->m_y = _coords[1];
            vertex->m_abgr = _abgr;
            ++vertex;

            vertex->m_x = _coords[(ii-1)*2+0];
            vertex->m_y = _coords[(ii-1)*2+1];
            vertex->m_abgr = _abgr;
            ++vertex;

            vertex->m_x = _coords[ii*2+0];
            vertex->m_y = _coords[ii*2+1];
            vertex->m_abgr = _abgr;
            ++vertex;
        }

        bgfx::setVertexBuffer(&tvb);
    }

    #undef MAX_TEMP_COORDS
}

void drawLine(float _x0, float _y0, float _x1, float _y1, float _r, uint32_t _abgr, float _fth = 1.0f)
{
    float dx = _x1 - _x0;
    float dy = _y1 - _y0;
    float d = sqrtf(dx * dx + dy * dy);
    if (d > 0.0001f)
    {
        d = 1.0f / d;
        dx *= d;
        dy *= d;
    }

    float nx = dy;
    float ny = -dx;
    float verts[4 * 2];
    _r -= _fth;
    _r *= 0.5f;
    if (_r < 0.01f)
    {
        _r = 0.01f;
    }

    dx *= _r;
    dy *= _r;
    nx *= _r;
    ny *= _r;

    verts[0] = _x0 - dx - nx;
    verts[1] = _y0 - dy - ny;

    verts[2] = _x0 - dx + nx;
    verts[3] = _y0 - dy + ny;

    verts[4] = _x1 + dx + nx;
    verts[5] = _y1 + dy + ny;

    verts[6] = _x1 + dx - nx;
    verts[7] = _y1 + dy - ny;

    drawPolygon(verts, 4, _fth, _abgr);
}

void initVertexDecls()
{
    PosColorTexCoord0Vertex::init();
    PosColorVertex::init();
}

static inline
uint32_t u32Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)
{
    return 0
           | (uint32_t(_r) <<  0)
           | (uint32_t(_g) <<  8)
           | (uint32_t(_b) << 16)
           | (uint32_t(_a) << 24)
           ;
}

int _main_(int _argc, char** _argv)
{
    Args args(_argc, _argv);

    uint32_t width = 1280;
    uint32_t height = 720;
    uint32_t debug = BGFX_DEBUG_TEXT;
    uint32_t reset = BGFX_RESET_VSYNC;

    bgfx::init(args.m_type, args.m_pciId);
    bgfx::reset(width, height, reset);

    // Enable debug text.
    bgfx::setDebug(debug);

    const bgfx::RendererType::Enum renderer = bgfx::getRendererType();
    g_texelHalf = bgfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
    g_originBottomLeft = bgfx::RendererType::OpenGL == renderer || bgfx::RendererType::OpenGLES == renderer;

    initVertexDecls();

    // Set view 0 clear state.
    bgfx::setViewClear(0
        , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
        , 0x303030ff
        , 1.0f
        , 0
        );

    // Create program from shaders.
    bgfx::ProgramHandle progQuad = loadProgram("vs_quad", "fs_quad");
    bgfx::ProgramHandle progLine = loadProgram("vs_line", "fs_line");

    while (!entry::processEvents(width, height, debug, reset) )
    {
        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, width, height);

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        int64_t now = bx::getHPCounter();
        static int64_t last = now;
        const int64_t frameTime = now - last;
        last = now;
        const double freq = double(bx::getHPFrequency() );
        const double toMs = 1000.0/freq;

        // Use debug font to print information about this example.
        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 1, 0x4f, "bgfx/examples/xx-quad-line");
        bgfx::dbgTextPrintf(0, 2, 0x6f, "Description: Drawing quads and line.");
        bgfx::dbgTextPrintf(0, 3, 0x0f, "Frame: % 7.3f[ms]", double(frameTime)*toMs);

        // Setup view transform.
        {
            float view[16];
            bx::mtxIdentity(view);

            float left   = -20.0f;
            float right  =  20.0f;
            float bottom =  20.0f;
            float top    = -20.0f;


            // -20,-20
            // .----------------|
            // |                |
            // |                |
            // |        0,0     |
            // |                |
            // |                |
            // |----------------| +20, +20
            float proj[16];
            bx::mtxOrtho(proj, left, right, bottom, top, 0.0f, 100.0f);
            bgfx::setViewTransform(0, view, proj);
        }

        // Draw quad.
        screenQuad(0.0f, 0.0f, 15.0f, 15.0f);
        bgfx::setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE);
        bgfx::submit(0, progQuad);

        drawLine(-2.0f, -2.0f, -6.0f, -6.0f, 1.0f/40.0f, u32Color(128, 128, 0), 1.0f/40.0f);
        bgfx::setState(BGFX_STATE_RGB_WRITE
                     | BGFX_STATE_ALPHA_WRITE
                  // | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                     );
        bgfx::submit(0, progLine);

        // Advance to next frame. Rendering thread will be kicked to
        // process submitted rendering primitives.
        bgfx::frame();
    }

    // Cleanup.
    bgfx::destroyProgram(progQuad);
    bgfx::destroyProgram(progLine);

    // Shutdown bgfx.
    bgfx::shutdown();

    return 0;
}
