#include "pch.h"

#include "WGLGraphics.h"

using namespace System::Threading::Tasks;
using namespace System::Windows;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;
using namespace System::Windows::Controls;

namespace WGLGraphics
{
    int _w = 0, _h = 0, fpsCounter = 0;
	bool isInitialized = false;

    void WGLGraphics::GLControl::OnRenderSizeChanged(System::Windows::SizeChangedInfo^ info)
    {
        _w = (int)info->NewSize.Width;
        _h = (int)info->NewSize.Height;

        if (!isInitialized)
        {
            Grid^ mainGrid = gcnew Grid();
            m_fpsCounter = gcnew TextBlock();
            m_fpsCounter->Margin = Thickness(3);
            m_fpsCounter->VerticalAlignment = System::Windows::VerticalAlignment::Bottom;

            m_lastUpdate = System::DateTime::Now;

            m_renderTimer = gcnew System::Windows::Threading::DispatcherTimer(System::Windows::Threading::DispatcherPriority::Render);
            m_renderTimer->Interval = System::TimeSpan::FromMilliseconds(16);
            m_renderTimer->Tick += gcnew System::EventHandler(this, &WGLGraphics::GLControl::OnTick);
            m_renderTimer->Start();

            m_ImageControl = gcnew Image();
            m_ImageControl->RenderTransformOrigin = Point(0.5, 0.5);
            m_ImageControl->RenderTransform = gcnew ScaleTransform(1.0, -1.0);

            mainGrid->Children->Add(m_ImageControl);
            mainGrid->Children->Add(m_fpsCounter);

			AddChild(mainGrid);

            System::Windows::Controls::Panel::SetZIndex(m_ImageControl, -1);

            // m_graphicsEngine = new MonoMaxGraphics::GraphicsEngine();
			// m_graphicsEngine->Init();

            isInitialized = true;
        }
        
        // m_graphicsEngine->Resize(_w, _h);
        m_writeableImg = gcnew WriteableBitmap(_w, _h, 96, 96, PixelFormats::Pbgra32, nullptr);
        m_WriteableBuffer = (char*)m_writeableImg->BackBuffer.ToPointer();
        m_ImageControl->Source = m_writeableImg;
    }

    void GLControl::Destroy(void)
	{
		// delete m_graphicsEngine;
	}

    void GLControl::UpdateImageData()
    {
        m_writeableImg->Lock();
        m_writeableImg->AddDirtyRect(Int32Rect(0, 0, _w, _h));
        m_writeableImg->Unlock();
    }

    void GLControl::OnTick(System::Object^ sender, System::EventArgs^ e)
    {
        System::TimeSpan elaped = (System::DateTime::Now - m_lastUpdate);
        if (elaped.TotalMilliseconds >= 1000)
        {
            m_fpsCounter->Text = "FPS= " + fpsCounter.ToString();
            fpsCounter = 0;
            m_lastUpdate = System::DateTime::Now;
        }

        // m_graphicsEngine->Render(m_WriteableBuffer);
        m_ImageControl->Dispatcher->Invoke(gcnew System::Action(this, &GLControl::UpdateImageData));

        fpsCounter++;
    }

}
