#include "ApiClientBase.h"

#include <QSettings>
#include <QElapsedTimer>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

QString ApiClientBase::s_baseUrl;
QMutex ApiClientBase::s_urlMutex;
bool ApiClientBase::s_initialized = false;

ApiServerNotifier *ApiServerNotifier::instance()
{
    static ApiServerNotifier s_instance;
    return &s_instance;
}

ApiClientBase::ApiClientBase(QObject *parent)
    : QObject(parent)
{
    initBaseUrl();
    connect(ApiServerNotifier::instance(), &ApiServerNotifier::serverUrlChanged,
            this, &ApiClientBase::serverUrlChanged);
}

void ApiClientBase::initBaseUrl()
{
    QMutexLocker locker(&s_urlMutex);
    if (s_initialized) {
        return;
    }

    QSettings settings("CarbonServices", "CarbonEngineClient");
    QString saved = settings.value("server/url").toString().trimmed();

    if (!saved.isEmpty()) {
        while (saved.endsWith('/')) {
            saved.chop(1);
        }
        s_baseUrl = saved;
    } else {
        s_baseUrl = QStringLiteral("http://localhost:%1").arg(ServerDefaults::DefaultPort);
    }

    s_initialized = true;
}

QNetworkAccessManager *ApiClientBase::sharedNetworkManager()
{
    static QNetworkAccessManager s_manager;
    return &s_manager;
}

QNetworkAccessManager &ApiClientBase::network()
{
    return *sharedNetworkManager();
}

QString ApiClientBase::activeServerUrl()
{
    initBaseUrl();
    QMutexLocker locker(&s_urlMutex);
    return s_baseUrl;
}

void ApiClientBase::setActiveServerUrl(const QString &url)
{
    initBaseUrl();
    QString trimmed = url.trimmed();
    while (trimmed.endsWith('/')) {
        trimmed.chop(1);
    }
    if (trimmed.isEmpty()) {
        trimmed = QStringLiteral("http://localhost:%1").arg(ServerDefaults::DefaultPort);
    }
    if (!trimmed.startsWith("http://", Qt::CaseInsensitive) && !trimmed.startsWith("https://", Qt::CaseInsensitive)) {
        trimmed.prepend("http://");
    }

    {
        QMutexLocker locker(&s_urlMutex);
        if (s_baseUrl == trimmed) {
            return;
        }
        s_baseUrl = trimmed;
    }

    QSettings settings("CarbonServices", "CarbonEngineClient");
    settings.setValue("server/url", trimmed);
    settings.sync();

    emit ApiServerNotifier::instance()->serverUrlChanged(trimmed);
}

bool ApiClientBase::isRemoteServer()
{
    QString url = activeServerUrl().toLower();
    return !url.contains("localhost") && !url.contains("127.0.0.1");
}

void ApiClientBase::resetToDefaultUrl()
{
    setActiveServerUrl(QStringLiteral("http://localhost:%1").arg(ServerDefaults::DefaultPort));
}

void ApiClientBase::testServerConnection(const QString &targetUrl)
{
    QString testUrl = targetUrl.trimmed();
    if (testUrl.isEmpty()) {
        testUrl = activeServerUrl();
    }
    while (testUrl.endsWith('/')) {
        testUrl.chop(1);
    }
    if (!testUrl.startsWith("http://", Qt::CaseInsensitive) && !testUrl.startsWith("https://", Qt::CaseInsensitive)) {
        testUrl.prepend("http://");
    }

    QUrl pingUrl(testUrl + QString(ApiRoutes::System));
    QNetworkRequest req(pingUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    auto *timer = new QElapsedTimer();
    timer->start();

    QNetworkReply *reply = network().get(req);

    // Timeout guard (5 seconds)
    auto *timeoutTimer = new QTimer(reply);
    timeoutTimer->setSingleShot(true);
    connect(timeoutTimer, &QTimer::timeout, reply, [reply]() {
        if (reply->isRunning()) {
            reply->abort();
        }
    });
    timeoutTimer->start(5000);

    connect(reply, &QNetworkReply::finished, this, [this, reply, timer, timeoutTimer]() {
        timeoutTimer->stop();
        qint64 elapsedMs = timer->elapsed();
        delete timer;
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            QString err = reply->errorString();
            if (reply->error() == QNetworkReply::OperationCanceledError) {
                err = "Connection timed out after 5 seconds";
            }
            emit serverConnectionTested(false, static_cast<int>(elapsedMs), err);
            emit connectionTested(false, static_cast<int>(elapsedMs), err);
            return;
        }

        auto doc = QJsonDocument::fromJson(reply->readAll());
        if (!doc.isObject()) {
            emit serverConnectionTested(false, static_cast<int>(elapsedMs), "Server returned non-JSON response");
            emit connectionTested(false, static_cast<int>(elapsedMs), "Server returned non-JSON response");
            return;
        }

        emit serverConnectionTested(true, static_cast<int>(elapsedMs), "Connected successfully");
        emit connectionTested(true, static_cast<int>(elapsedMs), "Connected successfully");
    });
}

void ApiClientBase::testConnection(const QString &targetUrl)
{
    testServerConnection(targetUrl);
}

QUrl ApiClientBase::endpoint(const QString &route)
{
    QString base = activeServerUrl();
    QString cleanRoute = route.startsWith('/') ? route : QString("/" + route);
    return QUrl(base + cleanRoute);
}

QNetworkRequest ApiClientBase::createJsonRequest(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Pass custom client header if needed
    request.setRawHeader("X-Client-Platform", "Carbon-ESG-Desktop");
    return request;
}
