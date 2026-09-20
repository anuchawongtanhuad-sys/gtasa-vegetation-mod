#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <android/log.h>

#define LOG_TAG "VegMod"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

uintptr_t game_base = 0;

float close_distance = 150.0f;
float far_distance = 800.0f;
float distance_multiplier = 2.0f;
float lod_multiplier = 2.0f;
float culling_distance = 1000.0f;
int veg_enabled = 1;
int keep_visible = 1;

void write32(uintptr_t addr, uint32_t val) {
    if (!game_base) return;
    uintptr_t target_addr = game_base + addr;
    uintptr_t page = target_addr & ~0xFFF;
    mprotect((void*)page, 4096, PROT_READ | PROT_WRITE | PROT_EXEC);
    *(uint32_t*)target_addr = val;
}

void writeFloat(uintptr_t addr, float val) {
    if (!game_base) return;
    uintptr_t target_addr = game_base + addr;
    uintptr_t page = target_addr & ~0xFFF;
    mprotect((void*)page, 4096, PROT_READ | PROT_WRITE | PROT_EXEC);
    *(float*)target_addr = val;
}

void parse_ini() {
    FILE* file = fopen("/sdcard/AMLMods/com.rockstargames.gtasa/net.reis.gtasa.vegetation.ini", "r");
    if (!file) {
        file = fopen("/sdcard/Android/data/com.rockstargames.gtasa/mods/net.reis.gtasa.vegetation.ini", "r");
    }
    if (file) {
        char line[128];
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "Enabled = %d", &veg_enabled);
            sscanf(line, "CloseDistance = %f", &close_distance);
            sscanf(line, "FarDistance = %f", &far_distance);
            sscanf(line, "DistanceMultiplier = %f", &distance_multiplier);
            sscanf(line, "LODMultiplier = %f", &lod_multiplier);
            sscanf(line, "CullingDistance = %f", &culling_distance);
            sscanf(line, "KeepVisible = %d", &keep_visible);
        }
        fclose(file);
        LOGI("INI erfolgreich geladen.");
    } else {
        LOGI("Keine INI gefunden, verwende Standardwerte.");
    }
}

void apply_patches() {
    if (!veg_enabled) {
        LOGI("Vegetation-Mod ist in der INI deaktiviert.");
        return;
    }

    // Sobald du die genauen Offsets deiner libGTASA.so hast, trägst du sie hier ein:
    // writeFloat(0xDEIN_OFFSET_HIER, far_distance);
    // writeFloat(0xDEIN_OFFSET_HIER, lod_multiplier);

    LOGI("Vegetation-Werte angewendet: FarDist=%.1f, LODMult=%.1f", far_distance, lod_multiplier);
}

void veg_configure() {
    game_base = (uintptr_t)dlopen("libGTASA.so", RTLD_LAZY);
    if (!game_base) {
        LOGI("libGTASA.so nicht gefunden!");
        return;
    }
    parse_ini();
    apply_patches();
    LOGI("Vegetation Patch vollständig geladen und angewendet.");
}

__attribute__((constructor))
void init() {
    veg_configure();
}