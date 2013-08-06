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

#ifndef SIGNON_APPARMOR_TST_EXTENSION_H
#define SIGNON_APPARMOR_TST_EXTENSION_H

#include <QObject>
#include <SignOn/AbstractAccessControlManager>

class ExtensionTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void test_appId();
    void test_access();
    void cleanupTestCase();

private:
    SignOn::AbstractAccessControlManager *m_acm;
};

#endif // SIGNON_APPARMOR_TST_EXTENSION_H
