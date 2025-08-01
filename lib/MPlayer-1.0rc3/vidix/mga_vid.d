vidix/mga_vid.d vidix/mga_vid.o: vidix/mga_vid.c vidix/vidix.h vidix/fourcc.h vidix/dha.h \
 vidix/pci_ids.h vidix/pci_vendors.h vidix/pci_names.h
vidix/pci_dev_ids.d vidix/pci_dev_ids.o: vidix/pci_dev_ids.c vidix/pci_names.h
vidix/pci_names.d vidix/pci_names.o: vidix/pci_names.c vidix/pci_names.h vidix/pci_dev_ids.c
