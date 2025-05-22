import os

def sanitize_name(name):
    return name.strip().replace(' ', '_')

def rename_all(root_dir):
    for current_root, dirs, files in os.walk(root_dir, topdown=False):
        for filename in files:
            new_name = sanitize_name(filename)
            if filename != new_name:
                old_path = os.path.join(current_root, filename)
                new_path = os.path.join(current_root, new_name)
                os.rename(old_path, new_path)
                print(f"Renamed file: {old_path} -> {new_path}")
        for dirname in dirs:
            new_name = sanitize_name(dirname)
            if dirname != new_name:
                old_path = os.path.join(current_root, dirname)
                new_path = os.path.join(current_root, new_name)
                os.rename(old_path, new_path)
                print(f"Renamed folder: {old_path} -> {new_path}")

if __name__ == "__main__":
    rename_all(os.path.dirname(os.path.abspath(__file__)))
