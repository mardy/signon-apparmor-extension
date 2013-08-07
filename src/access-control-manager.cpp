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

#include "access-control-manager.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

static const char keychainAppId[] = "SignondKeychain";

AccessControlManager::AccessControlManager(QObject *parent):
    SignOn::AbstractAccessControlManager(parent)
{
}

AccessControlManager::~AccessControlManager()
{
}

QString AccessControlManager::keychainWidgetAppId()
{
    return QLatin1String(keychainAppId);
}

bool AccessControlManager::isPeerAllowedToAccess(const QDBusMessage &peerMessage,
                                                 const QString &securityContext)
{
    QString appId = appIdOfPeer(peerMessage);

    bool allowed = (appId == securityContext ||
                    securityContext == QLatin1String("*"));
    qDebug() << "Process" << appId << "access to" << securityContext <<
        (allowed ? "ALLOWED" : "DENIED");
    return allowed;
}

QString AccessControlManager::appIdOfPeer(const QDBusMessage &peerMessage)
{
    QString uniqueConnectionId = peerMessage.service();
    QString appId;

    QDBusMessage msg =
        QDBusMessage::createMethodCall("org.freedesktop.DBus",
                                       "/org/freedesktop/DBus",
                                       "org.freedesktop.DBus",
                                       "GetConnectionAppArmorSecurityContext");
    QVariantList args;
    args << uniqueConnectionId;
    msg.setArguments(args);
    QDBusMessage reply =
        QDBusConnection::sessionBus().call(msg, QDBus::Block);
    if (reply.type() == QDBusMessage::ReplyMessage) {
        appId = reply.arguments().value(0, QString()).toString();
        qDebug() << "App ID:" << appId;
    } else {
        qWarning() << "Error getting app ID:" << reply.errorName() <<
            reply.errorMessage();
    }
    return appId;
}
