/*
 * This file is part of signon-apparmor-extension
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AP_FAKE_DBUS_H
#define AP_FAKE_DBUS_H

#include <QVariantMap>
#include <libqtdbusmock/DBusMock.h>

class FakeDBus
{
public:
    FakeDBus(QtDBusMock::DBusMock *mock): m_mock(mock) {
        m_mock->registerTemplate("fake.freedesktop.DBus",
                                 DBUS_MOCK_TEMPLATE,
                                 QDBusConnection::SessionBus);
    }

    void setCredentials(const QString &service, const QVariantMap &credentials) {
        mockedDBusService().call("SetCredentials", service, credentials);
    }

private:
    OrgFreedesktopDBusMockInterface &mockedDBusService() {
        return m_mock->mockInterface("fake.freedesktop.DBus",
                                     "/org/freedesktop/DBus",
                                     "org.freedesktop.DBus",
                                     QDBusConnection::SessionBus);
    }

private:
    QtDBusMock::DBusMock *m_mock;
};

#endif // AP_FAKE_DBUS_H
