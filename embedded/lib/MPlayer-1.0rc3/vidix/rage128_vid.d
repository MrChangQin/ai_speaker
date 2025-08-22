vidix/rage128_vid.d vidix/rage128_vid.o: vidix/rage128_vid.c vidix/radeon_vid.c config.h \
 libavutil/common.h libavutil/mem.h libavutil/common.h mpbswap.h config.h \
 libavutil/bswap.h libavutil/x86/bswap.h vidix/pci_ids.h \
 vidix/pci_vendors.h vidix/pci_names.h vidix/vidix.h vidix/fourcc.h \
 vidix/dha.h vidix/radeon.h
vidix/pci_dev_ids.d vidix/pci_dev_ids.o: vidix/pci_dev_ids.c vidix/pci_names.h
vidix/pci_names.d vidix/pci_names.o: vidix/pci_names.c vidix/pci_names.h vidix/pci_dev_ids.c
