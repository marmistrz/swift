#pragma once

#include "Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {
	class BonjourQuerier;

	class BonjourRegisterQuery : public DNSSDRegisterQuery, public BonjourQuery {
		public:	
			BonjourRegisterQuery(const String& name, int port, const ByteArray& txtRecord, boost::shared_ptr<BonjourQuerier> querier) : BonjourQuery(querier) {
				DNSServiceErrorType result = DNSServiceRegister(
						&sdRef, 0, 0, name.getUTF8Data(), "_presence._tcp", NULL, NULL, port, 
						txtRecord.getSize(), txtRecord.getData(), 
						&BonjourRegisterQuery::handleServiceRegisteredStatic, this);
				if (result != kDNSServiceErr_NoError) {
					sdRef = NULL;
				}
			}

			void registerService() {
				if (sdRef) {
					run();
				}
				else {
					MainEventLoop::postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
				}
			}

			void unregisterService() {
				finish();
			}

			void updateServiceInfo(const ByteArray& txtRecord) {
				boost::lock_guard<boost::mutex> lock(sdRefMutex);
				DNSServiceUpdateRecord(sdRef, NULL, NULL, txtRecord.getSize(), txtRecord.getData(), 0);
			}

		private:
			static void handleServiceRegisteredStatic(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context) {
				static_cast<BonjourRegisterQuery*>(context)->handleServiceRegistered(errorCode, name, regtype, domain);
			}

			void handleServiceRegistered(DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain) {
				if (errorCode != kDNSServiceErr_NoError) {
					MainEventLoop::postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
				}
				else {
					MainEventLoop::postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>(DNSSDServiceID(name, domain, regtype, 0))), shared_from_this());
				}
			}
	};
}
