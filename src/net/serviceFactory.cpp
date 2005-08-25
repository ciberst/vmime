//
// VMime library (http://www.vmime.org)
// Copyright (C) 2002-2005 Vincent Richard <vincent@vincent-richard.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include "vmime/net/serviceFactory.hpp"
#include "vmime/net/service.hpp"

#include "vmime/exception.hpp"
#include "vmime/config.hpp"

#include "src/net/builtinServices.inl"


namespace vmime {
namespace net {


serviceFactory::serviceFactory()
{
}


serviceFactory::~serviceFactory()
{
}


serviceFactory* serviceFactory::getInstance()
{
	static serviceFactory instance;
	return (&instance);
}


ref <service> serviceFactory::create
	(ref <session> sess, const string& protocol, ref <authenticator> auth)
{
	return (getServiceByProtocol(protocol)->create(sess, auth));
}


ref <service> serviceFactory::create
	(ref <session> sess, const utility::url& u, ref <authenticator> auth)
{
	ref <service> serv = create(sess, u.getProtocol(), auth);

	sess->getProperties()[serv->getInfos().getPropertyPrefix() + "server.address"] = u.getHost();

	if (u.getPort() != utility::url::UNSPECIFIED_PORT)
		sess->getProperties()[serv->getInfos().getPropertyPrefix() + "server.port"] = u.getPort();

	// Path portion of the URL is used to point a specific folder (empty = root).
	// In maildir, this is used to point to the root of the message repository.
	if (!u.getPath().empty())
		sess->getProperties()[serv->getInfos().getPropertyPrefix() + "server.rootpath"] = u.getPath();

	if (!u.getUsername().empty())
	{
		sess->getProperties()[serv->getInfos().getPropertyPrefix() + "auth.username"] = u.getUsername();
		sess->getProperties()[serv->getInfos().getPropertyPrefix() + "auth.password"] = u.getPassword();
	}

	return (serv);
}


ref <const serviceFactory::registeredService> serviceFactory::getServiceByProtocol(const string& protocol) const
{
	const string name(utility::stringUtils::toLower(protocol));

	for (std::vector <ref <registeredService> >::const_iterator it = m_services.begin() ;
	     it != m_services.end() ; ++it)
	{
		if ((*it)->getName() == name)
			return (*it);
	}

	throw exceptions::no_service_available(name);
}


const int serviceFactory::getServiceCount() const
{
	return (m_services.size());
}


ref <const serviceFactory::registeredService> serviceFactory::getServiceAt(const int pos) const
{
	return (m_services[pos]);
}


const std::vector <ref <const serviceFactory::registeredService> > serviceFactory::getServiceList() const
{
	std::vector <ref <const registeredService> > res;

	for (std::vector <ref <registeredService> >::const_iterator it = m_services.begin() ;
	     it != m_services.end() ; ++it)
	{
		res.push_back(*it);
	}

	return (res);
}


} // net
} // vmime