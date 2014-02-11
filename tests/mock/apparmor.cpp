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

#include <QByteArray>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <sys/apparmor.h>

#define UNCONFINED_PROFILE "unconfined"

QByteArray mockedProfile()
{
    QByteArray envVariable = qgetenv("APPARMOR_MOCK_PROFILE");
    return envVariable.isEmpty() ? UNCONFINED_PROFILE : envVariable;
}

int aa_getpeercon(int fd, char **con, char **mode)
{
    Q_UNUSED(fd);
    Q_UNUSED(mode);
    *con = strdup(mockedProfile().constData());
    return 0;
}

QDBusMessage QDBusConnection::call(const QDBusMessage &message,
                                   QDBus::CallMode mode,
                                   int timeout) const
{
    Q_UNUSED(mode);
    Q_UNUSED(timeout);
    return message.createReply(QVariantList() << mockedProfile().constData());
}
