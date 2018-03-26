# GNU Make solution makefile autogenerated by Premake
# Type "make help" for usage help

ifndef config
  config=debug
endif
export config

PROJECTS := ex1 ex2 lesson1 lesson2 lesson6 lesson7 texturing modelviewer grass sphereworld_color flying glm_modelviewer glm_texturing point_sprites glm_sphereworld_color glm_grass glm_flying left_handed gears

.PHONY: all clean help $(PROJECTS)

all: $(PROJECTS)

ex1: 
	@echo "==== Building ex1 ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f ex1.make

ex2: 
	@echo "==== Building ex2 ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f ex2.make

lesson1: 
	@echo "==== Building lesson1 ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f lesson1.make

lesson2: 
	@echo "==== Building lesson2 ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f lesson2.make

lesson6: 
	@echo "==== Building lesson6 ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f lesson6.make

lesson7: 
	@echo "==== Building lesson7 ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f lesson7.make

texturing: 
	@echo "==== Building texturing ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f texturing.make

modelviewer: 
	@echo "==== Building modelviewer ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f modelviewer.make

grass: 
	@echo "==== Building grass ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f grass.make

sphereworld_color: 
	@echo "==== Building sphereworld_color ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f sphereworld_color.make

flying: 
	@echo "==== Building flying ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f flying.make

glm_modelviewer: 
	@echo "==== Building glm_modelviewer ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f glm_modelviewer.make

glm_texturing: 
	@echo "==== Building glm_texturing ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f glm_texturing.make

point_sprites: 
	@echo "==== Building point_sprites ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f point_sprites.make

glm_sphereworld_color: 
	@echo "==== Building glm_sphereworld_color ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f glm_sphereworld_color.make

glm_grass: 
	@echo "==== Building glm_grass ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f glm_grass.make

glm_flying: 
	@echo "==== Building glm_flying ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f glm_flying.make

left_handed: 
	@echo "==== Building left_handed ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f left_handed.make

gears: 
	@echo "==== Building gears ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f gears.make

clean:
	@${MAKE} --no-print-directory -C build -f ex1.make clean
	@${MAKE} --no-print-directory -C build -f ex2.make clean
	@${MAKE} --no-print-directory -C build -f lesson1.make clean
	@${MAKE} --no-print-directory -C build -f lesson2.make clean
	@${MAKE} --no-print-directory -C build -f lesson6.make clean
	@${MAKE} --no-print-directory -C build -f lesson7.make clean
	@${MAKE} --no-print-directory -C build -f texturing.make clean
	@${MAKE} --no-print-directory -C build -f modelviewer.make clean
	@${MAKE} --no-print-directory -C build -f grass.make clean
	@${MAKE} --no-print-directory -C build -f sphereworld_color.make clean
	@${MAKE} --no-print-directory -C build -f flying.make clean
	@${MAKE} --no-print-directory -C build -f glm_modelviewer.make clean
	@${MAKE} --no-print-directory -C build -f glm_texturing.make clean
	@${MAKE} --no-print-directory -C build -f point_sprites.make clean
	@${MAKE} --no-print-directory -C build -f glm_sphereworld_color.make clean
	@${MAKE} --no-print-directory -C build -f glm_grass.make clean
	@${MAKE} --no-print-directory -C build -f glm_flying.make clean
	@${MAKE} --no-print-directory -C build -f left_handed.make clean
	@${MAKE} --no-print-directory -C build -f gears.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "   debug"
	@echo "   release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   ex1"
	@echo "   ex2"
	@echo "   lesson1"
	@echo "   lesson2"
	@echo "   lesson6"
	@echo "   lesson7"
	@echo "   texturing"
	@echo "   modelviewer"
	@echo "   grass"
	@echo "   sphereworld_color"
	@echo "   flying"
	@echo "   glm_modelviewer"
	@echo "   glm_texturing"
	@echo "   point_sprites"
	@echo "   glm_sphereworld_color"
	@echo "   glm_grass"
	@echo "   glm_flying"
	@echo "   left_handed"
	@echo "   gears"
	@echo ""
	@echo "For more information, see http://industriousone.com/premake/quick-start"
