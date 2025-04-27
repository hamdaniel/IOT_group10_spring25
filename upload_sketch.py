import subprocess
import sys

def upload_sketch(sketch_name):
    fqbn = "esp32:esp32:esp32doit-devkit-v1"
    port = "COM7"
    
    compile= [
        "arduino-cli",
        "compile",
        "--fqbn", fqbn,
        "--port", port,
        sketch_name
    ]
    upload= [
        "arduino-cli",
        "upload",
        "--fqbn", fqbn,
        "--port", port,
        sketch_name
    ]
    
    try:
        subprocess.run(compile, check=True)
        subprocess.run(upload, check=True)
        print(f"Successfully uploaded sketch '{sketch_name}'")
    except subprocess.CalledProcessError as e:
        print(f"Upload failed: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python upload_sketch.py <SketchName>")
        sys.exit(1)
    
    sketch_name = sys.argv[1]
    upload_sketch(sketch_name)
