/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jerzy Kasenbreg
 * Copyright (c) 2021 Koji KITAYAMA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef _USB_DESCRIPTORS_H_
#define _USB_DESCRIPTORS_H_

/* Time stamp base clock. It is a deprecated parameter. */
#define UVC_CLOCK_FREQUENCY 27000000
/* video capture path */
#define UVC_ENTITY_CAP_INPUT_TERMINAL  0x01
#define UVC_ENTITY_CAP_OUTPUT_TERMINAL 0x02

#define FRAME_WIDTH                    160
#define FRAME_HEIGHT                   120
#define FRAME_RATE                     9

enum { ITF_NUM_VIDEO_CONTROL, ITF_NUM_VIDEO_STREAMING, ITF_NUM_TOTAL };

// clang-format off
#define TUD_VIDEO_GUID_Y8     0x59,0x38,0x20,0x20,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71
#define TUD_VIDEO_GUID_Y16    0x59,0x31,0x36,0x20,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71
#define TUD_VIDEO_GUID_BGR3   0x7d,0xeb,0x36,0xe4,0x4f,0x52,0xce,0x11,0x9f,0x53,0x00,0x20,0xaf,0x0b,0xa7,0x70

#define TUD_VIDEO_CAPTURE_DESC_UNCOMPR_LEN (\
    TUD_VIDEO_DESC_IAD_LEN\
    /* control */\
    + TUD_VIDEO_DESC_STD_VC_LEN\
    + (TUD_VIDEO_DESC_CS_VC_LEN + 1/*bInCollection*/)\
    + TUD_VIDEO_DESC_CAMERA_TERM_LEN\
    + TUD_VIDEO_DESC_OUTPUT_TERM_LEN\
    /* Interface 1, Alternate 0 */\
    + TUD_VIDEO_DESC_STD_VS_LEN\
    + (TUD_VIDEO_DESC_CS_VS_IN_LEN + 1/*bNumFormats x bControlSize*/)\
    + TUD_VIDEO_DESC_CS_VS_FMT_UNCOMPR_LEN\
    + TUD_VIDEO_DESC_CS_VS_FRM_UNCOMPR_CONT_LEN\
    + TUD_VIDEO_DESC_CS_VS_COLOR_MATCHING_LEN\
    /* Interface 1, Alternate 1 */\
    + TUD_VIDEO_DESC_STD_VS_LEN\
    + 7/* Endpoint */\
  )

 /* Windows support YUY2 and NV12
  * https://docs.microsoft.com/en-us/windows-hardware/drivers/stream/usb-video-class-driver-overview */

#define TUD_VIDEO_DESC_CS_VS_FMT_Y8(_fmtidx, _numfmtdesc, _frmidx, _asrx, _asry, _interlace, _cp) \
    TUD_VIDEO_DESC_CS_VS_FMT_UNCOMPR(_fmtidx, _numfmtdesc, TUD_VIDEO_GUID_Y8, 8, _frmidx, _asrx, _asry, _interlace, _cp)
#define TUD_VIDEO_DESC_CS_VS_FRM_CONT_Y8(_frmidx, _cap, _width, _height, _fps) \
    TUD_VIDEO_DESC_CS_VS_FRM_UNCOMPR_CONT(_frmidx, _cap, _width, _height, _width * _height * 8, _width * _height * 8 * _fps, _width * _height * 8, (10000000 / _fps), (10000000 / _fps), (10000000 / _fps), (10000000 / _fps))

#define TUD_VIDEO_DESC_CS_VS_FMT_Y16(_fmtidx, _numfmtdesc, _frmidx, _asrx, _asry, _interlace, _cp) \
    TUD_VIDEO_DESC_CS_VS_FMT_UNCOMPR(_fmtidx, _numfmtdesc, TUD_VIDEO_GUID_Y16, 16, _frmidx, _asrx, _asry, _interlace, _cp)
#define TUD_VIDEO_DESC_CS_VS_FRM_CONT_Y16(_frmidx, _cap, _width, _height, _fps) \
    TUD_VIDEO_DESC_CS_VS_FRM_UNCOMPR_CONT(_frmidx, _cap, _width, _height, _width * _height * 16, _width * _height * 16 * _fps, _width * _height * 16, (10000000 / _fps), (10000000 / _fps), (10000000 / _fps), (10000000 / _fps))

#define TUD_VIDEO_DESC_CS_VS_FMT_BGR3(_fmtidx, _numfmtdesc, _frmidx, _asrx, _asry, _interlace, _cp) \
    TUD_VIDEO_DESC_CS_VS_FMT_UNCOMPR(_fmtidx, _numfmtdesc, TUD_VIDEO_GUID_BGR3, 24, _frmidx, _asrx, _asry, _interlace, _cp)
