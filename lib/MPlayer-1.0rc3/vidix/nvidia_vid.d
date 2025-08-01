vidix/nvidia_vid.d vidix/nvidia_vid.o: vidix/nvidia_vid.c config.h vidix/vidix.h vidix/fourcc.h \
 vidix/dha.h vidix/pci_ids.h vidix/pci_vendors.h vidix/pci_names.h \
 libavutil/common.h libavutil/mem.h libavutil/common.h mpbswap.h config.h \
 libavutil/bswap.h libavutil/x86/bswap.h
vidix/pci_dev_ids.d vidix/pci_dev_ids.o: vidix/pci_dev_ids.c vidix/pci_names.h
vidix/pci_names.d vidix/pci_names.o: vidix/pci_names.c vidix/pci_names.h vidix/pci_dev_ids.c
