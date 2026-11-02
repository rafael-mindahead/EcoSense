Import("env")

from pathlib import Path

project_dir = Path(env["PROJECT_DIR"])
repo_root = project_dir.parent
web_dir = repo_root / "web"
output = Path(env["PROJECT_INCLUDE_DIR"]) / "GeneratedWebAssets.h"

mime_types = {
    ".html": "text/html; charset=utf-8",
    ".css": "text/css; charset=utf-8",
    ".js": "application/javascript; charset=utf-8",
}

files = sorted(
    path for path in web_dir.rglob("*")
    if path.is_file() and path.suffix.lower() in mime_types
)

lines = [
    "#pragma once",
    "",
    "#include <Arduino.h>",
    "",
    "struct EmbeddedWebAsset {",
    "    const char* route;",
    "    const char* contentType;",
    "    const char* content;",
    "};",
    "",
]

entries = []

for index, path in enumerate(files):
    relative = path.relative_to(web_dir)
    if relative.parts[0] == "html" and path.suffix.lower() == ".html":
        route = "/" + path.stem
    else:
        route = "/" + relative.as_posix()

    content = path.read_text(encoding="utf-8")
    delimiter = f"ES{index}"
    while f"){delimiter}\"" in content:
        delimiter += "X"

    symbol = f"WEB_ASSET_{index}"
    lines.append(
        f'static const char {symbol}[] PROGMEM = R"{delimiter}({content}){delimiter}";'
    )
    lines.append("")
    entries.append((route, mime_types[path.suffix.lower()], symbol))

lines.append("static const EmbeddedWebAsset EMBEDDED_WEB_ASSETS[] = {")
for route, content_type, symbol in entries:
    lines.append(
        f'    {{"{route}", "{content_type}", {symbol}}},'
    )
lines.extend([
    "};",
    "",
    "inline const EmbeddedWebAsset* findEmbeddedWebAsset(const String& route)",
    "{",
    "    for (const auto& asset : EMBEDDED_WEB_ASSETS) {",
    "        if (route == asset.route) {",
    "            return &asset;",
    "        }",
    "    }",
    "",
    "    return nullptr;",
    "}",
    "",
])

output.write_text("\n".join(lines), encoding="utf-8")
print(f"Embedded {len(entries)} web assets into {output}")
