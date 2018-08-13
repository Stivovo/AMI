import sys
import os
import argparse
import pyAMI


def main():
    parser = argparse.ArgumentParser(usage='%(prog)s [options] <App.apk | AndroidManifest.xml>')
    parser.add_argument("-p", "--package-name", action="store_true", default=False)
    parser.add_argument("-v", "--version", action="store_true", default=False)
    parser.add_argument("-x", "--xml", action="store_true", default=False)
    parser.add_argument("--perms", action="store_true", default=False)
    parser.add_argument("target",
            help='Apk or AndroidManifest.xml file')

    args     = parser.parse_args()
    target   = args.target
    manifest = pyAMI.Manifest.open(target)

    if not manifest:
        print("Unable to open '{}'".format(target))
        sys.exit(1)

    if args.package_name:
        print(manifest.package_name)

    if args.version:
        print("{} - {}".format(manifest.version_name, manifest.version_code))

    if args.perms:
        for p in manifest.permissions:
            print(p)


    if args.xml:
        print(manifest.xml)


if __name__ == "__main__":
    main()

