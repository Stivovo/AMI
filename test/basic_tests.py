import unittest
from unittest import TestCase
import logging
import os

import pyAMI

CURRENT_DIR = os.path.dirname(os.path.realpath(__file__))

class TestBasic(TestCase):
    def setUp(self):
        self.logger = logging.getLogger(__name__)


    def test_file(self):
        manifest_path = os.path.join(CURRENT_DIR, "com.supercell.clashofclans.xml")
        manifest = pyAMI.Manifest.open(manifest_path)

        self.assertEqual(manifest.package_name, "com.supercell.clashofclans")
        self.assertEqual(manifest.version_name, "9.434.14")

        # Test services
        services = manifest.services
        self.assertEqual(len(services), 2)
        self.assertEqual(services[0].cls, "com.supercell.clashofclans.GCMIntentService")
        self.assertEqual(services[1].cls, "org.OpenUDID.OpenUDID_service")

        # Test permissions
        permissions = manifest.permissions
        self.assertEqual(len(permissions), 10)
        self.assertEqual(permissions[0].fullname, "com.supercell.clashofclans.permission.C2D_MESSAGE")
        self.assertEqual(permissions[5].fullname, "android.permission.WAKE_LOCK")
        self.assertEqual(permissions[9].fullname, "android.permission.READ_EXTERNAL_STORAGE")

        # Test activities
        activities = manifest.activities
        self.assertEqual(len(activities), 6)
        self.assertEqual(activities[0].cls, "com.supercell.clashofclans.GameApp")
        self.assertEqual(activities[3].cls, "com.facebook.FacebookActivity")
        self.assertEqual(activities[5].cls, "com.google.android.gms.common.api.GoogleApiActivity")

        # Test BroadcastReceiver
        receivers = manifest.receivers
        self.assertEqual(len(receivers), 3)
        self.assertEqual(receivers[0].cls, "com.supercell.clashofclans.TimeAlarm")
        self.assertEqual(receivers[1].cls, "com.google.android.gcm.GCMBroadcastReceiver")
        self.assertEqual(receivers[2].cls, "com.mobileapptracker.Tracker")
        xml_manifest = str(manifest.xml)
        ami_manifest_path = os.path.join(CURRENT_DIR, "com.supercell.clashofclans.ami.xml")
        with open(ami_manifest_path, "r") as f:
            ami_manifest_xml = f.read()

        ami_manifest_xml = ami_manifest_xml.replace("\r", "")
        xml_manifest     = xml_manifest.replace("\r", "")

        ami_manifest_xml = ami_manifest_xml.replace("\n", "")
        xml_manifest     = xml_manifest.replace("\n", "")

        self.assertMultiLineEqual(xml_manifest, ami_manifest_xml)

    def test_netflix(self):
        manifest_path = os.path.join(CURRENT_DIR, "com.netflix.mediaclient.xml")
        manifest = pyAMI.Manifest.open(manifest_path)

        self.assertEqual(manifest.package_name, "com.netflix.mediaclient")
        self.assertEqual(manifest.version_name, "6.3.0 build 27728")

        services = manifest.services
        self.assertEqual(len(services), 13)




    def test_visitor(self):
        # TODO
        pass

if __name__ == '__main__':

    root_logger = logging.getLogger()
    root_logger.setLevel(logging.DEBUG)

    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)

    root_logger.addHandler(ch)

    unittest.main(verbosity=2)
