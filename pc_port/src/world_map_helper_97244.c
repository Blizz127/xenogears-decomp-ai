/*
 * World-map helpers 0x80097244 and 0x80097440 — GTE matrix builders.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_97244.h"

#define SCRATCH 0x1F800000u

static s16 m_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 m_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 m_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void m_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void m_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_80097244(u32 input_data)
{
    VECTOR edge1, edge2, edge3;
    VECTOR norm1, norm2, norm3;

    /* Compute 3 edge vectors from input data (6 halfwords per edge) */
    edge1.vx = (s32)m_lh(input_data + 0x08) - (s32)m_lh(input_data + 0x00);
    edge1.vy = (s32)m_lh(input_data + 0x0A) - (s32)m_lh(input_data + 0x02);
    edge1.vz = (s32)m_lh(input_data + 0x0C) - (s32)m_lh(input_data + 0x04);

    /* Normalize edge1 */
    VectorNormal(&edge1, (VECTOR*)PSX_ADDR(SCRATCH + 0x30));

    /* Compute edge2 and normalize */
    edge2.vx = (s32)m_lh(input_data + 0x10);
    edge2.vy = (s32)m_lh(input_data + 0x12);
    edge2.vz = (s32)m_lh(input_data + 0x14);
    VectorNormal(&edge2, (VECTOR*)PSX_ADDR(SCRATCH + 0x10));

    /* Cross product edge1 × edge2 → scratch+0x48 */
    OuterProduct12((VECTOR*)PSX_ADDR(SCRATCH + 0x30),
                   (VECTOR*)PSX_ADDR(SCRATCH + 0x10),
                   (VECTOR*)PSX_ADDR(SCRATCH + 0x48));

    /* Compute edge3 and normalize */
    edge3.vx = (s32)m_lh(input_data + 0x20);
    edge3.vy = (s32)m_lh(input_data + 0x22);
    edge3.vz = (s32)m_lh(input_data + 0x24);
    VectorNormal(&edge3, (VECTOR*)PSX_ADDR(SCRATCH + 0x20));

    /* Cross product edge2 × edge3 → scratch+0x54 */
    OuterProduct12((VECTOR*)PSX_ADDR(SCRATCH + 0x20),
                   (VECTOR*)PSX_ADDR(SCRATCH + 0x10),
                   (VECTOR*)PSX_ADDR(SCRATCH + 0x54));

    /* Cross product edge3 × edge1 → scratch+0x60 */
    OuterProduct12((VECTOR*)PSX_ADDR(SCRATCH + 0x30),
                   (VECTOR*)PSX_ADDR(SCRATCH + 0x20),
                   (VECTOR*)PSX_ADDR(SCRATCH + 0x60));

    /* Build matrix from normalized vectors */
    {
        MATRIX* m = (MATRIX*)PSX_ADDR(SCRATCH + 0x48);
        VECTOR* n1 = (VECTOR*)PSX_ADDR(SCRATCH + 0x30);
        VECTOR* n2 = (VECTOR*)PSX_ADDR(SCRATCH + 0x10);
        VECTOR* n3 = (VECTOR*)PSX_ADDR(SCRATCH + 0x20);

        m->m[0][0] = (short)(n1->vx >> 4);
        m->m[0][1] = (short)(n1->vy >> 4);
        m->m[0][2] = (short)(n1->vz >> 4);
        m->m[1][0] = (short)(n2->vx >> 4);
        m->m[1][1] = (short)(n2->vy >> 4);
        m->m[1][2] = (short)(n2->vz >> 4);
        m->m[2][0] = (short)(n3->vx >> 4);
        m->m[2][1] = (short)(n3->vy >> 4);
        m->m[2][2] = (short)(n3->vz >> 4);
    }

    /* Store matrix to output */
    {
        MATRIX* src = (MATRIX*)PSX_ADDR(SCRATCH + 0x48);
        MATRIX* dst = (MATRIX*)PSX_ADDR(input_data + 0x10);
        memcpy(dst, src, sizeof(MATRIX));
    }
}

void wm_80097440(u32 input_data)
{
    MATRIX temp;
    SVECTOR rot;

    /* Apply rotation matrix composition */
    MATRIX* m = (MATRIX*)PSX_ADDR(input_data);

    /* RotMatrixX */
    rot.vx = (s16)m_lh(input_data + 0x20);
    rot.vy = 0;
    rot.vz = 0;
    RotMatrixX((long)rot.vx, &temp);

    /* MulMatrix0 with input matrix */
    MulMatrix0(m, &temp, m);

    /* RotMatrixY */
    rot.vx = 0;
    rot.vy = (s16)m_lh(input_data + 0x22);
    RotMatrixY((long)rot.vy, &temp);
    MulMatrix0(m, &temp, m);

    /* RotMatrixZ */
    rot.vy = 0;
    rot.vz = (s16)m_lh(input_data + 0x24);
    RotMatrixZ((long)rot.vz, &temp);
    MulMatrix0(m, &temp, m);

    /* ApplyMatrix for position */
    {
        VECTOR pos;
        pos.vx = m_lw(input_data + 0x28);
        pos.vy = m_lw(input_data + 0x2C);
        pos.vz = m_lw(input_data + 0x30);
        ApplyMatrix(m, (SVECTOR*)&pos, &pos);
    }

    /* TransMatrix */
    {
        VECTOR t;
        t.vx = m_lw(input_data + 0x34);
        t.vy = m_lw(input_data + 0x38);
        t.vz = m_lw(input_data + 0x3C);
        TransMatrix(m, &t);
    }
}
