#include "InvitationManager.h"

#include <QUrl>
#include <QUrlQuery>

const QString cUrlPath = "/join";
const QString cUrlParamRoom = "r";
const QString cUrlParamToken = "t";
const QString cUrlParamName = "n";

InvitationParams InvitationManager::GetInvitationParams(const QString &invitationUrl)
{
    InvitationParams inviteParams;

    QUrl url(invitationUrl);
    inviteParams.host_ = url.host().toStdString();
    QUrlQuery urlQuery(url.query());
    QList<QPair<QString, QString>> queryItems = urlQuery.queryItems();
    for (auto iter = queryItems.begin(); iter != queryItems.end(); ++iter)
    {
        QPair<QString, QString> queryItem = *iter;
        if (queryItem.first.compare(cUrlParamRoom) == 0)
        {
            inviteParams.room_ = queryItem.second.toStdString();
        }
        else if (queryItem.first.compare(cUrlParamToken) == 0)
        {
            inviteParams.token_ = queryItem.second.toStdString();
        }
        else if (queryItem.first.compare(cUrlParamName) == 0)
        {
            inviteParams.name_ = queryItem.second.toStdString();
        }
    }

    return inviteParams;
}

QString InvitationManager::MakeInvitationUrl(const InvitationParams &inviteParams)
{
    QList<QPair<QString, QString>> queryItems;
    queryItems.append(QPair<QString, QString>(cUrlParamRoom, QString::fromStdString(inviteParams.room_)));
    queryItems.append(QPair<QString, QString>(cUrlParamToken, QString::fromStdString(inviteParams.token_)));
    queryItems.append(QPair<QString, QString>(cUrlParamName, QString::fromStdString(inviteParams.name_)));
    QUrlQuery urlQuery;
    urlQuery.setQueryItems(queryItems);

    QUrl url;
    url.setScheme("https");
    url.setHost(QString::fromStdString(inviteParams.host_));
    url.setPath("/join");
    url.setQuery(urlQuery);

    return url.toString();
}
