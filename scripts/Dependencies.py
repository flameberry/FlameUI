import pathlib
import platform

flame_ui_project_dir = pathlib.Path(__file__).parent.parent
flame_ui_project_name = 'FlameUI'


def check_cmake_installation():
    cmake_path_apple = pathlib.Path('/usr/local/bin/cmake')
    cmake_path_windows = pathlib.Path('')
    cmake_path = pathlib.Path('')

    if platform.system() == 'Darwin':
        cmake_path = cmake_path_apple
    elif platform.system() == 'Windows':
        cmake_path = cmake_path_windows
    cmake_exists = cmake_path.is_file()
    if cmake_exists:
        print(
            f"[FLAMEUI]: CMake was found at the path \"{cmake_path.resolve()}\"")
    elif not cmake_exists:
        print("[FLAMEUI]: CMake not found.\n[FLAMEUI]: If you wish to install cmake, visit https://cmake.org/download/")
        exit()


def check_cmd_line_tools_installation():
    print("[FLAMEUI]: Searching for \'make\'...")
    make_path = list(pathlib.Path('/usr/').glob('**/make'))
    if make_path:
        make_exists = True
    elif not make_path:
        make_exists = False
    else:
        make_exists = False

    if make_exists:
        print(
            f"[FLAMEUI]: \'Make\' was found at the path \"{make_path[0].resolve()}\"")
    elif not make_exists:
        print("[FLAMEUI]: \'Make\' not found.\n[FLAMEUI]: To install \'make\', you need to install \'Command Line Tools\' using the command \'xcode-select --install\' in the Terminal App.")
        exit()
