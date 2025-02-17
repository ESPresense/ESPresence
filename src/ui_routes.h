/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,360 bytes
 * ui_app_immutable_chunks_js: 44,333 bytes
 * ui_app_immutable_entry_js: 224 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,730 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsIndexDn2mWWghCss);
    server->on("/app/immutable/assets/index.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsIndexBfkQnpt2Css);
    server->on("/app/immutable/chunks/D-SmHvjv.js", HTTP_GET, serveAppImmutableChunksDSmHvjvJs);
    server->on("/app/immutable/entry/app.TJ2wQKJr.js", HTTP_GET, serveAppImmutableEntryAppTj2wQkJrJs);
    server->on("/app/immutable/entry/start.kcfZn_UO.js", HTTP_GET, serveAppImmutableEntryStartKcfZnUoJs);
    server->on("/app/immutable/nodes/0.CcoGyTEH.js", HTTP_GET, serveAppImmutableNodes_0CcoGyTehJs);
    server->on("/app/immutable/nodes/1.CHthV01u.js", HTTP_GET, serveAppImmutableNodes_1CHthV01uJs);
    server->on("/app/immutable/nodes/2.BfyQha3v.js", HTTP_GET, serveAppImmutableNodes_2BfyQha3vJs);
    server->on("/app/immutable/nodes/3.cZP3swCq.js", HTTP_GET, serveAppImmutableNodes_3CZp3swCqJs);
    server->on("/app/immutable/nodes/4.Dvwn-4N5.js", HTTP_GET, serveAppImmutableNodes_4Dvwn_4N5Js);
    server->on("/app/immutable/nodes/5.CEBnuzZr.js", HTTP_GET, serveAppImmutableNodes_5CeBnuzZrJs);
    server->on("/app/immutable/nodes/6.BrmfJV5d.js", HTTP_GET, serveAppImmutableNodes_6BrmfJv5dJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
