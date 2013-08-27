/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
 *
 * This file is part of the signon-apparmor-extension
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SIGNON_APPARMOR_ACCESS_CONTROL_MANAGER_H
#define SIGNON_APPARMOR_ACCESS_CONTROL_MANAGER_H

#include <SignOn/AbstractAccessControlManager>

class AccessReply: public SignOn::AccessReply
{
    Q_OBJECT

public:
    AccessReply(const SignOn::AccessRequest &request, QObject *parent = 0);
};

class AccessControlManager: public SignOn::AbstractAccessControlManager
{
    Q_OBJECT

public:
    AccessControlManager(QObject *parent = 0);
    ~AccessControlManager();

    bool isPeerAllowedToAccess(const QDBusConnection &peerConnection,
                               const QDBusMessage &peerMessage,
                               const QString &securityContext) Q_DECL_OVERRIDE;

    QString appIdOfPeer(const QDBusConnection &peerConnection,
                        const QDBusMessage &peerMessage) Q_DECL_OVERRIDE;

    QString keychainWidgetAppId() Q_DECL_OVERRIDE;

    SignOn::AccessReply *handleRequest(const SignOn::AccessRequest &request)
        Q_DECL_OVERRIDE;
};

#endif // SIGNON_APPARMOR_ACCESS_CONTROL_MANAGER_H
