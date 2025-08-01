vidix/sis_vid.d vidix/sis_vid.o: vidix/sis_vid.c config.h vidix/vidix.h vidix/fourcc.h \
 vidix/dha.h vidix/pci_ids.h vidix/pci_vendors.h vidix/pci_names.h \
 vidix/sis_regs.h vidix/sis_defs.h
vidix/pci_dev_ids.d vidix/pci_dev_ids.o: vidix/pci_dev_ids.c vidix/pci_names.h
vidix/pci_names.d vidix/pci_names.o: vidix/pci_names.c vidix/pci_names.h vidix/pci_dev_ids.c
