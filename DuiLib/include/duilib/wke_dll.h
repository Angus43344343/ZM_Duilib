#ifndef __WKE_DLL_H__
#define __WKE_DLL_H__
#pragma once

namespace DuiLib {
#define WKE_CALL __cdecl

typedef char utf8;
typedef __int64 jsValue;

struct JsExecStateInfo;
typedef JsExecStateInfo *jsExecState;

struct _tagWkeWebView;
typedef struct _tagWkeWebView *wkeWebView;

struct _tagWkeString;
typedef struct _tagWkeString *wkeString;

typedef void *wkeWebFrameHandle;
typedef void *wkeNetJob;

typedef struct _tagWkeMediaPlayer *wkeMediaPlayer;
typedef struct _tagWkeMediaPlayerClient *wkeMediaPlayerClient;
typedef struct _tabblinkWebURLRequestPtr *blinkWebURLRequestPtr;


struct wkeRect
{
    int x;
    int y;
    int w;
    int h;
};

struct wkePoint
{
    int x;
    int y;
};

enum wkeMouseFlags
{
    WKE_LBUTTON = 0x01,
    WKE_RBUTTON = 0x02,
    WKE_SHIFT   = 0x04,
    WKE_CONTROL = 0x08,
    WKE_MBUTTON = 0x10,
};

enum wkeKeyFlags
{
    WKE_EXTENDED = 0x0100,
    WKE_REPEAT   = 0x4000,
};

enum wkeMouseMsg
{
    WKE_MSG_MOUSEMOVE     = 0x0200,
    WKE_MSG_LBUTTONDOWN   = 0x0201,
    WKE_MSG_LBUTTONUP     = 0x0202,
    WKE_MSG_LBUTTONDBLCLK = 0x0203,
    WKE_MSG_RBUTTONDOWN   = 0x0204,
    WKE_MSG_RBUTTONUP     = 0x0205,
    WKE_MSG_RBUTTONDBLCLK = 0x0206,
    WKE_MSG_MBUTTONDOWN   = 0x0207,
    WKE_MSG_MBUTTONUP     = 0x0208,
    WKE_MSG_MBUTTONDBLCLK = 0x0209,
    WKE_MSG_MOUSEWHEEL    = 0x020A,
};

enum wkeProxyType
{
    WKE_PROXY_NONE,
    WKE_PROXY_HTTP,
    WKE_PROXY_SOCKS4,
    WKE_PROXY_SOCKS4A,
    WKE_PROXY_SOCKS5,
    WKE_PROXY_SOCKS5HOSTNAME
};

enum wkeMenuItemId
{
    kWkeMenuSelectedAllId      = 1 << 1,
    kWkeMenuSelectedTextId     = 1 << 2,
    kWkeMenuUndoId             = 1 << 3,
    kWkeMenuCopyImageId        = 1 << 4,
    kWkeMenuInspectElementAtId = 1 << 5,
    kWkeMenuCutId              = 1 << 6,
    kWkeMenuPasteId            = 1 << 7,
    kWkeMenuPrintId            = 1 << 8,
    kWkeMenuGoForwardId        = 1 << 9,
    kWkeMenuGoBackId           = 1 << 10,
    kWkeMenuReloadId           = 1 << 11,
    kWkeMenuSaveImageId        = 1 << 12,
};

struct wkeGeolocationPosition
{
    double timestamp;
    double latitude;
    double longitude;
    double accuracy;
    bool providesAltitude;
    double altitude;
    bool providesAltitudeAccuracy;
    double altitudeAccuracy;
    bool providesHeading;
    double heading;
    bool providesSpeed;
    double speed;

    wkeGeolocationPosition(const wkeGeolocationPosition& other)
    {
        timestamp = other.timestamp;
        latitude = other.latitude;
        longitude = other.longitude;
        accuracy = other.accuracy;
        providesAltitude = other.providesAltitude;
        altitude = other.altitude;
        providesAltitudeAccuracy = other.providesAltitudeAccuracy;
        altitudeAccuracy = other.altitudeAccuracy;
        providesHeading = other.providesHeading;
        heading = other.heading;
        providesSpeed = other.providesSpeed;
        speed = other.speed;
    }
};

struct wkeSlist
{
    char *data;
    struct wkeSlist *next;
};

struct wkeProxy
{
    wkeProxyType type;
    char hostname[100];
    unsigned short port;
    char username[50];
    char password[50];
};

enum wkeSettingMask
{
    WKE_SETTING_PROXY                         = 1,
    WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD = 1 << 2,
};

struct wkeSettings
{
    wkeProxy proxy;
    unsigned int mask;
};

struct wkeViewSettings
{
    int size;
    unsigned int bgColor;
};

struct wkeMemBuf
{
    int size;
    void *data;
    size_t length;
};

enum wkeNavigationType
{
    WKE_NAVIGATION_TYPE_LINKCLICK,
    WKE_NAVIGATION_TYPE_FORMSUBMITTE,
    WKE_NAVIGATION_TYPE_BACKFORWARD,
    WKE_NAVIGATION_TYPE_RELOAD,
    WKE_NAVIGATION_TYPE_FORMRESUBMITT,
    WKE_NAVIGATION_TYPE_OTHER
};

enum wkeWebDragOperation
{
    wkeWebDragOperationNone    = 0,
    wkeWebDragOperationCopy    = 1,
    wkeWebDragOperationLink    = 2,
    wkeWebDragOperationGeneric = 4,
    wkeWebDragOperationPrivate = 8,
    wkeWebDragOperationMove    = 16,
    wkeWebDragOperationDelete  = 32,
    wkeWebDragOperationEvery   = 0xffffffff
};
typedef wkeWebDragOperation wkeWebDragOperationsMask;

struct wkeWebDragData
{
    struct Item
    {
        enum wkeStorageType
        {
            // String data with an associated MIME type. Depending on the MIME type, there may be
            // optional metadata attributes as well.
            StorageTypeString,
            // Stores the name of one file being dragged into the renderer.
            StorageTypeFilename,
            // An image being dragged out of the renderer. Contains a buffer holding the image data
            // as well as the suggested name for saving the image to.
            StorageTypeBinaryData,
            // Stores the filesystem URL of one file being dragged into the renderer.
            StorageTypeFileSystemFile,
        } storageType;

        // Only valid when storageType == StorageTypeString.
        wkeMemBuf *stringType;
        wkeMemBuf *stringData;
        // Only valid when storageType == StorageTypeFilename.
        wkeMemBuf *filenameData;
        wkeMemBuf *displayNameData;
        // Only valid when storageType == StorageTypeBinaryData.
        wkeMemBuf *binaryData;
        // Title associated with a link when stringType == "text/uri-list".
        // Filename when storageType == StorageTypeBinaryData.
        wkeMemBuf *title;
        // Only valid when storageType == StorageTypeFileSystemFile.
        wkeMemBuf *fileSystemURL;
        __int64 fileSystemFileSize;
        // Only valid when stringType == "text/html".
        wkeMemBuf *baseURL;
    };

    struct Item *m_itemList;
    int m_itemListLength;

