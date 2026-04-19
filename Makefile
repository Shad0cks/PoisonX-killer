# --- Configuration ---
OUT_DIR = out
OBJ_DIR = obj
TARGET  = $(OUT_DIR)\PoisonX-killer.exe

# --- Compilateurs ---
OLLVM_BIN  = C:\Path\To\OLLVM\bin
CLANG_CL   = "$(OLLVM_BIN)\clang-cl.exe"
MSVC_CL    = cl.exe

# --- Flags ---
CFLAGS     = /nologo /MT /O2 /W3 /EHsc /D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
LIBS       = advapi32.lib
OLLVM_FLAGS = -mllvm -sub -mllvm -split -mllvm -fla -mllvm -bcf

# --- Règles principales ---
all: normal

normal: $(OUT_DIR) $(OBJ_DIR)
	@echo [*] Compilation MSVC : main.cpp
	$(MSVC_CL) $(CFLAGS) /c main.cpp /Fo"$(OBJ_DIR)\main_msvc.obj"
	@echo [*] Liaison vers $(TARGET)...
	$(MSVC_CL) /nologo $(OBJ_DIR)\main_msvc.obj $(LIBS) /link /out:$(TARGET)

obfuscated: $(OUT_DIR) $(OBJ_DIR)
	@echo [*] Compilation OLLVM : main.cpp
	$(CLANG_CL) $(CFLAGS) $(OLLVM_FLAGS) /c main.cpp /Fo"$(OBJ_DIR)\main_ollvm.obj"
	@echo [*] Liaison vers $(TARGET)...
	$(CLANG_CL) /nologo $(OBJ_DIR)\main_ollvm.obj $(LIBS) /link /out:$(TARGET)

# --- Création des dossiers ---
$(OUT_DIR):
	@if not exist $(OUT_DIR) mkdir $(OUT_DIR)

$(OBJ_DIR):
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

# --- Nettoyage ---
clean:
	@echo [*] Nettoyage...
	@if exist $(OUT_DIR) rd /s /q $(OUT_DIR)
	@if exist $(OBJ_DIR) rd /s /q $(OBJ_DIR)
