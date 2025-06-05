from http.server import BaseHTTPRequestHandler, HTTPServer
import os
import json
import mimetypes

PARAMS_FILE = "params.json"
INDEX_FILE = "index.html"

class Handler(BaseHTTPRequestHandler):
    def _send_json(self, data, status=200):
        body = json.dumps(data).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def _send_file(self, path):
        try:
            with open(path, "rb") as f:
                content = f.read()
            self.send_response(200)
            mime, _ = mimetypes.guess_type(path)
            self.send_header("Content-Type", mime or "application/octet-stream")
            self.send_header("Content-Length", str(len(content)))
            self.end_headers()
            self.wfile.write(content)
        except FileNotFoundError:
            self.send_error(404)

    def do_GET(self):
        if self.path == "/params":
            if not os.path.exists(PARAMS_FILE):
                self.send_error(500, "params.json not found")
            else:
                with open(PARAMS_FILE) as f:
                    data = json.load(f)
                self._send_json(data)
        elif self.path == "/" or self.path == "/index.html":
            self._send_file(INDEX_FILE)
        else:
            # Serve static assets if any
            rel_path = self.path.lstrip("/")
            if os.path.exists(rel_path):
                self._send_file(rel_path)
            else:
                self.send_error(404)

def do_POST(self):
    if self.path == "/params":
        length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(length).decode()

        try:
            data = json.loads(body)
            print("\n[POST /params] Received update:")
            print(json.dumps(data, indent=2))
        except json.JSONDecodeError:
            print("\n[POST /params] Invalid JSON received:")
            print(body)

        self._send_json({"status": "ok"})
    else:
        self.send_error(404)


if __name__ == "__main__":
    print("Serving on http://localhost:8080")
    HTTPServer(('localhost', 8080), Handler).serve_forever()
