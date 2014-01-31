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

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusServer>
#include <QDebug>
#include <QPluginLoader>
#include <QTest>

#include <SignOn/AbstractAccessControlManager>

#define P2P_SOCKET "unix:path=/tmp/tst_extension_%1"

class ExtensionTest: public QObject
{
    Q_OBJECT

public:
    ExtensionTest();

private Q_SLOTS:
    void initTestCase();
    void test_appId();
    void test_appId_p2p();
    void test_access();
    void test_accessWildcard();
    void cleanupTestCase();

private:
    SignOn::AbstractAccessControlManager *m_acm;
    QDBusConnection m_busConnection;
    QDBusConnection m_p2pConnection;
};

ExtensionTest::ExtensionTest():
    m_busConnection(QDBusConnection::sessionBus()),
    m_p2pConnection(QStringLiteral("uninitialized"))
{
}

void ExtensionTest::initTestCase()
{
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
    QSKIP("Disable because of QTBUG-36475");

    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall(m_busConnection.baseService(),
                                       "/", "my.interface", "hi");
    QString appId = m_acm->appIdOfPeer(m_busConnection, msg);
    /* At the moment, AppArmor doesn't implement the
     * GetConnectionAppArmorSecurityContext method, so expect an error. */
    QCOMPARE(appId, QStringLiteral("unconfined"));
}

void ExtensionTest::test_appId_p2p()
{
    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall("", "/", "my.interface", "hi");
    QString appId = m_acm->appIdOfPeer(m_p2pConnection, msg);
    /* At the moment, AppArmor doesn't implement the
     * GetConnectionAppArmorSecurityContext method, so expect an error. */
    QCOMPARE(appId, QStringLiteral("unconfined"));
}

void ExtensionTest::test_access()
{
    QSKIP("Disable because of QTBUG-36475");

    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall(m_busConnection.baseService(),
                                       "/", "my.interface", "hi");
    bool allowed = m_acm->isPeerAllowedToAccess(m_busConnection, msg,
                                                "anyContext");
    /* At the moment, AppArmor doesn't implement the
     * GetConnectionAppArmorSecurityContext method, so expect an error. */
    QVERIFY(!allowed);
}

void ExtensionTest::test_accessWildcard()
{
    QSKIP("Disable because of QTBUG-36475");

    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall(m_busConnection.baseService(),
                                       "/", "my.interface", "hi");
    bool allowed = m_acm->isPeerAllowedToAccess(m_busConnection, msg, "*");
    /* Everything is allowed to access "*" */
    QVERIFY(allowed);
}

void ExtensionTest::cleanupTestCase()
{
    delete m_acm;
}

QTEST_MAIN(ExtensionTest)
#include "tst_extension.moc"