    int m_modifierKeyState; // State of Shift/Ctrl/Alt/Meta keys.
    wkeMemBuf *m_filesystemId;
};

enum wkeHttBodyElementType
{
    wkeHttBodyElementTypeData,
    wkeHttBodyElementTypeFile,
};

struct wkePostBodyElement
{
    int size;
    wkeHttBodyElementType type;
    wkeMemBuf *data;
    wkeString filePath;
    __int64 fileStart;
    __int64 fileLength; // -1 means to the end of the file.
};

struct wkePostBodyElements
{
    int size;
    wkePostBodyElement **element;
    size_t elementSize;
    bool isDirty;
};

enum wkeResourceType
{
    WKE_RESOURCE_TYPE_MAIN_FRAME     = 0,   // top level page
    WKE_RESOURCE_TYPE_SUB_FRAME      = 1,   // frame or iframe
    WKE_RESOURCE_TYPE_STYLESHEET     = 2,   // a CSS stylesheet
    WKE_RESOURCE_TYPE_SCRIPT         = 3,   // an external script
    WKE_RESOURCE_TYPE_IMAGE          = 4,   // an image (jpg/gif/png/etc)
    WKE_RESOURCE_TYPE_FONT_RESOURCE  = 5,   // a font
    WKE_RESOURCE_TYPE_SUB_RESOURCE   = 6,   // an "other" subresource.
    WKE_RESOURCE_TYPE_OBJECT         = 7,   // an object (or embed) tag for a plugin, or a resource that a plugin requested.
    WKE_RESOURCE_TYPE_MEDIA          = 8,   // a media resource.
    WKE_RESOURCE_TYPE_WORKER         = 9,   // the main resource of a dedicated worker.
    WKE_RESOURCE_TYPE_SHARED_WORKER  = 10,  // the main resource of a shared worker.
    WKE_RESOURCE_TYPE_PREFETCH       = 11,  // an explicitly requested prefetch
    WKE_RESOURCE_TYPE_FAVICON        = 12,  // a favicon
    WKE_RESOURCE_TYPE_XHR            = 13,  // a XMLHttpRequest
    WKE_RESOURCE_TYPE_PING           = 14,  // a ping request for <a ping>
    WKE_RESOURCE_TYPE_SERVICE_WORKER = 15,  // the main resource of a service worker.
    WKE_RESOURCE_TYPE_LAST_TYPE
};

struct wkeWillSendRequestInfo
{
    wkeString url;
    wkeString newUrl;
    wkeResourceType resourceType;
    int httpResponseCode;
    wkeString method;
    wkeString referrer;
    void *headers;
};

struct wkeTempCallbackInfo
{
    int size;
    wkeWebFrameHandle frame;
    wkeWillSendRequestInfo *willSendRequestInfo;
    const char *url;
    wkePostBodyElements *postBody;
    wkeNetJob job;
};

struct wkeScreenshotSettings
{
    int structSize;
    int width;
    int height;
};


// miniblink ��̬��ļ������ʼ�����ڲ�ʹ�����ü��������� Load ʱ�����ü�����1
// �˾����������mb apiǰ���ȵ��á���������mb api����͵���wkeInit���߳�Ϊͬ���߳�
// ������
// szWkeDll��miniblink ��·����·�� + �ļ�����
//   ���·�������exe�����·�� + �ļ���
//   ����·����ȫ·�� + �ļ���
// settings��
DUILIB_API bool LoadMiniBlink(LPCTSTR szWkeDll, const wkeSettings *settings = NULL);

// ���ú����ü�����1�������ü���Ϊ0ʱ��ж�� miniblink ��̬�⣬����mb api�������ٱ����ã�������б���
DUILIB_API void FreeMiniBlink(void);

// ����һЩ������
// ����
// typedef struct { wkeProxy proxy; unsigned int mask; } wkeSettings; mask����ȡ:
// WKE_SETTING_PROXY: Ч����wkeSetProxyһ��, ͨ��proxy����
// WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD�����Ǹ��߼��÷�������ʱ��on paint�ص�����������̣߳���ʵ������Ⱦ�̣߳���
// ���������ʵ�ֶ��߳��������ܣ����ܸ��졣
DUILIB_API void wkeConfigure(const wkeSettings *settings);
DUILIB_API bool wkeIsInitialize();
DUILIB_API void wkeUpdate();

DUILIB_API void wkeShutdown();
DUILIB_API void wkeShutdownForDebug(); // ����ʹ�ã����˽�ǧ����ã�

// ��ȡ�汾��
DUILIB_API unsigned int wkeGetVersion();
DUILIB_API CDuiString wkeGetVersionStr();

// ����һ��webview�����������洰�ڡ�һ������������Ⱦ�����Ϸ
DUILIB_API wkeWebView wkeCreateWebView();
// Ч��ͬwkeDestroyWebWindow
DUILIB_API void wkeDestroyWebView(wkeWebView webView);

// ����һЩwebview��ص�����.Ŀǰֻ�б�����ɫ��������
DUILIB_API void wkeSetViewSettings(wkeWebView webView, const wkeViewSettings *settings);
// ����һЩʵ����ѡ�
// ������
// debugString��
// "showDevTools"   ���������߹��ߣ���ʱparamҪ��д�����߹��ߵ���Դ·������file:///c:/miniblink-release/front_end/inspector.html��ע��param��ʱ������utf8����
// "wakeMinInterval"        ����֡�ʣ�Ĭ��ֵ��10��ֵԽ��֡��Խ��
// "drawMinInterval"        ����֡�ʣ�Ĭ��ֵ��3��ֵԽ��֡��Խ��
// "antiAlias"              ���ÿ������Ⱦ��param��������Ϊ"1"
// "minimumFontSize"        ��С����
// "minimumLogicalFontSize" ��С�߼�����
// "defaultFontSize"        Ĭ������
// "defaultFixedFontSize"   Ĭ��fixed����
// "consoleOutput"          ����ֵΪ"1"ʱ���ɹر�console��dbgview�µ����
DUILIB_API void wkeSetDebugConfig(wkeWebView webView, const CDuiString &debugString, const CDuiString &param);
DUILIB_API void *wkeGetDebugConfig(wkeWebView webView, const CDuiString &debugString);

// ����������Դ���ռ��
DUILIB_API void wkeSetResourceGc(wkeWebView webView, long intervalSec);

// �����ڴ滺�档��ҳ��ͼƬ�ȶ������ڴ滺����رպ��ڴ�ʹ�ûή��һЩ������������һЩ���⣬���������ô�ã���ñ�
DUILIB_API void wkeSetMemoryCacheEnable(wkeWebView webView, bool b);
// �����ر������Ϣ�������ڿ��������󣬹ر������Ϣ
DUILIB_API void wkeSetMouseEnabled(wkeWebView webView, bool b);
// ��������ģʽ�������������Ϣ���Զ�ת���ɴ�����Ϣ
DUILIB_API void wkeSetTouchEnabled(wkeWebView webView, bool b);
DUILIB_API void wkeSetContextMenuEnabled(wkeWebView webView, bool b);
// �رպ󣬵�a��ǩ�����ᵯ���´��ڣ������ڱ�������ת
DUILIB_API void wkeSetNavigationToNewWindowEnable(wkeWebView webView, bool b);
// �رպ󣬿����齫����ֹ����ʱ�������κο�������������ajax����������iframe
DUILIB_API void wkeSetCspCheckEnable(wkeWebView webView, bool b);
// �����ر�npapi�������flash
DUILIB_API void wkeSetNpapiPluginsEnabled(wkeWebView webView, bool b);
// ������ͷģʽ�������󣬽�������Ⱦҳ�棬��������ҳ���ܡ��˹��ܷ�������ʵ��һЩ���棬����ˢ������
DUILIB_API void wkeSetHeadlessEnabled(wkeWebView webView, bool b);
// �ɹر���ק�ļ�������ҳ
DUILIB_API void wkeSetDragEnable(wkeWebView webView, bool b);
// �ɹر���ק����������
DUILIB_API void wkeSetDragDropEnable(wkeWebView webView, bool b);
DUILIB_API void wkeSetLanguage(wkeWebView webView, const CDuiString &language);
// ����ĳ��menu�Ƿ���ʾ
DUILIB_API void wkeSetContextMenuItemShow(wkeWebView webView, wkeMenuItemId item, bool isShow);

DUILIB_API void wkeSetViewNetInterface(wkeWebView webView, const CDuiString &netInterface);

// ��������mb�Ĵ��롣�˾���ȫ����Ч
DUILIB_API void wkeSetProxy(const wkeProxy *proxy);
// ��������mb�Ĵ��롣�˾�������ض�webview��Ч
DUILIB_API void wkeSetViewProxy(wkeWebView webView, wkeProxy *proxy);

DUILIB_API CDuiString wkeGetName(wkeWebView webView);
DUILIB_API void wkeSetName(wkeWebView webView, const CDuiString &name);

// ����wkeWebView��Ӧ�Ĵ��ھ����
// ע�⣺ֻ�����޴���ģʽ�²���ʹ�á��������wkeCreateWebWindow������webview���Ѿ��Դ����ھ���ˡ�
DUILIB_API void wkeSetHandle(wkeWebView webView, HWND wnd);
// �����޴���ģʽ�µĻ���ƫ�ơ���ĳЩ����£���Ҫ������ģʽ�������Ƶĵط������洰�ڵ�(0, 0)��������Ҫ�ֶ����ô˽ӿ�void wkeSetCspCheckEnable(wkeWebView webView, bool b)
// �رպ󣬿����齫����ֹ����ʱ�������κο�������������ajax����������iframe
DUILIB_API void wkeSetHandleOffset(wkeWebView webView, int x, int y);

// �жϴ����Ƿ��Ƿֲ㴰�ڣ�layer window��
DUILIB_API bool wkeIsTransparent(wkeWebView webView);
// ֪ͨ�޴���ģʽ�£�webview����͸��ģʽ��
DUILIB_API void wkeSetTransparent(wkeWebView webView, bool transparent);

// ����webview��UA
DUILIB_API void wkeSetUserAgent(wkeWebView webView, const CDuiString &userAgent);
// ��ȡwebview��UA
DUILIB_API CDuiString wkeGetUserAgent(wkeWebView webView);

typedef void(WKE_CALL *wkeOnShowDevtoolsCallback)(wkeWebView webView, void *param);
DUILIB_API void wkeShowDevtools(wkeWebView webView, const CDuiString &path,
                                wkeOnShowDevtoolsCallback callback, void *param);

// ����url��url����������·������"http://qq.com/"
DUILIB_API void wkeLoadURL(wkeWebView webView, const CDuiString &url);
DUILIB_API void wkePostURL(wkeWebView wkeView, const CDuiString &url, const CDuiString &postData);
// ����һ��html
// ��������
// ע�⣺���html�������·�����������exe����Ŀ¼��·��
DUILIB_API void wkeLoadHTML(wkeWebView webView, const CDuiString &html);
// ����һ��html��������ָ��baseURL��Ҳ����������ĸ�Ŀ¼��url
DUILIB_API void wkeLoadHtmlWithBaseUrl(wkeWebView webView, const CDuiString &html, const CDuiString &baseUrl);

DUILIB_API void wkeLoadFile(wkeWebView webView, const CDuiString &filename);

// ��ȡwebview��frame��url
DUILIB_API CDuiString wkeGetURL(wkeWebView webView);

// ��ȡframe��Ӧ��url
DUILIB_API CDuiString wkeGetFrameUrl(wkeWebView webView, wkeWebFrameHandle frameId);

DUILIB_API bool wkeIsLoading(wkeWebView webView);
DUILIB_API bool wkeIsLoadingSucceeded(wkeWebView webView);
DUILIB_API bool wkeIsLoadingFailed(wkeWebView webView);
DUILIB_API bool wkeIsLoadingCompleted(wkeWebView webView);
// DOM �ĵ��ṹ�Ƿ������ɡ�
DUILIB_API bool wkeIsDocumentReady(wkeWebView webView);
// ֹͣ����ҳ��
DUILIB_API void wkeStopLoading(wkeWebView webView);
// ���¼���ҳ��
DUILIB_API void wkeReload(wkeWebView webView);
DUILIB_API void wkeGoToOffset(wkeWebView webView, int offset);
DUILIB_API void wkeGoToIndex(wkeWebView webView, int index);

DUILIB_API int wkeGetWebviewId(wkeWebView webView);
DUILIB_API bool wkeIsWebviewAlive(int id);
DUILIB_API bool wkeIsWebviewValid(wkeWebView webView);

DUILIB_API CDuiString wkeGetDocumentCompleteURL(wkeWebView webView, wkeWebFrameHandle frameId,
                                                const CDuiString &partialURL);

DUILIB_API wkeMemBuf *wkeCreateMemBuf(wkeWebView webView, void *buf, size_t length);
DUILIB_API void wkeFreeMemBuf(wkeMemBuf *buf);

// ��ȡҳ�����
DUILIB_API CDuiString wkeGetTitle(wkeWebView webView);

// ��������ҳ��Ŀ�ߡ����webView�Ǵ�����ģʽ�ģ��������洰�ڵĿ�ߡ�
DUILIB_API void wkeResize(wkeWebView webView, int w, int h);
// ��ȡҳ����
DUILIB_API int wkeGetWidth(wkeWebView webView);
// ��ȡҳ��߶�
DUILIB_API int wkeGetHeight(wkeWebView webView);
// ��ȡ��ҳ�Ű�����Ŀ��
DUILIB_API int wkeGetContentWidth(wkeWebView webView);
// ��ȡ��ҳ�Ű�����ĸ߶�
DUILIB_API int wkeGetContentHeight(wkeWebView webView);

DUILIB_API void wkeSetDirty(wkeWebView webView, bool dirty);
DUILIB_API bool wkeIsDirty(wkeWebView webView);
DUILIB_API void wkeAddDirtyArea(wkeWebView webView, int x, int y, int w, int h);
DUILIB_API void wkeLayoutIfNeeded(wkeWebView webView);
// ��ȡҳ�����ء�
// ������
// bits �ⲿ���벢���ݸ�mb��buffer����С��bufWid * bufHei * 4 �ֽ�
// bufW��bufH  bits�Ŀ��
// xDst��yDst  ���Ƶ�bits���ĸ�����
// w��h��xSrc��ySrc  mb��Ҫȡ�Ļ������ʼ����
// bCopyAlpha   �Ƿ񿽱������͸����ֵ
// ע�⣺�˺���һ���3d��Ϸʹ�á�����Ƶ��ʹ�ô˽ӿڲ������������������⡣�����wkeGetViewDC��ȥ����dc��
DUILIB_API void wkePaint2(wkeWebView webView, void *bits, int bufW, int bufH, int xDst, int yDst, int w,
                          int h, int xSrc, int ySrc, bool bCopyAlpha);
// ��ȡҳ������صļ򻯰溯����
// ������
// bits���ⲿ���벢���ݸ�mb��buffer����С��webview��� * �߶� * 4 �ֽڡ�
// pitch����0���ɡ�����������directX����Ӧ�ö�
DUILIB_API void wkePaint(wkeWebView webView, void *bits, int pitch);
DUILIB_API void wkeRepaintIfNeeded(wkeWebView webView);
// ��ȡwebview��DC
DUILIB_API HDC wkeGetViewDC(wkeWebView webView);
// ��ȡwebveiw��Ӧ�Ĵ��ھ����ʵ�ֺ�wkeGetWindowHandle��ȫ��ͬ
DUILIB_API HWND wkeGetHostHWND(wkeWebView webView);

// ҳ���Ƿ���Ժ��ˣ�ҳ�����
DUILIB_API bool wkeCanGoBack(wkeWebView webView);
DUILIB_API bool wkeGoBack(wkeWebView webView);
// ҳ���Ƿ����ǰ����ҳ��ǰ��
DUILIB_API bool wkeCanGoForward(wkeWebView webView);
DUILIB_API bool wkeGoForward(wkeWebView webView);

// ��webview����ȫѡ��ȡ��ѡ������
DUILIB_API void wkeEditorSelectAll(wkeWebView webView);
DUILIB_API void wkeEditorUnSelect(wkeWebView webView);
// ���������С�ճ����ɾ�������������� ҳ���ﱻѡ�е��ַ���
DUILIB_API void wkeEditorCopy(wkeWebView webView);
DUILIB_API void wkeEditorCut(wkeWebView webView);
DUILIB_API void wkeEditorPaste(wkeWebView webView);
DUILIB_API void wkeEditorDelete(wkeWebView webView);
DUILIB_API void wkeEditorUndo(wkeWebView webView);
DUILIB_API void wkeEditorRedo(wkeWebView webView);

// ��ȡҳ���cookie
DUILIB_API CDuiString wkeGetCookie(wkeWebView webView);
// ����ҳ��cookie��
// ע�⣺cookie�������curl��cookieд����һ�������ǣ�PERSONALIZE=123;expires=Monday, 13-Jun-2022 03:04:55 GMT; domain=.fidelity.com; path=/; secure
DUILIB_API void wkeSetCookie(wkeWebView webView, const CDuiString &url, const CDuiString &cookie);

typedef bool(WKE_CALL *wkeCookieVisitor)(
    void *params,
    const utf8 *name,
    const utf8 *value,
    const utf8 *domain,
    const utf8 *path, // If |path| is non-empty only URLs at or below the path will get the cookie value.
    int secure, // If |secure| is true the cookie will only be sent for HTTPS requests.
    int httpOnly, // If |httponly| is true the cookie will only be sent for HTTP requests.
    int *expires // The cookie expiration date is only valid if |has_expires| is true.
);
// ͨ��������visitor��������cookie��
// ������
// visitor����������
DUILIB_API void wkeVisitAllCookie(void *params, wkeCookieVisitor visitor);

enum wkeCookieCommand
{
    wkeCookieCommandClearAllCookies,
    wkeCookieCommandClearSessionCookies,
    wkeCookieCommandFlushCookiesToFile,
    wkeCookieCommandReloadCookiesFromFile,
};
// ͨ������mb���õ�curl������cookie��
// ������
// command:
// wkeCookieCommandClearAllCookies: �ڲ�ʵ��ִ����curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
// wkeCookieCommandClearSessionCookies: curl_easy_setopt(curl, CURLOPT_COOKIELIST, "SESS");
// wkeCookieCommandFlushCookiesToFile: curl_easy_setopt(curl, CURLOPT_COOKIELIST, "FLUSH");
// wkeCookieCommandReloadCookiesFromFile: curl_easy_setopt(curl, CURLOPT_COOKIELIST, "RELOAD");
// ע�⣺����ӿ�ֻ�ǵ���curl�������������ȥ�޸�js�������
DUILIB_API void wkePerformCookieCommand(wkeWebView webView, wkeCookieCommand command);

// ������ر�cookie
// ע�⣺����ӿ�ֻ��Ӱ��blink������������curl�����Ի��ǻ�����curl��cookie�ļ�
DUILIB_API void wkeSetCookieEnabled(wkeWebView webView, bool enable);
DUILIB_API bool wkeIsCookieEnabled(wkeWebView webView);
// ����cookie�ı����ļ�Ŀ¼��Ĭ���ǵ�ǰĿ¼��cookies���ڵ�ǰĿ¼�ġ�cookie.dat����
DUILIB_API void wkeSetCookieJarPath(wkeWebView webView, const CDuiString &path);
// ����cookie��ȫ·��+�ļ������硰c:\mb\cookie.dat��
DUILIB_API void wkeSetCookieJarFullPath(wkeWebView webView, const CDuiString &path);
DUILIB_API void wkeClearCookie(wkeWebView webView);
// ����local storage��ȫ·�����硰c:\mb\LocalStorage\��
// ע�⣺����ӿ�ֻ�ܽ���Ŀ¼��
DUILIB_API void wkeSetLocalStorageFullPath(wkeWebView webView, const CDuiString &path);
DUILIB_API void wkeAddPluginDirectory(wkeWebView webView, const CDuiString &path);

// ���á���ȡ ����
DUILIB_API void wkeSetMediaVolume(wkeWebView webView, float volume);
DUILIB_API float wkeGetMediaVolume(wkeWebView webView);

// ��mb���������Ϣ
// ������
// message����ȡWM_MOUSELEAVE��Windows��������Ϣ
// x��y������
// flags����ȡֵ��WKE_CONTROL��WKE_SHIFT��WKE_LBUTTON��WKE_MBUTTON��WKE_RBUTTON����ͨ�����򡱲���������
DUILIB_API bool wkeFireMouseEvent(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags);
// ��mb���Ͳ˵���Ϣ��δʵ�֣�
DUILIB_API bool wkeFireContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags);
// ��mb���͹�����Ϣ���÷��Ͳ�������wkeFireMouseEvent��
DUILIB_API bool wkeFireMouseWheelEvent(wkeWebView webView, int x, int y, int delta, unsigned int flags);
// ��mb����WM_KEYUP��Ϣ��
// ������
// virtualKeyCode����https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
// flags����ȡֵ��WKE_REPEAT��WKE_EXTENDED����ͨ�����򡱲��������� systemKey����ʱû��
DUILIB_API bool wkeFireKeyUpEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags,
                                  bool systemKey);
