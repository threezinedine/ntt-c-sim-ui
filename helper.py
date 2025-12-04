import os
import argparse
from config.utils import RunCommand, ValidateCommand


def Build(type: str) -> None:
    if os.name == "nt":
        RunCommand(
            f'cmake -S simui -B simui/build -DSIMUI_BUILD_EXAMPLES=ON -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE={type.capitalize()}'
        )
    else:
        RunCommand(
            f"cmake -S simui -B simui/build -DSIMUI_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE={type.capitalize()}"
        )

    RunCommand(f"cmake --build simui/build --config {type.capitalize()}")


def main() -> None:
    ValidateCommand("git")
    ValidateCommand("cmake")

    parser = argparse.ArgumentParser(
        description="Build and run SimUI example applications."
    )

    parser.add_argument(
        "action",
        choices=["build", "run"],
        help="Action to perform: build or run the examples.",
    )

    parser.add_argument(
        "-e",
        "--example",
        type=str,
        default="",
        help="Name of the example application to run (required for 'run' action).",
    )

    parser.add_argument(
        "-t",
        "--type",
        type=str,
        choices=["debug", "release"],
        default="debug",
        help="Build type: debug or release (default: debug).",
    )

    args = parser.parse_args()

    if args.action == "build":
        Build(args.type)
    elif args.action == "run":
        Build(args.type)
        if not args.example:
            raise ValueError("Example name must be provided for 'run' action.")
        if os.name == "nt":
            RunCommand(f"simui\\build\\examples\\Debug\\{args.example}.exe")
        else:
            RunCommand(f"./simui/build/examples/{args.example}")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}")
        exit(1)
