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
#include <QDBusError>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include <QStringList>
#include <sys/apparmor.h>

static const char keychainAppId[] = "unconfined";

AccessReply::AccessReply(const SignOn::AccessRequest &request,
                         QObject *parent):
    SignOn::AccessReply(request, parent)
{
    /* Always decline */
    QMetaObject::invokeMethod(this, "decline", Qt::QueuedConnection);
}

AccessControlManager::AccessControlManager(QObject *parent):
    SignOn::AbstractAccessControlManager(parent),
    m_dbusService(qEnvironmentVariableIsSet("SIGNON_APPARMOR_TEST") ?
                  "fake.freedesktop.DBus" : "org.freedesktop.DBus")
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
                    securityContext == QLatin1String("*") ||
                    /* Temporary workaround for
                     * https://bugs.launchpad.net/bugs/1376445 */
                    appId == QStringLiteral("unconfined") ||
                    /* Let the dash access any account:
                     * https://bugs.launchpad.net/bugs/1589444 */
                    appId == QStringLiteral("unity8-dash"));
    qDebug() << "Process" << appId << "access to" << securityContext <<
        (allowed ? "ALLOWED" : "DENIED");
    return allowed;
}

QString AccessControlManager::appIdOfPeer(const QDBusConnection &peerConnection,
                                          const QDBusMessage &peerMessage)
{
    Q_UNUSED(peerConnection);

    QString uniqueConnectionId = peerMessage.service();
    QString appId;

    if (uniqueConnectionId.isEmpty()) {
        /* it's a p2p connection; we treat the peer as "unconfined" */
        qDebug() << "Client connected via P2P socket; treating as unconfined";
        appId = "unconfined";
    } else {
        QDBusMessage msg =
            QDBusMessage::createMethodCall(m_dbusService,
                                           "/org/freedesktop/DBus",
                                           "org.freedesktop.DBus",
                                           "GetConnectionCredentials");
        QVariantList args;
        args << uniqueConnectionId;
        msg.setArguments(args);
        QDBusReply<QVariantMap> reply = QDBusConnection::sessionBus().call(msg, QDBus::Block);
        if (reply.isValid()) {
            QVariantMap map = reply.value();
            QByteArray context = map.value("LinuxSecurityLabel").toByteArray();
            if (!context.isEmpty()) {
                aa_splitcon(context.data(), NULL);
                appId = QString::fromUtf8(context);
            }
            qDebug() << "App ID:" << appId;
        } else {
            QDBusError error = reply.error();
            qWarning() << "Error getting app ID:" << error.name() <<
                error.message();
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
