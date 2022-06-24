{ pkgs ? import <nixpkgs> {} }:

  pkgs.mkShell {
	#nativeBuildInputs = with pkgs; [ pkgconfig SDL2 SDL2_image ];
    buildInputs = with pkgs; [ SDL2 ];
}