DUILIB_API bool wkeFireKeyDownEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags,
                                    bool systemKey);
// ������
// charCode��WM_CHAR��Ϣ��The character code of the key.
// ��https://msdn.microsoft.com/en-us/library/windows/desktop/ms646276(v=vs.85).aspx
DUILIB_API bool wkeFireKeyPressEvent(wkeWebView webView, unsigned int charCode, unsigned int flags,
                                     bool systemKey);
// ��mb��������windows��Ϣ������Ŀǰmb��Ҫ������������ء�mb���޴���ģʽ�£�Ҫ��Ӧ����¼�����Ҫͨ���������ֶ����͹����Ϣ
DUILIB_API bool wkeFireWindowsMessage(wkeWebView webView, HWND hWnd, UINT message, WPARAM wParam,
                                      LPARAM lParam, LRESULT *result);

// ����webview�ǽ���̬�����webveiw�����˴��ڣ�����Ҳ���н���
DUILIB_API void wkeSetFocus(wkeWebView webView);
DUILIB_API void wkeKillFocus(wkeWebView webView);

// ��ȡ�༭����Ǹ��α��λ��
DUILIB_API wkeRect wkeGetCaretRect(wkeWebView webView);

// ����һ��js������js��ֵjsValue��jsValue�Ǹ���װ���ڲ�v8�������͵��࣬�����Ҫ��ȡ��ϸ��Ϣ����jsXXX��ؽӿڿ��Ե��á���������
// ע�⣬�˺����Լ�wkeRunJS��ִ�е�js��Ҳ����script������һ���հ���
DUILIB_API jsValue wkeRunJS(wkeWebView webView, const CDuiString &script);

// ��ȡҳ����frame��jsExecState��jsExecState��ʲô����������
DUILIB_API jsExecState wkeGlobalExec(wkeWebView webView);
DUILIB_API jsExecState wkeGetGlobalExecByFrame(wkeWebView webView, wkeWebFrameHandle frameId);

DUILIB_API void wkeSleep(wkeWebView webView);
DUILIB_API void wkeWake(wkeWebView webView);
DUILIB_API bool wkeIsAwake(wkeWebView webView);

// ����ҳ������ϵ����Ĭ����1
DUILIB_API void wkeSetZoomFactor(wkeWebView webView, float factor);
DUILIB_API float wkeGetZoomFactor(wkeWebView webView);

DUILIB_API void wkeSetEditable(wkeWebView webView, bool editable);

// ��ȡwkeString�ṹ���Ӧ���ַ�����ansi/utf16����
DUILIB_API CDuiString wkeGetString(const wkeString s);
// ����wkeString�ṹ���Ӧ���ַ�����ansi/utf16����
DUILIB_API void wkeSetString(wkeString string, const CDuiString &str);

// ͨ��utf16������ַ���������һ��wkeString
DUILIB_API wkeString wkeCreateString(const CDuiString &str);
DUILIB_API size_t wkeGetStringLen(wkeString str);
// �������wkeString
DUILIB_API void wkeDeleteString(wkeString str);

