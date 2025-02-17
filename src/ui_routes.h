/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,370 bytes
 * ui_app_immutable_chunks_js: 44,348 bytes
 * ui_app_immutable_entry_js: 225 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,756 bytes
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
    server->on("/app/immutable/chunks/BcIbV14M.js", HTTP_GET, serveAppImmutableChunksBcIbV14MJs);
    server->on("/app/immutable/entry/app.1Dv5VJOP.js", HTTP_GET, serveAppImmutableEntryApp_1Dv5VjopJs);
    server->on("/app/immutable/entry/start.Dk8HtdBz.js", HTTP_GET, serveAppImmutableEntryStartDk8HtdBzJs);
    server->on("/app/immutable/nodes/0.DQHp1eNP.js", HTTP_GET, serveAppImmutableNodes_0DqHp1eNpJs);
    server->on("/app/immutable/nodes/1.DlHv4TM8.js", HTTP_GET, serveAppImmutableNodes_1DlHv4Tm8Js);
    server->on("/app/immutable/nodes/2.DckCM30p.js", HTTP_GET, serveAppImmutableNodes_2DckCm30pJs);
    server->on("/app/immutable/nodes/3.BRIT-msC.js", HTTP_GET, serveAppImmutableNodes_3BritMsCJs);
    server->on("/app/immutable/nodes/4.C2hfCMpT.js", HTTP_GET, serveAppImmutableNodes_4C2hfCMpTJs);
    server->on("/app/immutable/nodes/5.DrHAGLNE.js", HTTP_GET, serveAppImmutableNodes_5DrHaglneJs);
    server->on("/app/immutable/nodes/6.C0JN8b8D.js", HTTP_GET, serveAppImmutableNodes_6C0Jn8b8DJs);
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
