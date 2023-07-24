#!/usr/bin/python3
import os
import datetime

version_prefix = "2.0"


def update_version(file):
    print("\033[1;35m" + "updating version ..." + "\033[0m")
    with open(file, 'r') as f:
        while True:
            line = f.readline()
            if -1 == line.find("EAPIL_HANGYAN_VERSION"):
                continue

            v = line.split("\"")[1]
            main, sub, old_date, build_ver = v.split(".")

            date = datetime.datetime.now().strftime('%Y%m%d')
            if old_date != date:
                build_ver = "0"
            version = version_prefix + '.' + date + '.' + str(int(build_ver) + 1)
            f.close()
            print("\033[1;35m" + "new version = " + version + "\033[0m")
            break
    with open(file, 'w+') as f:
        f.write('#ifndef EAPIL_EapilHangYanVersion_HPP\n')
        f.write('#define EAPIL_EapilHangYanVersion_HPP\n')
        f.write('#define EAPIL_HANGYAN_VERSION "{}"\n'.format(version))
        f.write('#endif //EAPIL_EapilHangYanVersion_HPP\n')
        f.close()
    return version


if __name__ == "__main__":
    topdir = os.getcwd()

    ver = update_version(topdir + "/include/EapilHangYanVersion.h")
    output_date_dir = topdir + "/output/" + ver
    os.system("mkdir -p " + output_date_dir)
