import subprocess
import os
from Dependencies import flame_ui_project_dir
from Dependencies import check_cmake_installation

if __name__ == '__main__':
    os.chdir(flame_ui_project_dir)
    check_cmake_installation()

    print("[FLAMEUI]: Starting CMake Project Generation...\n")
    subprocess.run(["cmake", "-DMSDFGEN_INSTALL=Off", "-DMSDFGEN_BUILD_STANDALONE=Off", "-Wno-dev", "-S.", "-Bbuild/xcode/", "-GXcode"])

    print(f"\n[FLAMEUI]: Xcode project has been generated in the folder \"{flame_ui_project_dir}/build/xcode\"")
