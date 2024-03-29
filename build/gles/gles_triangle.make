# GNU Make project makefile autogenerated by Premake

ifndef config
  config=debug_linux
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild prelink

ifeq ($(config),debug_linux)
  RESCOMP = windres
  TARGETDIR = .
  TARGET = $(TARGETDIR)/gles_triangle
  OBJDIR = obj/linux/Debug
  DEFINES += -DDEBUG -DUSING_GLES2
  INCLUDES += -I../../inc -I../../src/glcommon
  FORCE_INCLUDE +=
  ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -g -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -g -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  LIBS += -lSDL2 -lGLESv2 -lm
  LDDEPS +=
  ALL_LDFLAGS += $(LDFLAGS)
  LINKCMD = $(CC) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
all: prebuild prelink $(TARGET)
	@:

endif

ifeq ($(config),debug_gmake)
  RESCOMP = windres
  TARGETDIR = .
  TARGET = $(TARGETDIR)/gles_triangle
  OBJDIR = obj/gmake/Debug
  DEFINES += -DDEBUG -DUSING_GLES2
  INCLUDES += -I../../inc -I../../src/glcommon
  FORCE_INCLUDE +=
  ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -g -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -g -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  LIBS +=
  LDDEPS +=
  ALL_LDFLAGS += $(LDFLAGS)
  LINKCMD = $(CC) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
all: prebuild prelink $(TARGET)
	@:

endif

ifeq ($(config),debug_windows)
  RESCOMP = windres
  TARGETDIR = .
  TARGET = $(TARGETDIR)/gles_triangle.exe
  OBJDIR = obj/windows/Debug
  DEFINES += -DDEBUG -DUSING_GLES2
  INCLUDES += -I../../inc -I../../src/glcommon
  FORCE_INCLUDE +=
  ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -g -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -g -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  LIBS +=
  LDDEPS +=
  ALL_LDFLAGS += $(LDFLAGS)
  LINKCMD = $(CC) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
all: prebuild prelink $(TARGET)
	@:

endif

ifeq ($(config),release_linux)
  RESCOMP = windres
  TARGETDIR = .
  TARGET = $(TARGETDIR)/gles_triangle
  OBJDIR = obj/linux/Release
  DEFINES += -DNDEBUG -DUSING_GLES2
  INCLUDES += -I../../inc -I../../src/glcommon
  FORCE_INCLUDE +=
  ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -O2 -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -O2 -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  LIBS += -lSDL2 -lGLESv2 -lm
  LDDEPS +=
  ALL_LDFLAGS += $(LDFLAGS) -s
  LINKCMD = $(CC) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
all: prebuild prelink $(TARGET)
	@:

endif

ifeq ($(config),release_gmake)
  RESCOMP = windres
  TARGETDIR = .
  TARGET = $(TARGETDIR)/gles_triangle
  OBJDIR = obj/gmake/Release
  DEFINES += -DNDEBUG -DUSING_GLES2
  INCLUDES += -I../../inc -I../../src/glcommon
  FORCE_INCLUDE +=
  ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -O2 -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -O2 -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  LIBS +=
  LDDEPS +=
  ALL_LDFLAGS += $(LDFLAGS) -s
  LINKCMD = $(CC) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
all: prebuild prelink $(TARGET)
	@:

endif

ifeq ($(config),release_windows)
  RESCOMP = windres
  TARGETDIR = .
  TARGET = $(TARGETDIR)/gles_triangle.exe
  OBJDIR = obj/windows/Release
  DEFINES += -DNDEBUG -DUSING_GLES2
  INCLUDES += -I../../inc -I../../src/glcommon
  FORCE_INCLUDE +=
  ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -O2 -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -O2 -std=c99 -fno-strict-aliasing -Wunused-variable -Wreturn-type -Wimplicit `pkg-config --cflags sdl2`
  ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  LIBS +=
  LDDEPS +=
  ALL_LDFLAGS += $(LDFLAGS) -s
  LINKCMD = $(CC) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
all: prebuild prelink $(TARGET)
	@:

endif

OBJECTS := \
	$(OBJDIR)/gltools.o \
	$(OBJDIR)/glad_gles2.o \
	$(OBJDIR)/gles_triangle.o \

RESOURCES := \

CUSTOMFILES := \

SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

$(TARGET): $(GCH) ${CUSTOMFILES} $(OBJECTS) $(LDDEPS) $(RESOURCES) | $(TARGETDIR)
	@echo Linking gles_triangle
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(CUSTOMFILES): | $(OBJDIR)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning gles_triangle
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(OBJECTS): $(GCH) $(PCH) | $(OBJDIR)
$(GCH): $(PCH) | $(OBJDIR)
	@echo $(notdir $<)
	$(SILENT) $(CC) -x c-header $(ALL_CFLAGS) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
else
$(OBJECTS): | $(OBJDIR)
endif

$(OBJDIR)/gltools.o: ../../src/glcommon/gltools.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/glad_gles2.o: ../../src/tests/glad_gles2.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/gles_triangle.o: ../../src/tests/gles_triangle.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(OBJDIR)/$(notdir $(PCH)).d
endif
