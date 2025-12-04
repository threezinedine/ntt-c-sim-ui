import subprocess


def RunCommand(
    command: str,
    folder: str | None = None,
) -> None:
    """
    Run a system command and return its output.

    Args:
        command (str): The command to run.
        folder (str | None): The folder in which to run the command. If None, runs in the current directory.
    """

    subprocess.run(
        command,
        shell=True,
        cwd=folder,
    )


def ValidateCommand(
    command: str,
) -> bool:
    """
    Validate if a system command can be executed.

    Args:
        command (str): The command to validate.
    """

    try:
        subprocess.run(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            shell=True,
        )
        return True
    except FileNotFoundError:
        return False
