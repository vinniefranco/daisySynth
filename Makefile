# Project Name
# APP_TYPE = BOOT_SRAM
TARGET = synth
DEBUG = 1

# Sources
CPP_SOURCES = synth.cpp 
CPP_SOURCES += engine/Voice.cpp 
CPP_SOURCES += engine/VoiceManager.cpp 
CPP_SOURCES += engine/EnvelopeGenerator.cpp 

# Library Locations
LIBDAISY_DIR = ../DaisyExamples/libDaisy/
DAISYSP_DIR = ../DaisyExamples/DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