DUILIB_API wkeWebView wkeGetWebViewForCurrentContext();
// ��webView����һ��key value��ֵ�ԡ��������������û��Լ�������κ�ָ��
DUILIB_API void wkeSetUserKeyValue(wkeWebView webView, const CDuiString &key, void *value);
DUILIB_API void *wkeGetUserKeyValue(wkeWebView webView, const CDuiString &key);


enum WkeCursorInfoType
{
    WkeCursorInfoPointer,
    WkeCursorInfoCross,
    WkeCursorInfoHand,
    WkeCursorInfoIBeam,
    WkeCursorInfoWait,
    WkeCursorInfoHelp,
    WkeCursorInfoEastResize,
    WkeCursorInfoNorthResize,
    WkeCursorInfoNorthEastResize,
    WkeCursorInfoNorthWestResize,
    WkeCursorInfoSouthResize,
    WkeCursorInfoSouthEastResize,
    WkeCursorInfoSouthWestResize,
    WkeCursorInfoWestResize,
    WkeCursorInfoNorthSouthResize,
    WkeCursorInfoEastWestResize,
    WkeCursorInfoNorthEastSouthWestResize,
    WkeCursorInfoNorthWestSouthEastResize,
    WkeCursorInfoColumnResize,
    WkeCursorInfoRowResize,
    WkeCursorInfoMiddlePanning,
    WkeCursorInfoEastPanning,
    WkeCursorInfoNorthPanning,
    WkeCursorInfoNorthEastPanning,
    WkeCursorInfoNorthWestPanning,
    WkeCursorInfoSouthPanning,
    WkeCursorInfoSouthEastPanning,
    WkeCursorInfoSouthWestPanning,
    WkeCursorInfoWestPanning,
    WkeCursorInfoMove,
    WkeCursorInfoVerticalText,
    WkeCursorInfoCell,
    WkeCursorInfoContextMenu,
    WkeCursorInfoAlias,
    WkeCursorInfoProgress,
    WkeCursorInfoNoDrop,
    WkeCursorInfoCopy,
    WkeCursorInfoNone,
    WkeCursorInfoNotAllowed,
    WkeCursorInfoZoomIn,
    WkeCursorInfoZoomOut,
    WkeCursorInfoGrab,
    WkeCursorInfoGrabbing,
    WkeCursorInfoCustom
};
DUILIB_API int wkeGetCursorInfoType(wkeWebView webView);
DUILIB_API void wkeSetCursorInfoType(wkeWebView webView, int type);

DUILIB_API void wkeSetDragFiles(wkeWebView webView, const POINT *clintPos, const POINT *screenPos,
                                wkeString files[], int filesCount);

// ����mbģ���Ӳ���豸��������Ҫ����αװ�ֻ��豸����
// ������
// device���豸���ַ�������ȡֵ�У�
// "navigator.maxTouchPoints"   ��ʱ paramInt ��Ҫ�����ã���ʾ touch �ĵ���
// "navigator.platform"         ��ʱ paramStr ��Ҫ�����ã���ʾjs���ȡ�� navigator.platform�ַ���
// "navigator.hardwareConcurrency"  ��ʱ paramInt ��Ҫ�����ã���ʾjs���ȡ�� navigator.hardwareConcurrency ����ֵ
// "screen.width"       ��ʱ paramInt ��Ҫ�����ã���ʾjs���ȡ�� screen.width ����ֵ
// "screen.height"      ��ʱ paramInt ��Ҫ�����ã���ʾjs���ȡ�� screen.height ����ֵ
// "screen.availWidth"  ��ʱ paramInt ��Ҫ�����ã���ʾjs���ȡ�� screen.availWidth ����ֵ
// "screen.availHeight" ��ʱ paramInt ��Ҫ�����ã���ʾjs���ȡ�� screen.availHeight ����ֵ
// "screen.pixelDepth"  ��ʱ paramInt ��Ҫ�����ã���ʾjs���ȡ�� screen.pixelDepth ����ֵ
// "screen.pixelDepth"  Ŀǰ�ȼ���"screen.pixelDepth"
// "window.devicePixelRatio"    ͬ��
// "navigator.connection.type"
DUILIB_API void wkeSetDeviceParameter(wkeWebView webView, const CDuiString &device,
                                      const CDuiString &paramStr, int paramInt, float paramFloat);

DUILIB_API wkeTempCallbackInfo *wkeGetTempCallbackInfo(wkeWebView webView);


//wke callback-----------------------------------------------------------------------------------
typedef void(WKE_CALL *wkeTitleChangedCallback)(wkeWebView webView, void *param, const wkeString title);
// ���ñ���仯��֪ͨ�ص�
// ������typedef void(*wkeTitleChangedCallback)(wkeWebView webView, void* param, const wkeString title);
// title��������ַ�����װ��wkeString��ô�ã�����ؽӿڡ� param��ͨ��wkeOnTitleChanged��callbackParam����
DUILIB_API void wkeOnTitleChanged(wkeWebView webView, wkeTitleChangedCallback callback, void *callbackParam);
// ��껮����Ԫ�أ�����ǣ�����ô˻ص���������a��ǩ��url
DUILIB_API void wkeOnMouseOverUrlChanged(wkeWebView webView, wkeTitleChangedCallback callback,
                                         void *callbackParam);

typedef void(WKE_CALL *wkeURLChangedCallback)(wkeWebView webView, void *param, const wkeString url);
// url�ı�ص�
DUILIB_API void wkeOnURLChanged(wkeWebView webView, wkeURLChangedCallback callback, void *callbackParam);
typedef void(WKE_CALL *wkeURLChangedCallback2)(wkeWebView webView, void *param,
                                               wkeWebFrameHandle frameId, const wkeString url);
// ���ϸ��ӿڲ�ͬ���ǣ��ص����˸�����
// ������typedef void(*wkeURLChangedCallback2)(wkeWebView webView, void* param, wkeWebFrameHandle frameId, const wkeString url)
// frameId����ʾframe��id������ؽӿڿ����ж����frameId�Ƿ�����frame
DUILIB_API void wkeOnURLChanged2(wkeWebView webView, wkeURLChangedCallback2 callback, void *callbackParam);

typedef void(WKE_CALL *wkePaintUpdatedCallback)(wkeWebView webView, void *param, const HDC hdc,
                                                int x, int y, int cx, int cy);
// ҳ�����κ���Ҫˢ�µĵط��������ô˻ص�
// ������typedef void(*wkePaintUpdatedCallback)(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy)
// x��y��cx��cy��ʾˢ�µ��������
DUILIB_API void wkeOnPaintUpdated(wkeWebView webView, wkePaintUpdatedCallback callback, void *callbackParam);

typedef void(WKE_CALL *wkePaintBitUpdatedCallback)(wkeWebView webView, void *param, const void *buffer,
                                                   const wkeRect *r, int width, int height);
// ͬ�ϡ���ͬ���ǻص����������������ص�buffer��������DC������Ƕ�뵽��Ϸ����������Ⱦ
DUILIB_API void wkeOnPaintBitUpdated(wkeWebView webView, wkePaintBitUpdatedCallback callback,
                                     void *callbackParam);

// ��ҳ����alert���ߵ�����ӿ�����Ļص�
typedef void(WKE_CALL *wkeAlertBoxCallback)(wkeWebView webView, void *param, const wkeString msg);
DUILIB_API void wkeOnAlertBox(wkeWebView webView, wkeAlertBoxCallback callback, void *callbackParam);

typedef bool(WKE_CALL *wkeConfirmBoxCallback)(wkeWebView webView, void *param, const wkeString msg);
DUILIB_API void wkeOnConfirmBox(wkeWebView webView, wkeConfirmBoxCallback callback, void *callbackParam);

typedef bool(WKE_CALL *wkePromptBoxCallback)(wkeWebView webView, void *param, const wkeString msg,
                                             const wkeString defaultResult, wkeString result);
DUILIB_API void wkeOnPromptBox(wkeWebView webView, wkePromptBoxCallback callback, void *callbackParam);

typedef bool(WKE_CALL *wkeNavigationCallback)(wkeWebView webView, void *param,
                                              wkeNavigationType navigationType, wkeString url);

// ��ҳ��ʼ����������ص�
// ������typedef bool(*wkeNavigationCallback)(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url);
// wkeNavigationType: ��ʾ���������ԭ�򡣿���ȡ��ֵ�У�
// WKE_NAVIGATION_TYPE_LINKCLICK�����a��ǩ����
// WKE_NAVIGATION_TYPE_FORMSUBMITTE�����form����
// WKE_NAVIGATION_TYPE_BACKFORWARD��ǰ�����˴���
// WKE_NAVIGATION_TYPE_RELOAD�����¼��ش���
// ע�⣺wkeNavigationCallback�ص��ķ���ֵ�������true����ʾ���Լ������������false��ʾ��ֹ���������
DUILIB_API void wkeOnNavigation(wkeWebView webView, wkeNavigationCallback callback, void *param);

struct wkeWindowFeatures
{
    int x;
    int y;
    int width;
    int height;

    bool menuBarVisible;
    bool statusBarVisible;
    bool toolBarVisible;
    bool locationBarVisible;
    bool scrollbarsVisible;
    bool resizable;
    bool fullscreen;
};

typedef wkeWebView(WKE_CALL *wkeCreateViewCallback)(wkeWebView webView, void *param,
                                                    wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures *windowFeatures);
// ��ҳ���a��ǩ�����´���ʱ�������ص�
// ������typedef wkeWebView(*wkeCreateViewCallback)(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures);
DUILIB_API void wkeOnCreateView(wkeWebView webView, wkeCreateViewCallback callback, void *param);

typedef void(WKE_CALL *wkeDocumentReadyCallback)(wkeWebView webView, void *param);
// ��Ӧjs���body onload�¼�
DUILIB_API void wkeOnDocumentReady(wkeWebView webView, wkeDocumentReadyCallback callback, void *param);

typedef void(WKE_CALL *wkeDocumentReady2Callback)(wkeWebView webView, void *param, wkeWebFrameHandle frameId);
// ͬ�ϡ�������wkeDocumentReady2Callback����wkeWebFrameHandle frameId�����������ж��Ƿ�����frame
DUILIB_API void wkeOnDocumentReady2(wkeWebView webView, wkeDocumentReady2Callback callback, void *param);

enum wkeLoadingResult
{
    WKE_LOADING_SUCCEEDED,
    WKE_LOADING_FAILED,
    WKE_LOADING_CANCELED
};

