import subprocess
import argparse

def upload_sketch(sketch_dir, port_number, compile_only):
    fqbn = "esp32:esp32:esp32doit-devkit-v1"
    port = f"COM{port_number}"

    compile_cmd = [
        "arduino-cli",
        "compile",
        "--fqbn", fqbn,
        sketch_dir
    ]
    upload_cmd = [
        "arduino-cli",
        "upload",
        "--fqbn", fqbn,
        "--port", port,
        sketch_dir
    ]

    try:
        subprocess.run(compile_cmd, check=True)
        print(f"Successfully compiled sketch '{sketch_dir}'")

        if not compile_only:
            subprocess.run(upload_cmd, check=True)
            print(f"Successfully uploaded sketch '{sketch_dir}'")
        else:
            print("Compile-only mode enabled. Skipping upload.")

    except subprocess.CalledProcessError as e:
        print(f"Operation failed: {e}")

if __name__ == "__main__":
    def_dir = "PoC"
    parser = argparse.ArgumentParser(description="Compile and upload an Arduino sketch.")
    parser.add_argument("sketch_dir", nargs="?", default=def_dir, help="Path to the sketch directory")
    parser.add_argument("--p", type=int, default=7, help="Port number (e.g., 7 for COM7)")
    parser.add_argument("--compile-only", action="store_true", help="Only compile the sketch, do not upload")

    args = parser.parse_args()

    upload_sketch(args.sketch_dir, args.p, args.compile_only)
