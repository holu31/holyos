#include <kernel/pci_tables.h>

const char* pci_get_class_name(uint8_t class_code) {
	switch(class_code) {
		case 0x00: return "Pre-2.0 device";
		case 0x01: return "Mass storage controller";
		case 0x02: return "Network controller";
		case 0x03: return "Display controller";
		case 0x04: return "Multimedia controller";
		case 0x05: return "Memory controller";
		case 0x06: return "Bridge device";
		case 0x07: return "Simple comm. controller";
		case 0x08: return "Base system peripheral";
		case 0x09: return "Input device";
		case 0x0A: return "Docking station";
		case 0x0B: return "Processor";
		case 0x0C: return "Serial bus controller";
		case 0x0D: return "Wireless controller";
		case 0x0E: return "Intelligent controller";
		case 0x0F: return "Satellite comm. controller";
		case 0x10: return "Encryption controller";
		case 0x11: return "Signal processing controller";
		case 0x12: return "Processing accelerator";
		case 0x13: return "Non-essential instrumentation";
		default:   return "Unknown device";
	}
}
