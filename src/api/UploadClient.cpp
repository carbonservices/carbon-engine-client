#include "UploadClient.h"

// Qt
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDir>
#include <QFile>
#include <QFileInfo>

UploadClient::UploadClient(QObject *parent) : ApiClientBase(parent) {}

void UploadClient::uploadDocuments(const QVariantList &filePaths)
{
    QStringList localPaths;
    for (const QVariant& item : filePaths) {
        QString pathStr;
        if (item.canConvert<QUrl>()) {
            QUrl url = item.toUrl();
            if (url.isLocalFile()) {
                pathStr = url.toLocalFile();
            } else {
                pathStr = url.toString();
            }
        } else {
            pathStr = item.toString();
        }

        if (pathStr.startsWith("file://", Qt::CaseInsensitive)) {
            QUrl url(pathStr);
            if (url.isLocalFile()) {
                pathStr = url.toLocalFile();
            } else if (pathStr.startsWith("file:///", Qt::CaseInsensitive)) {
                pathStr = pathStr.mid(8);
            } else {
                pathStr = pathStr.mid(7);
            }
        }

        pathStr = QDir::cleanPath(pathStr);
        if (!pathStr.isEmpty()) {
            localPaths.append(pathStr);
        }
    }

    if (localPaths.isEmpty()) {
        _log.error("Upload aborted: No valid local file paths provided");
        emit uploadFailed("No valid local file paths provided");
        return;
    }
    _log.info(QString("Uploading %1 document(s): %2").arg(localPaths.size()).arg(localPaths.join(", ")));

    QJsonArray pathArray;
    QJsonArray filesArray;

    for (const QString &p : localPaths) {
        pathArray.append(p);
        QFile file(p);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray content = file.readAll();
            file.close();
            QFileInfo fi(p);
            QJsonObject fObj;
            fObj["filename"] = fi.fileName();
            fObj["data"] = QString::fromLatin1(content.toBase64());
            filesArray.append(fObj);
        }
    }

    QJsonObject reqObj;
    if (isRemote()) {
        // Remote server: send base64 files array only; server stores files in its own scratch directory
        reqObj["files"] = filesArray;
    } else {
        reqObj["file_paths"] = pathArray;
        if (!filesArray.isEmpty()) {
            reqObj["files"] = filesArray;
        }
    }

    QNetworkRequest request = createJsonRequest(endpoint(ApiRoutes::Ingest));
    QJsonDocument body(reqObj);
    QNetworkReply *reply = network().post(request, body.toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, localPaths]()
    {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            QByteArray errData = reply->readAll();
            QString serverMsg;
            if (!errData.isEmpty()) {
                auto errDoc = QJsonDocument::fromJson(errData);
                if (errDoc.isObject() && errDoc.object().contains("error")) {
                    serverMsg = errDoc.object().value("error").toString();
                } else {
                    serverMsg = QString::fromUtf8(errData).trimmed();
                }
            }
            if (serverMsg.isEmpty()) {
                serverMsg = reply->errorString();
            }

            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            _log.error(QString("Upload failed for %1 document(s) (HTTP %2): %3")
                       .arg(localPaths.size()).arg(statusCode).arg(serverMsg));
            emit uploadFailed(serverMsg);
            return;
        }

        auto doc = QJsonDocument::fromJson(reply->readAll());
        int jobId = doc.object().value("job_id").toInt(-1);
        if (jobId < 0) {
            QString err = doc.object().value("error").toString("Server failed to assign valid job ID");
            _log.error(QString("Upload failed to produce valid job_id: %1").arg(err));
            emit uploadFailed(err);
            return;
        }
        _log.info(QString("Upload queued for %1 document(s), job_id=%2").arg(localPaths.size()).arg(jobId));
        emit jobQueued(jobId);
    });
}

void UploadClient::cancelJob(int jobId)
{
    QJsonObject reqObj;
    reqObj["job_id"] = jobId;

    QNetworkRequest request = createJsonRequest(endpoint(ApiRoutes::JobCancel));
    QJsonDocument body(reqObj);
    QNetworkReply *reply = network().post(request, body.toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, jobId]()
    {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            _log.error(QString("Cancel job failed for job %1: %2").arg(jobId).arg(reply->errorString()));
            emit uploadFailed("Failed to cancel job: " + reply->errorString());
            return;
        }
        _log.info(QString("Job %1 cancelled successfully").arg(jobId));
        emit jobCancelled(jobId);
    });
}
