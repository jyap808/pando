{ pkgs, lib, config, inputs, ... }:

{
  packages = [
    pkgs.zensical
  ];

  scripts.gen.exec = "cd $DEVENV_ROOT/ergogen && ergogen .";
  scripts.gensvg.exec = "cd $DEVENV_ROOT/ergogen && ergogen --svg .";
  scripts.serve.exec = "cd $DEVENV_ROOT && zensical serve";

  enterShell = ''
    echo "Ergogen is ready."
    echo "Run 'serve' to preview docs."
  '';
}
