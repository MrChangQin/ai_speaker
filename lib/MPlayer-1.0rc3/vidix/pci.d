vidix/pci.d vidix/pci.o: vidix/pci.c vidix/dha.h vidix/AsmMacros.h \
 vidix/sysdep/AsmMacros_x86.h config.h vidix/sysdep/pci_linux.c \
 libavutil/common.h libavutil/mem.h libavutil/common.h mpbswap.h config.h \
 libavutil/bswap.h libavutil/x86/bswap.h
vidix/pci_dev_ids.d vidix/pci_dev_ids.o: vidix/pci_dev_ids.c vidix/pci_names.h
vidix/pci_names.d vidix/pci_names.o: vidix/pci_names.c vidix/pci_names.h vidix/pci_dev_ids.c
