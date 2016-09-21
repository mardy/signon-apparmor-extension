/*
 * Copyright (C) 2016 Canonical Ltd.
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

#include "fake_dbus.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusServer>
#include <QDebug>
#include <QPluginLoader>
#include <QTest>

#include <SignOn/AbstractAccessControlManager>

#include <libqtdbusmock/DBusMock.h>


#define P2P_SOCKET "unix:path=/tmp/tst_extension_%1"

using namespace QtDBusMock;

static void setMockedProfile(const char *profile)
{
    if (profile) {
        qputenv("APPARMOR_MOCK_PROFILE", profile);
    } else {
#if QT_VERSION >= 0x050100
        qunsetenv("APPARMOR_MOCK_PROFILE");
#else
        qputenv("APPARMOR_MOCK_PROFILE", "");
#endif
    }
}

class ExtensionTest: public QObject
{
    Q_OBJECT

    struct SetupEnvironment {
        SetupEnvironment() { qunsetenv("LD_PRELOAD"); }
    };

public:
    ExtensionTest();

private Q_SLOTS:
    void initTestCase();
    void test_appId();
    void test_appId_p2p();
    void test_click_version();
    void test_access();
    void test_accessWildcard();
    void test_unconfined();
    void cleanupTestCase();

private:
    SignOn::AbstractAccessControlManager *m_acm;
    SetupEnvironment m_env;
    QtDBusTest::DBusTestRunner m_dbus;
    QtDBusMock::DBusMock m_mock;
    FakeDBus m_fakeDBus;
    QDBusConnection m_busConnection;
    QDBusConnection m_p2pConnection;
};

ExtensionTest::ExtensionTest():
    m_mock(m_dbus),
    m_fakeDBus(&m_mock),
    m_busConnection(m_dbus.sessionConnection()),
    m_p2pConnection(QStringLiteral("uninitialized"))
{
    DBusMock::registerMetaTypes();
}

void ExtensionTest::initTestCase()
{
    m_dbus.startServices();

    qputenv("SIGNON_APPARMOR_TEST", "1");

    QPluginLoader pluginLoader(PLUGIN_PATH);
    QObject *plugin = pluginLoader.instance();
    QVERIFY(plugin != 0);

    SignOn::ExtensionInterface3 *interface =
        qobject_cast<SignOn::ExtensionInterface3 *>(plugin);
    QVERIFY(interface != 0);

    m_acm = interface->accessControlManager(this);
    QVERIFY(m_acm != 0);

    QDBusServer *server;
    for (int i = 0; i < 10; i++) {
        server = new QDBusServer(QString::fromLatin1(P2P_SOCKET).arg(i), this);
        if (!server->isConnected()) {
            delete server;
        } else {
            break;
        }
    }

    QVERIFY(server->isConnected());

    m_p2pConnection = QDBusConnection::connectToPeer(server->address(),
                                                     QStringLiteral("tst"));
    QVERIFY(m_p2pConnection.isConnected());
}

void ExtensionTest::test_appId()
{
    m_fakeDBus.setCredentials(m_busConnection.baseService(), {
        { "LinuxSecurityLabel", QByteArray("unconfined") },
    });

    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall(m_busConnection.baseService(),
                                       "/", "my.interface", "hi");
    QString appId = m_acm->appIdOfPeer(m_busConnection, msg);
    QCOMPARE(appId, QStringLiteral("unconfined"));
}

void ExtensionTest::test_appId_p2p()
{
    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall("", "/", "my.interface", "hi");
    QString appId = m_acm->appIdOfPeer(m_p2pConnection, msg);
    QCOMPARE(appId, QStringLiteral("unconfined"));
}

void ExtensionTest::test_click_version()
{
    m_fakeDBus.setCredentials(":0.1", {
        { "LinuxSecurityLabel", QByteArray("com.ubuntu.myapp_myapp_0.2 (enforce)") },
    });
    /* forge a QDBusMessage */
    setMockedProfile("com.ubuntu.myapp_myapp_0.2");
    QDBusMessage msg =
        QDBusMessage::createMethodCall(":0.1", "/", "my.interface", "hi");
    bool allowed = m_acm->isPeerAllowedToAccess(m_busConnection, msg,
                                                "anyContext");
    QVERIFY(!allowed);

    allowed = m_acm->isPeerAllowedToAccess(m_busConnection, msg,
                                           "com.ubuntu.myapp_myapp_0.2");
    QVERIFY(allowed);

    /* A different version of the package should also work */
    allowed = m_acm->isPeerAllowedToAccess(m_busConnection, msg,
                                           "com.ubuntu.myapp_myapp_0.1");
    QVERIFY(allowed);
    setMockedProfile(NULL);
}

void ExtensionTest::test_access()
{
    m_fakeDBus.setCredentials(m_busConnection.baseService(), {
    });
    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall(m_busConnection.baseService(),
                                       "/", "my.interface", "hi");
    bool allowed = m_acm->isPeerAllowedToAccess(m_busConnection, msg,
                                                "anyContext");
    QVERIFY(!allowed);
}

void ExtensionTest::test_accessWildcard()
{
    m_fakeDBus.setCredentials(":0.1", {
        { "LinuxSecurityLabel", QByteArray("com.ubuntu.myapp_myapp_0.2 (enforce)") },
    });
    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall(m_busConnection.baseService(),
                                       "/", "my.interface", "hi");
    bool allowed = m_acm->isPeerAllowedToAccess(m_busConnection, msg, "*");
    /* Everything is allowed to access "*" */
    QVERIFY(allowed);
}

void ExtensionTest::test_unconfined()
{
    /* forge a QDBusMessage */
    setMockedProfile("unconfined");
    QDBusMessage msg =
        QDBusMessage::createMethodCall("", "/", "my.interface", "hi");
    bool allowed = m_acm->isPeerAllowedToAccess(m_busConnection, msg,
                                                "anyContext");
    QVERIFY(allowed);

    allowed = m_acm->isPeerAllowedToAccess(m_busConnection, msg,
                                           "com.ubuntu.myapp_myapp_0.2");
    QVERIFY(allowed);
    setMockedProfile(NULL);
}

void ExtensionTest::cleanupTestCase()
{
    delete m_acm;
}

QTEST_MAIN(ExtensionTest)
#include "tst_extension.moc"