#define TUD_VIDEO_DESC_CS_VS_FRM_CONT_BGR3(_frmidx, _cap, _width, _height, _fps) \
    TUD_VIDEO_DESC_CS_VS_FRM_UNCOMPR_CONT(_frmidx, _cap, _width, _height, _width * _height * 24, _width * _height * 24 * _fps, _width * _height * 24, (10000000 / _fps), (10000000 / _fps), (10000000 / _fps), (10000000 / _fps))

#define TUD_VIDEO_DESC_CS_VS_FMT_YUY2(_fmtidx, _numfmtdesc, _frmidx, _asrx, _asry, _interlace, _cp) \
    TUD_VIDEO_DESC_CS_VS_FMT_UNCOMPR(_fmtidx, _numfmtdesc, TUD_VIDEO_GUID_YUY2, 24, _frmidx, _asrx, _asry, _interlace, _cp)
#define TUD_VIDEO_DESC_CS_VS_FRM_CONT_YUY2(_frmidx, _cap, _width, _height, _fps) \
    TUD_VIDEO_DESC_CS_VS_FRM_UNCOMPR_CONT(_frmidx, _cap, _width, _height, _width * _height * 16, _width * _height * 16 * _fps, _width * _height * 16, (10000000 / _fps), (10000000 / _fps), (10000000 / _fps), (10000000 / _fps))


#define TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR(_stridx, _epin, _width, _height, _fps, _epsize)                       \
    TUD_VIDEO_DESC_IAD(ITF_NUM_VIDEO_CONTROL, /* 2 Interfaces */ 0x02, _stridx), /* Video control 0 */             \
    TUD_VIDEO_DESC_STD_VC(ITF_NUM_VIDEO_CONTROL, 0, _stridx),                                                      \
    TUD_VIDEO_DESC_CS_VC(                                                                                          \
        /* UVC 1.5*/ 0x0150, /* wTotalLength - bLength */                                                          \
        TUD_VIDEO_DESC_CAMERA_TERM_LEN + TUD_VIDEO_DESC_OUTPUT_TERM_LEN, UVC_CLOCK_FREQUENCY,                      \
        ITF_NUM_VIDEO_STREAMING),                                                                                  \
    TUD_VIDEO_DESC_CAMERA_TERM(                                                                                    \
        UVC_ENTITY_CAP_INPUT_TERMINAL, 0, 0, /*wObjectiveFocalLengthMin*/ 0, /*wObjectiveFocalLengthMax*/ 0,       \
        /*wObjectiveFocalLength*/ 0, /*bmControls*/ 0),                                                            \
    TUD_VIDEO_DESC_OUTPUT_TERM(                                                                                    \
        UVC_ENTITY_CAP_OUTPUT_TERMINAL, VIDEO_TT_STREAMING, 0, 1, 0), /* Video stream alt. 0 */                    \
    TUD_VIDEO_DESC_STD_VS(                                                                                         \
        ITF_NUM_VIDEO_STREAMING, 0, 0, _stridx), /* Video stream header for without still image capture */         \
    TUD_VIDEO_DESC_CS_VS_INPUT(                                                                                    \
        /*bNumFormats*/ 1, /*wTotalLength - bLength */                                                             \
        TUD_VIDEO_DESC_CS_VS_FMT_UNCOMPR_LEN + TUD_VIDEO_DESC_CS_VS_FRM_UNCOMPR_CONT_LEN +                         \
            TUD_VIDEO_DESC_CS_VS_COLOR_MATCHING_LEN,                                                               \
        _epin, /*bmInfo*/ 0, /*bTerminalLink*/ UVC_ENTITY_CAP_OUTPUT_TERMINAL, /*bStillCaptureMethod*/ 0,          \
        /*bTriggerSupport*/ 0, /*bTriggerUsage*/ 0, /*bmaControls(1)*/ 0), /* Video stream format */               \
    TUD_VIDEO_DESC_CS_VS_FMT_YUY2(                                                                                 \
        /*bFormatIndex*/ 1, /*bNumFrameDescriptors*/ 1, /*bDefaultFrameIndex*/ 1, 0, 0, 0,                         \
        /*bCopyProtect*/ 0), /* Video stream frame format */                                                       \
    TUD_VIDEO_DESC_CS_VS_FRM_CONT_YUY2(                                                                           \
        /*bFrameIndex */ 1, 0, _width, _height, _fps),                                                             \
    TUD_VIDEO_DESC_CS_VS_COLOR_MATCHING(                                                                           \
        VIDEO_COLOR_PRIMARIES_BT709, VIDEO_COLOR_XFER_CH_BT709, VIDEO_COLOR_COEF_SMPTE170M), /* VS alt 1 */        \
    TUD_VIDEO_DESC_STD_VS(ITF_NUM_VIDEO_STREAMING, 1, 1, _stridx),                           /* EP */              \
    TUD_VIDEO_DESC_EP_ISO(_epin, _epsize, 1)
// clang-format on

#endif