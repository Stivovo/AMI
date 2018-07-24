Android Manifest Inspector
============================

AMI is a library that aims to provide an user friend API to deal with ``AndroidManifest.xml``. Core is writting in C++ and Python binding are provided.

Getting Start
-------------

.. code-block:: console

  $ git clone https://github.com/romainthomas/AMI.git
  $ cd AMI
  $ python ./setup.py install --user

Here is an example of the Python API

.. code-block:: python

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

.. code-block::


  com.supercell.clashofclans.GCMIntentService - EXPORTED com.supercell.clashofclans.GCMIntentService
  org.OpenUDID.OpenUDID_service - EXPORTED
     org.OpenUDID.GETUDID
   org.OpenUDID.OpenUDID_service
  com.supercell.clashofclans.permission.C2D_MESSAGE
  com.google.android.c2dm.permission.RECEIVE
  com.android.vending.BILLING
  android.permission.INTERNET
  android.permission.ACCESS_NETWORK_STATE
  android.permission.WAKE_LOCK
  android.permission.CHANGE_WIFI_STATE
  android.permission.ACCESS_WIFI_STATE
  android.permission.WRITE_EXTERNAL_STORAGE
  android.permission.READ_EXTERNAL_STORAGE
  <manifest>
    <namespace prefix:android uri: http://schemas.android.com/apk/res/android
      <manifest android:installLocation=0 android:versionCode=945 android:versionName=9.434.14 platformBuildVersionCode=27 package=com.supercell.clashofclans platformBuildVersionName=8.1.0>
        <uses-sdk android:minSdkVersion=15 android:targetSdkVersion=26>
        </uses-sdk>

        <permission android:name=com.supercell.clashofclans.permission.C2D_MESSAGE android:protectionLevel=0x2>
        </permission>

        <uses-permission android:name=com.supercell.clashofclans.permission.C2D_MESSAGE>
        </uses-permission>

        <uses-permission android:name=com.google.android.c2dm.permission.RECEIVE>
        </uses-permission>
  ...


Authors
-------

Romain Thomas `@rh0main <https://twitter.com/rh0main>`_ - `Quarkslab <https://www.quarkslab.com>`_




