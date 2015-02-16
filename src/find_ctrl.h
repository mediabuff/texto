#include "stdafx.h"
// TExTO.  Copyright 2014, Carlos Pizano (carlos.pizano@gmail.com)
// TExTO is a text editor prototype this is the find-in-document
// control.

class FindControl {
  const plx::DPI& dpi_;
  D2D1_SIZE_F box_;
  plx::ComPtr<IDCompositionVisual2> visual_;
  plx::ComPtr<IDCompositionVisual2> root_visual_;
  plx::ComPtr<IDCompositionSurface> surface_;
  plx::ComPtr<IDWriteFactory> dwrite_factory_;

  plx::ComPtr<IDWriteTextLayout> dwrite_layout_;
  plx::ComPtr<IDWriteTextFormat> dwrite_fmt_;

  enum BrushesHover {
    brush_text,
    brush_background,
    brush_last
  };
  plx::D2D1BrushManager brushes_;

  const int width_ = 200;
  const int height_ = 60;
  
public:
  FindControl(const plx::DPI& dpi,
              plx::ComPtr<IDCompositionDesktopDevice> dco_device,
              plx::ComPtr<IDCompositionVisual2> root_visual,
              plx::ComPtr<IDWriteFactory> dwrite_factory) 
      : dpi_(dpi),
        box_(D2D1::SizeF(160.0f, 20.0f)),
        root_visual_(root_visual),
        dwrite_factory_(dwrite_factory),
        brushes_(brush_last) {

    surface_ = plx::CreateDCoSurface(
        dco_device,
        static_cast<unsigned int>(dpi_.to_physical_x(width_)),
        static_cast<unsigned int>(dpi_.to_physical_x(height_)));

    visual_ = plx::CreateDCoVisual(dco_device);
    visual_->SetContent(surface_.Get());
    root_visual_->AddVisual(visual_.Get(), TRUE, nullptr);

    {
      plx::ScopedD2D1DeviceContext dc(surface_, D2D1::SizeF(), dpi, nullptr);
      brushes_.set_solid(dc(), brush_text, 0xD68739, 1.0f);
      brushes_.set_solid(dc(), brush_background, 0x1E5D81, 0.5f);
    }

    dwrite_fmt_ = plx::CreateDWriteSystemTextFormat(
        dwrite_factory_, L"Consolas", 14.0f, plx::FontWSSParams::MakeNormal());

    update_layout(L"text to search");
    draw();
  }

  ~FindControl() {
    brushes_.release_all();
    root_visual_->RemoveVisual(visual_.Get());
  }
  
  void set_position(float x, float y) {
    visual_->SetOffsetX(x);
    visual_->SetOffsetY(y);
  }

private:
  void update_layout(const std::wstring& text) {
    plx::Range<const wchar_t> r(&text[0], text.size());
    dwrite_layout_ = plx::CreateDWTextLayout(
        dwrite_factory_, dwrite_fmt_, r, box_);
  }

  void draw() {
    D2D1::ColorF bk_color(0x0, 0.1f);
    plx::ScopedD2D1DeviceContext dc(surface_, D2D1::SizeF(), dpi_, &bk_color);

    dc()->FillRoundedRectangle(
        D2D1::RoundedRect(
            D2D1::Rect(3.0f, 3.0f, width_ - 6.0f, height_ - 6.0f), 3.0f, 3.0f),
        brushes_.solid(brush_background));

    dc()->DrawTextLayout(
        D2D1::Point2F(10.0f, 10.0f), dwrite_layout_.Get(), brushes_.solid(brush_text));
  }

};
 
