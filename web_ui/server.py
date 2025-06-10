import http.server
import socketserver
import json
import os

PORT = 8000
PARAMS_FILE = "params.json"
STATIC_DIR = "."

class Handler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Access-Control-Allow-Origin", "*")
        super().end_headers()

    def do_GET(self):
        if self.path == "/api/params":
            try:
                with open(PARAMS_FILE, "r") as f:
                    data = json.load(f)
                self._send_json(data)
            except Exception as e:
                self.send_error(500, f"Error reading params: {e}")
        else:
            # Serve static files
            if self.path == "/":
                self.path = "/index.html"
            super().do_GET()

    def do_POST(self):
        if self.path == "/api/update":
            length = int(self.headers.get("Content-Length", 0))
            body = self.rfile.read(length).decode()

            try:
                data = json.loads(body)
                print("\n[POST /api/update] Received:")
                print(json.dumps(data, indent=2))
                self._send_json({"status": "ok"})
            except json.JSONDecodeError:
                self.send_error(400, "Invalid JSON")
        else:
            self.send_error(404)

    def _send_json(self, data):
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())

os.chdir(STATIC_DIR)

with socketserver.TCPServer(("", PORT), Handler) as httpd:
    print(f"Serving UI at http://localhost:{PORT}")
    httpd.serve_forever()
