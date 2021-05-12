INCLUDE_FLAGS      := -I ./include -I ./vendor -I ./vendor/glm -I ./vendor/glad/include
COMPILE_FLAGS      := g++ $(INCLUDE_FLAGS)

SRC_DIR 			     := ./src/
GLAD_DIR           := ./vendor/glad/src/
IMGUI_DIR				   := ./vendor/imgui/
IMGUI_FB_DIR			 := ./vendor/imguibrowser/FileBrowser/
OUTPUT_DIR         := ./bin/

# Praise be to the wildcards! Preps to shotgun compile the engine source +
# submodules.
CORE_SOURCES 			 := $(wildcard $(SRC_DIR)Core/*.cpp)
GRAPHICS_SOURCES   := $(wildcard $(SRC_DIR)Graphics/*.cpp)
UTILS_SOURCES      := $(wildcard $(SRC_DIR)Utils/*.cpp)
GLAD_SOURCES       := $(wildcard $(GLAD_DIR)*.c)
IMGUI_SOURCES 	   := $(wildcard $(IMGUI_DIR)*.cpp)
IMGUI_FB_SOURCES   := $(wildcard $(IMGUI_FB_DIR)*.cpp)
CORE_OBJECTS 			 := $(patsubst $(SRC_DIR)Core/%.cpp, $(OUTPUT_DIR)Core/%.o, $(CORE_SOURCES))
GRAPHICS_OBJECTS   := $(patsubst $(SRC_DIR)Graphics/%.cpp, $(OUTPUT_DIR)Graphics/%.o, $(GRAPHICS_SOURCES))
UTILS_OBJECTS      := $(patsubst $(SRC_DIR)Utils/%.cpp, $(OUTPUT_DIR)Utils/%.o, $(UTILS_SOURCES))
GLAD_OBJECTS       := $(patsubst $(GLAD_DIR)%.c, $(OUTPUT_DIR)vendor/%.o, $(GLAD_SOURCES))
IMGUI_OBJECTS      := $(patsubst $(IMGUI_DIR)%.cpp, $(OUTPUT_DIR)vendor/%.o, $(IMGUI_SOURCES))
IMGUI_FB_OBJECTS   := $(patsubst $(IMGUI_FB_DIR)%.cpp, $(OUTPUT_DIR)vendor/%.o, $(IMGUI_FB_SOURCES))

makebuild: make_dir Application

# Link everything together.
Application: $(CORE_OBJECTS) $(GRAPHICS_OBJECTS) $(UTILS_OBJECTS) $(GLAD_OBJECTS) \
	$(IMGUI_FB_OBJECTS) $(IMGUI_OBJECTS)
	@echo Linking the application.
	@$(COMPILE_FLAGS) -o Application $^ -ldl -lglfw

# Compile the graphics engine.
$(OUTPUT_DIR)%.o: $(SRC_DIR)%.cpp
	@echo Compiling $<
	@$(COMPILE_FLAGS) -I$(SRC_DIR) -c $< -o $@

# Compile GLAD.
$(OUTPUT_DIR)vendor/%.o: $(GLAD_DIR)%.c
	@echo Compiling $<
	@$(COMPILE_FLAGS) -I$(GLAD_DIR) -c $< -o $@

# Compile Dear Imgui. Had to manually define GLAD and make a few modifications.
$(OUTPUT_DIR)vendor/%.o: $(IMGUI_DIR)%.cpp
	@echo Compiling $<
	@$(COMPILE_FLAGS) -I$(IMGUI_DIR) -c $< -o $@

# Compile the Imgui file browser.
$(OUTPUT_DIR)vendor/%.o: $(IMGUI_FB_DIR)%.cpp
	@echo Compiling $<
	@$(COMPILE_FLAGS) -I$(IMGUI_FB_DIR) -c $< -o $@

# Make the binary directory.
make_dir:
	@mkdir -p bin
	@mkdir -p bin/Core
	@mkdir -p bin/Graphics
	@mkdir -p bin/Utils
	@mkdir -p bin/vendor

# Delete the contents of bin.
clean:
	@echo Clearing the binary directories.
	@rm ./bin/Core/*.o
	@rm ./bin/Utils/*.o
	@rm ./bin/Graphics/*.o
