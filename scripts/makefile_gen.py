import pathlib
import subprocess
import os
from shutil import rmtree
from Dependencies import flame_ui_project_dir
from Dependencies import check_cmake_installation
from Dependencies import check_cmd_line_tools_installation

if __name__ == '__main__':
    os.chdir(flame_ui_project_dir)
    check_cmake_installation()
    check_cmd_line_tools_installation()

    cmake_build_type = "Debug"
    print("[FLAMEUI]: Enter project build configuration(Debug/Release)")
    build_type = input("[FLAMEUI]: ")

    if build_type.lower() == "debug":
        cmake_build_type = "Debug"
    elif build_type.lower() == "release":
        cmake_build_type = "Release"

    print(f"[FLAMEUI]: {cmake_build_type} configuration selected for FlameUI Project.")
    print("[FLAMEUI]: Starting CMake Project Generation...\n")
    subprocess.run(["cmake", f"-DCMAKE_BUILD_TYPE={cmake_build_type}", "-DMSDFGEN_INSTALL=Off", "-DMSDFGEN_BUILD_STANDALONE=Off", "-Wno-dev", "-S.", "-Bbuild/make"])

    print(f"\n[FLAMEUI]: MakeFiles have been generated in the folder \"{flame_ui_project_dir}/build/make\"")

    print("[FLAMEUI]: Do you want to build FlameUI project using make?(Y/n)")
    should_build_make_proj = input("[FLAMEUI]: ")

    if should_build_make_proj.lower() == 'y':
        os.chdir(flame_ui_project_dir / 'build/make')

        print("[FLAMEUI]: Building FlameUI project using 4 threads.")
        subprocess.run(["make", "-j4"])

        os.chdir(flame_ui_project_dir)

        if list(pathlib.Path(flame_ui_project_dir / f'bin/{cmake_build_type}').glob(f'**/SandboxApp_{cmake_build_type}')):
            print(f"\n[FLAMEUI]: FlameUI project binaries are built in the directory \"{flame_ui_project_dir}/bin\"")
        else:
            print("\n[FLAMEUI]: Some Error occurred while building MakeFiles\n[FLAMEUI]: Cleaning build files...")

            rmtree(flame_ui_project_dir / 'build/make')
            rmtree(flame_ui_project_dir / f'bin/{cmake_build_type}')

            print(f"[FLAMEUI]: Removed directory \"{flame_ui_project_dir}/build/make\"")
            print(f"[FLAMEUI]: Removed directory \"{flame_ui_project_dir}/bin/{cmake_build_type}\"")
            print("[FLAMEUI]: Finished Process.")

    elif should_build_make_proj.lower() == 'n':
        print("[FLAMEUI]: FlameUI project generation done.")
    else:
        print("[FLAMEUI]: Unrecognized input, taking \'n\' as answer.")
        print("[FLAMEUI]: FlameUI project generation done.")
