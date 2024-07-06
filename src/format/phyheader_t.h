#ifndef PHYHEADER_T_H
#define PHYHEADER_T_H

namespace PHYParser::Format {

struct phyheader_t {
	int size;
	int id;
	int solidCount;
	long checksum;
};

}

#endif //PHYHEADER_T_H
