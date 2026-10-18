#ifndef API_CLIENT_BASE_H
#define API_CLIENT_BASE_H

#include "logging/Logger.h"
#include "common/CommonConstants.h"

#include <QObject>
#include <QQmlEngine>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QString>
#include <QMutex>

class ApiServerNotifier : public QObject
{
    Q_OBJECT
public:
    static ApiServerNotifier *instance();

signals:
    void serverUrlChanged(const QString &newUrl);
};

class ApiClientBase : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(bool isRemote READ isRemote NOTIFY serverUrlChanged)

public:
    explicit ApiClientBase(QObject *parent = nullptr);
    ~ApiClientBase() override = default;

    static QNetworkAccessManager *sharedNetworkManager();
    static QUrl endpoint(const QString &route);
    static QNetworkRequest createJsonRequest(const QUrl &url);

    // Static Server URL Management
    static QString activeServerUrl();
    static void setActiveServerUrl(const QString &url);
    static bool isRemoteServer();

    // Instance accessors for QML
    Q_INVOKABLE QString serverUrl() const { return activeServerUrl(); }
    Q_INVOKABLE void setServerUrl(const QString &url) { setActiveServerUrl(url); }
    Q_INVOKABLE bool isRemote() const { return isRemoteServer(); }
    Q_INVOKABLE void resetToDefaultUrl();
    Q_INVOKABLE void testServerConnection(const QString &targetUrl = QString());
    Q_INVOKABLE void testConnection(const QString &targetUrl = QString());

    Q_INVOKABLE QString baseUrl() const { return endpoint(QStringLiteral("/")).toString(); }
    Q_INVOKABLE QUrl serverEndpoint(const QString &route = QStringLiteral("/")) const { return endpoint(route); }

signals:
    void serverUrlChanged(const QString &newUrl);
    void serverConnectionTested(bool success, int latencyMs, const QString &message);
    void connectionTested(bool success, int latencyMs, const QString &message);

protected:
    QNetworkAccessManager &network();

private:
    static void initBaseUrl();
    static QString s_baseUrl;
    static QMutex s_urlMutex;
    static bool s_initialized;
};

#endif // API_CLIENT_BASE_H
