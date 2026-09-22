for o in $(find build/src/field -name "*.c.o"|sort); do
  echo "###OBJ $o"
  mips-linux-gnu-objdump -d -r --section=.text "$o"
done