typedef void(WKE_CALL *wkeLoadingFinishCallback)(wkeWebView webView, void *param, const wkeString url,
                                                 wkeLoadingResult result, const wkeString failedReason);
DUILIB_API void wkeOnLoadingFinish(wkeWebView webView, wkeLoadingFinishCallback callback, void *param);

typedef bool(WKE_CALL *wkeDownloadCallback)(wkeWebView webView, void *param, const char *url);
// ҳ�������¼��ص������ĳЩ���ӣ��������ػ����
DUILIB_API void wkeOnDownload(wkeWebView webView, wkeDownloadCallback callback, void *param);

enum wkeDownloadOpt
{
    kWkeDownloadOptCancel,
    kWkeDownloadOptCacheData,
};

typedef void(WKE_CALL *wkeNetJobDataRecvCallback)(void *ptr, wkeNetJob job, const char *data, int length);
typedef void(WKE_CALL *wkeNetJobDataFinishCallback)(void *ptr, wkeNetJob job, wkeLoadingResult result);

struct wkeNetJobDataBind
{
    void *ptr;
    wkeNetJobDataRecvCallback recvCallback;
    wkeNetJobDataFinishCallback finishCallback;
};

typedef wkeDownloadOpt(WKE_CALL *wkeDownload2Callback)(
    wkeWebView webView,
    void *param,
    size_t expectedContentLength,
    const char *url,
    const char *mime,
    const char *disposition,
    wkeNetJob job,
    wkeNetJobDataBind *dataBind);
DUILIB_API void wkeOnDownload2(wkeWebView webView, wkeDownload2Callback callback, void *param);

enum wkeConsoleLevel
{
    wkeLevelDebug        = 4,
    wkeLevelLog          = 1,
    wkeLevelInfo         = 5,
    wkeLevelWarning      = 2,
    wkeLevelError        = 3,
    wkeLevelRevokedError = 6,
    wkeLevelLast         = wkeLevelInfo
};

typedef void(WKE_CALL *wkeConsoleCallback)(wkeWebView webView, void *param, wkeConsoleLevel level,
                                           const wkeString message, const wkeString sourceName, unsigned sourceLine, const wkeString stackTrace);
// ��ҳ����console����
DUILIB_API void wkeOnConsole(wkeWebView webView, wkeConsoleCallback callback, void *param);

typedef void(WKE_CALL *wkeOnCallUiThread)(wkeWebView webView, void *paramOnInThread);
typedef void(WKE_CALL *wkeCallUiThread)(wkeWebView webView, wkeOnCallUiThread func, void *param);
DUILIB_API void wkeSetUIThreadCallback(wkeWebView webView, wkeCallUiThread callback, void *param);

//wkeNet--------------------------------------------------------------------------------------
typedef bool(WKE_CALL *wkeLoadUrlBeginCallback)(wkeWebView webView, void *param, const utf8 *url,
                                                wkeNetJob job);
// �κ�����������ǰ�ᴥ���˻ص�
// ������typedef bool(*wkeLoadUrlBeginCallback)(wkeWebView webView, void* param, const utf8 *url, void *job)
// ע�⣺
// 1���˻ص�����ǿ���ڻص�������job������wkeNetHookRequest�����ʾmb�Ỻ���ȡ�����������ݣ���������������� ���������wkeOnLoadUrlEnd���õĻص���ͬʱ���ݻ�������ݡ��ڴ��ڼ䣬mb���ᴦ���������ݡ�
// 2�������wkeLoadUrlBeginCallback��û����wkeNetHookRequest���򲻻ᴥ��wkeOnLoadUrlEnd�ص���
// 3�����wkeLoadUrlBeginCallback�ص��ﷵ��true����ʾmb��������������󣨼Ȳ��ᷢ���������󣩡�����false����ʾmb��Ȼ�ᷢ����������
// �÷�������
// ������Ҫhook�ٶ�ĳ��url����httdiv://baidu.com/a.js��,�滻Ϊ����c:\b.js�����������ʵ�֣�
// void readJsFile(const wchar_t* divath, std::vector* buffer) {
//      HANDLE hFile = CreateFileW(divath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//      if (INVALID_HANDLE_VALUE == hFile) {
//          DebugBreak();
//          return;
//      }
//
//      DWORD fileSizeHigh;
//      const DWORD bufferSize = ::GetFileSize(hFile, &amdiv;fileSizeHigh);
//
//      DWORD numberOfBytesRead = 0;
//      buffer->resize(bufferSize);
//      BOOL b = ::ReadFile(hFile, &amdiv;buffer->at(0), bufferSize, &amdiv;numberOfBytesRead, nulldivtr);
//      ::CloseHandle(hFile);
//
//  }
//
//  static bool HookUrl(void* job, const utf8* url, const utf8* hookUrl, const wchar_t* localFile, const utf8* mime) {
//      if (0 != strstr(url, hookUrl)) {
//          wkeNetSetMIMETydive(job, (utf8*)mime); // "text/html" "text/javascridivt"
//          std::vector buffer;
//              ReadJsFile(localFile, &amdiv;buffer);
//              wkeNetSetData(job, &amdiv;buffer[0], buffer.size());
//              return true;
//      }
//
//      return false;
//  }
//
//  bool handleLoadUrlBegin(wkeWebView webView, void* divaram, const utf8* url, void* job) {
//      if (HookUrl(job, url, "httdiv://baidu.com/a.js", L"c:\\b.js", "text/javascridivt")) {
//          return true;
//      }
//      return false;
//  }
//
// �����Ҫ�õ�httdiv://baidu.com/a.js����ʵ�����������޸ģ�����ԣ�
// bool handleLoadUrlBegin(wkeWebView webView, void* divaram, const utf8* url, void* job) {
//     if (0 != strstr(url, "httdiv://baidu.com/a.js")) {
//         wkeNetHookRequest(job);
//         return false;
//     }
//     return false;
// }
//
// void handleLoadUrlEnd(wkeWebView webView, void* divaram, const utf8* url, void* job, void* buf, int len) {
//     utf8 code[] = "console.log('test')";
//     wkeNetSetData(job, code, sizeof(code));
// }
DUILIB_API void wkeOnLoadUrlBegin(wkeWebView webView, wkeLoadUrlBeginCallback callback, void *callbackParam);

typedef void(WKE_CALL *wkeLoadUrlEndCallback)(wkeWebView webView, void *param, const char *url,
                                              wkeNetJob job, void *buf, int len);
// ��wkeOnLoadUrlBegin������
DUILIB_API void wkeOnLoadUrlEnd(wkeWebView webView, wkeLoadUrlEndCallback callback, void *callbackParam);

typedef void(WKE_CALL *wkeLoadUrlFailCallback)(wkeWebView webView, void *param, const utf8 *url,
                                               wkeNetJob job);
DUILIB_API void wkeOnLoadUrlFail(wkeWebView webView, wkeLoadUrlFailCallback callback, void *callbackParam);

typedef void(WKE_CALL *wkeDidCreateScriptContextCallback)(wkeWebView webView, void *param,
                                                          wkeWebFrameHandle frameId, void *context, int extensionGroup, int worldId);
// javascript��v8ִ�л���������ʱ�����˻ص�
// ��������
// ע�⣺ÿ��frame����ʱ���ᴥ���˻ص�
DUILIB_API void wkeOnDidCreateScriptContext(wkeWebView webView, wkeDidCreateScriptContextCallback callback,
                                            void *callbackParam);

typedef void(WKE_CALL *wkeWillReleaseScriptContextCallback)(wkeWebView webView, void *param,
                                                            wkeWebFrameHandle frameId, void *context, int worldId);
// ÿ��frame��javascript��v8ִ�л������ر�ʱ�����˻ص�
DUILIB_API void wkeOnWillReleaseScriptContext(wkeWebView webView,
                                              wkeWillReleaseScriptContextCallback callback, void *param);

typedef bool(WKE_CALL *wkeWindowClosingCallback)(wkeWebView webView, void *param);
// wkeWebView������洰��ģʽ�������յ�WM_CLODE��Ϣʱ�����˻ص�������ͨ���ڻص��з���false�ܾ��رմ���
DUILIB_API void wkeOnWindowClosing(wkeWebView webView, wkeWindowClosingCallback callback, void *param);

typedef void(WKE_CALL *wkeWindowDestroyCallback)(wkeWebView webView, void *param);
// ���ڼ���������ʱ�����ص�������wkeOnWindowClosing����������޷�ȡ��
DUILIB_API void wkeOnWindowDestroy(wkeWebView webView, wkeWindowDestroyCallback callback, void *param);


struct wkeDraggableRegion
{
    RECT bounds;
    bool draggable;
};
typedef void(WKE_CALL *wkeDraggableRegionsChangedCallback)(wkeWebView webView, void *param,
                                                           const wkeDraggableRegion *rects, int rectCount);
DUILIB_API void wkeOnDraggableRegionsChanged(wkeWebView webView, wkeDraggableRegionsChangedCallback callback,
                                             void *param);

struct wkeMediaLoadInfo
{
    int size;
    int width;
    int height;
    double duration;
};
typedef void(WKE_CALL *wkeWillMediaLoadCallback)(wkeWebView webView, void *param, const char *url,
                                                 wkeMediaLoadInfo *info);
// video�ȶ�ý���ǩ����ʱ�����˻ص�
DUILIB_API void wkeOnWillMediaLoad(wkeWebView webView, wkeWillMediaLoadCallback callback,
                                   void *callbackParam);

typedef wkeString(WKE_CALL*wkeImageBufferToDataURL)(wkeWebView webView, void* param, const char* data, size_t size);

typedef void(WKE_CALL *wkeStartDraggingCallback)(
    wkeWebView webView,
    void *param,
    wkeWebFrameHandle frame,
    const wkeWebDragData *data,
    wkeWebDragOperationsMask mask,
    const void *image,
    const wkePoint *dragImageOffset
);
DUILIB_API void wkeOnStartDragging(wkeWebView webView, wkeStartDraggingCallback callback, void *param);

typedef void(WKE_CALL *wkeOnPrintCallback)(wkeWebView webView, void *param, wkeWebFrameHandle frameId,
                                           void *printParams);
DUILIB_API void wkeOnPrint(wkeWebView webView, wkeOnPrintCallback callback, void *param);

typedef void(WKE_CALL *wkeOnScreenshot)(wkeWebView webView, void *param, const char *data, size_t size);
DUILIB_API void wkeScreenshot(wkeWebView webView,
                              const wkeScreenshotSettings *settings,
                              wkeOnScreenshot callback, void *param);

