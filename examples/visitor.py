import pyAMI
import sys

class MyVisitor(pyAMI.XML.Visitor):
    def __init__(self):
        super().__init__()

    def Manifest(self, m):
        print("Manifest")

    def Element(self, m):
        print("Element")


def process(apk):
    m = pyAMI.Manifest.open(apk)
    visitor = MyVisitor()
    m.xml.accept(visitor)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <apk>")
        sys.exit(1)

    process(sys.argv[1])

