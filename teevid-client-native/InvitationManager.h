#ifndef INVITATIONMANAGER_H
#define INVITATIONMANAGER_H

#include <string>
#include <QString>

struct InvitationParams
{
    std::string host_;
    std::string room_;
    std::string token_;
    std::string name_; // empty for now
};


class InvitationManager
{
public:
    static InvitationParams GetInvitationParams(const QString& invitationUrl);
    static QString MakeInvitationUrl(const InvitationParams& inviteParams);
};

#endif // INVITATIONMANAGER_H
