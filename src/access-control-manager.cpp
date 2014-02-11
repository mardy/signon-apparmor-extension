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
#include <QStringList>
#include <dbus/dbus.h>
#include <sys/apparmor.h>

static const char keychainAppId[] = "SignondKeychain";

AccessReply::AccessReply(const SignOn::AccessRequest &request,
                         QObject *parent):
    SignOn::AccessReply(request, parent)
{
    /* Always decline */
    QMetaObject::invokeMethod(this, "decline", Qt::QueuedConnection);
}

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

bool AccessControlManager::isPeerAllowedToAccess(
                                       const QDBusConnection &peerConnection,
                                       const QDBusMessage &peerMessage,
                                       const QString &securityContext)
{
    QString appId = appIdOfPeer(peerConnection, peerMessage);

    bool allowed = (stripVersion(appId) == stripVersion(securityContext) ||
                    securityContext == QLatin1String("*"));
    qDebug() << "Process" << appId << "access to" << securityContext <<
        (allowed ? "ALLOWED" : "DENIED");
    return allowed;
}

QString AccessControlManager::appIdOfPeer(const QDBusConnection &peerConnection,
                                          const QDBusMessage &peerMessage)
{
    QString uniqueConnectionId = peerMessage.service();
    QString appId;

    if (uniqueConnectionId.isEmpty()) {
        /* it's a p2p connection; get the fd of the socket, and ask apparmor to
         * identify the peer. */
        DBusConnection *connection =
            (DBusConnection *)peerConnection.internalPointer();
        int fd = 0;
        dbus_bool_t ok = dbus_connection_get_unix_fd(connection, &fd);
        if (Q_LIKELY(ok)) {
            char *con = NULL, *mode = NULL;
            int ret = aa_getpeercon(fd, &con, &mode);
            if (Q_LIKELY(ret >= 0)) {
                appId = QString::fromUtf8(con);
                qDebug() << "App ID:" << appId;
                free(con);
            } else {
                qWarning() << "Couldn't get apparmor profile of peer";
            }
        } else {
            qWarning() << "Couldn't get fd of caller!";
        }
    } else {
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
    }
    return appId;
}

SignOn::AccessReply *
AccessControlManager::handleRequest(const SignOn::AccessRequest &request)
{
    return new AccessReply(request, this);
}

QString AccessControlManager::stripVersion(const QString &appId) const
{
    QStringList components = appId.split('_');
    if (components.count() != 3) return appId;

    /* Click packages have a profile of the form
     *  $name_$application_$version
     * (see https://wiki.ubuntu.com/SecurityTeam/Specifications/ApplicationConfinement/Manifest#Click)
     *
     * We assume that this is a click package, and strip out the last part.
     */
    components.removeLast();
    return components.join('_');
}
