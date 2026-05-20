import http.server
import socketserver
import json
import zipfile
import os
import io
import urllib.parse

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

version_info = {
    "latest_version": "2.0.0-alpha99",
    "download_url": "http://localhost:8080/update.zip",
    "patch_notes": "Mock Update from Local Server:\n- Server-side parsing works!\n- Bug fixes"
}

with open("mock_server/version.json", "w") as f:
    json.dump(version_info, f)

class MockGateHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)

        if parsed.path == "/ranking.json":
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()

            # Generate fake ranking
            qs = urllib.parse.parse_qs(parsed.query)
            song_id = qs.get("song", ["unknown"])[0]
            diff = qs.get("diff", ["0"])[0]

            rankings = []
            for i in range(20):
                rankings.append({
                    "rank": i + 1,
                    "player_name": f"ServerPlayer_{i+1}",
                    "score": 10000000 - (i * 15000),
                    "play_style": "Gamepad" if i % 3 == 0 else "Keyboard",
                    "date": "2026-05-20"
                })

            self.wfile.write(json.dumps(rankings).encode('utf-8'))
            return

        return super().do_GET()

# Serve
os.chdir("mock_server")

with socketserver.TCPServer(("", PORT), MockGateHandler) as httpd:
    print(f"Serving Mock Input Gate at port {PORT}")
    httpd.serve_forever()
