#[[
A library responsible for custom rendering in Audacity: wxWidgets specific code.
]]

set( SOURCES
   graphics/WXColor.cpp
   graphics/WXColor.h
   graphics/WXFontUtils.cpp
   graphics/WXFontUtils.h
   graphics/WXGraphicsContextPainter.cpp
   graphics/WXGraphicsContextPainter.h
   graphics/WXPainterFactory.cpp
   graphics/WXPainterFactory.h
   graphics/WXPainterUtils.cpp
   graphics/WXPainterUtils.h

   ########################################
   # Additional rendering backend loaders
   ########################################
   $<$<PLATFORM_ID:Windows>:loaders/D2DLoader.cpp>
)
set( LIBRARIES
   PUBLIC
      lib-graphics-interface
   PRIVATE
      lib-math-interface
      lib-screen-geometry-interface
      $<$<PLATFORM_ID:Windows>:lib-graphics-d2d-interface>
      wxwidgets::base
      wxwidgets::core
)

audacity_library( lib-graphics-wx "${SOURCES}" "${LIBRARIES}"
   "" ""
)
