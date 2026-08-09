# fix/winxp-mbedtls3

MbedTLS v3 uses CNG (bcrypt) as a source of entropy by default,
but Windows XP only has the older CryptoAPI so it fails to start
up. The fix is to set MBEDTLS_NO_PLATFORM_ENTROPY and
MBEDTLS_ENTROPY_HARDWARE_ALT in a copy of mbedtls_config.h
(mbedtls_user_config.h) and implement a function Rng::getEntropy()
that uses CryptoAPI and have that added as an entropy source to
the RNG and also implement mbedtls_hardware_poll() that also
calls Rng::getEntropy().

The modified mbedtls config header is signalled to the MbedTLS
library build with "-DMBEDTLS_CONFIG_FILE=mbedtls_user_config.h"
on the cmake command-line, and to the gssl build with
"-DMBEDTLS_USER_CONFIG" in the CXXFLAGS. The MBEDTLS_USER_CONFIG
switch results in "#include 'mbedtls_user_config.h'" as the first
include in gssl_mbedtls.h.

These changes are also needed in the keygen program which means
that it is no longer feasible to build it without a dependence
on gssl. The separate generateKey() function is therefore moved
into the gssl library as GSsl::MbedTls::Certificate::generate().

