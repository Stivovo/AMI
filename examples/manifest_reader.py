import sys
import os
import argparse
import pyAMI


def main():
    parser = argparse.ArgumentParser(usage='%(prog)s [options] <App.apk | AndroidManifest.xml>')
    parser.add_argument("target",
            help='Apk or AndroidManifest.xml file')

    args     = parser.parse_args()
    target   = args.target
    manifest = pyAMI.Manifest.open(target)

    if not manifest:
        print("Unable to open '{}'".format(target))
        sys.exit(1)

    print(manifest.xml)


if __name__ == "__main__":
    main()

