import http.server
import socketserver
import json
import zipfile
import os
import io

PORT = 8080

# Ensure dummy song zip exists
if not os.path.exists("mock_server"):
    os.makedirs("mock_server")

# Create a dummy zip
zip_buffer = io.BytesIO()
with zipfile.ZipFile(zip_buffer, "w", zipfile.ZIP_DEFLATED) as zip_file:
    zip_file.writestr("test_song/test.ksh", "title=Test Song\nartist=Test Artist\n")
    zip_file.writestr("test_song/jacket.png", "fake png data")

with open("mock_server/test_song.zip", "wb") as f:
    f.write(zip_buffer.getvalue())

# Create song list JSON
songs = [
    {
        "id": "test_song",
        "title": "Test Song (Local)",
        "artist": "Local Artist",
        "jacket_url": f"http://localhost:{PORT}/jacket.png",
        "download_url": f"http://localhost:{PORT}/test_song.zip",
        "preview_url": "",
        "hash": "dummy_hash",
        "size": len(zip_buffer.getvalue())
    }
]

with open("mock_server/list.json", "w") as f:
    json.dump(songs, f)

# Serve
os.chdir("mock_server")
Handler = http.server.SimpleHTTPRequestHandler

with socketserver.TCPServer(("", PORT), Handler) as httpd:
    print(f"Serving at port {PORT}")
    httpd.serve_forever()
