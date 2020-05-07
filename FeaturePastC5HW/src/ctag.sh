
find . -name "*.h" -o -name "*.c" -o -name "*.cpp" -o -name "*.cc" > src.files
ctags -R --c++-kinds=+px --fields=+iaS --extra=+q -L src.files
