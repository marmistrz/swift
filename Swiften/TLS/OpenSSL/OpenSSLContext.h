/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <openssl/ssl.h>
#include "Swiften/Base/boost_bsignals.h"
#include <boost/noncopyable.hpp>

#include "Swiften/TLS/TLSContext.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class PKCS12Certificate;

	class OpenSSLContext : public TLSContext, boost::noncopyable {
		public:
			OpenSSLContext();
			~OpenSSLContext();

			void connect();
			bool setClientCertificate(const PKCS12Certificate& cert);

			void handleDataFromNetwork(const ByteArray&);
			void handleDataFromApplication(const ByteArray&);

		private:
			static void ensureLibraryInitialized();	

			void doConnect();
			void sendPendingDataToNetwork();
			void sendPendingDataToApplication();

		private:
			enum State { Start, Connecting, Connected, Error };

			State state_;
			SSL_CTX* context_;
			SSL* handle_;
			BIO* readBIO_;
			BIO* writeBIO_;
	};
}
