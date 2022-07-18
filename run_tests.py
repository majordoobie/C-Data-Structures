import argparse
import pathlib
import subprocess
from multiprocessing import cpu_count
from pathlib import Path
from typing import Union, List


def main() -> None:
    args = _get_args()
    if args.list_all:
        for test_name in _get_test_names():
            print(test_name)

    elif args.run_all:
        subprocess.run(f"ctest -j {cpu_count()} --test-dir build", shell=True)

    else:
        if args.type not in _get_test_names():
            exit("Could not find the test name you supplied. Use the "
                 "--list-test to see all available tests")
        else:
            gtest = _get_gtest_binary(args.type)
            if gtest is None:
                raise exit("Could not find a gtest")
            subprocess.run(gtest.as_posix())


def _get_gtest_binary(name: str) -> Union[pathlib.Path, None]:
    test_path = Path(".") / "build/src"
    if not test_path.exists() or not test_path.is_dir():
        raise exit("Could not find the path to the data structures")

    for proj_dir in test_path.iterdir():
        if proj_dir.name == name:
            for file in (proj_dir / "tests").iterdir():
                if file.name.endswith("gtest"):
                    print(file)
                    return file

    return None


def _get_test_names() -> List[str]:
    test_path = Path(".") / "src"
    if not test_path.exists() or not test_path.is_dir():
        raise exit("Could not find the path to the data structures")

    return [proj_dir.name for proj_dir in test_path.iterdir() if proj_dir.name != ".DS_Store"]


def _get_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="%(prog)s runs either all unit tests in the project or"
                    "the tests for a specific data structure"
    )

    parser.add_argument(
        "-l", "--list-tests",
        help="List the name of the tests you can choose",
        action="store_true",
        dest="list_all",
        default=False
    )
    group = parser.add_mutually_exclusive_group()

    group.add_argument(
        "-r", "--run-all",
        help="Run all tests available",
        dest="run_all",
        action="store_true",
        default=False
    )
    group.add_argument(
        "-t", "--test-name",
        help="Run a specific data structures tests. Use the \"--list-tests\" "
             "to get a listing of the available test names",
        type=str,
        dest="type"
    )
    group.add_argument(
        "-b", "--build",
        type=Path,
        help="Build and compile the project into the build directory specified "
             "(Default: %(default)s"
    )

    return parser.parse_args()


if __name__ == "__main__":
    main()
