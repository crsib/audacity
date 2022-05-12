#include "GLFunctions.h"
/*  SPDX-License-Identifier: GPL-2.0-or-later */
/**********************************************************************

  Audacity: A Digital Audio Editor

  GLFunctions.h

  Dmitry Vedenko

**********************************************************************/

bool graphics::gl::GLFunctions::LoadFunctions()
{
   bool loaded = true;

   loaded = GetFunction(Clear, "glClear", true) || loaded;
   loaded = GetFunction(ClearColor, "glClearColor", true) || loaded;
   loaded = GetFunction(ClearDepthf, "glClearDepthf", true) || loaded;
   loaded = GetFunction(ClearStencil, "glClearStencil", true) || loaded;
   loaded = GetFunction(ColorMask, "glColorMask", true) || loaded;
   loaded = GetFunction(CullFace, "glCullFace", true) || loaded;
   loaded = GetFunction(Enable, "glEnable", true) || loaded;
   loaded = GetFunction(Disable, "glDisable", true) || loaded;
   loaded = GetFunction(Finish, "glFinish", true) || loaded;
   loaded = GetFunction(Flush, "glFlush", true) || loaded;
   loaded = GetFunction(FrontFace, "glFrontFace", true) || loaded;
   loaded = GetFunction(GetError, "glGetError", true) || loaded;
   loaded = GetFunction(GetFloatv, "glGetFloatv", true) || loaded;
   loaded = GetFunction(GetIntegerv, "glGetIntegerv", true) || loaded;
   loaded = GetFunction(GetBooleanv, "glGetBooleanv", true) || loaded;
   loaded = GetFunction(GetString, "glGetString", true) || loaded;
   loaded = GetFunction(GetStringi, "glGetStringi", true) || loaded;
   loaded = GetFunction(Hint, "glHint", true) || loaded;
   loaded = GetFunction(PixelStorei, "glPixelStorei", true) || loaded;
   loaded = GetFunction(PolygonOffset, "glPolygonOffset", true) || loaded;
   loaded = GetFunction(ReadPixels, "glReadPixels", true) || loaded;
   loaded = GetFunction(Scissor, "glScissor", true) || loaded;
   loaded = GetFunction(Viewport, "glViewport", true) || loaded;
   loaded = GetFunction(DepthFunc, "glDepthFunc", true) || loaded;
   loaded = GetFunction(DepthMask, "glDepthMask", true) || loaded;
   loaded = GetFunction(DepthRangef, "glDepthRangef", true) || loaded;
   loaded = GetFunction(StencilFunc, "glStencilFunc", true) || loaded;
   loaded = GetFunction(StencilFuncSeparate, "glStencilFuncSeparate", true) || loaded;
   loaded = GetFunction(StencilMask, "glStencilMask", true) || loaded;
   loaded = GetFunction(StencilMaskSeparate, "glStencilMaskSeparate", true) || loaded;
   loaded = GetFunction(StencilOp, "glStencilOp", true) || loaded;
   loaded = GetFunction(StencilOpSeparate, "glStencilOpSeparate", true) || loaded;
   loaded = GetFunction(ActiveTexture, "glActiveTexture", true) || loaded;
   loaded = GetFunction(GenTextures, "glGenTextures", true) || loaded;
   loaded = GetFunction(BindTexture, "glBindTexture", true) || loaded;
   loaded = GetFunction(CompressedTexImage2D, "glCompressedTexImage2D", true) || loaded;
   loaded = GetFunction(CompressedTexSubImage2D, "glCompressedTexSubImage2D", true) || loaded;
   loaded = GetFunction(DeleteTextures, "glDeleteTextures", true) || loaded;
   loaded = GetFunction(GenerateMipmap, "glGenerateMipmap", true) || loaded;
   loaded = GetFunction(IsTexture, "glIsTexture", true) || loaded;
   loaded = GetFunction(TexParameterf, "glTexParameterf", true) || loaded;
   loaded = GetFunction(TexParameterfv, "glTexParameterfv", true) || loaded;
   loaded = GetFunction(TexParameteri, "glTexParameteri", true) || loaded;
   loaded = GetFunction(TexParameteriv, "glTexParameteriv", true) || loaded;
   loaded = GetFunction(TexImage2D, "glTexImage2D", true) || loaded;
   loaded = GetFunction(TexSubImage2D, "glTexSubImage2D", true) || loaded;
   loaded = GetFunction(EnableVertexAttribArray, "glEnableVertexAttribArray", true) || loaded;
   loaded = GetFunction(DisableVertexAttribArray, "glDisableVertexAttribArray", true) || loaded;
   loaded = GetFunction(VertexAttribPointer, "glVertexAttribPointer", true) || loaded;
   loaded = GetFunction(DrawArrays, "glDrawArrays", true) || loaded;
   loaded = GetFunction(DrawElements, "glDrawElements", true) || loaded;
   loaded = GetFunction(BlendColor, "glBlendColor", true) || loaded;
   loaded = GetFunction(BlendEquation, "glBlendEquation", true) || loaded;
   loaded = GetFunction(BlendEquationSeparate, "glBlendEquationSeparate", true) || loaded;
   loaded = GetFunction(BlendFunc, "glBlendFunc", true) || loaded;
   loaded = GetFunction(BlendFuncSeparate, "glBlendFuncSeparate", true) || loaded;
   loaded = GetFunction(GenBuffers, "glGenBuffers", true) || loaded;
   loaded = GetFunction(BindBuffer, "glBindBuffer", true) || loaded;
   loaded = GetFunction(BufferData, "glBufferData", true) || loaded;
   loaded = GetFunction(BufferSubData, "glBufferSubData", true) || loaded;
   loaded = GetFunction(DeleteBuffers, "glDeleteBuffers", true) || loaded;
   loaded = GetFunction(IsBuffer, "glIsBuffer", true) || loaded;
   loaded = GetFunction(MapBufferRange, "glMapBufferRange", true) || loaded;
   loaded = GetFunction(MapBuffer, "glMapBuffer", true) || loaded;
   loaded = GetFunction(UnmapBuffer, "glUnmapBuffer", true) || loaded;
   loaded = GetFunction(FlushMappedBufferRange, "glFlushMappedBufferRange", true) || loaded;
   loaded = GetFunction(GenFramebuffers, "glGenFramebuffers", true) || loaded;
   loaded = GetFunction(GenRenderbuffers, "glGenRenderbuffers", true) || loaded;
   loaded = GetFunction(BindFramebuffer, "glBindFramebuffer", true) || loaded;
   loaded = GetFunction(BindRenderbuffer, "glBindRenderbuffer", true) || loaded;
   loaded = GetFunction(CheckFramebufferStatus, "glCheckFramebufferStatus", true) || loaded;
   loaded = GetFunction(DeleteFramebuffers, "glDeleteFramebuffers", true) || loaded;
   loaded = GetFunction(DeleteRenderbuffers, "glDeleteRenderbuffers", true) || loaded;
   loaded = GetFunction(FramebufferRenderbuffer, "glFramebufferRenderbuffer", true) || loaded;
   loaded = GetFunction(FramebufferTexture2D, "glFramebufferTexture2D", true) || loaded;
   loaded = GetFunction(IsFramebuffer, "glIsFramebuffer", true) || loaded;
   loaded = GetFunction(IsRenderbuffer, "glIsRenderbuffer", true) || loaded;
   loaded = GetFunction(RenderbufferStorage, "glRenderbufferStorage", true) || loaded;
   loaded = GetFunction(RenderbufferStorageMultisample, "glRenderbufferStorageMultisample", true) || loaded;
   loaded = GetFunction(BindVertexArray, "glBindVertexArray", true) || loaded;
   loaded = GetFunction(DeleteVertexArrays, "glDeleteVertexArrays", true) || loaded;
   loaded = GetFunction(GenVertexArrays, "glGenVertexArrays", true) || loaded;
   loaded = GetFunction(IsVertexArray, "glIsVertexArray", true) || loaded;
   loaded = GetFunction(GenSamplers, "glGenSamplers", true) || loaded;
   loaded = GetFunction(DeleteSamplers, "glDeleteSamplers", true) || loaded;
   loaded = GetFunction(IsSampler, "glIsSampler", true) || loaded;
   loaded = GetFunction(BindSampler, "glBindSampler", true) || loaded;
   loaded = GetFunction(SamplerParameteri, "glSamplerParameteri", true) || loaded;
   loaded = GetFunction(SamplerParameteriv, "glSamplerParameteriv", true) || loaded;
   loaded = GetFunction(SamplerParameterf, "glSamplerParameterf", true) || loaded;
   loaded = GetFunction(SamplerParameterfv, "glSamplerParameterfv", true) || loaded;
   loaded = GetFunction(AttachShader, "glAttachShader", true) || loaded;
   loaded = GetFunction(BindAttribLocation, "glBindAttribLocation", true) || loaded;
   loaded = GetFunction(CompileShader, "glCompileShader", true) || loaded;
   loaded = GetFunction(CreateProgram, "glCreateProgram", true) || loaded;
   loaded = GetFunction(CreateShader, "glCreateShader", true) || loaded;
   loaded = GetFunction(DeleteProgram, "glDeleteProgram", true) || loaded;
   loaded = GetFunction(DeleteShader, "glDeleteShader", true) || loaded;
   loaded = GetFunction(DetachShader, "glDetachShader", true) || loaded;
   loaded = GetFunction(GetAttribLocation, "glGetAttribLocation", true) || loaded;
   loaded = GetFunction(GetProgramiv, "glGetProgramiv", true) || loaded;
   loaded = GetFunction(GetProgramInfoLog, "glGetProgramInfoLog", true) || loaded;
   loaded = GetFunction(GetShaderiv, "glGetShaderiv", true) || loaded;
   loaded = GetFunction(GetShaderInfoLog, "glGetShaderInfoLog", true) || loaded;
   loaded = GetFunction(GetUniformLocation, "glGetUniformLocation", true) || loaded;
   loaded = GetFunction(IsProgram, "glIsProgram", true) || loaded;
   loaded = GetFunction(IsShader, "glIsShader", true) || loaded;
   loaded = GetFunction(LinkProgram, "glLinkProgram", true) || loaded;
   loaded = GetFunction(ShaderBinary, "glShaderBinary", true) || loaded;
   loaded = GetFunction(GetShaderSource, "glGetShaderSource", true) || loaded;
   loaded = GetFunction(ShaderSource, "glShaderSource", true) || loaded;
   loaded = GetFunction(ShaderSource, "glShaderSource", true) || loaded;
   loaded = GetFunction(ShaderSource, "glShaderSource", true) || loaded;
   loaded = GetFunction(Uniform1f, "glUniform1f", true) || loaded;
   loaded = GetFunction(Uniform1fv, "glUniform1fv", true) || loaded;
   loaded = GetFunction(Uniform1i, "glUniform1i", true) || loaded;
   loaded = GetFunction(Uniform1iv, "glUniform1iv", true) || loaded;
   loaded = GetFunction(Uniform2f, "glUniform2f", true) || loaded;
   loaded = GetFunction(Uniform2fv, "glUniform2fv", true) || loaded;
   loaded = GetFunction(Uniform2i, "glUniform2i", true) || loaded;
   loaded = GetFunction(Uniform2iv, "glUniform2iv", true) || loaded;
   loaded = GetFunction(Uniform3f, "glUniform3f", true) || loaded;
   loaded = GetFunction(Uniform3fv, "glUniform3fv", true) || loaded;
   loaded = GetFunction(Uniform3i, "glUniform3i", true) || loaded;
   loaded = GetFunction(Uniform3iv, "glUniform3iv", true) || loaded;
   loaded = GetFunction(Uniform4f, "glUniform4f", true) || loaded;
   loaded = GetFunction(Uniform4fv, "glUniform4fv", true) || loaded;
   loaded = GetFunction(Uniform4i, "glUniform4i", true) || loaded;
   loaded = GetFunction(Uniform4iv, "glUniform4iv", true) || loaded;
   loaded = GetFunction(UniformMatrix2fv, "glUniformMatrix2fv", true) || loaded;
   loaded = GetFunction(UniformMatrix3fv, "glUniformMatrix3fv", true) || loaded;
   loaded = GetFunction(UniformMatrix4fv, "glUniformMatrix4fv", true) || loaded;
   loaded = GetFunction(UseProgram, "glUseProgram", true) || loaded;
   loaded = GetFunction(ValidateProgram, "glValidateProgram", true) || loaded;
   loaded = GetFunction(Uniform1ui, "glUniform1ui", true) || loaded;
   loaded = GetFunction(Uniform2ui, "glUniform2ui", true) || loaded;
   loaded = GetFunction(Uniform3ui, "glUniform3ui", true) || loaded;
   loaded = GetFunction(Uniform4ui, "glUniform4ui", true) || loaded;
   loaded = GetFunction(Uniform1uiv, "glUniform1uiv", true) || loaded;
   loaded = GetFunction(Uniform2uiv, "glUniform2uiv", true) || loaded;
   loaded = GetFunction(Uniform3uiv, "glUniform3uiv", true) || loaded;
   loaded = GetFunction(Uniform4uiv, "glUniform4uiv", true) || loaded;
   loaded = GetFunction(GetUniformBlockIndex, "glGetUniformBlockIndex", true) || loaded;
   loaded = GetFunction(UniformBlockBinding, "glUniformBlockBinding", true) || loaded;
   loaded = GetFunction(BindBufferBase, "glBindBufferBase", true) || loaded;
   loaded = GetFunction(GetTexImage, "glGetTexImage", true) || loaded;
   loaded = GetFunction(DiscardFramebuffer, "glDiscardFramebuffer", false) || loaded;
   loaded = GetFunction(BlitFramebuffer, "glBlitFramebuffer", false) || loaded;
   loaded = GetFunction(ResolveMultisampleFramebufferAPPLE, "glResolveMultisampleFramebufferAPPLE", false) || loaded;
   loaded = GetFunction(UniformMatrix2x3fv, "glUniformMatrix2x3fv", true) || loaded;
   loaded = GetFunction(UniformMatrix3x2fv, "glUniformMatrix3x2fv", true) || loaded;
   loaded = GetFunction(UniformMatrix2x4fv, "glUniformMatrix2x4fv", true) || loaded;
   loaded = GetFunction(UniformMatrix4x2fv, "glUniformMatrix4x2fv", true) || loaded;
   loaded = GetFunction(UniformMatrix3x4fv, "glUniformMatrix3x4fv", true) || loaded;
   loaded = GetFunction(UniformMatrix4x3fv, "glUniformMatrix4x3fv", true) || loaded;

   return loaded;
}