enum wkeOtherLoadType
{
    WKE_DID_START_LOADING,
    WKE_DID_STOP_LOADING,
    WKE_DID_NAVIGATE,
    WKE_DID_NAVIGATE_IN_PAGE,
    WKE_DID_GET_RESPONSE_DETAILS,
    WKE_DID_GET_REDIRECT_REQUEST,
    WKE_DID_POST_REQUEST,
};
typedef void(WKE_CALL *wkeOnOtherLoadCallback)(wkeWebView webView, void *param, wkeOtherLoadType type,
                                               wkeTempCallbackInfo *info);
DUILIB_API void wkeOnOtherLoad(wkeWebView webView, wkeOnOtherLoadCallback callback, void *param);

DUILIB_API bool wkeIsProcessingUserGesture(wkeWebView webView);

// ��wkeOnLoadUrlBegin�ص�����ã���ʾ����http�����MIME type
DUILIB_API void wkeNetSetMIMEType(wkeNetJob jobPtr, const CDuiString &type);
// ��������2���������Դ�nullptr
DUILIB_API CDuiString wkeNetGetMIMEType(wkeNetJob jobPtr, wkeString mime);
// ��wkeOnLoadUrlBegin�ص�����ã���ʾ����http���󣨻���file:///Э�飩�� http header field��responseһֱҪ�����ó�false
DUILIB_API void wkeNetSetHTTPHeaderField(wkeNetJob jobPtr, const CDuiString &key, const CDuiString &value,
                                         bool response);

DUILIB_API CDuiString wkeNetGetHTTPHeaderField(wkeNetJob jobPtr, const CDuiString &key);
DUILIB_API CDuiString wkeNetGetHTTPHeaderFieldFromResponse(wkeNetJob jobPtr, const CDuiString &key);

// ��wkeOnLoadUrlEnd�ﱻ���ã���ʾ����hook�󻺴������
// ���ô˺�����,������յ����ݻ�洢��һbuf��,����������ɺ���ӦOnLoadUrlEnd�¼�.#�˵�������Ӱ������,����
// �˺�����wkeNetHookRequest�������ǣ�wkeNetHookRequest���ڽ��ܵ������������ݺ��ٵ��ûص���������ص��޸��������ݡ�"\
// ��wkeNetSetData�����������ݻ�û���͵�ʱ���޸�
DUILIB_API void wkeNetSetData(wkeNetJob jobPtr, void *buf, int len);
DUILIB_API void wkeNetHookRequest(wkeNetJob jobPtr);

typedef bool(WKE_CALL *wkeNetResponseCallback)(wkeWebView webView, void *param, const utf8 *url,
                                               wkeNetJob job);
// һ�����������ͺ��յ�������response�����ص�
DUILIB_API void wkeNetOnResponse(wkeWebView webView, wkeNetResponseCallback callback, void *param);

enum wkeRequestType
{
    kWkeRequestTypeInvalidation,
    kWkeRequestTypeGet,
    kWkeRequestTypePost,
    kWkeRequestTypePut,
};

// ��ȡ�������method����post����get
DUILIB_API wkeRequestType wkeNetGetRequestMethod(wkeNetJob jobPtr);

typedef void(WKE_CALL *wkeOnNetGetFaviconCallback)(wkeWebView webView, void *param, const utf8 *url,
                                                   wkeMemBuf *buf);
// ��ȡfavicon��
// ��������
// ע�⣺�˽ӿڱ�����wkeOnLoadingFinish�ص�����á����������淽ʽ���ж��Ƿ�����frame��LoadingFinish:
//      wkeTempCallbackInfo* temInfo = wkeGetTempCallbackInfo(webView);
//      if (::wkeIsMainFrame(webView, temInfo->frame)) {
//          ::wkeNetGetFavicon(webView, HandleFaviconReceived, divaram);
//      }
DUILIB_API int wkeNetGetFavicon(wkeWebView webView, wkeOnNetGetFaviconCallback callback, void *param);

DUILIB_API void wkeNetContinueJob(wkeNetJob jobPtr);
DUILIB_API CDuiString wkeNetGetUrlByJob(wkeNetJob jobPtr);
DUILIB_API const wkeSlist *wkeNetGetRawHttpHead(wkeNetJob jobPtr);
DUILIB_API const wkeSlist *wkeNetGetRawResponseHead(wkeNetJob jobPtr);
// ��wkeOnLoadUrlBegin�ص�����ã����ú󣬴����󽫱�ȡ����
DUILIB_API void wkeNetCancelRequest(wkeNetJob jobPtr);

// �߼��÷�����wkeOnLoadUrlBegin�ص�����á� ��ʱ��wkeOnLoadUrlBegin�����ص�һ������󣬲��������жϳ��������ʱ���Ե��ñ��ӿڣ�Ȼ���� �첽��ĳ��ʱ�̣�����wkeNetContinueJob���ô������������
// ��������
// ����ֵ��TRUE����ɹ���FALSE�������ʧ�ܣ������ٵ���wkeNetContinueJob��
DUILIB_API BOOL wkeNetHoldJobToAsynCommit(wkeNetJob jobPtr);

DUILIB_API void wkeNetChangeRequestUrl(wkeNetJob jobPtr, const CDuiString &url);

typedef struct wkeWebUrlRequest *wkeWebUrlRequestPtr;
typedef struct wkeWebUrlResponse *wkeWebUrlResponsePtr;

DUILIB_API wkeWebUrlRequestPtr wkeNetCreateWebUrlRequest(const CDuiString &url, const CDuiString &method,
                                                         const CDuiString &mime);
DUILIB_API wkeWebUrlRequestPtr wkeNetCreateWebUrlRequest2(const blinkWebURLRequestPtr request);
DUILIB_API blinkWebURLRequestPtr wkeNetCopyWebUrlRequest(wkeNetJob jobPtr, bool needExtraData);
DUILIB_API void wkeNetDeleteBlinkWebURLRequestPtr(blinkWebURLRequestPtr request);
DUILIB_API void wkeNetAddHTTPHeaderFieldToUrlRequest(wkeWebUrlRequestPtr request, const CDuiString &name,
                                                     const CDuiString &value);

typedef void(WKE_CALL *wkeOnUrlRequestWillRedirectCallback)(wkeWebView webView, void *param,
                                                            wkeWebUrlRequestPtr oldRequest, wkeWebUrlRequestPtr request,
                                                            wkeWebUrlResponsePtr redirectResponse);
typedef void(WKE_CALL *wkeOnUrlRequestDidReceiveResponseCallback)(wkeWebView webView, void *param,
        wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr response);
typedef void(WKE_CALL *wkeOnUrlRequestDidReceiveDataCallback)(wkeWebView webView, void *param,
        wkeWebUrlRequestPtr request, const char *data, int dataLength);
typedef void(WKE_CALL *wkeOnUrlRequestDidFailCallback)(wkeWebView webView, void *param,
                                                       wkeWebUrlRequestPtr request, const utf8 *error);
typedef void(WKE_CALL *wkeOnUrlRequestDidFinishLoadingCallback)(wkeWebView webView, void *param,
        wkeWebUrlRequestPtr request, double finishTime);

struct wkeUrlRequestCallbacks
{
    wkeOnUrlRequestWillRedirectCallback willRedirectCallback;
    wkeOnUrlRequestDidReceiveResponseCallback didReceiveResponseCallback;
    wkeOnUrlRequestDidReceiveDataCallback didReceiveDataCallback;
    wkeOnUrlRequestDidFailCallback didFailCallback;
    wkeOnUrlRequestDidFinishLoadingCallback didFinishLoadingCallback;
};
DUILIB_API int wkeNetStartUrlRequest(wkeWebView webView, wkeWebUrlRequestPtr request, void *param,
                                     const wkeUrlRequestCallbacks *callbacks);
DUILIB_API int wkeNetGetHttpStatusCode(wkeWebUrlResponsePtr response);

DUILIB_API __int64 wkeNetGetExpectedContentLength(wkeWebUrlResponsePtr response);
DUILIB_API CDuiString wkeNetGetResponseUrl(wkeWebUrlResponsePtr response);
DUILIB_API void wkeNetCancelWebUrlRequest(int requestId);

// ��ȡ�������е�post���ݡ�ֻ�е�������postʱ����Ч��
DUILIB_API wkePostBodyElements *wkeNetGetPostBody(wkeNetJob jobPtr);

// ���ĸ��ӿ�Ҫ�������ʹ�á�
// ��wkeOnLoadUrlBegin���ж���postʱ������ͨ��wkeNetCreatePostBodyElements������һ���µ�post���ݰ���
// Ȼ��wkeNetFreePostBodyElements���ͷ�ԭpost���ݡ�
DUILIB_API wkePostBodyElements *wkeNetCreatePostBodyElements(wkeWebView webView, size_t length);
DUILIB_API void wkeNetFreePostBodyElements(wkePostBodyElements *elements);

DUILIB_API wkePostBodyElement *wkeNetCreatePostBodyElement(wkeWebView webView);
DUILIB_API void wkeNetFreePostBodyElement(wkePostBodyElement *element);

// �ж�frameId�Ƿ�����frame
DUILIB_API bool wkeIsMainFrame(wkeWebView webView, wkeWebFrameHandle frameId);
DUILIB_API bool wkeIsWebRemoteFrame(wkeWebView webView, wkeWebFrameHandle frameId);
// ��ȡ��frame�ľ��
DUILIB_API wkeWebFrameHandle wkeWebFrameGetMainFrame(wkeWebView webView);

// ����js��ָ����frame�ϣ�ͨ��frameId
// ������isInClosure��ʾ�Ƿ���������function() {}��ʽ�ıհ�
// ע�⣺�����Ҫ����ֵ����isInClosureΪtrueʱ����Ҫдreturn��Ϊfalse����
DUILIB_API jsValue wkeRunJsByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const CDuiString &script,
                                   bool isInClosure);
DUILIB_API void wkeInsertCSSByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const CDuiString &cssText);

typedef void *v8ContextPtr;
DUILIB_API void wkeWebFrameGetMainWorldScriptContext(wkeWebView webView, wkeWebFrameHandle webFrameId,
                                                     v8ContextPtr contextOut);

typedef void *v8Isolate;
DUILIB_API v8Isolate wkeGetBlinkMainThreadIsolate();


