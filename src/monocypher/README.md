# Monocypher

`monocypher.c` and `monocypher.h` are Monocypher 4.0.2, unchanged, from
https://github.com/LoupVaillant/Monocypher (tag `4.0.2`, `src/`). The
copies on GitHub say `version __git__` in place of the version number.

Monocypher is dual-licensed under the 2-clause BSD licence and CC0; see
`LICENCE.md`.

MicroEMACS uses it in `src/ecrypt2.c` for CRYPT mode: XChaCha20-Poly1305
(`crypto_aead_lock`/`crypto_aead_unlock`) to encrypt files and Argon2id
(`crypto_argon2`) to make keys from passwords.
