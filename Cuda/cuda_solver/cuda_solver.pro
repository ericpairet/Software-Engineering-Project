QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtcuda
TEMPLATE = app

# project build directories (if not using shadow build)
DESTDIR     = $$system(pwd)
BUILDDIR    = $$DESTDIR/build

MOC_DIR     = $$BUILDDIR # moc_... files
UI_DIR      = $$BUILDDIR # ui_mainwindow.cpp

OBJECTS_DIR = $$BUILDDIR/bin # .o binary files



INCLUDEPATH += /usr/local/include                               # *** modified ***



SOURCES += \
    main.cpp
        #mainwindow.cpp                                         # *** modified ***

#HEADERS  += mainwindow.h                                       # *** modified ***

#FORMS    += mainwindow.ui                                      # *** modified ***

# NOTE: C++ flags are needed here for
#       the CUDA Thrust library
############### UNIX FLAGS #####################
unix {
    QMAKE_CXXFLAGS += -std=c++11
}
############### MAC FLAGS #####################
macx {
    # libs that don't get passed to nvcc (we'll remove them from LIBS later)
    NON_CUDA_LIBS += -stdlib=libc++
    LIBS += $$NON_CUDA_LIBS

    QMAKE_CXXFLAGS += -stdlib=libc++ -mmacosx-version-min=10.7
    QMAKE_LFLAGS += -mmacosx-version-min=10.7
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

    # specific to computers without older sdks
    MAC_SDK  = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.    sdk/
    if( exists( $$MAC_SDK) ) {
        QMAKE_MAC_SDK = macosx10.9 # lowest sdk on my computer :/
    }

    # don't show warnings for c++11 extentions
    QMAKE_CXXFLAGS += -Wno-c++11-extensions
}


################### CUDA ###################### (similar to your setup)

unix:!macx {
    SED_STUFF = 2>&1 | sed -r \"s/\\(([0-9]+)\\)/:\\1/g\" 1>&2
}
macx {

    SED_STUFF = 2>&1 | sed -E \"s/\\(([0-9]+)\\)/:\\1/g\" 1>&2
}

CUDA_DIR = /usr/local/cuda

# make sure cuda is available on the computer
if ( exists( $$CUDA_DIR/ ) ) {
    message( "Configuring for cuda...");
    DEFINES += CUDA_7 #  # same as putting this in code ->  #define CUDA_7

    # Cuda sources
    #CUDA_SOURCES += cuda/wrappers.cu                           # *** modified ***
    #CUDA_SOURCES += cuda_code.cu

    # show files in working tree
    OHTER_FILES += cuda_code.cu
    #OTHER_FILES +=  cuda/wrappers.cu \                         # *** modified ***
    #                cuda/wrappers.cuh \                        # *** modified ***
    #                cuda/helper_cuda.h                         # *** modified ***

    # Path to cuda install
    CUDA_LIB = $$CUDA_DIR/lib

    # Pather to header and lib files
    INCLUDEPATH += $$CUDA_DIR/include \
                   cuda # my cuda files

    QMAKE_LIBDIR += $$CUDA_LIB

    # prevents warnings from code we didn't write
    QMAKE_CXXFLAGS += -isystem $$CUDA_DIR/include

    #LIBS += -lcudart # add other cuda libs here (-lcublas -lcurand, etc.)           # *** modified ***
    LIBS += -lcudart \
            -lcusparse

    # SPECIFY THE R PATH FOR NVCC!!!!! (your problem...previously my problem)
    QMAKE_LFLAGS += -Wl,-rpath,$$CUDA_LIB
    NVCCFLAGS = -Xlinker -rpath,$$CUDA_LIB

    # libs used in the code
    CUDA_LIBS = $$LIBS
    CUDA_LIBS -= $$NON_CUDA_LIBS # remove libs nvcc won't recognize

    # GPU architecture (might be a way to detect this somehow instead of hardcoding)
    CUDA_ARCH     = sm_20 # <- based on specs from your code. This was tested with sm_30

    # Some default NVCC flags
    NVCCFLAGS     += --compiler-options -fno-strict-aliasing -use_fast_math --ptxas-options=-v --std=c++11

    # Prepare the extra compiler configuration (taken from the nvidia forum)
    CUDA_INC = $$join(INCLUDEPATH,' -I','-I',' ')

    cuda.commands = $$CUDA_DIR/bin/nvcc -m64 -O3 -arch=$$CUDA_ARCH -c $$NVCCFLAGS \
                    $$CUDA_INC $$CUDA_LIBS  ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT} \
                    $$SED_STUFF
    # nvcc error printout format ever so slightly different from gcc
    # http://forums.nvidia.com/index.php?showtopic=171651

    cuda.dependency_type = TYPE_C
    cuda.depend_command = $$CUDA_DIR/bin/nvcc -O3 -M $$CUDA_INC $$NVCCFLAGS   ${QMAKE_FILE_NAME}

    cuda.input = CUDA_SOURCES
    cuda.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}_cuda.o

    # Tell Qt that we want add more stuff to the Makefile
    QMAKE_EXTRA_COMPILERS += cuda

} # endif CUDA

HEADERS +=

