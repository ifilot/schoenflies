# Schoenflies

/ˈʃøːnfliːs/

Determine and visualise molecular symmetry.

## Dependencies (Debian)

```
sudo apt install build-essential qt5-default libboost-all-dev libglm-dev
```

## Compilation (Debian)

```
$ cd build
$ cmake ..
$ make -j5
```

## Testing

After compilation, run `build/test/schoenflies_test`. Ensure that the working
directory is a descendant of the project's root directory (or the project's
root directory itself) so that required test files can be found.

## License

This project is licensed under [the GNU GPL v3 license.](LICENSE.md)
