#!/usr/bin/python3
import os
import sys
import utils


def build(top, app_dir, media_dir, ext_def=""):
    utils.cmd_check(
        "cmake {} -DINSTALL_APP_DIR={} -DINSTALL_MEDIA_DIR={} {}".format(top, app_dir, media_dir, ext_def))
    utils.cmd_check("make -j6")
    utils.cmd_check("make install")


if __name__ == "__main__":
    utils.print_info("./build.py [rebuild]")

    argc = len(sys.argv)

    topdir = os.getcwd() + "/.."
    os.environ["LANG"] = "C"
    build_dir = topdir + "/build"
    os.system("mkdir -p " + build_dir)
    os.chdir(build_dir)
    rebuild = (len(sys.argv) == 2 and sys.argv[1] == "rebuild")
    if rebuild:
        os.system("rm -r *")
    # build(topdir, os.path.join(topdir, "firmware/app"), os.path.join(topdir, "firmware/media"))
    build(topdir, "/home/hn/eapil-work/firmware/app", "/home/hn/eapil-work/firmware/media")
    utils.print_info("Build done. rebuild={}".format(rebuild))
