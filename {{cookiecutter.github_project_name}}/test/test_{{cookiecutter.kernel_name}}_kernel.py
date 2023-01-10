#############################################################################
# Copyright (c) {% now 'utc', '%Y' %}, {{cookiecutter.full_name}}
#
# Distributed under the terms of the {{cookiecutter.open_source_license}}.
#
# The full license is in the file LICENSE, distributed with this software.
#############################################################################

import tempfile
import unittest
import jupyter_kernel_test


class KernelTests(jupyter_kernel_test.KernelTests):

    kernel_name = "{{cookiecutter.kernel_name}}"
    language_name = "{{cookiecutter.language}}"
    code_hello_world = "hello, world"
    code_page_something = "?"
    completion_samples = [{"text": "H", "matches": {"Hello", "Hey", "Howdy"}}]
    complete_code_samples = ["hello, world"]
    incomplete_code_samples = ["incomplete"]
    invalid_code_samples = ["invalid"]
    code_inspect_sample = "print"

    #######################################################################################
    # TODO Uncomment these code lines in order to enable the tests (to be modified/adapted)
    #######################################################################################
    # These tests examples check that the given code "hello, world" would be published as
    # text stdout stream message and "error" as stderr
    # Other tests are performed inside the jupyter_kernel_test `execute_helper` function
    # and based on the variables defined above

    #def test_stdout(self):
        #self.flush_channels()
        #reply, output_msgs = self.execute_helper(code="hello, world")
        #self.assertEqual(output_msgs[0]["msg_type"], "stream")
        #self.assertEqual(output_msgs[0]["content"]["name"], "stdout")
        #self.assertEqual(output_msgs[0]["content"]["text"], "hello, world")

    #def test_stderr(self):
        #self.flush_channels()
        #reply, output_msgs = self.execute_helper(code="error")
        #self.assertEqual(output_msgs[0]["msg_type"], "stream")
        #self.assertEqual(output_msgs[0]["content"]["name"], "stderr")


if __name__ == "__main__":
    unittest.main()