//wkewindow-----------------------------------------------------------------------------------
enum wkeWindowType
{
    WKE_WINDOW_TYPE_POPUP,
    WKE_WINDOW_TYPE_TRANSPARENT,
    WKE_WINDOW_TYPE_CONTROL
};
// ����һ������ʵ���ڵ�wkeWebView
// ������wkeWindowType
// WKE_WINDOW_TYPE_POPUP��      ��ͨ����
// WKE_WINDOW_TYPE_TRANSPARENT��͸�����ڡ�mb�ڲ�ͨ��layer windowʵ��
// WKE_WINDOW_TYPE_CONTROL��    Ƕ���ڸ���������Ӵ��ڡ���ʱparent��Ҫ������
DUILIB_API wkeWebView wkeCreateWebWindow(wkeWindowType type, HWND parent, int x, int y, int width,
                                         int height);

struct wkeWindowCreateInfo
{
    int size;
    HWND parent;
    DWORD style;
    DWORD styleEx;
    int x;
    int y;
    int width;
    int height;
    COLORREF color;
};
DUILIB_API wkeWebView wkeCreateWebCustomWindow(const wkeWindowCreateInfo *info);

// ����wkeWebView��Ӧ���������ݽṹ��������ʵ���ڵ�
DUILIB_API void wkeDestroyWebWindow(wkeWebView webView);
// ��ȡ���ڶ�Ӧ����ʵ�������wkeGetHostHWND��ʵ����ȫ��ͬ
DUILIB_API HWND wkeGetWindowHandle(wkeWebView webView);

DUILIB_API void wkeShowWindow(wkeWebView webView, bool showFlag);
DUILIB_API void wkeEnableWindow(wkeWebView webView, bool enableFlag);

DUILIB_API void wkeMoveWindow(wkeWebView webView, int x, int y, int width, int height);
// �����ڸ����ڻ���Ļ�����
DUILIB_API void wkeMoveToCenter(wkeWebView webView);
// resize���ڣ���wkeResizeЧ��һ��
DUILIB_API void wkeResizeWindow(wkeWebView webView, int width, int height);

DUILIB_API wkeWebDragOperation wkeDragTargetDragEnter(wkeWebView webView, const wkeWebDragData *webDragData,
                                                      const POINT *clientPoint, const POINT *screenPoint, wkeWebDragOperationsMask operationsAllowed,
                                                      int modifiers);
DUILIB_API wkeWebDragOperation wkeDragTargetDragOver(wkeWebView webView, const POINT *clientPoint,
                                                     const POINT *screenPoint, wkeWebDragOperationsMask operationsAllowed, int modifiers);
DUILIB_API void wkeDragTargetDragLeave(wkeWebView webView);
DUILIB_API void wkeDragTargetDrop(wkeWebView webView, const POINT *clientPoint, const POINT *screenPoint,
                                  int modifiers);
DUILIB_API void wkeDragTargetEnd(wkeWebView webView, const POINT *clientPoint, const POINT *screenPoint,
                                 wkeWebDragOperation operation);

typedef void(WKE_CALL *wkeUiThreadRunCallback)(HWND hWnd, void *param);
typedef int(WKE_CALL *wkeUiThreadPostTaskCallback)(HWND hWnd, wkeUiThreadRunCallback callback,
                                                   void *param);

DUILIB_API void wkeUtilSetUiCallback(wkeUiThreadPostTaskCallback callback);
DUILIB_API CDuiString wkeUtilSerializeToMHTML(wkeWebView webView);

struct wkePdfDatas
{
    int count;
    size_t *sizes;
    const void **datas;
};

struct wkePrintSettings
{
    int  structSize;
    int  dpi;
    int  width; // in px
    int  height;
    int  marginTop;
    int  marginBottom;
    int  marginLeft;
    int  marginRight;
    BOOL isPrintPageHeadAndFooter;
    BOOL isPrintBackgroud;
    BOOL isLandscape;
};

DUILIB_API const wkePdfDatas *wkeUtilPrintToPdf(wkeWebView webView, wkeWebFrameHandle frameId,
                                                const wkePrintSettings *settings);
DUILIB_API const wkeMemBuf *wkePrintToBitmap(wkeWebView webView, wkeWebFrameHandle frameId,
                                             const wkeScreenshotSettings *settings);
DUILIB_API void wkeUtilRelasePrintPdfDatas(const wkePdfDatas *datas);

DUILIB_API void wkeSetWindowTitle(wkeWebView webView, const CDuiString &title);

typedef void(WKE_CALL *wkeNodeOnCreateProcessCallback)(wkeWebView webView, void *param,
                                                       const WCHAR *applicationPath, const WCHAR *arguments, STARTUPINFOW *startup);
DUILIB_API void wkeNodeOnCreateProcess(wkeWebView webView, wkeNodeOnCreateProcessCallback callback,
                                       void *param);

typedef void(WKE_CALL *wkeOnPluginFindCallback)(wkeWebView webView, void *param, const utf8 *mime,
                                                void *initializeFunc, void *getEntryPointsFunc, void *shutdownFunc);
DUILIB_API void wkeOnPluginFind(wkeWebView webView, const CDuiString &mime, wkeOnPluginFindCallback callback,
                                void *param);
DUILIB_API void wkeAddNpapiPlugin(wkeWebView webView, void *initializeFunc, void *getEntryPointsFunc,
                                  void *shutdownFunc);

DUILIB_API void wkePluginListBuilderAddPlugin(void *builder, const CDuiString &name, const CDuiString &desc,
                                              const CDuiString &fileName);
DUILIB_API void wkePluginListBuilderAddMediaTypeToLastPlugin(void *builder, const CDuiString &name,
        const CDuiString &desc);
DUILIB_API void wkePluginListBuilderAddFileExtensionToLastMediaType(void *builder,
        const CDuiString &fileExtension);

DUILIB_API wkeWebView wkeGetWebViewByNData(void *ndata);

DUILIB_API bool wkeRegisterEmbedderCustomElement(wkeWebView webView, wkeWebFrameHandle frameId,
                                                 const CDuiString &name, void *options, void *outResult);

typedef wkeMediaPlayer(WKE_CALL *wkeMediaPlayerFactory)(wkeWebView webView, wkeMediaPlayerClient client,
                                                        void *npBrowserFuncs, void *npPluginFuncs);
typedef bool(WKE_CALL *wkeOnIsMediaPlayerSupportsMIMEType)(const utf8 *mime);
DUILIB_API void wkeSetMediaPlayerFactory(wkeWebView webView, wkeMediaPlayerFactory factory,
                                         wkeOnIsMediaPlayerSupportsMIMEType callback);

DUILIB_API CDuiString wkeGetContentAsMarkup(wkeWebView webView, wkeWebFrameHandle frame, size_t *size);

DUILIB_API CDuiString wkeUtilDecodeURLEscape(const CDuiString &url);
DUILIB_API CDuiString wkeUtilEncodeURLEscape(const CDuiString &url);
DUILIB_API CDuiString wkeUtilBase64Encode(const CDuiString &str);
DUILIB_API CDuiString wkeUtilBase64Decode(const CDuiString &str);
DUILIB_API const wkeMemBuf *wkeUtilCreateV8Snapshot(const CDuiString &str);

DUILIB_API void wkeRunMessageLoop();

//JavaScript Bind-----------------------------------------------------------------------------------
#define JS_CALL __fastcall

typedef jsValue(JS_CALL *jsNativeFunction)(jsExecState es);

// ��һ��ȫ�ֺ�������frame��window�ϡ�
// ��������
// ע�⣺�˽ӿ�ֻ�ܰ���frame�������ر���Ҫע����ǣ���Ϊ��ʷԭ�򣬴˽ӿ���fastcall����Լ��������wkeJsBindFunction���ǣ�
// ����˽ӿں�wkeJsBindFunction������webview����ǰ����
// ʹ��ʾ����
// c++�
//  --------
//  jsValue JS_CALL onNativeFunction(jsExecState es) {
//      jsValue v = jsArg(es, 0);
//      const wchar_t* str = jsToTemdivStringW(es, v);
//      OutdivutdebugStringW(str);
//  }
//  jsBindFunction("testCall", onNativeFunction�� 1);
//
// js�
//  --------
//  window.testCall('testStrt');
DUILIB_API void __fastcall jsBindFunction(const CDuiString &name, jsNativeFunction fn, unsigned int argCount);
// ��js winows��һ�����Է���������js��windows.XXX������ʽ����ʱ��fn�ᱻ����
// ʾ����jsBindGetter("XXX")
DUILIB_API void jsBindGetter(const CDuiString &name, jsNativeFunction fn);
// ��js winows��һ��������������
DUILIB_API void jsBindSetter(const CDuiString &name, jsNativeFunction fn);

typedef jsValue(WKE_CALL *jsNativeFunctionEx)(jsExecState es, void *param);

// ��jsBindFunction�������ƣ���������һ�㣬���Դ�һ��param���Զ������ݡ�
// �˽ӿں�wkeJsBindFunction������webview����ǰ����
DUILIB_API void jsBindFunctionEx(const CDuiString &name, jsNativeFunctionEx fn, void *param,
                                 unsigned int argCount);
// ��js winows��һ�����Է���������js��windows.XXX������ʽ����ʱ��fn�ᱻ����
// ʾ����jsBindGetterEx("XXX")
DUILIB_API void jsBindGetterEx(const CDuiString &name, jsNativeFunctionEx fn, void *param);
// ��js winows��һ��������������
DUILIB_API void jsBindSetterEx(const CDuiString &name, jsNativeFunctionEx fn, void *param);

// ��ȡes���Ĳ���������һ�����ڰ󶨵�js����c++�ص���ʹ�ã��ж�js�����˶��ٲ�����c++
DUILIB_API int jsArgCount(jsExecState es);

enum jsType
{
    JSTYPE_NUMBER,
    JSTYPE_STRING,
    JSTYPE_BOOLEAN,
    JSTYPE_OBJECT,
    JSTYPE_FUNCTION,
    JSTYPE_UNDEFINED,
    JSTYPE_ARRAY,
    JSTYPE_NULL,
};

// �жϵ�argIdx�������Ĳ������͡�argIdx���Ǹ�0��ʼ������ֵ���������jsArgCount���ص�ֵ������������
DUILIB_API jsType jsArgType(jsExecState es, int argIdx);

// ��ȡ��argIdx��Ӧ�Ĳ�����jsValueֵ��
DUILIB_API jsValue jsArgValue(jsExecState es, int argIdx);

// ��ȡv��Ӧ�����͡�
DUILIB_API jsType jsTypeOf(jsValue v);

