#include "balloc.hpp"

#include <iostream>


static void dump_range(const Balloc::RangeVector& ranges) {
    for(size_t i = 0; i < ranges.size; i++) {
        std::cout << "range : {" << std::hex << ranges[i].begin << ", " << std::hex << ranges[i].end << "}" << std::endl;
    }
}

int main(int argc, const char** argv) { 
    Balloc balloc;

    balloc.AddToRange(balloc.fFreeRanges, 0x1000, 0x9fc00);
    balloc.AddToRange(balloc.fFreeRanges, 0x100000, 0x7fe0000);

    balloc.RemoveFromRange(balloc.fFreeRanges, 0x1000e8, 0x319050);
    dump_range(balloc.fFreeRanges);
    return 0; 
}