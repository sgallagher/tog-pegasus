import unittest
import urllib
import urllib2

class TestPegRSInstanceCollectionRetrieval(unittest.TestCase):
    def setUp(self):
        self.uri = "http://localhost:5988/cimrs/namespaces/test/TestProvider/classes/cmpiPerf_TestClassB/instances/"
        self.response = urllib2.urlopen(self.uri)

    def testShouldHaveCimCollectionHeaders(self):
        self.assertEqual(200, self.response.code, "Should yield a HTTP status code of 200 OK")
        self.assertEqual("application/vnd.cim.collection+json", self.response.headers["Content-type"])

    def testEntityShouldBeArray(self):
        data = self.response.read()
        self.assertTrue(len(data) > 0)
        self.assertEqual('[', data[0], "Instance collection should be array")
        self.assertEqual(']', data[-1], "Instance collection should be array")

    def testEntityShouldContainCorrectData(self):
        try:
            import json
            instances = json.load(self.response)
            for instance in instances:
                self.assertEqual(type(int()), type(instance["theKey"]))
                self.assertEqual(type(unicode()), type(instance["$ref"]), "Link should be a string")
                self.assertEqual(int(instance["$ref"]), instance["theKey"], "Link should equal key")
                self.assertEqual(None, instance["Name"])
        except ValueError, e:
            self.fail(e.reason)
        except ImportError:
            # json module requires Python 2.6
            pass

class TestPegRSInstanceRetrieval(unittest.TestCase):
    def setUp(self):
        self.uri = "http://localhost:5988/cimrs/namespaces/test/TestProvider/classes/cmpiPerf_TestClassB/instances/1"
        self.response = urllib2.urlopen(self.uri)

    def testShouldHaveCimInstanceHeader(self):
        self.assertEqual(200, self.response.code, "Should yield a HTTP status code of 200 OK")
        self.assertEqual("application/vnd.cim.instance+json", self.response.headers["Content-type"])

    def testEntityShouldBeObject(self):
        data = self.response.read()
        self.assertTrue(len(data) > 0, "Response should not be empty")
        self.assertEqual('{', data[0], "Response should be object")
        self.assertEqual('}', data[-1], "Response should be object")

    def testEntityShouldContainCorrectData(self):
        try:
            import json
            instance = json.load(self.response)
            self.assertEqual(type(int()), type(instance["theKey"]))
            self.assertEqual(1, instance["theKey"])
            self.assertFalse("$ref" in instance.keys(), "Should not have a link")
            self.assertEqual(None, instance["Name"])
        except ValueError, e:
            self.fail(e.reason)
        except ImportError:
            # json module requires Python 2.6
            pass

class TestPegRSClassRetrieval(unittest.TestCase):
    def setUp(self):
        self.uri = "http://localhost:5988/cimrs/namespaces/test/TestProvider/classes/cmpiPerf_TestClassB?IncludeQualifiers=true"
        self.response = urllib2.urlopen(self.uri)

    def testEntityShouldHaveCimClassHeader(self):
        self.assertEqual(200, self.response.code, "Should yield a HTTP status code of 200 OK")
        self.assertEqual("application/vnd.cim.class+json", self.response.headers["Content-type"])

    def testEntityShouldBeObject(self):
        data = self.response.read()
        self.assertTrue(len(data) > 0, "Response should not be empty")
        self.assertEqual('{', data[0], "Response should be object")
        self.assertEqual('}', data[-1], "Response should be object")

    def testEntityShouldContainCorrectData(self):
        try:
            import json
            cimClass = json.load(self.response)

            self.assertEqual('cmpiPerf_TestClassB', cimClass['name'])
            self.assertEqual('CIM_EnabledLogicalElement', cimClass['superclass'])

            properties = cimClass['properties']
            self.assertEqual(26, len(properties.keys()))

            qualifiers = cimClass['qualifiers']
            self.assertEqual(2, len(qualifiers.keys()))

        except ValueError, e:
            self.fail(e.reason)
        except ImportError:
            # json module requires Python 2.6
            pass


if __name__ == '__main__':
    unittest.main()
