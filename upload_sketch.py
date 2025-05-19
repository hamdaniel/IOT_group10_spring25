import subprocess
import argparse

def upload_sketch(sketch_dir, port_number):
    fqbn = "esp32:esp32:esp32doit-devkit-v1"
    port = f"COM{port_number}"

    compile_cmd = [
        "arduino-cli",
        "compile",
        "--fqbn", fqbn,
        "--port", port,
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
        subprocess.run(upload_cmd, check=True)
        print(f"Successfully uploaded sketch '{sketch_dir}'")
    except subprocess.CalledProcessError as e:
        print(f"Upload failed: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Compile and upload an Arduino sketch.")
    parser.add_argument("sketch_dir", help="Path to the sketch directory")
    parser.add_argument("--p", type=int, default=7, help="Port number (e.g., 7 for COM7)")
    
    args = parser.parse_args()

    upload_sketch(args.sketch_dir, args.p)
