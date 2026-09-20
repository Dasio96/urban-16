set -e
python3 tests/make_test_obj.py >/dev/null
OUT=$(./lc3_emu test.obj)
echo "$OUT" | grep -q "URBAN"
./lc3_emu tests/test_alu.obj >/dev/null
./lc3_emu tests/test_branch.obj >/dev/null
./lc3_emu tests/test_mem.obj >/dev/null
./lc3_emu tests/test_illegal.obj 2>&1 | grep -q "Illegal instruction"
TRAP_OUT=$(./lc3_emu tests/test_traps.obj)
echo "$TRAP_OUT" | grep -q "AOK"
echo "X" | ./lc3_emu tests/test_traps.obj >/dev/null
echo "ALL TESTS PASSED"
