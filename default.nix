with import <nixpkgs> {};
# fastStdenv.mkDerivation { # for faster running times (8-12%) BUT... nondeterministic builds :(
fastStdenv.mkDerivation {
  name = "vanitygen++";
  src = ./.;
  
  nativeBuildInputs = [ pkg-config ];

  buildInputs = [ gcc pcre openssl opencl-clhpp ocl-icd curl curlpp ];

  makeFlags = [ "-j${toString stdenv.jobs}" ];
  NIX_CFLAGS_COMPILE = [ "-O3" "-march=native" "-mtune=native" ];

  doCheck = false;

  buildPhase = ''
    make all
  '';

  # for a generic copy of all compiled executables:
  # cp $(find * -maxdepth 1 -executable -type f) $out/bin/
  installPhase = ''
    mkdir -p $out/bin
    cp keyconv oclvanitygen++ oclvanityminer vanitygen++ $out/bin/
  '';
}
# to run this on nix/nixos, just run `nix-build` in the directory containing this file
# and then run any executable in the result/bin directory
# e.g. `./result/bin/vanitygen++ -h`
