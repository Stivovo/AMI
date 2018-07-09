import pyAMI

# Load Manifest
manifest = pyAMI.Manifest.open("AndroidManifest.xml")

# 1. Print services registred
for service in manifest.services:
    print(service, service.cls) # Prettyprint + Java Class


# 2. Print permission used
for perm in manifest.permissions:
    print(perm)

# 3. Print Manifest as XML
print(manifest.xml)
