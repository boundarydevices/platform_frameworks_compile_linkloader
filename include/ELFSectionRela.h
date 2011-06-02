#ifndef ELF_SECTION_RELA_H
#define ELF_SECTION_RELA_H

#include "ELFTypes.h"

#include "utils/raw_ostream.h"

#include <string>

#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include <stdint.h>

template <unsigned Bitwidth> class ELFObject;
template <unsigned Bitwidth> class ELFSectionRela;
template <unsigned Bitwidth> class ELFSectionRela_CRTP;


template <unsigned Bitwidth>
class ELFSectionRela : public ELFSectionRel<Bitwidth> {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

protected:
  addend_t r_addend;

private:
  ELFSectionRela() { }

public:
  addend_t getAddend() const {
    return r_addend;
  }

  bool isValid() const {
    // FIXME: Should check the correctness of the section header.
    return true;
  }

  template <typename Archiver>
  static ELFSectionRela *
  read(Archiver &AR, ELFObject<Bitwidth> const *owner, size_t index = 0);

  void print(bool shouldPrintHeader = false) const;

private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(TypeTraits<ELFSectionRela>::size);

    AR & this->r_offset;
    AR & this->r_info;
    AR & this->r_addend;

    AR.epilogue(TypeTraits<ELFSectionRela>::size);
    return AR;
  }

};

//==================Inline Member Function Definition==========================

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFSectionRela<Bitwidth> *
ELFSectionRela<Bitwidth>::read(Archiver &AR,
                               ELFObject<Bitwidth> const *owner,
                               size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  llvm::OwningPtr<ELFSectionRela> sh(new ELFSectionRela());

  if (!sh->serialize(AR)) {
    // Unable to read the structure.  Return NULL.
    return 0;
  }

  if (!sh->isValid()) {
    // Rela read from archiver is not valid.  Return NULL.
    return 0;
  }

  // Set the section header index
  sh->index = index;

  // Set the owner elf object
  sh->owner = owner;

  return sh.take();
}

template <unsigned Bitwidth>
inline void ELFSectionRela<Bitwidth>::
  print(bool shouldPrintHeader) const {
  using namespace llvm;
  if (shouldPrintHeader) {
    out() << '\n' << fillformat('=', 79) << '\n';
    out().changeColor(raw_ostream::WHITE, true);
    out() << "ELF Relaocation Table(with Addend) Entry "
          << this->getIndex() << '\n';
    out().resetColor();
    out() << fillformat('-', 79) << '\n';
  } else {
    out() << fillformat('-', 79) << '\n';
    out().changeColor(raw_ostream::YELLOW, true);
    out() << "ELF Relaocation Table(with Addend) Entry "
          << this->getIndex() << " : " << '\n';
    out().resetColor();
  }

#define PRINT_LINT(title, value) \
  out() << format("  %-13s : ", (char const *)(title)) << (value) << '\n'
  PRINT_LINT("Offset",       this->getOffset()       );
  PRINT_LINT("SymTab Index", this->getSymTabIndex()  );
  PRINT_LINT("Type",         this->getType()         );
  PRINT_LINT("Addend",       this->getAddend()       );
#undef PRINT_LINT
}

#endif // ELF_SECTION_RELA_H