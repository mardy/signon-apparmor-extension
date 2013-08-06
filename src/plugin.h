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

#ifndef SIGNON_APPARMOR_PLUGIN_H
#define SIGNON_APPARMOR_PLUGIN_H

#include <QObject>
#include <SignOn/ExtensionInterface>

class Plugin: public QObject, public SignOn::ExtensionInterface3
{
    Q_OBJECT
    Q_INTERFACES(SignOn::ExtensionInterface3)
    Q_PLUGIN_METADATA(IID "com.nokia.SingleSignOn.ExtensionInterface/3.0")

public:
    Plugin(QObject *parent = 0);

    SignOn::AbstractAccessControlManager *
        accessControlManager(QObject *parent = 0) const Q_DECL_OVERRIDE;
};

#endif // SIGNON_APPARMOR_PLUGIN_H
