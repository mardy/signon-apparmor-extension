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

#include <QDBusMessage>
#include <QDebug>
#include <QPluginLoader>
#include <QTest>

#include <SignOn/AbstractAccessControlManager>

class ExtensionTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void test_appId();
    void test_access();
    void test_accessWildcard();
    void cleanupTestCase();

private:
    SignOn::AbstractAccessControlManager *m_acm;
};

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
}

void ExtensionTest::test_appId()
{
    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall(":0.3", "/", "my.interface", "hi");
    QString appId = m_acm->appIdOfPeer(msg);
    /* At the moment, AppArmor doesn't implement the
     * GetConnectionAppArmorSecurityContext method, so expect an error. */
    QCOMPARE(appId, QString());
}

void ExtensionTest::test_access()
{
    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall(":0.3", "/", "my.interface", "hi");
    bool allowed = m_acm->isPeerAllowedToAccess(msg, "anyContext");
    /* At the moment, AppArmor doesn't implement the
     * GetConnectionAppArmorSecurityContext method, so expect an error. */
    QVERIFY(!allowed);
}

void ExtensionTest::test_accessWildcard()
{
    /* forge a QDBusMessage */
    QDBusMessage msg =
        QDBusMessage::createMethodCall(":0.3", "/", "my.interface", "hi");
    bool allowed = m_acm->isPeerAllowedToAccess(msg, "*");
    /* Everything is allowed to access "*" */
    QVERIFY(allowed);
}

void ExtensionTest::cleanupTestCase()
{
    delete m_acm;
}

QTEST_MAIN(ExtensionTest)
#include "tst_extension.moc"
