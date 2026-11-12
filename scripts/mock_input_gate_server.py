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

scores_db = []

class MockGateHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)

        if parsed.path == "/ranking.json":
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()

            # Generate ranking list
            qs = urllib.parse.parse_qs(parsed.query)
            song_id = qs.get("song", ["unknown"])[0]
            diff = qs.get("diff", ["0"])[0]

            rankings = []
            # Include submitted scores if any exist
            for idx, sc in enumerate(sorted(scores_db, key=lambda x: x.get("score", 0), reverse=True)):
                rankings.append({
                    "rank": idx + 1,
                    "player_name": sc.get("player_name", "Player"),
                    "score": sc.get("score", 0),
                    "play_style": "Keyboard",
                    "date": "2026-05-20"
                })

            base_rank = len(rankings)
            for i in range(20 - base_rank):
                rankings.append({
                    "rank": base_rank + i + 1,
                    "player_name": f"ServerPlayer_{base_rank + i + 1}",
                    "score": 10000000 - ((base_rank + i) * 15000),
                    "play_style": "Gamepad" if i % 3 == 0 else "Keyboard",
                    "date": "2026-05-20"
                })

            self.wfile.write(json.dumps(rankings).encode('utf-8'))
            return

        return super().do_GET()

    def do_POST(self):
        content_length = int(self.headers.get("Content-Length", 0))
        post_data = self.rfile.read(content_length)

        try:
            score_data = json.loads(post_data.decode("utf-8"))
            scores_db.append(score_data)
            with open("scores.json", "w") as f:
                json.dump(scores_db, f)

            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            response = {
                "status": "ok",
                "message": "Score recorded",
                "received_score": score_data.get("score", 0)
            }
            self.wfile.write(json.dumps(response).encode("utf-8"))
        except Exception as e:
            self.send_response(400)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            response = {"status": "error", "message": str(e)}
            self.wfile.write(json.dumps(response).encode("utf-8"))

# Serve
os.chdir("mock_server")

with socketserver.TCPServer(("", PORT), MockGateHandler) as httpd:
    print(f"Serving Mock Input Gate at port {PORT}")
    httpd.serve_forever()
