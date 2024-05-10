#ifndef COMPRES_SAVEUTILS_H
#define COMPRES_SAVEUTILS_H

#include "types.h"

#include <memory>

namespace compres {

Errors saveCompressed(const char*                           fileName,
                      const std::shared_ptr<compres::Data>& data);
Errors saveDecompressed(const char*                           fileName,
                        const std::shared_ptr<compres::Data>& data);
} // namespace compres

#endif // COMPRES_SAVEUTILS_H
