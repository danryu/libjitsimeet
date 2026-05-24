#include <optional>
#include <vector>

#include <openssl/evp.h>
#include <openssl/x509.h>

#include "../macros/assert.hpp"
#include "../macros/autoptr.hpp"

namespace cert {
namespace {
declare_autoptr(PKey, EVP_PKEY, EVP_PKEY_free);
declare_autoptr(PKeyCtx, EVP_PKEY_CTX, EVP_PKEY_CTX_free);
declare_autoptr(PKCS8PKeyInfo, PKCS8_PRIV_KEY_INFO, PKCS8_PRIV_KEY_INFO_free);
declare_autoptr(X509, X509, X509_free);
declare_autoptr(BIO, BIO, BIO_free);
} // namespace

struct Cert {
    AutoPKey pkey;
    AutoX509 x;
};

namespace {
constexpr auto error_value = nullptr;

auto generate_pkey() -> AutoPKey {
    const auto ctx = AutoPKeyCtx(EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL));
    ensure_v(ctx != NULL);
    ensure_v(EVP_PKEY_keygen_init(ctx.get()) == 1);
    ensure_v(EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx.get(), NID_X9_62_prime256v1) == 1);
    auto pkey = (EVP_PKEY*)(nullptr);
    ensure_v(EVP_PKEY_keygen(ctx.get(), &pkey) == 1);
    return AutoPKey(pkey);
}

auto generate_x509(EVP_PKEY* const pkey) -> AutoX509 {
    auto x = AutoX509(X509_new());
    ensure_v(x != NULL);
    ASN1_INTEGER_set(X509_get_serialNumber(x.get()), 1);
    X509_gmtime_adj(X509_get_notBefore(x.get()), 0);
    X509_gmtime_adj(X509_get_notAfter(x.get()), 365 * 24 * 60 * 60);
    X509_set_pubkey(x.get(), pkey);

    const auto name = X509_get_subject_name(x.get());
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char*)"JP", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)"gstjitsimeet", -1, -1, 0);
    X509_set_issuer_name(x.get(), name);
    ensure_v(X509_sign(x.get(), pkey, EVP_sha256()));
    return x;
}

auto serialize_der(const auto i2d, const auto ptr) -> std::optional<std::vector<std::byte>> {
    const auto len = i2d(ptr, NULL);
    ensure(len > 0);
    auto buf     = std::vector<std::byte>(len);
    auto buf_ptr = std::bit_cast<unsigned char*>(buf.data());
    ensure(i2d(ptr, &buf_ptr) == len);
    return buf;
}
} // namespace

auto cert_new() -> Cert* {
    auto pkey = generate_pkey();
    ensure(pkey);
    auto x = generate_x509(pkey.get());
    ensure(x);
    return new Cert{std::move(pkey), std::move(x)};
}

auto cert_delete(const Cert* const cert) -> void {
    delete cert;
}

auto serialize_cert_der(const Cert* const cert) -> std::optional<std::vector<std::byte>> {
    return serialize_der(i2d_X509, cert->x.get());
}

auto serialize_private_key_der(const Cert* const cert) -> std::optional<std::vector<std::byte>> {
    return serialize_der(i2d_PrivateKey, cert->pkey.get());
}

auto serialize_private_key_pkcs8_der(const Cert* const cert) -> std::optional<std::vector<std::byte>> {
    const auto pkcs8 = AutoPKCS8PKeyInfo(EVP_PKEY2PKCS8(cert->pkey.get()));
    return serialize_der(i2d_PKCS8_PRIV_KEY_INFO, pkcs8.get());
}
} // namespace cert
