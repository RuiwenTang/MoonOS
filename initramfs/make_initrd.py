
import os
import sys


if __name__ == "__main__":
    # os.popen("touch {}".format(sys.argv[2]))
    os.popen("find {} -print0 | cpio --null -ov --format=newc > {}".format(sys.argv[1], sys.argv[2]))
