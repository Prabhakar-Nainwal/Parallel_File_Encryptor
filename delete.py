import os
import shutil

def delete_generated_files(path):
    """
    Deletes the specified directory and all its contents.

    Args:
        path (str): The path to the directory to be deleted.
    """
    if os.path.exists(path) and os.path.isdir(path):
        try:
            # shutil.rmtree() recursively deletes a directory and all its contents.
            shutil.rmtree(path)
            print(f"Successfully deleted directory and all its contents: {path}")
        except OSError as e:
            print(f"Error: {e.strerror} - Could not delete {path}.")
    elif os.path.exists(path):
        print(f"Error: {path} is a file, not a directory. Cannot cleanup.")
    else:
        print(f"Directory not found, nothing to delete: {path}")

if __name__ == "__main__":
    # The path 'test' matches the one used in your makeFiles script
    delete_generated_files("test")
