//TEST(compute, vulkan):COMPARE_COMPUTE(filecheck-buffer=BUF):-vk -compute -entry computeMain -allow-glsl
//TEST(compute, vulkan):COMPARE_COMPUTE(filecheck-buffer=BUF):-vk -compute -entry computeMain -allow-glsl -emit-spirv-via-glsl

#version 450

//TEST_INPUT:ubuffer(data=[0xA802 0x1349 0xC2 0x91 0xB2 0x72], stride=4):out,name=inputBuffer
layout(scalar) buffer MyBlock1
{
    uint32_t a;
    uint32_t b;

    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
} inputBuffer;
// BUF: A802
// BUF-NEXT: 1349
// BUF-NEXT: C2
// BUF-NEXT: 91
// BUF-NEXT: B2
// BUF-NEXT: 72

//TEST_INPUT:ubuffer(data=[0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0], stride=4):out,name=outputBuffer
layout(scalar) buffer MyBlock2
{
    uvec4 a;
    ivec4 b;

    uvec2 c;
    ivec2 d;

    uvec2 e;
    ivec2 f;

    uint32_t g;
    int32_t h;

    uint32_t i;
    int32_t j;

    uint32_t k;
    int32_t l;

    uint32_t m;
    uint32_t n;
} outputBuffer;

layout(local_size_x = 1) in;
void computeMain()
{
    uint32_t a = 0xF2845678;
    outputBuffer.a = unpack8(a);
    // BUF-NEXT: 78
    // BUF-NEXT: 56
    // BUF-NEXT: 84
    // BUF-NEXT: F2

    int32_t b = 0xF2845678;
    outputBuffer.b = unpack8(b);
    // BUF: 78
    // BUF-NEXT: 56
    // BUF-NEXT: FFFFFF84
    // BUF-NEXT: FFFFFFF2

    uint16_t c = 0xF256;
    outputBuffer.c = unpack8(c);
    // BUF-NEXT: 56
    // BUF-NEXT: F2

    int16_t d = 0xF256;
    outputBuffer.d = unpack8(d);
    // BUF-NEXT: 56
    // BUF-NEXT: FFFFFFF2

    uint32_t e = 0xF2845678;
    outputBuffer.e = unpack16(e);
    // BUF-NEXT: 5678
    // BUF-NEXT: F284

    int32_t f = 0xF2845678;
    outputBuffer.f = unpack16(f);
    // BUF-NEXT: 5678
    // BUF-NEXT: FFFFF284

    u16vec2 g = {0xF256, 0x1234};
    outputBuffer.g = pack32(g);
    // BUF-NEXT: 1234F256

    i16vec2 h = {0xF256, 0x1234};
    outputBuffer.h = pack32(h);
    // BUF-NEXT: 1234F256

    u8vec4 i = {0xF2, 0x56, 0x12, 0x34};
    outputBuffer.i = pack32(i);
    // BUF-NEXT: 341256F2

    i8vec4 j = {0x82, 0x56, 0x12, 0x80};
    outputBuffer.j = pack32(j);
    // BUF-NEXT: 80125682

    // Note: Below tests are mainly to verify that we don't emit invalid spirv code
    u16vec2 k = {inputBuffer.a, inputBuffer.b};
    outputBuffer.k = pack32(k);
    // BUF-NEXT: 1349A802

    i16vec2 l = {inputBuffer.a, inputBuffer.b};
    outputBuffer.l = pack32(l);
    // BUF-NEXT: 1349A802

    u8vec4 m = {inputBuffer.c, inputBuffer.d, inputBuffer.e, inputBuffer.f};
    outputBuffer.m = pack32(m);
    // BUF-NEXT: 72B291C2

    u8vec4 n = {inputBuffer.c, inputBuffer.d, inputBuffer.e, inputBuffer.f};
    outputBuffer.n = pack32(n);
    // BUF-NEXT: 72B291C2
}
