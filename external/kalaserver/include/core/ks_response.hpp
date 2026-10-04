//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

#include "core_utils.hpp"

namespace KalaServer::Core
{
    using std::string;
    using std::string_view;
    using std::vector;

    using u8 = uint8_t;

    enum class OptionalSendType : u8
    {
        S_INVALID = 0u,

        S_DOWNLOAD = 1u,
        //prevents browser from caching this, good for sensitive data
        //adds:
        //  Cache-Control: no-store, no-cache, must-revalidate
        //  Pragma: no-cache
        //  Expires: 0
        S_NO_CACHE = 2u,
        //force-closes the socket, ignores all other send types
        //should be used when:
        //  client uses blacklisted keyword
        //  client abuses connection (rate limit etc)
        //  client sends malformed request
        //  client requests invalid content length size for Content-Length
        //  client sends garbage instead of valid hex chunk size
        //  client sends both Content-Length and Transfer-Encoding: chunked
        //  server cannot trust where next TCP stream request begins
        //adds:
        //  Connection: close
        S_FORCE_CLOSE = 3u
    };

    enum class ResponseType : u8
    {
        R_INVALID = 0u,

        //OK
        R_200 = 1u,
        //No content
        R_204 = 2u,
        //Partial content
        R_206 = 3u,

        //Bad request
        R_400 = 4u,
        //Unauthorized
        R_401 = 5u,
        //Not found
        R_404 = 6u,
        //Method not allowed
        R_405 = 7u,
        //Payload too large
        R_413 = 8u,
        //I'm a teapot
        R_418 = 9u,

        //Internal server error
        R_500 = 10u,
        //Service unavailable
        R_503 = 11u,
        //HTTP version not supported
        R_505 = 12u
    };

    enum class ContentType : u8
    {
        CT_INVALID = 0u,

        //
        // SCRIPTS
        //

        //.html, text/html
        CT_HTML = 1u,
        //.css, text/css
        CT_CSS = 2u,
        //.js, application/javascript
        CT_JS = 3u,

        //
        // IMAGES
        //

        //.png, image/png
        CT_PNG = 4u,
        //.jpeg, image/jpeg
        CT_JPEG = 5u,
        //.jpg, image/jpeg
        CT_JPG = 6u,
        //.gif, image/gif
        CT_GIF = 7u,
        //.webp, image/webp
        CT_WEBP = 8u,
        //.ico, image/vnd.microsoft.icon
        CT_ICO = 9u,
        //.svg, image/svg+xml
        CT_SVG_XML = 10u,

        //
        // AUDIO
        //

        //.mp3, audio/mpeg
        CT_MP3 = 11u,
        //.m4a, audio/mp4
        CT_M4A = 12u,
        //.aac, audio/aac
        CT_AAC = 13u,
        //.ogg, audio/ogg
        CT_OGG = 14u,
        //.opus, audio/opus
        CT_OPUS = 15u,
        //.flac, audio/flac
        CT_FLAC = 16u,
        //.wav, audio/wav
        CT_WAV = 17u,

        //
        // VIDEO
        //

        //.mp4, video/mp4
        CT_MP4 = 18u,
        //.webm, video/webm
        CT_WEBM = 19u,
        //.mkv, video/x-matroska
        CT_MKV = 20u,
        //.mov, video/quicktime
        CT_MOV = 21u,
        //.avi, video/x-msvideo
        CT_AVI = 22u,

        //
        // FONTS
        //

        //.woff, font/woff
        CT_WOFF = 23u,
        //.woff2, font/woff2
        CT_WOFF2 = 24u,
        //.ttf, font/ttf
        CT_TTF = 25u,
        //.otf, font/otf
        CT_OTF = 26u,

        //
        // DOCUMENTS AND TEXT
        //

        //.txt, text/plain
        CT_PLAIN = 27u,
        //.json, application/json
        CT_JSON = 28u,
        //.xml, application/xml
        CT_XML = 29u,
        //.csv, text/csv
        CT_CSV = 30u,
        //.md, text/markdown
        CT_MARKDOWN = 31u,

        //
        // ARCHIVES AND BINARIES
        //

        //.zip, application/zip
        CT_ZIP = 32u,
        //.gz, application/gzip
        CT_GZ = 33u,
        //.tar, application/x-tar
        CT_TAR = 34u,
        //.7z, application/x-7z-compressed
        CT_7Z = 35u,
        //.rar, application/vnd.rar
        CT_RAR = 36u,
        //.pdf, application/pdf
        CT_PDF = 37u,
        //.wasm, application/wasm
        CT_WASM = 38u,

        //unknown types, application/octet-stream
        CT_OCTET = 39u
    };

    //forward-declare ks_core.hpp Connection struct
	struct Connection;

    //Who receives what response
    struct LIB_API ResponseData
    {
        ResponseType responseType{};
        ContentType contentType{};
        vector<OptionalSendType> optionalSendTypes{};

        //If left empty then this is filled with placeholder data for the target response type.
        //Exceptions:
        //200 or 206 but empty body = 204
        //200 or 206 but generation failed = 500
        string responseBody{};

        Connection* connection{};
        uintptr_t connectionSocket{};
    };

    class LIB_API Response
    {
    public:
        static void SendResponse(const ResponseData& data);
        
        static ContentType ExtensionToContentType(string_view input);
        static string_view ContentTypeToExtension(ContentType type);

        static ContentType MimeTypeToContentType(string_view input);
        static string_view ContentTypeToMimeType(ContentType type);

        static ResponseType StringToResponseType(string_view input);
        static string_view ResponseTypeToString(ResponseType type);

        static OptionalSendType StringToSendType(string_view input);
        static string_view SendTypeToString(OptionalSendType type);
    };
}