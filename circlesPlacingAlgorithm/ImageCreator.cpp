#include "ImageCreator.hpp"

#include <objidl.h>
#include <gdiplus.h>
#include <stdexcept>

namespace imagecreator {
    struct GdiplusInit {
        GdiplusInit() {
            Gdiplus::GdiplusStartupInput inp;
            Gdiplus::GdiplusStartupOutput outp;
            if (Gdiplus::Ok != Gdiplus::GdiplusStartup(&token_, &inp, &outp))
                throw std::runtime_error("GdiplusStartup");
        }
        ~GdiplusInit() {
            Gdiplus::GdiplusShutdown(token_);
        }
    private:
        ULONG_PTR token_;
    };

    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
    {
        UINT  num = 0;          // number of image encoders
        UINT  size = 0;         // size of the image encoder array in bytes

        Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

        Gdiplus::GetImageEncodersSize(&num, &size);
        if (size == 0)
            return -1;  // Failure

        pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
        if (pImageCodecInfo == NULL)
            return -1;  // Failure

        Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT j = 0; j < num; ++j)
        {
            if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
            {
                *pClsid = pImageCodecInfo[j].Clsid;
                free(pImageCodecInfo);
                return j;  // Success
            }
        }

        free(pImageCodecInfo);
        return -1;  // Failure
    }

    void createResultingImage(const objects::Scene& scene, const objects::ResultData& results, const std::string& path) {
        GdiplusInit gdiplusinit;

        int offset = 10;
        int scale = 10;

        int width = (scene.getZone().maxPoint().x - scene.getZone().minPoint().x) * scale;
        int height = (scene.getZone().maxPoint().y - scene.getZone().minPoint().y) * scale;

        Gdiplus::Bitmap myBitmap(width + offset * 2, height + offset * 2, PixelFormat32bppARGB);
        Gdiplus::Graphics g(&myBitmap);
        Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 3);
        Gdiplus::SolidBrush brush(Gdiplus::Color(150, 0, 0, 0));
        Gdiplus::SolidBrush weak_brush(Gdiplus::Color(50, 0, 0, 0));

        g.Clear(Gdiplus::Color::White);
        for (auto& area : scene.getExclusionAreas()) {
            Gdiplus::REAL x0 = (area.minPoint().x - scene.getZone().minPoint().x) * scale;
            Gdiplus::REAL x = (area.maxPoint().x - area.minPoint().x) * scale;
            Gdiplus::REAL y0 = (scene.getZone().maxPoint().y - area.maxPoint().y) * scale;
            Gdiplus::REAL y = (area.maxPoint().y - area.minPoint().y) * scale;
            g.FillRectangle(&brush, x0 + offset, y0 + offset, x, y);
            g.DrawRectangle(&pen, x0 + offset, y0 + offset, x, y);
        }
        for (auto& c : results.circles) {
            double outRad = c.outRad();
            double inRad = c.inRad();
            Gdiplus::REAL x = (c.position.x - scene.getZone().minPoint().x - outRad) * scale;
            Gdiplus::REAL y = (scene.getZone().maxPoint().y - c.position.y - outRad) * scale;
            Gdiplus::REAL r = outRad * 2.0 * scale;
            g.DrawEllipse(&pen, x + offset, y + offset, r, r);
            g.FillEllipse(&weak_brush, x + offset, y + offset, r, r);

            Gdiplus::REAL x0 = (c.position.x - scene.getZone().minPoint().x - inRad) * scale;
            Gdiplus::REAL y0 = (scene.getZone().maxPoint().y - c.position.y - inRad) * scale;
            Gdiplus::REAL r0 = inRad * 2.0 * scale;
            g.DrawEllipse(&pen, x0 + offset, y0 + offset, r0, r0);
        }
        g.DrawRectangle(&pen, offset, offset, width, height);

        std::wstring widestr = std::wstring(path.begin(), path.end());
        CLSID pngClsid;
        int result = GetEncoderClsid(L"image/png", &pngClsid);
        if (result == -1)
            throw std::runtime_error("GetEncoderClsid");
        if (Gdiplus::Ok != myBitmap.Save(widestr.c_str(), &pngClsid, NULL))
            throw std::runtime_error("Bitmap::Save");
    }

}