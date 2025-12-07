import os
from ntt_project_manager import Manager


def main():
    Manager(baseDir=os.getcwd())


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"An error occurred: {e}")
