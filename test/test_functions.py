#!/usr/bin/python3

import subprocess

tests = {
    "pht"   :"pht/test_pht",
    "expand_passphrase" : "expand_passphrase/test_expand_passphrase",
    "invert_ctx" : "invert_ctx/test_invert_ctx",
    "block" :"block/test_block",
    "encrypt_file_cbc" : "encrypt_file_cbc/test_encrypt_file_cbc"
    }

exit_codes = {}

print("\n\u001b[32m[*] Running tests.\u001b[37m\n")

for k in tests.keys():

    child = subprocess.Popen("test/" + tests[k], stdout=subprocess.PIPE)
    data = child.communicate()[0]
    code = child.returncode

    exit_codes[k] = code

    print (data.decode("ascii"),end='')

all_passed = True

for k in exit_codes.keys():

    if exit_codes[k] > 0:

        print("\u001b[31m[X] " + str(exit_codes[k]) + " tests failed in " + k + "\n")

        all_passed = False

if all_passed:

        print("\u001b[32m[*] All tests passed.\n")



print("\u001b[37m")