// �ж�v�Ƿ�Ϊ���֡��ַ�����bool
DUILIB_API bool jsIsNumber(jsValue v);
DUILIB_API bool jsIsString(jsValue v);
DUILIB_API bool jsIsBoolean(jsValue v);
// ��v�������֡��ַ�����undefined��null��������ʱ�򣬴˽ӿڷ���true
DUILIB_API bool jsIsObject(jsValue v);
DUILIB_API bool jsIsFunction(jsExecState es, jsValue v);
DUILIB_API bool jsIsUndefined(jsExecState es, jsValue v);
DUILIB_API bool jsIsNull(jsExecState es, jsValue v);
DUILIB_API bool jsIsArray(jsExecState es, jsValue v);
// ���v�����Ǹ�����ֵ�����ض�Ӧ��true����false������Ǹ�����JSTYPE_OBJECT��������false������ע�⣩
DUILIB_API bool jsIsTrue(jsValue v);
// �ȼ���!jsIsTrue(v)
DUILIB_API bool jsIsFalse(jsValue v);


// ���v�Ǹ����λ��߸��㣬������Ӧֵ������Ǹ��㣬����ȡ�����ֵ����������������ͣ�����0������ע�⣩
DUILIB_API int jsToInt(jsExecState es, jsValue v);
// ���v�Ǹ������Σ�������Ӧֵ��������������ͣ�����0.0������ע�⣩
DUILIB_API double jsToFloat(jsExecState es, jsValue v);
DUILIB_API double jsToDouble(jsExecState es, jsValue v);
DUILIB_API bool jsToBoolean(jsExecState es, jsValue v);

// ����һ��js��arraybuffer���͵�jaValue����Ҫ��������һЩ���������ݣ�ע���Ǵ���
DUILIB_API jsValue jsArrayBuffer(jsExecState es, char *buffer, size_t size);
// ��ȡһ��js��arraybuffer���͵����ݡ���Ҫ��������һЩ����������
DUILIB_API wkeMemBuf *jsGetArrayBuffer(jsExecState es, jsValue v);

// ���v�Ǹ��ַ�����������Ӧֵ��������������ͣ�����L""������ע�⣩ ���⣬���ص��ַ�������Ҫ�ⲿ�ͷš�mb������һ֡�Զ��ͷ�
DUILIB_API CDuiString jsToTempString(jsExecState es, jsValue v);

// return v8::Persistent<v8::Value>*
DUILIB_API void *jsToV8Value(jsExecState es, jsValue v);

// ������һ��int�͵�jsValue��ע���Ǵ���
DUILIB_API jsValue jsInt(int v);
DUILIB_API jsValue jsFloat(float v);
DUILIB_API jsValue jsDouble(jsExecState es, double v);
DUILIB_API jsValue jsBoolean(jsExecState es, bool v);

DUILIB_API jsValue jsUndefined();
DUILIB_API jsValue jsNull();
DUILIB_API jsValue jsTrue();
DUILIB_API jsValue jsFalse();

// ����һ��utf8������ַ����ĵ�jsValue��str�����ڲ��������棬ע���Ǵ���
DUILIB_API jsValue jsString(jsExecState es, const CDuiString &str);
// ����һ����ʱjs object��jsValue��ע���Ǵ���
DUILIB_API jsValue jsEmptyObject(jsExecState es);
DUILIB_API jsValue jsEmptyArray(jsExecState es);

typedef jsValue(WKE_CALL *jsGetPropertyCallback)(jsExecState es, jsValue obj, const char *propertyName);
typedef bool(WKE_CALL *jsSetPropertyCallback)(jsExecState es, jsValue obj, const char *propertyName,
                                              jsValue value);
typedef jsValue(WKE_CALL *jsCallAsFunctionCallback)(jsExecState es, jsValue obj, jsValue *args,
                                                    int argCount);
struct tagjsData; // declare warning fix
typedef void(WKE_CALL *jsFinalizeCallback)(struct tagjsData *data);

typedef struct tagjsData
{
    char typeName[100];
    jsGetPropertyCallback propertyGet;
    jsSetPropertyCallback propertySet;
    jsFinalizeCallback finalize;
    jsCallAsFunctionCallback callAsFunction;
} jsData;

// ����һ��js Objcet�����Դ��ݸ�jsʹ�á�
// ������
// DUILIB_API typedef jsValue(*jsGetPropertyCallback)(jsExecState es, jsValue obj, const utf8* propertyName);;
//   ���Է���������js��XXX.yyy��������ʱ��jsGetPropertyCallback�ᱻ����������propertyName��ֵΪyyy
// typedef bool(*jsSetPropertyCallback)(jsExecState es, jsValue obj, const utf8* propertyName, jsValue value);
//   ��������������js��XXX.yyy=1��������ʱ��jsSetPropertyCallback�ᱻ����������propertyName��ֵΪyyy��value��ֵ���ַ������͡�����ͨ��֮ǰ�ᵽ���ַ��������ӿ�����ȡ
// typedef jsValue(*jsCallAsFunctionCallback)(jsExecState es, jsValue obj, jsValue* args, int argCount);
//   ��js��XXX()��������ʱ�ᴥ����
// typedef void(*jsFinalizeCallback)(struct tagjsData* data);
//   ��js��û�����ã�����������ʱ�ᴥ��
DUILIB_API jsValue jsObject(jsExecState es, jsData *data);
// ����һ����frame��ȫ�ֺ�����jsData���÷����ϡ�js���ã�XXX() ��ʱjsData��callAsFunction������ ��ʵjsFunction��jsObject���ܻ������ơ���jsObject�Ĺ��ܸ�ǿ��һЩ
DUILIB_API jsValue jsFunction(jsExecState es, jsData *data);
// ��ȡjsObject��jsFunction������jsValue��Ӧ��jsDataָ�롣
DUILIB_API jsData *jsGetData(jsExecState es, jsValue value);

// ���object�Ǹ�js��object�����ȡpropָ�������ԡ����object����js object���ͣ��򷵻�jsUndefined
DUILIB_API jsValue jsGet(jsExecState es, jsValue obj, const  CDuiString &prop);
// ����object������
DUILIB_API void jsSet(jsExecState es, jsValue obj, const CDuiString &prop, jsValue v);

// ����js arrary�ĵ�index����Ա��ֵ��object������js array�����ã�����᷵��jsUndefined
DUILIB_API jsValue jsGetAt(jsExecState es, jsValue obj, int index);
// ����js arrary�ĵ�index����Ա��ֵ��object������js array�����á�
DUILIB_API void jsSetAt(jsExecState es, jsValue obj, int index, jsValue value);

struct jsKeys
{
    unsigned int length;
    const utf8 **keys;
};
// ��ȡobject����Щkey
DUILIB_API jsKeys *jsGetKeys(jsExecState es, jsValue obj);
DUILIB_API void jsFreeKeys(jsKeys *obj);

DUILIB_API bool jsIsJsValueValid(jsExecState es, jsValue obj);
DUILIB_API bool jsIsValidExecState(jsExecState es);
DUILIB_API void jsDeleteObjectProp(jsExecState es, jsValue obj, const CDuiString &prop);

// ��ȡjs arrary�ĳ��ȣ�object������js array�����á�
DUILIB_API int jsGetLength(jsExecState es, jsValue obj);
// ����js arrary�ĳ��ȣ�object������js array�����á�
DUILIB_API void jsSetLength(jsExecState es, jsValue obj, int length);

DUILIB_API jsValue jsGlobalObject(jsExecState es);
// ��ȡes��Ӧ��webview
DUILIB_API wkeWebView jsGetWebView(jsExecState es);

// ִ��һ��js��������ֵ��
// ע�⣺str�Ĵ������mb�ڲ��Զ���������һ��function(){}�С�����ʹ�õı����ᱻ���� ע�⣺Ҫ��ȡ����ֵ����дreturn�����wke��̫һ����wke����Ҫдretrun
DUILIB_API jsValue jsEval(jsExecState es, const CDuiString &str);
// �������ӿڵ������ǣ�isInClosure��ʾ�Ƿ�Ҫ����һ��function(){}��jsEvalW�൱��jsEvalExW(es, str, false)
// ע�⣺�����Ҫ����ֵ����isInClosureΪtrueʱ����Ҫдreturn��Ϊfalse����
DUILIB_API jsValue jsEvalExW(jsExecState es, const CDuiString &str, bool isInClosure);

// ����һ��func��Ӧ��js�����������js�����ǳ�Ա����������Ҫ��thisValue�� ������Դ�jsUndefined��args�Ǹ����飬������argCount���ơ� func�����Ǵ�js��ȡ�ģ�Ҳ���������й���ġ�
DUILIB_API jsValue jsCall(jsExecState es, jsValue func, jsValue thisValue, jsValue *args, int argCount);
// ����window�ϵ�ȫ�ֺ���
DUILIB_API jsValue jsCallGlobal(jsExecState es, jsValue func, jsValue *args, int argCount);

// ��ȡwindow�ϵ�����
DUILIB_API jsValue jsGetGlobal(jsExecState es, const CDuiString &prop);
// ����window�ϵ�����
DUILIB_API void jsSetGlobal(jsExecState es, const CDuiString &prop, jsValue v);

// ǿ����������
DUILIB_API void jsGC();

DUILIB_API bool jsAddRef(jsExecState es, jsValue val);
DUILIB_API bool jsReleaseRef(jsExecState es, jsValue val);

struct jsExceptionInfo
{
    const utf8 *message; // Returns the exception message.
    const utf8 *sourceLine; // Returns the line of source code that the exception occurred within.
    const utf8
    *scriptResourceName; // Returns the resource name for the script from where the function causing the error originates.
    int lineNumber; // Returns the 1-based number of the line where the error occurred or 0 if the line number is unknown.
    int startPosition; // Returns the index within the script of the first character where the error occurred.
    int endPosition; // Returns the index within the script of the last character where the error occurred.
    int startColumn; // Returns the index within the line of the first character where the error occurred.
    int endColumn; // Returns the index within the line of the last character where the error occurred.
    const utf8 *callstackString;
};

// ��wkeRunJs��jsCall�Ƚӿڵ���ʱ�����ִ�е�js�������쳣���˽ӿڽ���ȡ���쳣��Ϣ�����򷵻�nullptr��
DUILIB_API jsExceptionInfo *jsGetLastErrorIfException(jsExecState es);

DUILIB_API jsValue jsThrowException(jsExecState es, const utf8 *exception);
DUILIB_API CDuiString jsGetCallstack(jsExecState es);

}

#endif // __WKE_DLL_H__
